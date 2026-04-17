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
import QtQuick.Dialogs

Item {
  id: root
  implicitHeight: pageCol.implicitHeight

  // Filled from NetworkFlyout when a password is needed
  property string pendingApPath: ""
  property string pendingApSsid: ""
  property string pendingSecurityType: ""

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

  // Helper: file picker

  FileDialog {
    id: filePicker
    property var targetField: null
    onAccepted: {
      if (targetField) {
        targetField.text = selectedFile.toString().replace("file://", "")
      }
    }
  }

  // Password input

  component PwdField: Rectangle {
    id: pwdFieldRoot
    property alias text: input.text
    property bool isPassword: true
    property string placeholder: ""
    signal accepted()

    height: 44; radius: 8
    color: Theme.surface_container  // qmllint disable unqualified
    border.color: input.activeFocus
      ? Theme.primary : "transparent"  // qmllint disable unqualified
    border.width: 1

    TextInput {
      id: input
      anchors { fill: parent; leftMargin: 12; rightMargin: 12 }
      verticalAlignment: Text.AlignVCenter
      font.pixelSize: 14
      color: Theme.surface_fg  // qmllint disable unqualified
      echoMode: pwdFieldRoot.isPassword ? TextInput.Password : TextInput.Normal
      onAccepted: pwdFieldRoot.accepted()

      Text {
        visible: input.text.length === 0 && !input.activeFocus
          && pwdFieldRoot.placeholder.length > 0
        text: pwdFieldRoot.placeholder
        font: input.font
        color: Theme.surface_variant_fg  // qmllint disable unqualified
        opacity: 0.6
        anchors { verticalCenter: parent.verticalCenter; left: parent.left }
      }
    }
  }

  // Noral input
  component LabeledField: ColumnLayout {
    id: lfRoot
    property string label: ""
    property alias text: lfField.text
    property bool isPassword: false
    property bool hasFilePicker: false
    signal accepted()

    spacing: 4

    Text {
      text: lfRoot.label
      font.pixelSize: 12; font.weight: Font.Medium
      color: Theme.surface_variant_fg  // qmllint disable unqualified
    }

    RowLayout {
      Layout.fillWidth: true
      spacing: 6

      PwdField {
        id: lfField
        Layout.fillWidth: true
        isPassword: lfRoot.isPassword
        onAccepted: lfRoot.accepted()
      }

      Rectangle {
        visible: lfRoot.hasFilePicker
        width: 36
        height: 36
        radius: 8
        color: Theme.surface_container  // qmllint disable unqualified

        Text {
          anchors.centerIn: parent
          text: "folder_open"
          font.family: "Material Symbols Rounded"
          font.pixelSize: 18
          color: Theme.surface_variant_fg  // qmllint disable unqualified
        }

        MouseArea {
          anchors.fill: parent
          onClicked: {
            filePicker.targetField = lfField
            filePicker.open()
          }
        }
      }
    }
  }

  component Md3Dropdown: Item {
    id: dropDonRoot
    property var model: []
    property int currentIndex: 0
    readonly property string currentValue:
      dropDonRoot.model.length > 0 ? dropDonRoot.model[dropDonRoot.currentIndex] : ""

    height: 44
    z: 100

    property bool expanded: false

    Rectangle {
      id: ddBtn
      anchors.left: parent.left
      anchors.right: parent.right
      height: 44
      radius: 8
      color: Theme.surface_container  // qmllint disable unqualified
      border.color: dropDonRoot.expanded
        ? Theme.primary : "transparent"  // qmllint disable unqualified
      border.width: 1; clip: true

      RowLayout {
        anchors { fill: parent; leftMargin: 12; rightMargin: 8 }
        spacing: 4
        Text {
          Layout.fillWidth: true
          text: dropDonRoot.currentValue
          font.pixelSize: 14
          color: Theme.surface_fg  // qmllint disable unqualified
          elide: Text.ElideRight
          verticalAlignment: Text.AlignVCenter
        }
  
        Text {
          id: expendIcon
          text: "expand_more"
          font.family: "Material Symbols Rounded"; font.pixelSize: 18
          color: Theme.surface_variant_fg  // qmllint disable unqualified
          transform: Rotation {
            origin.x: expendIcon.width / 2
            origin.y: expendIcon.height / 2
            angle: dropDonRoot.expanded ? 180 : 0
            Behavior on angle {
              NumberAnimation { duration: 160; easing.type: Easing.OutCubic }
            }
          }
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: dropDonRoot.expanded = !dropDonRoot.expanded
      }
    }

    Rectangle {
      anchors.top: ddBtn.bottom
      anchors.topMargin: 2
      anchors.left: parent.left
      anchors.right: parent.right
      implicitHeight: ddPopCol.implicitHeight + 8
      radius: 8
      clip: true
      color: Theme.surface_container  // qmllint disable unqualified
      border.color: Theme.primary
      border.width: 1  // qmllint disable unqualified
      visible: opacity > 0
      opacity: dropDonRoot.expanded ? 1.0 : 0.0
      Behavior on opacity {
        NumberAnimation { duration: 160; easing.type: Easing.OutCubic }
      }

      Column {
        id: ddPopCol
        anchors { left: parent.left; right: parent.right
                  top: parent.top; margins: 4
                }
        spacing: 2

        Repeater {
          model: dropDonRoot.model
          Rectangle {
            id: dropDownENtry
            required property string modelData
            required property int index

            width: ddPopCol.width; height: 36
            radius: 6
            color: dropDownENtry.index === dropDonRoot.currentIndex
              ? Theme.primary_container : "transparent"  // qmllint disable unqualified

            Text {
              anchors { verticalCenter: parent.verticalCenter
                        left: parent.left; leftMargin: 12
                        right: parent.right; rightMargin: 12
                      }
              text: dropDownENtry.modelData
              font.pixelSize: 13
              color: dropDownENtry.index === dropDonRoot.currentIndex
                ? Theme.primary : Theme.surface_fg  // qmllint disable unqualified
              elide: Text.ElideRight
            }

            MouseArea {
              anchors.fill: parent
              onClicked: {
                dropDonRoot.currentIndex = dropDownENtry.index
                dropDonRoot.expanded = false
              }
            }
          }
        }
      }
    }
  }

  // ---- Password overlay ----
  // Opened by NetworkFlyout when passwordRequired() is emitted.

  Rectangle {
    id: passwordOverlay
    anchors { fill: parent; leftMargin: 3; rightMargin: 3 }
    color: Theme.surface_bg  // qmllint disable unqualified
    radius: 10
    visible: false
    z: 20

    Flickable {
      anchors { fill: parent; margins: 16 }
      contentHeight: overlayCol.implicitHeight
      clip: true
      boundsBehavior: Flickable.DragAndOvershootBounds

      ColumnLayout {
        id: overlayCol
        width: parent.width
        spacing: 12

        Text {
          Layout.fillWidth: true
          text: NetworkTranslator.Tr("Connecting2") + root.pendingApSsid
          font.pixelSize: 14; font.weight: Font.Medium
          color: Theme.surface_fg  // qmllint disable unqualified
          wrapMode: Text.WordWrap
        }

        // WPA Personal type
        ColumnLayout {
          visible: root.pendingSecurityType === "wpa_personal"
          Layout.fillWidth: true
          spacing: 8

          PwdField {
            id: wpaPwField
            Layout.fillWidth: true
            isPassword: true
            placeholder: NetworkTranslator.Tr("Password")
            onAccepted: connectWithCredentials()
          }
        }

        // WEP
        ColumnLayout {
          visible: root.pendingSecurityType === "wep"
          Layout.fillWidth: true
          spacing: 8

          PwdField {
            id: wepPwField
            Layout.fillWidth: true
            isPassword: true
            placeholder: NetworkTranslator.Tr("Password")
            onAccepted: connectWithCredentials()
          }

          Text {
            text: NetworkTranslator.Tr("WEPIndex")
            font.pixelSize: 12;
            font.weight: Font.Medium
            color: Theme.surface_variant_fg  // qmllint disable unqualified
          }
          Md3Dropdown {
            id: wepIndexDrop
            Layout.fillWidth: true
            model: ["1", "2", "3", "4"]
            currentIndex: 0
          }

          Text {
            text: NetworkTranslator.Tr("Authentication")
            font.pixelSize: 12
            font.weight: Font.Medium
            color: Theme.surface_variant_fg  // qmllint disable unqualified
          }
          Md3Dropdown {
            id: wepAuthDrop
            Layout.fillWidth: true
            model: [NetworkTranslator.Tr("OpenSystem"), NetworkTranslator.Tr("SharedKey")]
            currentIndex: 0
          }
        }

        // WPA ENterprise
        ColumnLayout {
          visible: root.pendingSecurityType === "enterprise"
          Layout.fillWidth: true
          spacing: 8

          Text {
            text: "Authentication Method"
            font.pixelSize: 12; font.weight: Font.Medium
            color: Theme.surface_variant_fg  // qmllint disable unqualified
          }
          Md3Dropdown {
            id: eapMethodDrop
            Layout.fillWidth: true
            model: ["TLS", "LEAP", "PWD", "FAST", "TTLS",
                    "PEAP"
                   ]
            currentIndex: 5
          }

          // TLS section
          ColumnLayout {
            visible: eapMethodDrop.currentIndex === 0
            Layout.fillWidth: true
            spacing: 8

            LabeledField { id: tlsIdentity; Layout.fillWidth: true; label: NetworkTranslator.Tr("Identity") }
            LabeledField { id: tlsDomain; Layout.fillWidth: true; label: NetworkTranslator.Tr("Domain") }
            LabeledField { id: tlsUserCert; Layout.fillWidth: true; label: NetworkTranslator.Tr("UserCertificate") }
            LabeledField { id: tlsCaCert; Layout.fillWidth: true; label: NetworkTranslator.Tr("CACertificate") }
            LabeledField { id: tlsSubject; Layout.fillWidth: true; label: NetworkTranslator.Tr("SubjectMatch") }
            LabeledField { id: tlsAltSubject; Layout.fillWidth: true; label: NetworkTranslator.Tr("AltSubjectMatch") }
            LabeledField { id: tlsServers; Layout.fillWidth: true; label: NetworkTranslator.Tr("Connect2TheseServers") }
            LabeledField { id: tlsPrivKey; Layout.fillWidth: true; label: NetworkTranslator.Tr("PrivateKey") }
            LabeledField { id: tlsPrivKeyPw; Layout.fillWidth: true; label: NetworkTranslator.Tr("PrivateKeyPassword"); isPassword: true }
          }

          // LEAP / PWD section
          ColumnLayout {
            visible: eapMethodDrop.currentIndex === 1
              || eapMethodDrop.currentIndex === 2
            Layout.fillWidth: true
            spacing: 8

            LabeledField { id: leapUsername; Layout.fillWidth: true; label: NetworkTranslator.Tr("Username") }
            LabeledField { id: leapPassword; Layout.fillWidth: true; label: NetworkTranslator.Tr("Password"); isPassword: true }
          }

          // FAST section
          ColumnLayout {
            visible: eapMethodDrop.currentIndex === 3
            Layout.fillWidth: true
            spacing: 8

            LabeledField { id: fastAnonId;   Layout.fillWidth: true; label: NetworkTranslator.Tr("AnonymousIdentity") }

            Text {
              text: NetworkTranslator.Tr("ProvisionPAC")
              font.pixelSize: 12
              font.weight: Font.Medium
              color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Md3Dropdown {
              id: fastProvPac
              Layout.fillWidth: true
              model: [NetworkTranslator.Tr("Disabled"), NetworkTranslator.Tr("Anonymous"), NetworkTranslator.Tr("Authenticated")]
              currentIndex: 0
            }

            LabeledField {
              id: fastPacFile
              Layout.fillWidth: true
              label: NetworkTranslator.Tr("PACFile")
              hasFilePicker: true
            }

            Text {
              text: NetworkTranslator.Tr("InnerAuthentication")
              font.pixelSize: 12
              font.weight: Font.Medium
              color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Md3Dropdown {
              id: fastInnerAuth
              Layout.fillWidth: true
              model: ["GTC", "MSCHAPv2"]
              currentIndex: 0
            }

            LabeledField { id: fastUsername; Layout.fillWidth: true; label: NetworkTranslator.Tr("Username") }
            LabeledField { id: fastPassword; Layout.fillWidth: true; label: NetworkTranslator.Tr("Password"); isPassword: true }
          }

          // TTLS section
          ColumnLayout {
            visible: eapMethodDrop.currentIndex === 4
            Layout.fillWidth: true
            spacing: 8

            LabeledField { id: ttlsAnonId; Layout.fillWidth: true; label: NetworkTranslator.Tr("AnonymousIdentity") }
            LabeledField { id: ttlsDomain; Layout.fillWidth: true; label: NetworkTranslator.Tr("Domain") }
            LabeledField { id: ttlsCaCert; Layout.fillWidth: true; label: NetworkTranslator.Tr("CACertificate"); hasFilePicker: true }

            Text {
              text: NetworkTranslator.Tr("InnerAuthentication")
              font.pixelSize: 12
              font.weight: Font.Medium
              color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Md3Dropdown {
              id: ttlsInnerAuth
              Layout.fillWidth: true
              model: ["PAP", "MSCHAP", "MSCHAPv2", "CHAP"]
              currentIndex: 0
            }

            LabeledField { id: ttlsUsername; Layout.fillWidth: true; label: NetworkTranslator.Tr("Username") }
            LabeledField { id: ttlsPassword; Layout.fillWidth: true; label: NetworkTranslator.Tr("Password"); isPassword: true }
          }

          // PEAP section
          ColumnLayout {
            visible: eapMethodDrop.currentIndex === 5
            Layout.fillWidth: true
            spacing: 8

            LabeledField { id: peapAnonId; Layout.fillWidth: true; label: "Anonymous identity" }
            LabeledField { id: peapDomain; Layout.fillWidth: true; label: "Domain" }
            LabeledField { id: peapCaCert; Layout.fillWidth: true; label: "CA certificate"; hasFilePicker: true }

            Text {
              text: NetworkTranslator.Tr("PEAPVersion")
              font.pixelSize: 12
              font.weight: Font.Medium
              color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Md3Dropdown {
              id: peapVersion
              Layout.fillWidth: true
              model: [NetworkTranslator.Tr("Auto"), "0", "1"]
              currentIndex: 0
            }

            Text {
              text: NetworkTranslator.Tr("InnerAuthentication")
              font.pixelSize: 12
              font.weight: Font.Medium
              color: Theme.surface_variant_fg  // qmllint disable unqualified
            }
            Md3Dropdown {
              id: peapInnerAuth
              Layout.fillWidth: true
              model: ["MSCHAPv2", "MD5", "GTC"]
              currentIndex: 0
            }

            LabeledField { id: peapUsername; Layout.fillWidth: true; label: NetworkTranslator.Tr("Username") }
            LabeledField { id: peapPassword; Layout.fillWidth: true; label: NetworkTranslator.Tr("Password"); isPassword: true }
          }
        }

        // Button Layout
        RowLayout {
          Layout.fillWidth: true
          spacing: 8

          Item { Layout.fillWidth: true }

          NetActionButton {
            buttonText: NetworkTranslator.Tr("Cancel")
            onActionClicked: dismissPasswordDialog()
          }

          NetActionButton {
            buttonText: NetworkTranslator.Tr("Connect")
            onActionClicked: connectWithCredentials()
          }
        }
      }
    }
  }

  function showPasswordDialog(ssid, apPath, securityCategory) {
    root.pendingApSsid = ssid
    root.pendingApPath = apPath
    root.pendingSecurityType = securityCategory
    passwordOverlay.visible = true
  }

  function dismissPasswordDialog() {
    passwordOverlay.visible = false
    wpaPwField.text = ""
    wepPwField.text = ""
  }

  function connectWithCredentials() {
    var ap = root.pendingApPath
    var sec = root.pendingSecurityType

    if (sec === "wpa_personal") {
      if (wpaPwField.text.length === 0) return
      NetworkHandler.connectWlan(ap, wpaPwField.text)  // qmllint disable unqualified
    } else if (sec === "wep") {
      var wepAuthAlg = wepAuthDrop.currentIndex === 0 ? "open" : "shared"
      NetworkHandler.connectWlanAdvanced(ap, {  // qmllint disable unqualified
        "type": "wep",
        "password": wepPwField.text,
        "wep_key_idx": wepIndexDrop.currentIndex,
        "auth_alg": wepAuthAlg
      })
    } else if (sec === "enterprise") {
      var methodNames = ["tls", "leap", "pwd", "fast", "ttls", "peap"]
      var method = methodNames[eapMethodDrop.currentIndex]
      var creds = {"type": "enterprise", "eap_method": method}

      if (method === "tls") {
        creds["identity"] = tlsIdentity.text
        creds["domain"] = tlsDomain.text
        creds["user_cert"] = tlsUserCert.text
        creds["ca_cert"] = tlsCaCert.text
        creds["subject_match"] = tlsSubject.text
        creds["alt_subject_match"] = tlsAltSubject.text
        creds["server_name"] = tlsServers.text
        creds["private_key"] = tlsPrivKey.text
        creds["private_key_password"] = tlsPrivKeyPw.text
      } else if (method === "leap" || method === "pwd") {
        creds["username"] = leapUsername.text
        creds["password"] = leapPassword.text
      } else if (method === "fast") {
        var pacProv = ["0", "1", "2"]
        var fastInner = fastInnerAuth.currentIndex === 0 ? "gtc" : "mschapv2"
        creds["anon_identity"] = fastAnonId.text
        creds["pac_provisioning"] = pacProv[fastProvPac.currentIndex]
        creds["pac_file"] = fastPacFile.text
        creds["inner_auth"] = fastInner
        creds["username"] = fastUsername.text
        creds["password"] = fastPassword.text
      } else if (method === "ttls") {
        var ttlsInner = ["pap", "mschap", "mschapv2", "chap"]
        creds["anon_identity"] = ttlsAnonId.text
        creds["domain"] = ttlsDomain.text
        creds["ca_cert"] = ttlsCaCert.text
        creds["inner_auth"] = ttlsInner[ttlsInnerAuth.currentIndex]
        creds["username"] = ttlsUsername.text
        creds["password"] = ttlsPassword.text
      } else {
        // PEAP
        var peapVer = ["auto", "0", "1"]
        var peapInner = ["mschapv2", "md5", "gtc"]
        creds["anon_identity"] = peapAnonId.text
        creds["domain"] = peapDomain.text
        creds["ca_cert"] = peapCaCert.text
        creds["peap_version"] = peapVer[peapVersion.currentIndex]
        creds["inner_auth"] = peapInner[peapInnerAuth.currentIndex]
        creds["username"] = peapUsername.text
        creds["password"] = peapPassword.text
      }

      NetworkHandler.connectWlanAdvanced(ap, creds)  // qmllint disable unqualified
    }

    dismissPasswordDialog()
  }
}
