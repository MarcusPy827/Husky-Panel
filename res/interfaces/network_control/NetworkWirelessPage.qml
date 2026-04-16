/*
 * NetworkWirelessPage.qml
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
import QtQuick.Controls

Item {
  id: root
  implicitHeight: pageCol.implicitHeight

  // Filled from NetworkFlyout when a password is needed
  property string pendingApPath: ""
  property string pendingApSsid: ""

  // Helpers

  readonly property bool isAirplane:
    NetworkHandler.airplaneModeEnabled  // qmllint disable unqualified
  readonly property bool isAdapterOff:
    !isAirplane && !NetworkHandler.wlanDeviceEnabled  // qmllint disable unqualified
  readonly property bool noAdapter:
    !NetworkHandler.showWlanIcon  // qmllint disable unqualified
  readonly property bool showList:
    !isAirplane && !isAdapterOff && !noAdapter

  // Layouts

  Column {
    id: pageCol
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: 0

    // Airplane mode
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
      x: 8; width: parent.width - 16; height: airplaneRow.implicitHeight + 20
      radius: 12
      color: Theme.surface_container  // qmllint disable unqualified

      RowLayout {
        id: airplaneRow
        anchors { left: parent.left; right: parent.right
                  verticalCenter: parent.verticalCenter
                  leftMargin: 14; rightMargin: 14
                }
        spacing: 12

        ColumnLayout {
          Layout.fillWidth: true
          spacing: 2
          Text {
            text: NetworkTranslator.Tr("AirplaneMode")
            font.pixelSize: 14; font.weight: Font.Medium
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
          id: airplaneSwitch
          checked: root.isAirplane
          onToggled: NetworkHandler.setAirplaneMode(!root.isAirplane) // qmllint disable unqualified
        }
      }
    }

    // Selecter (wlo dev.)
    Item { width: 1; height: 12 }

    Text {
      x: 16
      text: NetworkTranslator.Tr("TargetDevice")
      color: Theme.primary  // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6 }

    // Dropdown
    Item {
      id: deviceSelector
      x: 8; width: parent.width - 16
      height: 52
      z: 10

      property bool expanded: false

      Rectangle {
        id: deviceSelectorBtn
        anchors.left: parent.left; anchors.right: parent.right
        height: 52; radius: 12
        color: Theme.surface_container  // qmllint disable unqualified
        clip: true

        RowLayout {
          anchors { fill: parent; leftMargin: 14; rightMargin: 14 }
          spacing: 8

          Text {
            Layout.fillWidth: true
            text: {
              var devs = NetworkHandler.wlanDevices  // qmllint disable unqualified
              var sel = NetworkHandler.selectedWlanDevice  // qmllint disable unqualified
              for (var i = 0; i < devs.length; ++i) {
                if (devs[i].path === sel) return devs[i].name
              }
              return "No device"
            }
            color: Theme.surface_fg  // qmllint disable unqualified
            font.pixelSize: 14
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
          }

          Text {
            id: deviceChevron
            text: "expand_more"
            font.family: "Material Symbols Rounded"; font.pixelSize: 18
            color: Theme.surface_variant_fg  // qmllint disable unqualified
            transform: Rotation {
              origin.x: deviceChevron.width / 2; origin.y: deviceChevron.height / 2
              angle: deviceSelector.expanded ? 180 : 0
              Behavior on angle { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
            }
          }
        }

        MouseArea {
          anchors.fill: parent
          onClicked: deviceSelector.expanded = !deviceSelector.expanded
        }
      }

      Rectangle {
        z: 11
        anchors.top: deviceSelectorBtn.bottom; anchors.topMargin: 4
        anchors.left: parent.left; anchors.right: parent.right
        implicitHeight: deviceDropCol.implicitHeight + 8
        radius: 12
        color: Theme.surface_container  // qmllint disable unqualified
        border.color: Theme.primary  // qmllint disable unqualified
        border.width: 1; clip: true
        visible: opacity > 0
        opacity: deviceSelector.expanded ? 1.0 : 0.0
        Behavior on opacity { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }

        Column {
          id: deviceDropCol
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: 4 }
          spacing: 2

          Repeater {
            model: NetworkHandler.wlanDevices  // qmllint disable unqualified
            Rectangle {
              id: devDropItem
              required property var modelData
              width: deviceDropCol.width; height: 40; radius: 8; clip: true
              color: modelData.path === NetworkHandler.selectedWlanDevice  // qmllint disable unqualified
                ? Theme.primary_container : "transparent"  // qmllint disable unqualified

              Text {
                anchors { verticalCenter: parent.verticalCenter
                          left: parent.left; leftMargin: 12
                          right: parent.right; rightMargin: 12 }
                text: devDropItem.modelData.name
                color: devDropItem.modelData.path === NetworkHandler.selectedWlanDevice  // qmllint disable unqualified
                  ? Theme.primary : Theme.surface_fg  // qmllint disable unqualified
                font.pixelSize: 13; elide: Text.ElideRight
              }

              MouseArea {
                anchors.fill: parent
                onClicked: {
                  NetworkHandler.selectWlanDevice(devDropItem.modelData.path) // qmllint disable unqualified
                  deviceSelector.expanded = false
                }
              }
            }
          }
        }
      }
    }

    // Adapter enabler
    Item { width: 1; height: 8 }

    Rectangle {
      x: 8; width: parent.width - 16; height: enabledRow.implicitHeight + 20
      radius: 12
      color: Theme.surface_container  // qmllint disable unqualified

      RowLayout {
        id: enabledRow
        anchors { left: parent.left; right: parent.right
                  verticalCenter: parent.verticalCenter
                  leftMargin: 14; rightMargin: 14 }
        spacing: 12

        Text {
          Layout.fillWidth: true
          text: NetworkTranslator.Tr("Enabled")
          font.pixelSize: 14; font.weight: Font.Medium
          color: Theme.surface_fg  // qmllint disable unqualified
        }

        Md3Switch {
          checked: NetworkHandler.wlanDeviceEnabled  // qmllint disable unqualified
          onToggled: NetworkHandler.setWlanDeviceEnabled( // qmllint disable unqualified
            !NetworkHandler.wlanDeviceEnabled)           // qmllint disable unqualified
        }
      }
    }

    Item { width: 1; height: 12 }

    // Current wlan
    Text {
      x: 16
      visible: root.showList
               && Object.keys(NetworkHandler.currentWlanNetwork).length > 0  // qmllint disable unqualified
      text: NetworkTranslator.Tr("CurrentNetwork")
      color: Theme.primary  // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item {
      width: 1; height: 6
      visible: root.showList
               && Object.keys(NetworkHandler.currentWlanNetwork).length > 0  // qmllint disable unqualified
    }

    Rectangle {
      x: 8; width: parent.width - 16
      height: currentNetRow.implicitHeight + 20
      radius: 12
      color: Theme.surface_container  // qmllint disable unqualified
      visible: root.showList
               && Object.keys(NetworkHandler.currentWlanNetwork).length > 0  // qmllint disable unqualified

      RowLayout {
        id: currentNetRow
        anchors { left: parent.left; right: parent.right
                  verticalCenter: parent.verticalCenter
                  leftMargin: 14; rightMargin: 14 }
        spacing: 10

        Text {
          text: NetworkHandler.currentWlanNetwork["icon"] ?? "wifi_find"  // qmllint disable unqualified
          font.family: "Material Symbols Rounded"; font.pixelSize: 22
          color: Theme.surface_variant_fg  // qmllint disable unqualified
          verticalAlignment: Text.AlignVCenter
        }

        ColumnLayout {
          Layout.fillWidth: true
          spacing: 2

          Text {
            Layout.fillWidth: true
            text: NetworkHandler.currentWlanNetwork["ssid"] ?? ""  // qmllint disable unqualified
            font.pixelSize: 14; font.weight: Font.Medium
            elide: Text.ElideRight
            color: Theme.surface_fg  // qmllint disable unqualified
          }

          // Security + share tags
          Flow {
            Layout.fillWidth: true
            spacing: 4

            Text {
              text: NetworkTranslator.Tr("SecurityDesc")
              font.pixelSize: 12; font.weight: Font.Medium
              color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Text {
              text: NetworkHandler.currentWlanNetwork["security"] ?? NetworkTranslator.Tr("None")  // qmllint disable unqualified
              font.pixelSize: 12
              color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Text {
              visible: NetworkHandler.currentWlanNetwork["isHotspot"] === true  // qmllint disable unqualified
              text: NetworkTranslator.Tr("HotspotDesc")
              font.pixelSize: 12; color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Text {
              visible: NetworkHandler.currentWlanNetwork["isUsb"] === true  // qmllint disable unqualified
              text: NetworkTranslator.Tr("ViaUSB")
              font.pixelSize: 12; color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Text {
              visible: NetworkHandler.currentWlanNetwork["isBluetooth"] === true  // qmllint disable unqualified
              text: NetworkTranslator.Tr("ViaBT")
              font.pixelSize: 12; color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
          }

          // Status text (errors / connecting states)
          Text {
            visible: {
              var s = NetworkHandler.currentWlanNetwork["status"]  // qmllint disable unqualified
              return s === "failed" || s === "incorrect_password"
                  || s === "connecting" || s === "pending_auth"
                  || s === "no_internet"
            }
            text: {
              var s = NetworkHandler.currentWlanNetwork["status"]  // qmllint disable unqualified
              if (s === "failed") return NetworkTranslator.Tr("Failed2Connect")
              if (s === "incorrect_password") return NetworkTranslator.Tr("IncorrectPassword")
              if (s === "connecting") return NetworkTranslator.Tr("Connecting")
              if (s === "pending_auth") return NetworkTranslator.Tr("PendingAuth")
              if (s === "no_internet") return NetworkTranslator.Tr("NoInternet")
              return ""
            }
            font.pixelSize: 12
            color: {
              var s = NetworkHandler.currentWlanNetwork["status"]  // qmllint disable unqualified
              return (s === "connecting" || s === "pending_auth")
                ? Theme.primary : Theme.error  // qmllint disable unqualified
            }
          }
        }

        // Disconnect button
        NetActionButton {
          buttonText: NetworkTranslator.Tr("Disconnect")
          onActionClicked: NetworkHandler.disconnectWlan(  // qmllint disable unqualified
            NetworkHandler.currentWlanNetwork["devicePath"] ?? "")  // qmllint disable unqualified
        }
      }
    }

    Item { width: 1; height: 12 }

    // Available network
    Text {
      x: 16
      visible: root.showList
      text: NetworkTranslator.Tr("AvailableNetwork")
      color: Theme.primary  // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6; visible: root.showList }

    // Network list (max 4 visible)
    Flickable {
      x: 8; width: parent.width - 16
      visible: root.showList
      height: Math.min(netListCol.implicitHeight, 4 * 74)
      contentHeight: netListCol.implicitHeight
      clip: true
      boundsBehavior: Flickable.DragAndOvershootBounds

      Column {
        id: netListCol
        width: parent.width
        spacing: 6

        Repeater {
          model: NetworkHandler.availableWlanNetworks  // qmllint disable unqualified

          Rectangle {
            id: netItem
            required property var modelData
            width: netListCol.width
            height: netRow.implicitHeight + 20
            radius: 12
            color: Theme.surface_container  // qmllint disable unqualified

            RowLayout {
              id: netRow
              anchors { left: parent.left; right: parent.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: 14; rightMargin: 14 }
              spacing: 10

              Text {
                text: netItem.modelData["icon"] ?? "wifi_find"
                font.family: "Material Symbols Rounded"; font.pixelSize: 22
                color: Theme.surface_variant_fg  // qmllint disable unqualified
                verticalAlignment: Text.AlignVCenter
              }

              ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                  Layout.fillWidth: true
                  text: netItem.modelData["ssid"] ?? ""
                  font.pixelSize: 14; font.weight: Font.Medium
                  elide: Text.ElideRight
                  color: Theme.surface_fg  // qmllint disable unqualified
                }

                Flow {
                  Layout.fillWidth: true
                  spacing: 4
                  Text {
                    text: NetworkTranslator.Tr("SecurityDesc")
                    font.pixelSize: 12; font.weight: Font.Medium
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                  Text {
                    text: netItem.modelData["security"] ?? NetworkTranslator.Tr("None")
                    font.pixelSize: 12
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                  Text {
                    visible: netItem.modelData["isHotspot"] === true
                    text: NetworkTranslator.Tr("HotspotDesc");
                    font.pixelSize: 12
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                  Text {
                    visible: netItem.modelData["isUsb"] === true
                    text: NetworkTranslator.Tr("ViaUSB");
                    font.pixelSize: 12
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                  Text {
                    visible: netItem.modelData["isBluetooth"] === true
                    text: NetworkTranslator.Tr("ViaBT");
                    font.pixelSize: 12
                    color: Theme.surface_variant_fg  // qmllint disable unqualified
                  }
                }

                // Failed state label
                Text {
                  visible: {
                    var s = netItem.modelData["status"]
                    return s === "failed" || s === "incorrect_password"
                  }
                  text: netItem.modelData["status"] === "incorrect_password"
                      ? NetworkTranslator.Tr("IncorrectPassword") : NetworkTranslator.Tr("Failed2Connect")
                  font.pixelSize: 12
                  color: Theme.error  // qmllint disable unqualified
                }
              }

              // Connect / Disconnect / Connecting button
              NetActionButton {
                buttonText: {
                  var s = netItem.modelData["status"]
                  if (s === "connecting") return NetworkTranslator.Tr("Connecting")
                  return NetworkTranslator.Tr("Connect")
                }
                enabled: netItem.modelData["status"] !== "connecting"
                onActionClicked: {
                  NetworkHandler.connectWlan(  // qmllint disable unqualified
                    netItem.modelData["apPath"], "")
                }
              }
            }
          }
        }

        // Empty list placeholder
        Item {
          width: netListCol.width; height: 48
          visible: NetworkHandler.availableWlanNetworks.length === 0  // qmllint disable unqualified
          Text {
            anchors.centerIn: parent
            text: NetworkTranslator.Tr("NoNetworkFound")
            font.pixelSize: 13
            color: Theme.surface_variant_fg  // qmllint disable unqualified
            opacity: 0.6
          }
        }
      }
    }

    // Status text replacing the list in special cases
    Item {
      width: parent.width; height: statusText.implicitHeight + 24
      visible: !root.showList
      Text {
        id: statusText
        anchors.centerIn: parent
        text: root.isAirplane ? NetworkTranslator.Tr("AirplaneModeOn")
            : root.isAdapterOff ? NetworkTranslator.Tr("AdapterTurnedOff")
            : root.noAdapter ? NetworkTranslator.Tr("NoAdapterFound")
            : ""
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

  // PWD input
  // Opened by NetworkFlyout when passwordRequired() is emitted.
  Rectangle {
    id: passwordOverlay
    anchors.fill: parent
    color: Theme.surface_bg  // qmllint disable unqualified
    radius: 10
    visible: false
    z: 20

    ColumnLayout {
      anchors { left: parent.left; right: parent.right
                top: parent.top; margins: 16 }
      spacing: 12

      Text {
        Layout.fillWidth: true
        text: NetworkTranslator.Tr("Connecting2") + root.pendingApSsid
        font.pixelSize: 14; font.weight: Font.Medium
        color: Theme.surface_fg  // qmllint disable unqualified
        wrapMode: Text.WordWrap
      }

      // Password field
      Rectangle {
        Layout.fillWidth: true; height: 44
        radius: 8
        color: Theme.surface_container  // qmllint disable unqualified
        border.color: pwField.activeFocus
          ? Theme.primary : "transparent"  // qmllint disable unqualified
        border.width: 1

        TextInput {
          id: pwField
          anchors { fill: parent; leftMargin: 12; rightMargin: 12 }
          verticalAlignment: Text.AlignVCenter
          font.pixelSize: 14
          color: Theme.surface_fg  // qmllint disable unqualified
          echoMode: TextInput.Password
          onAccepted: connectWithPassword()

          Text {
            visible: pwField.text.length === 0 && !pwField.activeFocus
            text: NetworkTranslator.Tr("Password")
            font: pwField.font
            color: Theme.surface_variant_fg  // qmllint disable unqualified
            opacity: 0.6
            anchors { verticalCenter: parent.verticalCenter; left: parent.left }
          }
        }
      }

      RowLayout {
        Layout.fillWidth: true
        spacing: 8

        Item { Layout.fillWidth: true }

        NetActionButton {
          buttonText: NetworkTranslator.Tr("Cancel")
          onActionClicked: {
            passwordOverlay.visible = false
            pwField.text = ""
          }
        }

        NetActionButton {
          buttonText: NetworkTranslator.Tr("Connect")
          onActionClicked: connectWithPassword()
        }
      }
    }
  }

  function showPasswordDialog(ssid, apPath) {
    root.pendingApSsid = ssid
    root.pendingApPath = apPath
    passwordOverlay.visible = true
  }

  function connectWithPassword() {
    if (pwField.text.length === 0) return
    NetworkHandler.connectWlan(root.pendingApPath, pwField.text) // qmllint disable unqualified
    passwordOverlay.visible = false
    pwField.text = ""
  }
}
