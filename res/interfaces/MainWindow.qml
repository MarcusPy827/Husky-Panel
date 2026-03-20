/*
 * AppIndicator.qml
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

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
  id: root
  width: Screen.width
  height: 32

  Rectangle {
    anchors.fill: parent
    color: "#000000"

    Rectangle {
      id: baseLayer
      anchors.fill: parent
      color: Theme.status_bar_bg
      topLeftRadius: 8
      topRightRadius: 8

      RowLayout {
        anchors {
          fill: parent
          leftMargin: 8
          rightMargin: 8
        }
        spacing: 6

        RowLayout {
          spacing: 4
          Layout.alignment: Qt.AlignVCenter

          AppIndicator {}
        }

        Item { Layout.fillWidth: true }

        RowLayout {
          spacing: 4
          Layout.alignment: Qt.AlignVCenter
        }

        Item { Layout.fillWidth: true }

        RowLayout {
          spacing: 4
          Layout.alignment: Qt.AlignVCenter
        }
      }
    }
  }
}
