/*
 * NetworkFlyout.qml
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

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

Item {
  id: root
  opacity: 0
  implicitWidth: 320
  implicitHeight: frame.implicitHeight

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

  // Forward password requests to the wireless page
  Connections {
    target: NetworkHandler  // qmllint disable unqualified
    function onPasswordRequired(ssid, apPath, securityCategory) {
      tabBar.currentTab = 0
      wirelessPage.showPasswordDialog(ssid, apPath, securityCategory)
    }
  }

  // Main frame
  Rectangle {
    id: frame
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    implicitHeight: mainColumn.implicitHeight
    radius: 12
    color: Theme.surface_bg    // qmllint disable unqualified
    border.color: Theme.primary  // qmllint disable unqualified
    border.width: 2

    MouseArea { anchors.fill: parent }

    ColumnLayout {
      id: mainColumn
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: parent.top
      spacing: 0

      // Header
      Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: headerCol.implicitHeight + 16
        Layout.topMargin: 3
        Layout.leftMargin: 3
        Layout.rightMargin: 3
        color: Theme.surface_container  // qmllint disable unqualified
        topLeftRadius: 10
        topRightRadius: 10

        ColumnLayout {
          id: headerCol
          anchors {
            left: parent.left; right: parent.right
            top: parent.top; margins: 16
          }
          spacing: 12

          Text {
            text: NetworkTranslator.Tr("Network")
            color: Theme.primary   // qmllint disable unqualified
            font.pixelSize: 16; font.weight: Font.Medium
          }

          // Tab bar
          Item {
            id: tabBar
            Layout.fillWidth: true
            height: 40

            property int currentTab: 0

            Row {
              anchors.fill: parent
              spacing: 0

              Repeater {
                model: [
                  NetworkTranslator.Tr("Wireless"),
                  NetworkTranslator.Tr("Cabled"),
                  NetworkTranslator.Tr("Hotspot")
                ]

                Rectangle {
                  id: tabItem
                  required property string modelData
                  required property int index

                  width: tabBar.width / 3
                  height: tabBar.height
                  color: "transparent"

                  readonly property bool active: tabBar.currentTab === index

                  Text {
                    anchors.centerIn: parent
                    text: tabItem.modelData
                    color: tabItem.active
                      ? Theme.primary           // qmllint disable unqualified
                      : Theme.surface_variant_fg // qmllint disable unqualified
                    font.pixelSize: 14
                    font.weight: tabItem.active ? Font.Medium : Font.Normal
                    Behavior on color { ColorAnimation { duration: 200 } }
                  }

                  MouseArea {
                    anchors.fill: parent
                    onClicked: tabBar.currentTab = tabItem.index
                  }
                }
              }
            }

            // Indicator pill
            Rectangle {
              width: 48; height: 3; radius: 1.5
              anchors.bottom: parent.bottom
              color: Theme.primary  // qmllint disable unqualified
              x: tabBar.currentTab * (tabBar.width / 3) +
                 (tabBar.width / 3 - width) / 2
              Behavior on x {
                NumberAnimation {
                  duration: 360; easing.type: Easing.OutBack
                  easing.overshoot: 1.8
                }
              }
            }

            // Swipe gesture
            DragHandler {
              id: swipeDrag
              target: null
              xAxis.enabled: true; yAxis.enabled: false

              property real _startX: 0

              onActiveChanged: {
                if (active) {
                  snapAnim.stop()
                  _startX = tabContent._liveX
                } else {
                  var moved     = tabContent._liveX - _startX
                  var threshold = tabContent.width * 0.25
                  var nextTab   = tabBar.currentTab

                  if      (moved < -threshold && tabBar.currentTab < 2) nextTab = tabBar.currentTab + 1
                  else if (moved >  threshold && tabBar.currentTab > 0) nextTab = tabBar.currentTab - 1

                  tabContent._swipeSnapping = true
                  tabBar.currentTab = nextTab
                  tabContent._swipeSnapping = false

                  snapAnim.from = tabContent._liveX
                  snapAnim.to   = nextTab * -tabContent.width
                  snapAnim.restart()
                }
              }

              onCentroidChanged: {
                if (active) {
                  var base  = tabBar.currentTab * -tabContent.width
                  var delta = centroid.position.x - centroid.pressPosition.x
                  var raw   = base + delta
                  var minX  = -tabContent.width * 2
                  var maxX  = 0
                  if      (raw > maxX) raw = maxX + (raw - maxX) * 0.25
                  else if (raw < minX) raw = minX + (raw - minX) * 0.25
                  tabContent._liveX = raw
                }
              }
            }
          }
        }
      }

      // Tab
      Item {
        id: tabContent
        Layout.fillWidth: true
        Layout.topMargin: 2
        clip: true

        implicitHeight: tabBar.currentTab === 0 ? wirelessPage.implicitHeight
                      : tabBar.currentTab === 1 ? cabledPage.implicitHeight
                      :                           hotspotPage.implicitHeight
        Behavior on implicitHeight {
          NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
        }

        property real _liveX: 0
        property bool _swipeSnapping: false

        NumberAnimation {
          id: snapAnim
          target: tabContent; property: "_liveX"
          duration: 360; easing.type: Easing.OutBack
          easing.overshoot: 1.8
        }

        Connections {
          target: tabBar
          function onCurrentTabChanged() {
            if (!tabContent._swipeSnapping) {
              snapAnim.stop()
              snapAnim.from = tabContent._liveX
              snapAnim.to   = tabBar.currentTab * -tabContent.width
              snapAnim.restart()
            }
          }
        }

        // Three pages side by side
        Item {
          x: tabContent._liveX
          width: tabContent.width * 3

          NetworkWirelessPage {
            id: wirelessPage
            x: 0
            width: tabContent.width
          }

          NetworkCabledPage {
            id: cabledPage
            x: tabContent.width
            width: tabContent.width
          }

          NetworkHotspotPage {
            id: hotspotPage
            x: tabContent.width * 2
            width: tabContent.width
          }
        }
      }

      // Bottom spacer
      Item { Layout.preferredHeight: 8 }
    }
  }
}
