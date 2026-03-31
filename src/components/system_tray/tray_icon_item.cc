/*
 * tray_icon_item.cc
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

#include <QFile>
#include <QCursor>
#include <QMenu>
#include <QtEndian>
#include <QDBusReply>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusVariant>
#include <QDBusArgument>
#include <QDBusObjectPath>
#include <QIcon>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/tray_handler/tray_def.h"
#include "src/components/system_tray/tray_icon_item.h"

namespace panel {
namespace frontend {
namespace {

QImage OverrideDarkIcons(const QImage& input) {
  if (input.isNull()) return input;

  QImage img = input.convertToFormat(QImage::Format_ARGB32);
  int64_t total_brightness = 0;
  int pixel_count = 0;
  for (int y = 0; y < img.height(); ++y) {
    const QRgb* line = reinterpret_cast<const QRgb*>(img.constScanLine(y));
    for (int x = 0; x < img.width(); ++x) {
      if (qAlpha(line[x]) > 30) {
        total_brightness += qGray(line[x]);
        pixel_count++;
      }
    }
  }
  if (pixel_count == 0) return input;

  int avg_brightness = total_brightness / pixel_count;
  if (avg_brightness < 90) {
    for (int y = 0; y < img.height(); ++y) {
      QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
      for (int x = 0; x < img.width(); ++x) {
        int alpha = qAlpha(line[x]);
        if (alpha > 0) line[x] = qRgba(255, 255, 255, alpha);
      }
    }
  }
  return img;
}

}  // namespace

TrayIconItem::TrayIconItem(const QString& service, QObject* parent)
    : QObject(parent) {
  if (service.isEmpty()) {
    LOG(ERROR) << absl::StrCat("TrayIconItem: empty service, aborting.");
    return;
  }

  int slash = service.indexOf('/');
  if (slash != -1) {
    real_service_ = service.left(slash);
    real_path_ = service.mid(slash);
  } else {
    real_service_ = service;
    real_path_ = QStringLiteral("/StatusNotifierItem");
  }

  LOG(INFO) << absl::StrCat("TrayIconItem: connecting to '",
    real_service_.toStdString(), "' at '", real_path_.toStdString(), "'.");

  dbus_ = new QDBusInterface(real_service_, real_path_,
    QStringLiteral("org.kde.StatusNotifierItem"),
    QDBusConnection::sessionBus(), this);

  QDBusConnection::sessionBus().connect(real_service_, real_path_,
    QStringLiteral("org.kde.StatusNotifierItem"),
    QStringLiteral("NewIcon"), this, SLOT(UpdateIcon()));
  QDBusConnection::sessionBus().connect(real_service_, real_path_,
    QStringLiteral("org.kde.StatusNotifierItem"),
    QStringLiteral("NewToolTip"), this, SLOT(UpdateTooltip()));
  QDBusConnection::sessionBus().connect(real_service_, real_path_,
    QStringLiteral("org.kde.StatusNotifierItem"),
    QStringLiteral("NewStatus"), this, SLOT(UpdateStatus(QString)));

  // Set up DBusMenu importer if the item exposes a Menu path.
  QVariant menu_path_raw = dbus_->property("Menu");
  QString menu_path = menu_path_raw.value<QDBusObjectPath>().path();
  if (!menu_path.isEmpty() && menu_path != QLatin1String("/")) {
    menu_importer_ = new DBusMenuImporter(real_service_, menu_path, this);
    // Disable the default aboutToShow auto-fetch; we'll populate on demand.
    if (menu_importer_->menu()) {
      disconnect(menu_importer_->menu(), SIGNAL(aboutToShow()),
        menu_importer_, SLOT(slotMenuAboutToShow()));
    }
  }

  UpdateIcon();
  UpdateTooltip();

  QString initial_status = dbus_->property("Status").toString();
  if (!initial_status.isEmpty()) {
    UpdateStatus(initial_status);
  }
}

void TrayIconItem::Activate(int, int) {
  // Use QCursor::pos() for reliable screen coordinates under Wayland.
  // QML's mapToGlobal() returns window-relative coordinates, not absolute
  // screen coordinates, so we bypass the passed values entirely.
  if (dbus_) dbus_->call(QStringLiteral("Activate"),
    QCursor::pos().x(), QCursor::pos().y());
}

void TrayIconItem::ContextMenu(int, int) {
  if (menu_importer_ && menu_importer_->menu()) {
    menu_importer_->menu()->popup(QCursor::pos());
  } else if (dbus_) {
    dbus_->call(QStringLiteral("ContextMenu"),
      QCursor::pos().x(), QCursor::pos().y());
  }
}

void TrayIconItem::SecondaryActivate(int, int) {
  if (dbus_) dbus_->call(QStringLiteral("SecondaryActivate"),
    QCursor::pos().x(), QCursor::pos().y());
}

void TrayIconItem::Scroll(int delta) {
  if (dbus_) dbus_->call(QStringLiteral("Scroll"), delta,
    QStringLiteral("vertical"));
}

void TrayIconItem::UpdateIcon() {
  if (!dbus_ || !dbus_->isValid()) return;

  QImage result;

  QString icon_name = dbus_->property("IconName").toString();
  QString icon_theme_path = dbus_->property("IconThemePath").toString();

  if (!icon_name.isEmpty()) {
    QIcon final_icon;
    if (!icon_theme_path.isEmpty()) {
      QString base = icon_theme_path.endsWith('/')
        ? icon_theme_path : icon_theme_path + '/';
      if (QFile::exists(base + icon_name + ".png"))
        final_icon = QIcon(base + icon_name + ".png");
      else if (QFile::exists(base + icon_name + ".svg"))
        final_icon = QIcon(base + icon_name + ".svg");
      else if (QFile::exists(base + icon_name))
        final_icon = QIcon(base + icon_name);
    }
    bool is_symbolic = false;
    if (final_icon.isNull()) {
      // Priority: full-colour icon from the system theme first (e.g. the
      // coloured fcitx-rime logo).  Only fall back to -symbolic when the
      // theme has no entry for the plain name; symbolic icons are guaranteed
      // monochrome and get forced to white without heuristics.
      final_icon = QIcon::fromTheme(icon_name);
      if (final_icon.isNull()) {
        final_icon = QIcon::fromTheme(icon_name + QStringLiteral("-symbolic"));
        is_symbolic = !final_icon.isNull();
      }
    }
    if (!final_icon.isNull()) {
      QImage img = final_icon.pixmap(24, 24).toImage()
                              .convertToFormat(QImage::Format_ARGB32);
      if (is_symbolic) {
        for (int y = 0; y < img.height(); ++y) {
          QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
          for (int x = 0; x < img.width(); ++x) {
            int a = qAlpha(line[x]);
            if (a > 0) line[x] = qRgba(255, 255, 255, a);
          }
        }
        icon_image_ = img;
      } else {
        icon_image_ = OverrideDarkIcons(img);
      }
      emit IconChanged();
      return;
    }
  }

  // Fall back to pixmap via D-Bus property.
  QDBusMessage call = QDBusMessage::createMethodCall(
    dbus_->service(), dbus_->path(),
    QStringLiteral("org.freedesktop.DBus.Properties"),
    QStringLiteral("Get"));
  call << QStringLiteral("org.kde.StatusNotifierItem")
       << QStringLiteral("IconPixmap");

  QDBusReply<QDBusVariant> reply = QDBusConnection::sessionBus().call(call);
  if (!reply.isValid()) {
    call = QDBusMessage::createMethodCall(
      dbus_->service(), dbus_->path(),
      QStringLiteral("org.freedesktop.DBus.Properties"),
      QStringLiteral("Get"));
    call << QStringLiteral("org.freedesktop.StatusNotifierItem")
         << QStringLiteral("IconPixmap");
    reply = QDBusConnection::sessionBus().call(call);
  }
  if (!reply.isValid()) return;

  QVariant v = reply.value().variant();
  const QDBusArgument& arg = v.value<QDBusArgument>();
  if (arg.currentType() != QDBusArgument::ArrayType) return;

  arg.beginArray();
  QImage best_image;
  int best_diff = 9999;
  constexpr int kTargetSize = 24;

  while (!arg.atEnd()) {
    int w = 0, h = 0;
    QByteArray data;
    arg.beginStructure();
    arg >> w >> h >> data;
    arg.endStructure();

    if (w <= 0 || h <= 0 || data.size() != w * h * 4) continue;

    // Convert ARGB network byte order to host ARGB32.
    // Referenced from LXQt system tray (GPL v2.0+):
    // https://github.com/lxqt/lxqt-panel.git
    int diff = std::abs(w - kTargetSize);
    if (diff < best_diff) {
      best_diff = diff;
      QByteArray buf = data;
      uchar* ptr = reinterpret_cast<uchar*>(buf.data());
      for (uchar* end = ptr + buf.size(); ptr < end; ptr += 4) {
        quint32 pixel = qFromUnaligned<quint32>(ptr);
        qToUnaligned(qToBigEndian<quint32>(pixel), ptr);
      }
      QImage tmp(reinterpret_cast<const uchar*>(buf.data()), w, h,
        QImage::Format_ARGB32);
      best_image = tmp.copy();
    }
  }
  arg.endArray();

  if (!best_image.isNull()) {
    icon_image_ = OverrideDarkIcons(best_image);
    emit IconChanged();
  }
}

void TrayIconItem::UpdateTooltip() {
  if (!dbus_ || !dbus_->isValid()) return;

  // QDBusInterface::property() fails for complex types that Qt D-Bus hasn't
  // fully mapped (SNIToolTip is "(sa(iiay)ss)"). Use the raw Properties.Get
  // call instead, just like UpdateIcon() does for IconPixmap.
  QDBusMessage call = QDBusMessage::createMethodCall(
    dbus_->service(), dbus_->path(),
    QStringLiteral("org.freedesktop.DBus.Properties"),
    QStringLiteral("Get"));
  call << QStringLiteral("org.kde.StatusNotifierItem")
       << QStringLiteral("ToolTip");

  QDBusReply<QDBusVariant> reply = QDBusConnection::sessionBus().call(call);
  if (!reply.isValid()) return;

  QVariant v = reply.value().variant();
  const QDBusArgument& arg = v.value<QDBusArgument>();
  backend::SNIToolTip tt;
  arg >> tt;
  tooltip_ = tt.title.isEmpty() ? real_service_ : tt.title;
  emit TooltipChanged();
}

void TrayIconItem::UpdateStatus(const QString& status) {
  status_ = status;
  emit StatusChanged();
}

}  // namespace frontend
}  // namespace panel
