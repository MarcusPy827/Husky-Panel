/*
 * ConfigPanelSidePaneItem.qml
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

  property string icon: ""
  property string label: ""
  property bool isActive: false

  signal clicked()

  implicitHeight: 48

  Rectangle {
    id: bg
    anchors.fill: parent
    radius: 24
    color: "transparent"

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
        var r = bg.radius;
        ctx.beginPath();
        ctx.moveTo(r, 0);
        ctx.lineTo(width - r, 0);
        ctx.arcTo(width, 0, width, r, r);
        ctx.lineTo(width, height - r);
        ctx.arcTo(width, height, width - r, height, r);
        ctx.lineTo(r, height);
        ctx.arcTo(0, height, 0, height - r, r);
        ctx.lineTo(0, r);
        ctx.arcTo(0, 0, r, 0, r);
        ctx.closePath();
        ctx.clip();
        var c = Theme.state_layer_pressed; // qmllint disable unqualified
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

    RowLayout {
      anchors {
        fill: parent
        leftMargin: 16
        rightMargin: 24
      }
      spacing: 12

      Text {
        font.family: "Material Symbols Rounded"
        font.pixelSize: 24
        text: root.icon
        color: root.isActive
          ? Theme.secondary_container_fg  // qmllint disable unqualified
          : Theme.surface_variant_fg  // qmllint disable unqualified
        Layout.alignment: Qt.AlignVCenter
      }

      Text {
        text: root.label
        font.pixelSize: 14
        color: root.isActive
          ? Theme.secondary_container_fg  // qmllint disable unqualified
          : Theme.surface_variant_fg  // qmllint disable unqualified
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
        elide: Text.ElideRight
      }
    }

    MouseArea {
      anchors.fill: parent
      hoverEnabled: true

      onEntered: stateLayer.color = Theme.state_layer_hover  // qmllint disable unqualified
      onExited:  stateLayer.color = "transparent"
      onPressed: (mouse) => {
        stateLayer.color = Theme.state_layer_pressed  // qmllint disable unqualified
        ripple.centerX = mouse.x
        ripple.centerY = mouse.y
        rippleAnim.restart()
      }
      onReleased: stateLayer.color =
        containsMouse ? Theme.state_layer_hover : "transparent" // qmllint disable unqualified
      onClicked: root.clicked()
    }
  }
}
