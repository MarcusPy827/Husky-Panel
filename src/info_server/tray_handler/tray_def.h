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

#ifndef SRC_INFO_SERVER_TRAY_HANDLER_TRAY_DEF_H_
#define SRC_INFO_SERVER_TRAY_HANDLER_TRAY_DEF_H_

#include <QObject>
#include <QDBusArgument>
#include <QDBusMetaType>
#include <QList>
#include <QVector>
namespace panel {
namespace backend {

struct SNIImage {
  int width;
  int height;
  QByteArray data;
};

struct SNIToolTip {
  QString iconName;
  QList<SNIImage> iconData;
  QString title;
  QString description;
};

}  // namespace backend
}  // namespace panel

Q_DECLARE_METATYPE(panel::backend::SNIImage)
Q_DECLARE_METATYPE(QList<panel::backend::SNIImage>)
Q_DECLARE_METATYPE(panel::backend::SNIToolTip)

namespace panel {
namespace backend {

inline QDBusArgument &operator<<(QDBusArgument &arg, const SNIImage &img) {
  arg.beginStructure();
  arg << img.width << img.height << img.data;
  arg.endStructure();
  return arg;
}
inline const QDBusArgument &operator>>(const QDBusArgument &arg,
    SNIImage &img) {
  arg.beginStructure();
  arg >> img.width >> img.height >> img.data;
  arg.endStructure();
  return arg;
}

inline const QDBusArgument &operator>>(const QDBusArgument &arg,
    QList<SNIImage> &list) {
  arg.beginArray();
  list.clear();
  while (!arg.atEnd()) {
    SNIImage item;
    arg >> item;
    list.append(item);
  }
  arg.endArray();
  return arg;
}

inline QDBusArgument &operator<<(QDBusArgument &arg,
    const QList<SNIImage> &list) {
  arg.beginArray(qMetaTypeId<SNIImage>());
  for (const auto &item : list) {
    arg << item;
  }
  arg.endArray();
  return arg;
}

inline QDBusArgument &operator<<(QDBusArgument &arg, const SNIToolTip &tt) {
  arg.beginStructure();
  arg << tt.iconName << tt.iconData << tt.title << tt.description;
  arg.endStructure();
  return arg;
}

inline const QDBusArgument &operator>>(const QDBusArgument &arg,
    SNIToolTip &tt) {
  arg.beginStructure();
  arg >> tt.iconName >> tt.iconData >> tt.title >> tt.description;
  arg.endStructure();
  return arg;
}

inline static void InitSystemTrayTypes() {
  qRegisterMetaType<SNIImage>("SNIImage");
  qRegisterMetaType<QList<SNIImage>>("QList<SNIImage>");
  qRegisterMetaType<SNIToolTip>("SNIToolTip");

  qDBusRegisterMetaType<SNIImage>();
  qDBusRegisterMetaType<QList<SNIImage>>();
  qDBusRegisterMetaType<SNIToolTip>();
}

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_TRAY_HANDLER_TRAY_DEF_H_
