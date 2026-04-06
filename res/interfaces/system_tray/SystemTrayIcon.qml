/*
 * SystemTrayIcon.qml
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
import QtQuick.Layouts

// A single system tray icon button with hover state layer, ripple on press,
// and opacity blink when the icon is in NeedsAttention status.
Rectangle {
  id: root

  required property string service
  required property string tooltip
  required property bool   is_visible
  required property bool   is_config_visible
  required property bool   needs_attention
  required property int    icon_revision
  required property int    icon_key

  property bool inOverflow: false

  visible: is_visible && (inOverflow ? !is_config_visible : is_config_visible)

  implicitWidth: 32
  Layout.fillHeight: true
  radius: 6
  color: "transparent"

  readonly property color stateHover:   Theme.status_bar_state_layer_hover
  readonly property color statePressed: Theme.status_bar_state_layer_pressed

  // Hover / press state layer
  Rectangle {
    id: stateLayer
    anchors.fill: parent
    radius: parent.radius
    color: "transparent"
  }

  // Ripple
  Canvas {
    id: ripple
    anchors.fill: parent
    opacity: 0

    property real centerX: width / 2
    property real centerY: height / 2
    property real rippleRadius: 0

    onPaint: {
      var ctx = getContext("2d")
      ctx.clearRect(0, 0, width, height)
      ctx.save()
      var r = root.radius
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
      var c = root.statePressed
      ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
      ctx.beginPath()
      ctx.arc(centerX, centerY, rippleRadius, 0, 2 * Math.PI)
      ctx.fill()
      ctx.restore()
    }

    onOpacityChanged: requestPaint()
    onRippleRadiusChanged: requestPaint()

    ParallelAnimation {
      id: rippleAnim
      NumberAnimation {
        target: ripple; property: "rippleRadius"
        from: 0
        to: Math.sqrt(ripple.width * ripple.width + ripple.height * ripple.height)
        duration: 400; easing.type: Easing.OutCubic
      }
      SequentialAnimation {
        PropertyAction { target: ripple; property: "opacity"; value: 0.3 }
        PauseAnimation { duration: 150 }
        NumberAnimation {
          target: ripple; property: "opacity"
          from: 0.3; to: 0
          duration: 250; easing.type: Easing.OutCubic
        }
      }
    }
  }

  // Icon image
  Image {
    id: iconImage
    anchors.centerIn: parent
    width: 18
    height: 18
    source: (icon_key >= 0)
      ? "image://trayicon/" + icon_key + "/" + icon_revision
      : ""
    fillMode: Image.PreserveAspectFit
    smooth: true
    cache: false

    opacity: needs_attention ? blinkOpacity : 1.0

    // Blink animation: only active when in NeedsAttention status.
    property real blinkOpacity: 1.0

    SequentialAnimation {
      id: blinkAnim
      running: root.needs_attention
      loops: Animation.Infinite
      NumberAnimation {
        target: iconImage; property: "blinkOpacity"
        to: 0.3; duration: 600; easing.type: Easing.InOutSine
      }
      NumberAnimation {
        target: iconImage; property: "blinkOpacity"
        to: 1.0; duration: 600; easing.type: Easing.InOutSine
      }
      // Reset opacity when animation stops (attention cleared).
      onRunningChanged: {
        if (!running) iconImage.blinkOpacity = 1.0
      }
    }
  }

  // Input handling
  MouseArea {
    id: mouseArea
    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton

    onEntered: stateLayer.color = root.stateHover
    onExited:  stateLayer.color = "transparent"

    onPressed: (mouse) => {
      stateLayer.color = root.statePressed
      ripple.centerX = mouse.x
      ripple.centerY = mouse.y
      rippleAnim.restart()
    }

    onReleased: stateLayer.color = containsMouse ? root.stateHover : "transparent"

    onClicked: (mouse) => {
      var g = mapToGlobal(mouse.x, mouse.y)
      if (mouse.button === Qt.LeftButton) {
        TrayHandler.activate(root.service, g.x, g.y)
      } else if (mouse.button === Qt.MiddleButton) {
        TrayHandler.secondaryActivate(root.service, g.x, g.y)
      } else if (mouse.button === Qt.RightButton) {
        TrayHandler.contextMenu(root.service, g.x, g.y)
      }
    }

    onWheel: (wheel) => {
      TrayHandler.scroll(root.service, wheel.angleDelta.y)
    }
  }
}
