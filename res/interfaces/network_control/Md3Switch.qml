/*
 * Md3Switch.qml  — Material 3 toggle switch for the network control flyout
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

  implicitWidth: 52
  implicitHeight: 32
  Layout.alignment: Qt.AlignVCenter

  property bool checked: false

  signal toggled

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
    var c = checked ? Theme.primary : Theme.surface_fg        // qmllint disable unqualified
    if (isPressed) return Qt.rgba(c.r, c.g, c.b, 0.10)
    if (isHovered) return Qt.rgba(c.r, c.g, c.b, 0.08)
    return "transparent"
  }

  // Track
  Rectangle {
    anchors.fill: parent
    radius: 16
    color: root.checked
      ? Theme.primary                // qmllint disable unqualified
      : Theme.surface_container_high // qmllint disable unqualified
    border.width: root.checked ? 0 : 2
    border.color: Theme.surface_variant_fg // qmllint disable unqualified
    Behavior on color        { ColorAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on border.width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
  }

  // State-layer
  Rectangle {
    width: 40; height: 40; radius: 20
    anchors.verticalCenter: parent.verticalCenter
    x: root.handleCenterX - 20
    color: root.stateLayerColor
    Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on color { ColorAnimation  { duration: 150; easing.type: Easing.OutCubic } }
  }

  // Handle
  Rectangle {
    width: root.handleSize; height: width; radius: width / 2
    anchors.verticalCenter: parent.verticalCenter
    x: root.handleCenterX - root.handleSize / 2
    color: root.handleColor
    Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on color { ColorAnimation  { duration: 200; easing.type: Easing.OutCubic } }
  }

  MouseArea {
    anchors.fill: parent
    hoverEnabled: true
    onEntered:  root.isHovered = true
    onExited:   { root.isHovered = false; root.isPressed = false }
    onPressed:  root.isPressed  = true
    onReleased: root.isPressed  = false
    onClicked:  root.toggled()
  }
}
