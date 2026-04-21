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
import "config_panel"
import "krunner_btn"
import "wlan_indicator"
import "system_tray"
import "volume_control"
import "bluetooth"
import "network_control"

Window {
  id: root
  width: Screen.width
  height: Screen.height
  color: "transparent"

  readonly property int barHeight: 32
  readonly property int drawerTopMargin: 8
  readonly property int drawerW: 960
  readonly property int drawerH: 648
  readonly property int calendarFlyoutW: 320
  readonly property int volumeFlyoutW: 320

  // Active flyout key: "" | "drawer" | "calendar" | "trayOverflow" | "volume"
  //                      | "bluetooth" | "network" | "battery"
  // Only one flyout may be open at a time; set this property to switch or close.
  property string activeFlyout: ""

  property bool drawerExpanded: false
  property bool calendarFlyoutExpanded: false
  property bool trayOverflowExpanded: false
  property bool volumeFlyoutExpanded: false
  property bool bluetoothFlyoutExpanded: false
  property bool networkFlyoutExpanded: false
  property bool batteryFlyoutExpanded: false

  // Restrict input to the bar on startup; the window is always drawerH taller
  // than the visible bar, so without this the transparent area eats clicks.
  Component.onCompleted: LayerShellHelper.setBarOnlyMask(root, barHeight)

  onActiveFlyoutChanged: {
    // Close any flyout that is no longer active (starts its close animation)
    if (activeFlyout !== "drawer"       && drawerExpanded)          appDrawer.close()
    if (activeFlyout !== "calendar"     && calendarFlyoutExpanded)  calendarFlyout.close()
    if (activeFlyout !== "trayOverflow" && trayOverflowExpanded)    trayOverflowFlyout.close()
    if (activeFlyout !== "volume"       && volumeFlyoutExpanded)    volumeFlyout.close()
    if (activeFlyout !== "bluetooth"    && bluetoothFlyoutExpanded) bluetoothFlyout.close()
    if (activeFlyout !== "network"      && networkFlyoutExpanded)   networkFlyout.close()
    if (activeFlyout !== "battery"      && batteryFlyoutExpanded)   batteryFlyout.close()

    switch (activeFlyout) {
      case "":
        // Revert mask immediately — do not wait for close animations
        LayerShellHelper.setBarOnlyMask(root, barHeight)
        break
      case "drawer":
        drawerExpanded = true
        LayerShellHelper.setFullMask(root)
        appDrawer.open()
        break
      case "calendar":
        calendarFlyoutExpanded = true
        LayerShellHelper.setFullMask(root)
        calendarFlyout.open()
        break
      case "trayOverflow":
        trayOverflowExpanded = true
        LayerShellHelper.setFullMask(root)
        trayOverflowFlyout.open()
        break
      case "volume":
        volumeFlyoutExpanded = true
        LayerShellHelper.setFullMask(root)
        volumeFlyout.open()
        break
      case "bluetooth":
        bluetoothFlyoutExpanded = true
        LayerShellHelper.setFullMask(root)
        bluetoothFlyout.open()
        break
      case "network":
        networkFlyoutExpanded = true
        LayerShellHelper.setFullMask(root)
        networkFlyout.open()
        break
      case "battery":
        batteryFlyoutExpanded = true
        LayerShellHelper.setFullMask(root)
        batteryFlyout.open()
        break
    }
  }

  // Background click-catcher
  MouseArea {
    anchors.fill: parent
    visible: root.activeFlyout !== "" && !confirmDialog.visible
    onClicked: root.activeFlyout = ""
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
          text: confirmDialog.pendingLabel !== "" ? StatusBarTranslator.Tr(confirmDialog.pendingLabel) : "" // qmllint disable unqualified
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

      // Config menu
      Menu {
        id: barContextMenu

        onAboutToShow: LayerShellHelper.setFullMask(root)
        onClosed: {
          if (root.activeFlyout === "")
            LayerShellHelper.setBarOnlyMask(root, root.barHeight)
        }

        MenuItem {
          text: StatusBarTranslator.Tr("ConfigurePanel")
          onTriggered: configPanelWindow.open()
        }

        MenuItem {
          text: StatusBarTranslator.Tr("ExitPanel")
          onTriggered: Qt.quit()
        }
      }

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
            id: appDrawerBtn
            onClicked: root.activeFlyout = root.activeFlyout === "drawer" ? "" : "drawer"
          }

          // KRunner button
          KRunnerBtn {
            onClicked: KRunnerToggler.toggleKRunner() // qmllint disable unqualified
          }
        }

        Item {
          Layout.fillWidth: true
          Layout.fillHeight: true
          MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: barContextMenu.popup()
          }
        }

        // Middle slot (reserved for future use)
        RowLayout {
          spacing: 4
          Layout.alignment: Qt.AlignVCenter
        }

        Item {
          Layout.fillWidth: true
          Layout.fillHeight: true
          MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: barContextMenu.popup()
          }
        }

        RowLayout {
          id: rightBarGroup
          spacing: 4
          Layout.alignment: Qt.AlignVCenter

          // System tray
          SystemTray {
            id: systemTray
            onOverflowClicked: root.activeFlyout = root.activeFlyout === "trayOverflow" ? "" : "trayOverflow"
          }

          // Bluetooth indicator
          BluetoothIndicator {
            id: bluetoothIndicatorBtn
            onClicked: root.activeFlyout = root.activeFlyout === "bluetooth" ? "" : "bluetooth"
          }

          // Volume indicator
          VolumeIndicator {
            id: volumeIndicatorBtn
            onClicked: root.activeFlyout = root.activeFlyout === "volume" ? "" : "volume"
          }

          // WLAN / network indicator
          WLANIndicator {
            id: networkIndicatorBtn
            onClicked: root.activeFlyout = root.activeFlyout === "network" ? "" : "network"
          }

          // Battery indicator
          BatteryIndicator {
            id: batteryIndicatorBtn
            onClicked: root.activeFlyout = root.activeFlyout === "battery" ? "" : "battery"
          }

          // Clock button
          ClockBtn {
            id: clockBtn
            onClicked: root.activeFlyout = root.activeFlyout === "calendar" ? "" : "calendar"
          }
        }
      }
    }
  }

  // App drawer
  Item {
    x: {
      var _track = appDrawerBtn.x + appDrawerBtn.width // qmllint disable missing-property
      var btnCenterX = appDrawerBtn.mapToItem(baseLayer, appDrawerBtn.width / 2, 0).x // qmllint disable missing-property
      return Math.max(8, Math.min(root.width - root.drawerW - 8,
                                  btnCenterX - root.drawerW / 2))
    }
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

  // Calendar flyout
  Item {
    x: {
      var _track = rightBarGroup.x + clockBtn.x + clockBtn.width // qmllint disable missing-property
      var btnCenterX = clockBtn.mapToItem(baseLayer, clockBtn.width / 2, 0).x // qmllint disable missing-property
      return Math.max(8, Math.min(root.width - root.calendarFlyoutW - 8,
                                  btnCenterX - root.calendarFlyoutW / 2))
    }
    y: root.barHeight + root.drawerTopMargin
    width: root.calendarFlyoutW
    height: calendarFlyout.implicitHeight
    visible: root.calendarFlyoutExpanded

    CalendarFlyout {
      id: calendarFlyout
      anchors.fill: parent
      onCloseAnimationFinished: root.calendarFlyoutExpanded = false
    }
  }

  // System tray overflow flyout
  Item {
    x: {
      var _track = rightBarGroup.x + systemTray.x + systemTray.overflowBtnCenterX // qmllint disable missing-property
      var btnCenterX = systemTray.mapToItem(baseLayer, systemTray.overflowBtnCenterX, 0).x // qmllint disable missing-property
      var fw = trayOverflowFlyout.implicitWidth // qmllint disable missing-property
      return Math.max(8, Math.min(root.width - fw - 8,
                                  btnCenterX - fw / 2))
    }
    y: root.barHeight + root.drawerTopMargin
    width: trayOverflowFlyout.implicitWidth
    height: trayOverflowFlyout.implicitHeight
    visible: root.trayOverflowExpanded

    SystemTrayOverflowFlyout {
      id: trayOverflowFlyout
      anchors.fill: parent
      onCloseAnimationFinished: root.trayOverflowExpanded = false
    }
  }

  // Volume flyout
  Item {
    x: {
      var _track = rightBarGroup.x + volumeIndicatorBtn.x + volumeIndicatorBtn.width // qmllint disable missing-property
      var btnCenterX = volumeIndicatorBtn.mapToItem(baseLayer, volumeIndicatorBtn.width / 2, 0).x // qmllint disable missing-property
      return Math.max(8, Math.min(root.width - root.volumeFlyoutW - 8,
                                  btnCenterX - root.volumeFlyoutW / 2))
    }
    y: root.barHeight + root.drawerTopMargin
    width: root.volumeFlyoutW
    height: volumeFlyout.implicitHeight
    visible: root.volumeFlyoutExpanded

    VolumeFlyout {
      id: volumeFlyout
      anchors.fill: parent
      onCloseAnimationFinished: root.volumeFlyoutExpanded = false
    }
  }

  // Bluetooth flyout
  Item {
    readonly property int flyoutW: 320
    x: {
      var _track = rightBarGroup.x + bluetoothIndicatorBtn.x + bluetoothIndicatorBtn.width // qmllint disable missing-property
      var btnCenterX = bluetoothIndicatorBtn.mapToItem( // qmllint disable missing-property
        baseLayer, bluetoothIndicatorBtn.width / 2, 0).x // qmllint disable missing-property
      return Math.max(8, Math.min(root.width - flyoutW - 8,
                                  btnCenterX - flyoutW / 2))
    }
    y: root.barHeight + root.drawerTopMargin
    width: flyoutW
    height: bluetoothFlyout.implicitHeight
    visible: root.bluetoothFlyoutExpanded

    BluetoothFlyout {
      id: bluetoothFlyout
      anchors.fill: parent
      onCloseAnimationFinished: root.bluetoothFlyoutExpanded = false
    }
  }

  // Network flyout
  Item {
    readonly property int flyoutW: 320
    x: {
      var _track = rightBarGroup.x + networkIndicatorBtn.x + networkIndicatorBtn.width // qmllint disable missing-property
      var btnCenterX = networkIndicatorBtn.mapToItem( // qmllint disable missing-property
        baseLayer, networkIndicatorBtn.width / 2, 0).x // qmllint disable missing-property
      return Math.max(8, Math.min(root.width - flyoutW - 8,
                                  btnCenterX - flyoutW / 2))
    }
    y: root.barHeight + root.drawerTopMargin
    width: flyoutW
    height: networkFlyout.implicitHeight
    visible: root.networkFlyoutExpanded

    NetworkFlyout {
      id: networkFlyout
      anchors.fill: parent
      onCloseAnimationFinished: root.networkFlyoutExpanded = false
    }
  }

  // Battery flyout
  Item {
    readonly property int flyoutW: 320
    x: {
      var _track = rightBarGroup.x + batteryIndicatorBtn.x + batteryIndicatorBtn.width // qmllint disable missing-property
      var btnCenterX = batteryIndicatorBtn.mapToItem( // qmllint disable missing-property
        baseLayer, batteryIndicatorBtn.width / 2, 0).x // qmllint disable missing-property
      return Math.max(8, Math.min(root.width - flyoutW - 8,
                                  btnCenterX - flyoutW / 2))
    }
    y: root.barHeight + root.drawerTopMargin
    width: flyoutW
    height: batteryFlyout.implicitHeight
    visible: root.batteryFlyoutExpanded

    BatteryFlyout {
      id: batteryFlyout
      anchors.fill: parent
      onCloseAnimationFinished: root.batteryFlyoutExpanded = false
    }
  }

  // Config panel (standalone frameless window)
  ConfigPanelWindow {
    id: configPanelWindow
  }

}
