/*
 * SystemTrayOverflowFlyout.qml
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

Item {
  id: root
  opacity: 0
  implicitWidth: card.width
  implicitHeight: card.implicitHeight

  readonly property int slideOffset: 24

  transform: Translate {
    id: slideTransform
    y: -root.slideOffset
  }

  signal closeAnimationFinished()

  function open() {
    slideTransform.y = -root.slideOffset
    root.opacity = 0
    openAnim.restart()
  }

  function close() {
    closeAnim.restart()
  }

  // Open animation
  ParallelAnimation {
    id: openAnim
    NumberAnimation {
      target: root; property: "opacity"
      from: 0; to: 1
      duration: 220; easing.type: Easing.OutCubic
    }
    NumberAnimation {
      target: slideTransform; property: "y"
      from: -root.slideOffset; to: 0
      duration: 220; easing.type: Easing.OutCubic
    }
  }

  // Close animation
  ParallelAnimation {
    id: closeAnim
    NumberAnimation {
      target: root; property: "opacity"
      from: 1; to: 0
      duration: 160; easing.type: Easing.InCubic
    }
    NumberAnimation {
      target: slideTransform; property: "y"
      from: 0; to: -root.slideOffset
      duration: 160; easing.type: Easing.InCubic
    }
    onStopped: root.closeAnimationFinished()
  }

  // Card
  Rectangle {
    id: card
    width: inner.implicitWidth + 8
    implicitHeight: inner.implicitHeight + 8
    radius: 12
    color: Theme.status_bar_bg // qmllint disable unqualified
    border.color: Theme.primary // qmllint disable unqualified
    border.width: 0

    MouseArea { anchors.fill: parent }

    Row {
      id: inner
      anchors {
        top: parent.top
        left: parent.left
        margins: 4
      }
      spacing: 4

      Repeater {
        model: TrayHandler // qmllint disable unqualified
        delegate: SystemTrayIcon {
          inOverflow: true
          width: 32
          height: 32
        }
      }
    }
  }
}
