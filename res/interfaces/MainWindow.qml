/*
 * MainWindow.qml
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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import "app_drawer"
import "app_indicator"
import "battery_indicator"
import "clock_btn"
import "krunner_btn"

Window {
  id: root
  width: Screen.width
  height: Screen.height
  color: "transparent"

  readonly property int barHeight: 32
  readonly property int drawerTopMargin: 8
  readonly property int drawerW: 960
  readonly property int drawerH: 648

  property bool appDrawerOpen: false
  property bool drawerExpanded: false

  // Restrict input to the bar on startup; the window is always drawerH taller
  // than the visible bar, so without this the transparent area eats clicks.
  Component.onCompleted: LayerShellHelper.setBarOnlyMask(root, barHeight)

  onAppDrawerOpenChanged: {
    if (appDrawerOpen) {
      drawerExpanded = true
      LayerShellHelper.setFullMask(root)
      appDrawer.open()
    } else {
      appDrawer.close()
      // drawerExpanded → false is set by onCloseAnimationFinished below
    }
  }

  onDrawerExpandedChanged: {
    if (!drawerExpanded)
      LayerShellHelper.setBarOnlyMask(root, barHeight)
  }

  // Background click-catcher
  MouseArea {
    anchors.fill: parent
    visible: root.appDrawerOpen && !confirmDialog.visible
    onClicked: root.appDrawerOpen = false
  }

  // Confirm dialog (standalone layer-shell overlay window)
  Window {
    id: confirmDialog
    width: Screen.width
    height: Screen.height
    color: "transparent"
    flags: Qt.FramelessWindowHint
    visible: false

    property string pendingAction: ""
    property string pendingLabel: ""

    Component.onCompleted: LayerShellHelper.setupOverlayWindow(confirmDialog)

    onVisibleChanged: {
      if (visible) LayerShellHelper.setFullMask(confirmDialog)
    }

    // Scrim — dims background and eats stray clicks
    Rectangle {
      anchors.fill: parent
      color: "#52000000"
      MouseArea { anchors.fill: parent }
    }

    // Dialog card
    Rectangle {
      anchors.centerIn: parent
      width: 312
      height: dialogLayout.implicitHeight
      radius: 28
      color: Theme.surface_container_low

      ColumnLayout {
        id: dialogLayout
        width: parent.width
        spacing: 0

        Text {
          Layout.fillWidth: true
          Layout.topMargin: 24
          Layout.leftMargin: 24
          Layout.rightMargin: 24
          text: StatusBarTranslator.Tr("Action_" + confirmDialog.pendingLabel)
          font.pixelSize: 24
          color: Theme.surface_fg
          wrapMode: Text.WordWrap
        }

        Text {
          Layout.fillWidth: true
          Layout.topMargin: 16
          Layout.leftMargin: 24
          Layout.rightMargin: 24
          text: StatusBarTranslator.Tr("ActionCannotBeUndone")
          font.pixelSize: 14
          color: Theme.surface_variant_fg
          wrapMode: Text.WordWrap
        }

        RowLayout {
          Layout.fillWidth: true
          Layout.topMargin: 24
          Layout.rightMargin: 24
          Layout.bottomMargin: 24
          spacing: 8

          Item { Layout.fillWidth: true }

          ToolButton {
            id: cancelBtn
            implicitHeight: 40
            leftPadding: 12; rightPadding: 12
            onClicked: confirmDialog.visible = false
            onPressed: {
              cancelRipple.centerX = pressX
              cancelRipple.centerY = pressY
              cancelRippleAnim.restart()
            }
            contentItem: Text {
              text: StatusBarTranslator.Tr("Cancel")
              font.pixelSize: 14; font.letterSpacing: 0.1
              color: Theme.primary
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
              radius: 20
              color: "transparent"

              Rectangle {
                anchors.fill: parent
                radius: parent.radius
                color: cancelBtn.hovered && !cancelBtn.pressed
                       ? Theme.state_layer_hover : "transparent"
              }

              Canvas {
                id: cancelRipple
                anchors.fill: parent
                opacity: 0

                property real centerX: width / 2
                property real centerY: height / 2
                property real rippleRadius: 0

                onPaint: {
                  var ctx = getContext("2d")
                  ctx.clearRect(0, 0, width, height)
                  ctx.save()
                  var r = 20
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
                  var c = Theme.state_layer_pressed
                  ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
                  ctx.beginPath()
                  ctx.arc(centerX, centerY, rippleRadius, 0, 2 * Math.PI)
                  ctx.fill()
                  ctx.restore()
                }

                onOpacityChanged: requestPaint()
                onRippleRadiusChanged: requestPaint()

                ParallelAnimation {
                  id: cancelRippleAnim
                  NumberAnimation {
                    target: cancelRipple; property: "rippleRadius"
                    from: 0
                    to: Math.sqrt(cancelRipple.width * cancelRipple.width
                                  + cancelRipple.height * cancelRipple.height)
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: cancelRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: cancelRipple; property: "opacity"
                      from: 0.3; to: 0
                      duration: 250; easing.type: Easing.OutCubic
                    }
                  }
                }
              }
            }
          }

          ToolButton {
            id: confirmBtn
            implicitHeight: 40
            leftPadding: 12; rightPadding: 12
            onClicked: {
              confirmDialog.visible = false
              var a = confirmDialog.pendingAction
              if      (a === "suspend")    PowerProvider.suspend()
              else if (a === "hibernate")  PowerProvider.hibernate()
              else if (a === "reboot")     PowerProvider.reboot()
              else if (a === "powerOff")   PowerProvider.powerOff()
              else if (a === "lock")       SessionProvider.lock()
              else if (a === "logout")     SessionProvider.logout()
              else if (a === "switchUser") SessionProvider.switchUser()
            }
            onPressed: {
              confirmRipple.centerX = pressX
              confirmRipple.centerY = pressY
              confirmRippleAnim.restart()
            }
            contentItem: Text {
              text: StatusBarTranslator.Tr("ConfirmAndExecute")
              font.pixelSize: 14; font.weight: Font.Medium; font.letterSpacing: 0.1
              color: Theme.primary
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
              radius: 20
              color: "transparent"

              Rectangle {
                anchors.fill: parent
                radius: parent.radius
                color: confirmBtn.hovered && !confirmBtn.pressed
                       ? Theme.state_layer_hover : "transparent"
              }

              Canvas {
                id: confirmRipple
                anchors.fill: parent
                opacity: 0

                property real centerX: width / 2
                property real centerY: height / 2
                property real rippleRadius: 0

                onPaint: {
                  var ctx = getContext("2d")
                  ctx.clearRect(0, 0, width, height)
                  ctx.save()
                  var r = 20
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
                  var c = Theme.state_layer_pressed
                  ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
                  ctx.beginPath()
                  ctx.arc(centerX, centerY, rippleRadius, 0, 2 * Math.PI)
                  ctx.fill()
                  ctx.restore()
                }

                onOpacityChanged: requestPaint()
                onRippleRadiusChanged: requestPaint()

                ParallelAnimation {
                  id: confirmRippleAnim
                  NumberAnimation {
                    target: confirmRipple; property: "rippleRadius"
                    from: 0
                    to: Math.sqrt(confirmRipple.width * confirmRipple.width
                                  + confirmRipple.height * confirmRipple.height)
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: confirmRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: confirmRipple; property: "opacity"
                      from: 0.3; to: 0
                      duration: 250; easing.type: Easing.OutCubic
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    Shortcut {
      sequence: "Escape"
      onActivated: confirmDialog.visible = false
    }
  }

  // Bar
  Rectangle {
    x: 0; y: 0
    width: parent.width
    height: root.barHeight
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
          Layout.fillHeight: true

          // App indicator
          AppIndicator {}

          // App drawer button
          AppsBtn {
            onClicked: root.appDrawerOpen = !root.appDrawerOpen
          }

          // KRunner button
          KRunnerBtn {
            onClicked: KRunnerToggler.toggleKRunner()
          }
        }

        Item { Layout.fillWidth: true }

        // Middle slot (reserved for future use)
        RowLayout {
          spacing: 4
          Layout.alignment: Qt.AlignVCenter
        }

        Item { Layout.fillWidth: true }

        RowLayout {
          spacing: 4
          Layout.alignment: Qt.AlignVCenter

          // Battery indicator
          BatteryIndicator {}

          // Clock button
          ClockBtn {
            onClicked: ClockProvider.toggleCalendar()
          }
        }
      }
    }
  }

  // App drawer
  Item {
    x: 8
    y: root.barHeight + root.drawerTopMargin
    width: root.drawerW
    height: root.drawerH
    visible: root.drawerExpanded

    AppDrawer {
      id: appDrawer
      anchors.fill: parent
      onCloseAnimationFinished: root.drawerExpanded = false
      onConfirmActionRequested: (action, label) => {
        confirmDialog.pendingAction = action
        confirmDialog.pendingLabel  = label
        confirmDialog.visible = true
      }
    }
  }

}
