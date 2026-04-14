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

#ifndef SRC_COMPONENTS_BLUETOOTH_BLUETOOTH_HANDLER_H_
#define SRC_COMPONENTS_BLUETOOTH_BLUETOOTH_HANDLER_H_

#include <QObject>
#include <QString>
#include <QVariantList>

#include "src/info_server/bluetooth/bluetooth_info.h"

namespace panel {
namespace frontend {

class BluetoothHandler : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool bluetoothEnabled
    READ GetBluetoothEnabled
    NOTIFY EnabledChanged)

  Q_PROPERTY(QVariantList adapters
    READ GetAdapters
    NOTIFY AdaptersChanged)

  Q_PROPERTY(QString selectedAdapterPath
    READ GetSelectedAdapterPath
    NOTIFY AdaptersChanged)

  Q_PROPERTY(QVariantList devices
    READ GetDevices
    NOTIFY DevicesChanged)

  Q_PROPERTY(QString btIcon
    READ GetBtIcon
    NOTIFY DevicesChanged)

 public:
  /* ---------- For backend usage ---------- */
  explicit BluetoothHandler(QObject* parent = nullptr);

  bool GetBluetoothEnabled() const;
  QVariantList GetAdapters() const;
  QString GetSelectedAdapterPath() const;
  QVariantList GetDevices() const;
  QString GetBtIcon() const;

  /* ---------- For QML usage ---------- */
  Q_INVOKABLE void setEnabled(bool enabled);
  Q_INVOKABLE void selectAdapter(const QString& path);
  Q_INVOKABLE void connectDevice(const QString& address);
  Q_INVOKABLE void disconnectDevice(const QString& address);
  Q_INVOKABLE void openPairingSettings();

 signals:
  void EnabledChanged();
  void AdaptersChanged();
  void DevicesChanged();

 private:
  backend::BluetoothInfo* bluetooth_info_ = nullptr;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_BLUETOOTH_BLUETOOTH_HANDLER_H_
