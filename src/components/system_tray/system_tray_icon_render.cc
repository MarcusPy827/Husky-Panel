/*
 * Copyright (C) 2026 MarcusPy827
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QHBoxLayout>
#include <QVariant>
#include <QDBusObjectPath>
#include <QMenu>
#include <QMouseEvent>
#include <QFile>
#include <QtEndian>
#include <QDBusReply>
#include <QCursor>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/tray_handler/tray_def.h"
#include "src/components/system_tray/system_tray_icon_render.h"

namespace panel {
namespace frontend {
namespace {
QImage OverrideDarkIcons(const QImage &input) {
  if (input.isNull()) {
    return input;
  }

  QImage img = input.convertToFormat(QImage::Format_ARGB32);
  int64_t total_brightness = 0;
  int pixel_count = 0;
  for (int y = 0; y < img.height(); ++y) {
    const QRgb *line = (const QRgb*)img.constScanLine(y);
    for (int x = 0; x < img.width(); ++x) {
      if (qAlpha(line[x]) > 30) {
        total_brightness += qGray(line[x]);
        pixel_count++;
      }
    }
  }

  if (pixel_count == 0) {
    return input;
  }

  int avg_brightness = total_brightness / pixel_count;
  if (avg_brightness < 90) {
    for (int y = 0; y < img.height(); ++y) {
      QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
      for (int x = 0; x < img.width(); ++x) {
        int alpha = qAlpha(line[x]);
        if (alpha > 0) {
          line[x] = qRgba(255, 255, 255, alpha);
        }
      }
    }
  }
  return img;
}

}  // namespace

SystemTrayIcon::SystemTrayIcon(QString service, QWidget *parent) : QWidget(
    parent) {
  QHBoxLayout * layout_gen = new QHBoxLayout();
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  if (btn_ == nullptr) {
    btn_ = new QToolButton();
    btn_->setProperty("class", "common_bar_icon_btn");
  }
  layout_gen->addWidget(btn_);

  btn_->setIconSize(QSize(20, 20));
  btn_->setAutoRaise(true);

  QGraphicsOpacityEffect * opacity_effect = new QGraphicsOpacityEffect(btn_);
  opacity_effect->setOpacity(1.0);
  btn_->setGraphicsEffect(opacity_effect);

  QTimer * blink_timer = new QTimer(this);
  blink_timer->setObjectName("blink_timer");
  blink_timer->setInterval(1000);
  connect(blink_timer, &QTimer::timeout, this, [opacity_effect]() {
    qreal val = opacity_effect->opacity();
    opacity_effect->setOpacity(val > 0.8 ? 0.4 : 1.0);
  });

  QObject::connect(btn_, &QToolButton::clicked, this, [this]() {
    if (icon_dbus_interface_) {
      icon_dbus_interface_->call("Activate", QCursor::pos().x(),
        QCursor::pos().y());
    }
  });

  btn_->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(btn_, &QToolButton::customContextMenuRequested, this,
      [this](const QPoint &) {
    if (!icon_dbus_interface_) {
      return;
    }

    if (menu_imported_ && menu_imported_->menu()) {
      menu_imported_->menu()->popup(QCursor::pos());
    } else {
      icon_dbus_interface_->call("ContextMenu", QCursor::pos().x(),
        QCursor::pos().y());
    }
  });

  btn_->installEventFilter(this);

  if (service.isEmpty()) {
    LOG(ERROR) << absl::StrCat("SystemTrayIcon got empty service name, ",
      "abort loading...");
    return;
  }

  int slash_index = service.indexOf('/');
  if (slash_index != -1) {
    real_service_ = service.left(slash_index);
    real_path_ = service.mid(slash_index);
  } else {
    real_service_ = service;
    real_path_ = "/StatusNotifierItem";
  }

  LOG(INFO) << absl::StrCat("Connecting to Service '",
    real_service_.toStdString(), "' via path '", real_path_.toStdString(),
    "'.");

  icon_dbus_interface_ = new QDBusInterface(
    real_service_,
    real_path_,
    "org.kde.StatusNotifierItem",
    QDBusConnection::sessionBus(),
    this);

  QDBusConnection::sessionBus().connect(real_service_, real_path_,
    "org.kde.StatusNotifierItem", "NewIcon", this, SLOT(UpdateIcon()));
  QDBusConnection::sessionBus().connect(real_service_, real_path_,
    "org.kde.StatusNotifierItem", "NewToolTip", this, SLOT(UpdateToolTip()));
  QDBusConnection::sessionBus().connect(real_service_, real_path_,
    "org.kde.StatusNotifierItem", "NewStatus", this, SLOT(
    UpdateStatus(QString)));

  QVariant menu_path_raw = icon_dbus_interface_->property("Menu");
  QString menu_path = menu_path_raw.value<QDBusObjectPath>().path();

  if (!menu_path.isEmpty() && menu_path != "/") {
    menu_imported_ = new DBusMenuImporter(real_service_, menu_path, this);
    if (menu_imported_->menu()) {
      disconnect(menu_imported_->menu(), SIGNAL(aboutToShow()),
        menu_imported_, SLOT(slotMenuAboutToShow()));
    }
  }

  UpdateIcon();
  UpdateTooltip();

  QString initial_status = icon_dbus_interface_->property("Status").toString();
  if (!initial_status.isEmpty()) {
    UpdateStatus(initial_status);
  }
}

QToolButton* SystemTrayIcon::GetBtn() {
  return btn_;
}

bool SystemTrayIcon::eventFilter(QObject * watched, QEvent * event) {
  if (watched == btn_) {
    if (event->type() == QEvent::MouseButtonRelease) {
      QMouseEvent * mouse_event = static_cast<QMouseEvent *>(event);
      if (mouse_event->button() == Qt::MiddleButton) {
        if (icon_dbus_interface_) {
          icon_dbus_interface_->call("SecondaryActivate",
            QCursor::pos().x(), QCursor::pos().y());
        }
         return true;
      }
    } else if (event->type() == QEvent::Wheel) {
      QWheelEvent * wheel_event = static_cast<QWheelEvent *>(event);
      int delta = wheel_event->angleDelta().y();
      if (icon_dbus_interface_) {
        icon_dbus_interface_->call("Scroll", delta, "vertical");
      }
      return true;
    }
  }
  return QWidget::eventFilter(watched, event);
}

void SystemTrayIcon::UpdateIcon() {
  if (!btn_ || !icon_dbus_interface_->isValid()) {
    return;
  }

  QIcon final_icon;
  QImage processed_image;

  // If we have icon name -- the simpliest job...
  QString icon_name = icon_dbus_interface_->property("IconName").toString();
  QString icon_theme_path = icon_dbus_interface_->property("IconThemePath")
    .toString();

  if (!icon_name.isEmpty()) {
    if (!icon_theme_path.isEmpty()) {
      QString full_path = icon_theme_path;
      if (!full_path.endsWith('/')) {
        full_path += '/';
      }

      if (QFile::exists(full_path + icon_name + ".png")) {
        final_icon = QIcon(full_path + icon_name + ".png");
      } else if (QFile::exists(full_path + icon_name + ".svg"))  {
        final_icon = QIcon(full_path + icon_name + ".svg");
      } else if (QFile::exists(full_path + icon_name)) {
        final_icon = QIcon(full_path + icon_name);
      }
    }

    if (final_icon.isNull()) {
      final_icon = QIcon::fromTheme(icon_name);
    }

    if (!final_icon.isNull()) {
      QImage tmp = final_icon.pixmap(24, 24).toImage();
      processed_image = OverrideDarkIcons(tmp);
      btn_->setIcon(QPixmap::fromImage(processed_image));
      return;
    }
  }

  // Otherwise it will be a pixmap...
  QDBusMessage call = QDBusMessage::createMethodCall(
      icon_dbus_interface_->service(),
      icon_dbus_interface_->path(),
      "org.freedesktop.DBus.Properties", "Get");
  call << "org.kde.StatusNotifierItem" << "IconPixmap";

  QDBusReply<QDBusVariant> reply = QDBusConnection::sessionBus().call(call);

  if (!reply.isValid()) {
      call = QDBusMessage::createMethodCall(
          icon_dbus_interface_->service(),
          icon_dbus_interface_->path(),
          "org.freedesktop.DBus.Properties", "Get");
      call << "org.freedesktop.StatusNotifierItem" << "IconPixmap";
      reply = QDBusConnection::sessionBus().call(call);
  }

  if (!reply.isValid()) return;

  const QDBusVariant &dbus_variant = reply.value();
  QVariant v = dbus_variant.variant();
  const QDBusArgument &arg = v.value<QDBusArgument>();

  if (arg.currentType() != QDBusArgument::ArrayType) {
    return;
  }

  arg.beginArray();
  QImage best_image;
  int best_diff = 9999;
  int target_size = 24;

  while (!arg.atEnd()) {
    int w = 0, h = 0;
    QByteArray data;
    arg.beginStructure();
    arg >> w >> h >> data;
    arg.endStructure();

    if (w <= 0 || h <= 0 || data.isEmpty()) {
      continue;
    }

    if (data.size() != w * h * 4) {
      continue;
    }

    int diff = std::abs(w - target_size);
    if (diff < best_diff) {
      best_diff = diff;
      QByteArray mutable_dta = data;
      uchar * pixel_dta = reinterpret_cast<uchar*>(mutable_dta.data());
      const uchar * end = pixel_dta + mutable_dta.size();
      for (uchar * ptr = pixel_dta; ptr < end; ptr += 4) {
        quint32 pixel = qFromUnaligned<quint32>(ptr);
        qToUnaligned(qToBigEndian<quint32>(pixel), ptr);
      }

      QImage temp_img(pixel_dta, w, h, QImage::Format_ARGB32);
      best_image = temp_img.copy();
    }
  }
  arg.endArray();

  if (!best_image.isNull()) {
    processed_image = OverrideDarkIcons(best_image);
    btn_->setIcon(QPixmap::fromImage(best_image));
  }
}

void SystemTrayIcon::UpdateTooltip() {
  QVariant tool_tip_variant = icon_dbus_interface_->property("ToolTip");
  if (tool_tip_variant.isValid() && btn_ != nullptr) {
    const QDBusArgument &arg = tool_tip_variant.value<QDBusArgument>();
    backend::SNIToolTip tt;
    arg >> tt;
    btn_->setToolTip(tt.title.isEmpty() ? real_service_ : tt.title);
  }
}

void SystemTrayIcon::UpdateStatus(const QString& new_stat) {
  if (!btn_) return;
  QTimer * timer = this->findChild<QTimer*>("blink_timer");
  QGraphicsOpacityEffect * effect = dynamic_cast<QGraphicsOpacityEffect*>(
    btn_->graphicsEffect());

  if (new_stat == "Passive") {
    btn_->setVisible(false);
    if (timer) {
      timer->stop();
    }

    if (effect) {
      effect->setOpacity(1.0);
    }
  } else if (new_stat == "NeedsAttention") {
    btn_->setVisible(true);
    if (timer && !timer->isActive()) {
      timer->start();
    }
  } else {
    btn_->setVisible(true);
    if (timer) {
      timer->stop();
    }

    if (effect) {
      effect->setOpacity(1.0);
    }
  }
}

}  // namespace frontend
}  // namespace panel
