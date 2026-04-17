/*
 * BluetoothFlyout.qml
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
  opacity: 0
  implicitWidth: 320
  implicitHeight: frame.implicitHeight

  readonly property int slideOffset: 24

  transform: Translate {
    id: slideTransform
    y: -root.slideOffset
  }

  signal closeAnimationFinished()

  function open() {
    slideTransform.y = -root.slideOffset
    root.opacity = 0
    openAnim.restart()
  }

  function close() {
    closeAnim.restart()
  }

  // Open animation
  ParallelAnimation {
    id: openAnim
    NumberAnimation {
      target: root; property: "opacity"
      from: 0; to: 1
      duration: 220; easing.type: Easing.OutCubic
    }
    NumberAnimation {
      target: slideTransform; property: "y"
      from: -root.slideOffset; to: 0
      duration: 220; easing.type: Easing.OutCubic
    }
  }

  // Close animation
  ParallelAnimation {
    id: closeAnim
    NumberAnimation {
      target: root; property: "opacity"
      from: 1; to: 0
      duration: 160; easing.type: Easing.InCubic
    }
    NumberAnimation {
      target: slideTransform; property: "y"
      from: 0; to: -root.slideOffset
      duration: 160; easing.type: Easing.InCubic
    }
    onStopped: root.closeAnimationFinished()
  }

  // Main frame
  Rectangle {
    id: frame
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    implicitHeight: mainColumn.implicitHeight
    radius: 12
    color: Theme.surface_bg // qmllint disable unqualified
    border.color: Theme.primary // qmllint disable unqualified
    border.width: 2

    MouseArea { anchors.fill: parent }

    ColumnLayout {
      id: mainColumn
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: parent.top
      spacing: 0

      // Headers
      Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: headerRow.implicitHeight + 24
        Layout.topMargin: 3
        Layout.leftMargin: 3
        Layout.rightMargin: 3
        color: Theme.surface_container // qmllint disable unqualified
        topLeftRadius: 10
        topRightRadius: 10

        RowLayout {
          id: headerRow
          anchors {
            left: parent.left; right: parent.right
            top: parent.top
            leftMargin: 16; rightMargin: 16; topMargin: 12
          }
          spacing: 8

          // Title
          Text {
            text: BluetoothTranslator.Tr("Bluetooth")
            color: Theme.primary // qmllint disable unqualified
            font.pixelSize: 16
            font.weight: Font.Medium
            Layout.fillWidth: true
          }

          // Power switch
          Item {
            id: btSwitch
            implicitWidth: 52
            implicitHeight: 32
            Layout.alignment: Qt.AlignVCenter

            readonly property bool checked:
              BluetoothHandler.bluetoothEnabled // qmllint disable unqualified

            property bool isHovered: false
            property bool isPressed: false

            readonly property real handleSize:
              isPressed ? 28 : (checked ? 24 : 16)
            readonly property real handleCenterX: checked ? 36 : 16
            readonly property color handleColor:
              checked
                ? ((isHovered || isPressed) ? Theme.primary_container   // qmllint disable unqualified
                                            : Theme.surface_bg)         // qmllint disable unqualified
                : Theme.surface_variant_fg                              // qmllint disable unqualified
            readonly property color stateLayerColor: {
              var c = checked ? Theme.primary : Theme.surface_fg // qmllint disable unqualified
              if (isPressed) return Qt.rgba(c.r, c.g, c.b, 0.10)
              if (isHovered) return Qt.rgba(c.r, c.g, c.b, 0.08)
              return "transparent"
            }

            Rectangle {
              anchors.fill: parent
              radius: 16
              color: btSwitch.checked
                ? Theme.primary                 // qmllint disable unqualified
                : Theme.surface_container_high  // qmllint disable unqualified
              border.width: btSwitch.checked ? 0 : 2
              border.color: Theme.surface_variant_fg // qmllint disable unqualified
              Behavior on color        { ColorAnimation { duration: 200; easing.type: Easing.OutCubic } }
              Behavior on border.width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
            }

            Rectangle {
              width: 40; height: 40; radius: 20
              anchors.verticalCenter: parent.verticalCenter
              x: btSwitch.handleCenterX - 20
              color: btSwitch.stateLayerColor
              Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
              Behavior on color { ColorAnimation  { duration: 150; easing.type: Easing.OutCubic } }
            }

            Rectangle {
              width: btSwitch.handleSize
              height: width
              radius: width / 2
              anchors.verticalCenter: parent.verticalCenter
              x: btSwitch.handleCenterX - btSwitch.handleSize / 2
              color: btSwitch.handleColor
              Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
              Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
              Behavior on color { ColorAnimation  { duration: 200; easing.type: Easing.OutCubic } }
            }

            MouseArea {
              anchors.fill: parent
              hoverEnabled: true
              onEntered:  btSwitch.isHovered = true
              onExited:   { btSwitch.isHovered = false; btSwitch.isPressed = false }
              onPressed:  btSwitch.isPressed  = true
              onReleased: btSwitch.isPressed  = false
              onClicked:  BluetoothHandler.setEnabled(!btSwitch.checked) // qmllint disable unqualified
            }
          }

          // Pairing button
          Item {
            id: pairBtn
            implicitWidth: 36
            implicitHeight: 36
            Layout.alignment: Qt.AlignVCenter

            property bool isHovered: false
            property bool isPressed: false

            Rectangle {
              anchors.fill: parent
              radius: 8
              color: pairBtn.isPressed
                ? Theme.primary_container // qmllint disable unqualified
                : pairBtn.isHovered
                  ? Qt.rgba(Theme.primary.r, Theme.primary.g, // qmllint disable unqualified
                            Theme.primary.b, 0.08)            // qmllint disable unqualified
                  : "transparent"
              Behavior on color { ColorAnimation { duration: 150 } }
            }

            Canvas {
              id: pairBtnRipple
              anchors.fill: parent
              opacity: 0

              property real centerX: width / 2
              property real centerY: height / 2
              property real rippleRadius: 0

              onPaint: {
                var w = pairBtnRipple.width  // qmllint disable missing-property
                var h = pairBtnRipple.height // qmllint disable missing-property
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, w, h)
                ctx.save()
                var r = 8
                ctx.beginPath()
                ctx.moveTo(r, 0)
                ctx.lineTo(w - r, 0)
                ctx.arcTo(w, 0, w, r, r)
                ctx.lineTo(w, h - r)
                ctx.arcTo(w, h, w - r, h, r)
                ctx.lineTo(r, h)
                ctx.arcTo(0, h, 0, h - r, r)
                ctx.lineTo(0, r)
                ctx.arcTo(0, 0, r, 0, r)
                ctx.closePath()
                ctx.clip()
                var c = Theme.primary // qmllint disable unqualified
                ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
                ctx.beginPath()
                ctx.arc(centerX, centerY, rippleRadius, 0, 2 * Math.PI)
                ctx.fill()
                ctx.restore()
              }

              onOpacityChanged: requestPaint()
              onRippleRadiusChanged: requestPaint()

              ParallelAnimation {
                id: pairBtnRippleAnim
                NumberAnimation {
                  target: pairBtnRipple
                  property: "rippleRadius" // qmllint disable missing-property
                  from: 0
                  to: Math.sqrt( // qmllint disable missing-property
                    pairBtnRipple.width  * pairBtnRipple.width +
                    pairBtnRipple.height * pairBtnRipple.height)
                  duration: 400; easing.type: Easing.OutCubic
                }
                SequentialAnimation {
                  PropertyAction {
                    target: pairBtnRipple
                    property: "opacity" // qmllint disable missing-property
                    value: 0.3          // qmllint disable missing-property
                  }
                  PauseAnimation { duration: 150 }
                  NumberAnimation {
                    target: pairBtnRipple
                    property: "opacity" // qmllint disable missing-property
                    from: 0.3; to: 0
                    duration: 250; easing.type: Easing.OutCubic
                  }
                }
              }
            }

            Text {
              anchors.centerIn: parent
              text: "add"
              color: Theme.primary // qmllint disable unqualified
              font.pixelSize: 20
              font.family: "Material Symbols Rounded"
            }

            MouseArea {
              anchors.fill: parent
              hoverEnabled: true
              onEntered:  pairBtn.isHovered = true
              onExited:   { pairBtn.isHovered = false; pairBtn.isPressed = false }
              onPressed: (mouse) => {
                pairBtn.isPressed = true
                pairBtnRipple.centerX = mouse.x
                pairBtnRipple.centerY = mouse.y
                pairBtnRippleAnim.restart()
              }
              onReleased: pairBtn.isPressed = false
              onClicked:  BluetoothHandler.openPairingSettings() // qmllint disable unqualified
            }
          }
        }
      }

      // Connection area
      ColumnLayout {
        Layout.fillWidth: true
        Layout.topMargin: 2
        Layout.leftMargin: 2
        Layout.rightMargin: 2
        Layout.bottomMargin: 2
        spacing: 0

        // Bluetooth off msg
        Item {
          Layout.fillWidth: true
          Layout.preferredHeight: 64
          visible: !BluetoothHandler.bluetoothEnabled // qmllint disable unqualified

          Text {
            anchors.centerIn: parent
            text: BluetoothTranslator.Tr("BluetoothOff")
            font.pixelSize: 13
            color: Theme.surface_variant_fg // qmllint disable unqualified
            opacity: 0.6
          }
        }

        // Device list
        Column {
          id: deviceList
          Layout.fillWidth: true
          Layout.topMargin: 8
          Layout.leftMargin: 8
          Layout.rightMargin: 8
          Layout.bottomMargin: 12
          visible: BluetoothHandler.bluetoothEnabled // qmllint disable unqualified
          spacing: 6

          Repeater {
            model: BluetoothHandler.devices // qmllint disable unqualified

            delegate: Rectangle {
              id: deviceDelegate
              required property var modelData
              width: parent.width
              height: deviceRow.implicitHeight + 20
              radius: 12

              readonly property string devState: modelData["state"]
              readonly property bool isFailed:     devState === "failed"
              readonly property bool isConnecting: devState === "connecting"
              readonly property bool isConnected:  devState === "connected"

              color: isFailed
                ? Theme.error_container   // qmllint disable unqualified
                : isConnecting
                  ? Theme.secondary_container // qmllint disable unqualified
                  : Theme.surface_container   // qmllint disable unqualified

              Behavior on color { ColorAnimation { duration: 200 } }

              RowLayout {
                id: deviceRow
                anchors {
                  left: parent.left; right: parent.right
                  verticalCenter: parent.verticalCenter
                  leftMargin: 14; rightMargin: 14
                }
                spacing: 12

                // Device icon
                Text {
                  text: deviceDelegate.modelData["icon"]
                  font.pixelSize: 22
                  font.family: "Material Symbols Rounded"
                  color: deviceDelegate.isFailed
                    ? Theme.on_error_container       // qmllint disable unqualified
                    : deviceDelegate.isConnecting
                      ? Theme.secondary_container_fg // qmllint disable unqualified
                      : Theme.surface_variant_fg     // qmllint disable unqualified
                  verticalAlignment: Text.AlignVCenter
                  Behavior on color { ColorAnimation { duration: 200 } }
                }

                // Name w/ status
                ColumnLayout {
                  Layout.fillWidth: true
                  spacing: 2

                  Text {
                    Layout.fillWidth: true
                    text: deviceDelegate.modelData["name"]
                    font.pixelSize: 14
                    elide: Text.ElideRight
                    color: deviceDelegate.isFailed
                      ? Theme.on_error_container       // qmllint disable unqualified
                      : deviceDelegate.isConnecting
                        ? Theme.secondary_container_fg // qmllint disable unqualified
                        : Theme.surface_variant_fg     // qmllint disable unqualified
                    Behavior on color { ColorAnimation { duration: 200 } }
                  }

                  Text {
                    text: deviceDelegate.isConnected
                      ? BluetoothTranslator.Tr("Connected")
                      : deviceDelegate.isConnecting
                        ? BluetoothTranslator.Tr("Connecting")
                        : deviceDelegate.isFailed
                          ? BluetoothTranslator.Tr("ConnectionFailed")
                          : BluetoothTranslator.Tr("Paired")
                    font.pixelSize: 12
                    color: deviceDelegate.isFailed
                      ? Theme.error                  // qmllint disable unqualified
                      : deviceDelegate.isConnecting
                        ? Theme.secondary            // qmllint disable unqualified
                        : Theme.surface_variant_fg   // qmllint disable unqualified
                    Behavior on color { ColorAnimation { duration: 200 } }
                  }
                }

                // Action button
                Item {
                  id: actionBtn
                  implicitWidth: actionBtnLabel.implicitWidth + 16
                  implicitHeight: 32
                  Layout.alignment: Qt.AlignVCenter
                  visible: !deviceDelegate.isConnecting

                  property bool isHovered: false
                  property bool isPressed: false

                  readonly property color btnTextColor:
                    deviceDelegate.isFailed
                      ? Theme.on_error_container // qmllint disable unqualified
                      : Theme.primary            // qmllint disable unqualified

                  Rectangle {
                    anchors.fill: parent
                    radius: 8
                    color: "transparent"
                    border.color: actionBtn.btnTextColor
                    border.width: 1
                    opacity: actionBtn.isHovered || actionBtn.isPressed ? 0.6 : 1
                    Behavior on opacity { NumberAnimation { duration: 120 } }
                  }

                  Canvas {
                    id: actionBtnRipple
                    anchors.fill: parent
                    opacity: 0

                    property real centerX: width / 2
                    property real centerY: height / 2
                    property real rippleRadius: 0
                    property color rippleColor: actionBtn.btnTextColor

                    onPaint: {
                      var ctx = getContext("2d")
                      ctx.clearRect(0, 0, width, height)
                      ctx.save()
                      var r = 8
                      ctx.beginPath()
                      ctx.moveTo(r, 0)
                      ctx.lineTo(width - r, 0)
                      ctx.arcTo(width, 0, width, r, r)
                      ctx.lineTo(width, height - r)
                      ctx.arcTo(width, height, width - r, height, r)
                      ctx.lineTo(r, height)
                      ctx.arcTo(0, height, 0, height - r, r)
                      ctx.lineTo(0, r)
                      ctx.arcTo(0, 0, r, 0, r)
                      ctx.closePath()
                      ctx.clip()
                      var c = actionBtnRipple.rippleColor
                      ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
                      ctx.beginPath()
                      ctx.arc(centerX, centerY, rippleRadius, 0, 2 * Math.PI)
                      ctx.fill()
                      ctx.restore()
                    }

                    onOpacityChanged: requestPaint()
                    onRippleRadiusChanged: requestPaint()

                    ParallelAnimation {
                      id: actionBtnRippleAnim
                      NumberAnimation {
                        target: actionBtnRipple; property: "rippleRadius"
                        from: 0
                        to: Math.sqrt(
                          actionBtnRipple.width  * actionBtnRipple.width +
                          actionBtnRipple.height * actionBtnRipple.height)
                        duration: 400; easing.type: Easing.OutCubic
                      }
                      SequentialAnimation {
                        PropertyAction {
                          target: actionBtnRipple; property: "opacity"
                          value: 0.3
                        }
                        PauseAnimation { duration: 150 }
                        NumberAnimation {
                          target: actionBtnRipple; property: "opacity"
                          from: 0.3; to: 0
                          duration: 250; easing.type: Easing.OutCubic
                        }
                      }
                    }
                  }

                  Text {
                    id: actionBtnLabel
                    anchors.centerIn: parent
                    text: deviceDelegate.isConnected ? BluetoothTranslator.Tr("Disconnect")
                          : deviceDelegate.isFailed  ? BluetoothTranslator.Tr("Retry")
                          : BluetoothTranslator.Tr("Connect")
                    font.pixelSize: 13
                    color: actionBtn.btnTextColor
                    Behavior on color { ColorAnimation { duration: 200 } }
                  }

                  MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:  actionBtn.isHovered = true
                    onExited:   { actionBtn.isHovered = false; actionBtn.isPressed = false }
                    onPressed: (mouse) => {
                      actionBtn.isPressed = true
                      actionBtnRipple.centerX = mouse.x
                      actionBtnRipple.centerY = mouse.y
                      actionBtnRippleAnim.restart()
                    }
                    onReleased: actionBtn.isPressed = false
                    onClicked: {
                      if (deviceDelegate.isConnected) {
                        BluetoothHandler.disconnectDevice( // qmllint disable unqualified
                          deviceDelegate.modelData["address"])
                      } else {
                        BluetoothHandler.connectDevice( // qmllint disable unqualified
                          deviceDelegate.modelData["address"])
                      }
                    }
                  }
                }

                // COnnection msg
                Text {
                  visible: deviceDelegate.isConnecting
                  text: BluetoothTranslator.Tr("PleaseWait")
                  font.pixelSize: 13
                  color: Theme.secondary // qmllint disable unqualified
                }
              }
            }
          }

          // Empty state msg
          Item {
            width: parent.width
            height: 48
            visible: BluetoothHandler.devices.length === 0 // qmllint disable unqualified

            Text {
              anchors.centerIn: parent
              text: BluetoothTranslator.Tr("NoPairedDevices")
              font.pixelSize: 13
              color: Theme.surface_variant_fg // qmllint disable unqualified
              opacity: 0.6
            }
          }
        }
      }
    }
  }
}
