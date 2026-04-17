/*
 * NetActionButton.qml  — 8px-radius outlined button with ripple
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

Item {
  id: root

  property string buttonText: ""
  property bool enabled: true

  signal actionClicked

  implicitWidth: btnLabel.implicitWidth + 24
  implicitHeight: 32
  Layout.alignment: Qt.AlignVCenter

  property bool isHovered: false
  property bool isPressed: false

  readonly property color btnColor:
    root.enabled ? Theme.primary : Theme.surface_variant_fg  // qmllint disable unqualified

  // Border
  Rectangle {
    anchors.fill: parent
    radius: 8
    color: "transparent"
    border.color: root.btnColor
    border.width: 1
    opacity: (root.isHovered || root.isPressed) ? 0.6 : (root.enabled ? 1.0 : 0.4)
    Behavior on opacity { NumberAnimation { duration: 120 } }
  }

  // Ripple
  Canvas {
    id: btnRipple
    anchors.fill: parent
    opacity: 0

    property real centerX: width / 2
    property real centerY: height / 2
    property real rippleRadius: 0

    onPaint: {
      var ctx = getContext("2d")
      ctx.clearRect(0, 0, width, height)
      ctx.save()
      ctx.beginPath()
      ctx.moveTo(8, 0); ctx.lineTo(width - 8, 0)
      ctx.arcTo(width, 0, width, 8, 8)
      ctx.lineTo(width, height - 8)
      ctx.arcTo(width, height, width - 8, height, 8)
      ctx.lineTo(8, height)
      ctx.arcTo(0, height, 0, height - 8, 8)
      ctx.lineTo(0, 8)
      ctx.arcTo(0, 0, 8, 0, 8)
      ctx.closePath(); ctx.clip()
      var c = root.btnColor
      ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
      ctx.beginPath()
      ctx.arc(btnRipple.centerX, btnRipple.centerY, btnRipple.rippleRadius, 0, 2 * Math.PI)
      ctx.fill(); ctx.restore()
    }

    onOpacityChanged: requestPaint()
    onRippleRadiusChanged: requestPaint()

    ParallelAnimation {
      id: btnRippleAnim
      NumberAnimation {
        target: btnRipple; property: "rippleRadius"
        from: 0
        to: Math.sqrt(btnRipple.width * btnRipple.width +
                      btnRipple.height * btnRipple.height)
        duration: 400; easing.type: Easing.OutCubic
      }
      SequentialAnimation {
        PropertyAction  { target: btnRipple; property: "opacity"; value: 0.3 }
        PauseAnimation  { duration: 150 }
        NumberAnimation {
          target: btnRipple; property: "opacity"
          from: 0.3; to: 0; duration: 250; easing.type: Easing.OutCubic
        }
      }
    }
  }

  Text {
    id: btnLabel
    anchors.centerIn: parent
    text: root.buttonText
    font.pixelSize: 13
    color: root.btnColor
    opacity: root.enabled ? 1.0 : 0.4
  }

  MouseArea {
    anchors.fill: parent
    hoverEnabled: true
    enabled: root.enabled
    onEntered:  root.isHovered = true
    onExited:   { root.isHovered = false; root.isPressed = false }
    onPressed: (mouse) => {
      root.isPressed = true
      btnRipple.centerX = mouse.x
      btnRipple.centerY = mouse.y
      btnRippleAnim.restart()
    }
    onReleased: root.isPressed = false
    onClicked:  root.actionClicked()
  }
}
