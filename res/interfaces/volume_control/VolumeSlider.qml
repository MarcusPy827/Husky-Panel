/*
 * VolumeSlider.qml
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

// Volume slider.  Range: 0 – 100.
Item {
  id: root

  property int  value:   0
  property bool pressed: dragHandler.active

  signal moved(int v)

  implicitHeight: 20
  implicitWidth:  120

  function computeValue(posX) {
    var ratio = Math.max(0.0, Math.min(1.0, posX / root.width))
    return Math.round(ratio * 100)
  }

  // Inactive track
  Rectangle {
    anchors.verticalCenter: parent.verticalCenter
    width: parent.width
    height: 4
    radius: 2
    color: Theme.surface_container_high // qmllint disable unqualified

    // Active track
    Rectangle {
      anchors.left: parent.left
      anchors.verticalCenter: parent.verticalCenter
      width: (root.value / 100) * parent.width
      height: parent.height
      radius: parent.radius
      color: Theme.primary // qmllint disable unqualified

      Behavior on width {
        enabled: !root.pressed
        NumberAnimation { duration: 60; easing.type: Easing.OutCubic }
      }
    }
  }

  // Thumb
  Rectangle {
    anchors.verticalCenter: parent.verticalCenter
    x: (root.value / 100) * (root.width - width)
    width: 16; height: 16
    radius: 8
    color: Theme.primary // qmllint disable unqualified

    Behavior on x {
      enabled: !root.pressed
      NumberAnimation { duration: 60; easing.type: Easing.OutCubic }
    }
  }

  // Horizontal drag
  // Cooperates with vertical Flickable via Qt 6 handler priority.
  // Only emits moved() on release (not on every pixel) to avoid triggering a
  // backend round-trip and Repeater delegate recreation on every frame.
  DragHandler {
    id: dragHandler
    target: null
    xAxis.enabled: true
    yAxis.enabled: false
    // Extend vertical hit area so the thumb is easy to grab
    margin: 8

    onCentroidChanged: {
      if (active) {
        // Update display locally: no signal, no backend call yet
        root.value = root.computeValue(centroid.position.x)
      }
    }

    onActiveChanged: {
      if (!active) {
        // Drag ended: send the final value once
        root.moved(root.value)
      }
    }
  }

  // Click-to-seek: immediate commit on tap
  TapHandler {
    onTapped: (point) => {
      var v = root.computeValue(point.position.x)
      root.value = v
      root.moved(v)
    }
  }
}
