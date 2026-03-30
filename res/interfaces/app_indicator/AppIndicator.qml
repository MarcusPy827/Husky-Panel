/*
 * AppIndicator.qml
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

  implicitWidth: contentLayout.implicitWidth + 16
  Layout.fillHeight: true

  clip: true
  color: "transparent"
  layer.enabled: true

  ColumnLayout {
    id: contentLayout

    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: 8
    spacing: 0

    Text {
      text: CurrentWindow.package_name
      color: Theme.status_bar_surface_fg
      opacity: 0.7
      font.pixelSize: 10
      Layout.alignment: Qt.AlignLeft
    }

    Text {
      text: CurrentWindow.app_name
      color: Theme.status_bar_primary
      font.pixelSize: 12
      font.bold: true
      Layout.alignment: Qt.AlignLeft
    }
  }

  // Item ripples
  Item {
    id: rippleContainer
    anchors.fill: parent
    clip: true

    property bool rippleReleased: false
    property int pressTimestamp: 0
    readonly property int longPressThreshold: 300

    Rectangle {
      id: ripple

      width: 80
      height: 80
      radius: width / 2
      color: Theme.status_bar_surface_fg
      opacity: 0
      transformOrigin: Item.Center

      NumberAnimation {
        id: rippleExpandAnim
        target: ripple
        property: "scale"
        from: 0
        to: 3.5
        duration: 800
        easing.type: Easing.OutQuad

        onStopped: {
          if (rippleContainer.rippleReleased) {
            rippleFadeAnim.restart()
          }
        }
      }

      NumberAnimation {
        id: rippleFadeAnim
        target: ripple
        property: "opacity"
        to: 0
        duration: 300
        easing.type: Easing.OutQuad
      }
    }

    MouseArea {
      anchors.fill: parent

      onPressed: function(mouse) {
        rippleContainer.rippleReleased = false
        rippleContainer.pressTimestamp = Date.now()
        ripple.x = mouse.x - ripple.width / 2
        ripple.y = mouse.y - ripple.height / 2
        ripple.scale = 0
        ripple.opacity = 0.10
        rippleFadeAnim.stop()
        rippleExpandAnim.restart()
      }

      onReleased: {
        rippleContainer.rippleReleased = true
        const isLongPress = (Date.now() - rippleContainer.pressTimestamp) >= rippleContainer.longPressThreshold
        if (!isLongPress || !rippleExpandAnim.running) {
          // If it is a single on click or ripple finishedexpanding,
          // fade out immediately.
          rippleFadeAnim.restart()
        }
      }
    }
  }
}
