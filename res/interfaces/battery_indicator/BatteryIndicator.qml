/*
 * BatteryIndicator.qml
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

Rectangle {
  id: root

  signal clicked

  implicitWidth: contentLayout.implicitWidth + 16
  Layout.fillHeight: true
  radius: 6
  color: "transparent"

  readonly property color stateHover: Theme.status_bar_state_layer_hover
  readonly property color statePressed: Theme.status_bar_state_layer_pressed

  Rectangle {
    id: stateLayer
    anchors.fill: parent
    radius: parent.radius
    color: "transparent"
  }

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

  RowLayout {
    id: contentLayout
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: 8
    spacing: 0

    Text {
      text: BatteryProvider.battery_icon
      color: Theme.status_bar_surface_fg
      font.pixelSize: 24
      font.family: "Material Symbols Rounded"
    }

    Text {
      text: "bolt"
      leftPadding: 8
      visible: BatteryProvider.is_charging
      color: Theme.status_bar_surface_fg
      font.pixelSize: 14
      font.family: "Material Symbols Rounded"
    }
  }

  MouseArea {
    anchors.fill: parent
    hoverEnabled: true
    onEntered:  stateLayer.color = root.stateHover
    onExited:   stateLayer.color = "transparent"
    onPressed: (mouse) => {
      stateLayer.color = root.statePressed
      ripple.centerX = mouse.x
      ripple.centerY = mouse.y
      rippleAnim.restart()
    }
    onReleased: stateLayer.color = containsMouse ? root.stateHover : "transparent"
    onClicked: root.clicked()
  }
}
