/*
 * NetworkCabledPage.qml
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
  implicitHeight: cabledCol.implicitHeight

  readonly property bool isAirplane:
    NetworkHandler.airplaneModeEnabled  // qmllint disable unqualified
  readonly property bool hasEthernet:
    NetworkHandler.showEthernetIcon  // qmllint disable unqualified
  readonly property bool showList: !isAirplane && hasEthernet

  Column {
    id: cabledCol
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: 0

    // Airplane Mode
    Item { width: 1; height: 12 }

    Text {
      x: 16
      text: NetworkTranslator.Tr("AirplaneMode")
      color: Theme.primary  // qmllint disable unqualified
      font.pixelSize: 12;
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6 }

    Rectangle {
      x: 8
      width: parent.width - 16
      height: airplaneRow.implicitHeight + 20
      radius: 12; color: Theme.surface_container  // qmllint disable unqualified

      RowLayout {
        id: airplaneRow
        anchors { left: parent.left; right: parent.right
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

    // ETHERNET dev list.
    Text {
      x: 16
      visible: root.showList
      text: NetworkTranslator.Tr("AllDevices")
      color: Theme.primary  // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6; visible: root.showList }

    // Device list (max 4 visible)
    Flickable {
      x: 8
      width: parent.width - 16
      visible: root.showList
      height: Math.min(ethListCol.implicitHeight, 4 * 74)
      contentHeight: ethListCol.implicitHeight
      clip: true
      boundsBehavior: Flickable.DragAndOvershootBounds

      Column {
        id: ethListCol
        width: parent.width
        spacing: 6

        Repeater {
          model: NetworkHandler.ethernetDevices  // qmllint disable unqualified

          Rectangle {
            id: ethItem
            required property var modelData
            width: ethListCol.width
            height: ethRow.implicitHeight + 20
            radius: 12; color: Theme.surface_container  // qmllint disable unqualified

            RowLayout {
              id: ethRow
              anchors { left: parent.left
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: 14
                        rightMargin: 14
                      }
              spacing: 10

              Text {
                text: ethItem.modelData["icon"] ?? "globe_2_cancel"
                font.family: "Material Symbols Rounded"; font.pixelSize: 22
                color: Theme.surface_variant_fg  // qmllint disable unqualified
                verticalAlignment: Text.AlignVCenter
              }

              ColumnLayout {
                Layout.fillWidth: true; spacing: 2

                Text {
                  Layout.fillWidth: true
                  text: ethItem.modelData["name"] ?? ""
                  font.pixelSize: 14; font.weight: Font.Medium
                  elide: Text.ElideRight
                  color: Theme.surface_fg  // qmllint disable unqualified
                }

                Flow {
                  Layout.fillWidth: true; spacing: 4
                  Text {
                    text: NetworkTranslator.Tr("StatusDesc")
                    font.pixelSize: 12
                    font.weight: Font.Medium
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                  Text {
                    text: {
                      var s = ethItem.modelData["status"]
                      if (s === "connected") return NetworkTranslator.Tr("Connected")
                      if (s === "no_network") return NetworkTranslator.Tr("NoInternet")
                      if (s === "pending_authentication") return NetworkTranslator.Tr("PendingAuth")
                      if (s === "unplugged") return NetworkTranslator.Tr("Unplugged")
                      return s ?? NetworkTranslator.Tr("UnknownStatus")
                    }
                    font.pixelSize: 12
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                  Text {
                    visible: ethItem.modelData["isHotspot"] === true
                    text: NetworkTranslator.Tr("HotspotDesc")
                    font.pixelSize: 12
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                  Text {
                    visible: ethItem.modelData["isUsb"] === true
                    text: NetworkTranslator.Tr("ViaUSB")
                    font.pixelSize: 12
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                }
              }

              // Disconnect button — only when ethernet is validly connected
              NetActionButton {
                visible: ethItem.modelData["isConnected"] === true
                buttonText: NetworkTranslator.Tr("Disconnect")
                onActionClicked: NetworkHandler.disconnectEthernet(  // qmllint disable unqualified
                  ethItem.modelData["path"] ?? "")
              }
            }
          }
        }
      }
    }

    // Status text replacing the list in special cases
    Item {
      width: parent.width; height: cabledStatusText.implicitHeight + 24
      visible: !root.showList
      Text {
        id: cabledStatusText
        anchors.centerIn: parent
        text: root.isAirplane ? NetworkTranslator.Tr("AirplaneModeOn")
            : NetworkTranslator.Tr("NoAdapterFound")
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
