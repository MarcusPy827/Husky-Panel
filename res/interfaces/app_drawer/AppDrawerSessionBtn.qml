/*
 * AppDrawerSessionBtn.qml
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

Item {
  id: root

  implicitWidth: 48
  implicitHeight: 48

  signal confirmActionRequested(string action, string label)

  function closeMenu() { sessionMenu.close() }

  ToolTip.text: qsTr("Session")
  ToolTip.visible: mouseArea.containsMouse && !sessionMenu.visible
  ToolTip.delay: 500

  Rectangle {
    id: bg
    anchors.fill: parent
    radius: 24
    color: Theme.primary_container

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

      property real centerX: parent.width / 2
      property real centerY: parent.height / 2
      property real rippleRadius: 0

      onPaint: {
        var ctx = getContext("2d");
        ctx.clearRect(0, 0, width, height);
        ctx.save();
        ctx.beginPath();
        ctx.arc(width / 2, height / 2, width / 2, 0, 2 * Math.PI);
        ctx.clip();
        var c = Theme.state_layer_pressed;
        ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0);
        ctx.beginPath();
        ctx.arc(centerX, centerY, rippleRadius, 0, 2 * Math.PI);
        ctx.fill();
        ctx.restore();
      }

      onOpacityChanged: requestPaint()
      onRippleRadiusChanged: requestPaint()

      ParallelAnimation {
        id: rippleAnim
        NumberAnimation {
          target: ripple; property: "rippleRadius"
          from: 0; to: bg.width * 1.25
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
  }

  Text {
    anchors.centerIn: parent
    font.family: "Material Symbols Rounded"
    font.pixelSize: 24
    text: "account_circle"
    color: Theme.secondary_container_fg
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
  }

  MouseArea {
    id: mouseArea
    anchors.fill: parent
    hoverEnabled: true

    onEntered: stateLayer.color = Theme.state_layer_hover
    onExited:  stateLayer.color = "transparent"
    onPressed: (mouse) => {
      stateLayer.color = Theme.state_layer_pressed
      ripple.centerX = mouse.x
      ripple.centerY = mouse.y
      rippleAnim.restart()
    }
    onReleased: stateLayer.color =
      containsMouse ? Theme.state_layer_hover : "transparent"
    onClicked: sessionMenu.visible ? sessionMenu.close() : sessionMenu.open()
  }

  AppDrawerSessionMenu {
    id: sessionMenu
    y: -implicitHeight - 8
    x: root.width - implicitWidth
    onConfirmActionRequested: (action, label) => root.confirmActionRequested(action, label)
  }
}
