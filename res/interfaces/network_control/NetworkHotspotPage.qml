/*
 * NetworkHotspotPage.qml
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

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

Item {
  id: root
  implicitHeight: hotspotCol.implicitHeight

  readonly property bool isAirplane:
    NetworkHandler.airplaneModeEnabled  // qmllint disable unqualified
  readonly property bool hasSpare:
    NetworkHandler.hotspotAvailable  // qmllint disable unqualified
  readonly property bool showConfig: !isAirplane && hasSpare

  Column {
    id: hotspotCol
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: 0

    // Airplane mod
    Item { width: 1; height: 12 }

    Text {
      x: 16
      text: NetworkTranslator.Tr("AirplaneMode")
      color: Theme.primary  // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6 }

    Rectangle {
      x: 8; width: parent.width - 16
      height: airplaneRow.implicitHeight + 20
      radius: 12
      color: Theme.surface_container  // qmllint disable unqualified

      RowLayout {
        id: airplaneRow
        anchors { left: parent.left
                  right: parent.right
                  verticalCenter: parent.verticalCenter
                  leftMargin: 14
                  rightMargin: 14
                }
        spacing: 12

        ColumnLayout {
          Layout.fillWidth: true; spacing: 2
          Text {
            text: NetworkTranslator.Tr("AirplaneMode")
            font.pixelSize: 14
            font.weight: Font.Medium
            color: Theme.surface_fg  // qmllint disable unqualified
          }
          Text {
            text: NetworkTranslator.Tr("ThisWillDisableConnections")
            font.pixelSize: 12
            color: Theme.surface_variant_fg  // qmllint disable unqualified
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
          }
        }

        Md3Switch {
          checked: root.isAirplane
          onToggled: NetworkHandler.setAirplaneMode(!root.isAirplane)  // qmllint disable unqualified
        }
      }
    }

    Item { width: 1; height: 12 }

    // Hotspot config
    Text {
      x: 16
      visible: root.showConfig
      text: NetworkTranslator.Tr("HotspotConfiguration")
      color: Theme.primary  // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6; visible: root.showConfig }

    // Enabled toggle
    Rectangle {
      x: 8; width: parent.width - 16
      height: hotspotEnabledRow.implicitHeight + 20
      visible: root.showConfig
      radius: 12
      color: Theme.surface_container  // qmllint disable unqualified

      RowLayout {
        id: hotspotEnabledRow
        anchors { left: parent.left
                  right: parent.right
                  verticalCenter: parent.verticalCenter
                  leftMargin: 14
                  rightMargin: 14 
                }
        spacing: 12

        Text {
          Layout.fillWidth: true
          text: NetworkTranslator.Tr("Enabled") 
          font.pixelSize: 14
          font.weight: Font.Medium
          color: Theme.surface_fg   // qmllint disable unqualified
        }

        Md3Switch {
          checked: NetworkHandler.hotspotEnabled  // qmllint disable unqualified
          onToggled: NetworkHandler.setHotspotEnabled( // qmllint disable unqualified
            !NetworkHandler.hotspotEnabled)  // qmllint disable unqualified
        }
      }
    }

    Item { width: 1; height: 8; visible: root.showConfig }

    // SSID input
    Rectangle {
      x: 8; width: parent.width - 16
      height: 48
      visible: root.showConfig
      radius: 12
      color: Theme.surface_container  // qmllint disable unqualified
      border.color: nameField.activeFocus
        ? Theme.primary : "transparent"  // qmllint disable unqualified
      border.width: 1

      TextInput {
        id: nameField
        anchors { fill: parent; leftMargin: 14; rightMargin: 14 }
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 14
        color: Theme.surface_fg  // qmllint disable unqualified
        text: NetworkHandler.hotspotName  // qmllint disable unqualified
        onEditingFinished: NetworkHandler.setHotspotConfig( // qmllint disable unqualified
          nameField.text, passwordField.text)

        // Placeholder
        Text {
          visible: nameField.text.length === 0 && !nameField.activeFocus
          text: NetworkTranslator.Tr("Name")
          font: nameField.font
          color: Theme.surface_variant_fg  // qmllint disable unqualified
          opacity: 0.6
          anchors { verticalCenter: parent.verticalCenter
                    left: parent.left
                  }
        }
      }
    }

    Item { width: 1; height: 6; visible: root.showConfig }

    // Password input
    Rectangle {
      x: 8; width: parent.width - 16
      height: 48
      visible: root.showConfig
      radius: 12
      color: Theme.surface_container  // qmllint disable unqualified
      border.color: passwordField.activeFocus
        ? Theme.primary : "transparent"  // qmllint disable unqualified
      border.width: 1

      TextInput {
        id: passwordField
        anchors { fill: parent; leftMargin: 14; rightMargin: 14 }
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 14
        color: Theme.surface_fg  // qmllint disable unqualified
        echoMode: TextInput.Password
        text: NetworkHandler.hotspotPassword  // qmllint disable unqualified
        onEditingFinished: NetworkHandler.setHotspotConfig(  // qmllint disable unqualified
          nameField.text, passwordField.text)

        Text {
          visible: passwordField.text.length === 0 && !passwordField.activeFocus
          text: NetworkTranslator.Tr("Password")
          font: passwordField.font
          color: Theme.surface_variant_fg  // qmllint disable unqualified
          opacity: 0.6
          anchors { verticalCenter: parent.verticalCenter
                    left: parent.left
                  }
        }
      }
    }

    // Status text replacing config in special cases
    Item {
      width: parent.width; height: hotspotStatusText.implicitHeight + 24
      visible: !root.showConfig
      Text {
        id: hotspotStatusText
        anchors.centerIn: parent
        text: root.isAirplane ? NetworkTranslator.Tr("AirplaneModeOn")
            : NetworkTranslator.Tr("NoSpareAdapter")
        font.pixelSize: 13
        color: Theme.surface_variant_fg  // qmllint disable unqualified
        opacity: 0.6
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 32
      }
    }

    Item { width: 1; height: 8 }
  }
}
