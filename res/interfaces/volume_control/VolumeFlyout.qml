/*
 * VolumeFlyout.qml
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

  // Main frame
  Rectangle {
    id: frame
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    implicitHeight: mainColumn.implicitHeight
    radius: 12
    color: Theme.surface_bg // qmllint disable unqualified
    border.color: Theme.primary // qmllint disable unqualified
    border.width: 2

    MouseArea { anchors.fill: parent }

    ColumnLayout {
      id: mainColumn
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: parent.top
      spacing: 0

      // Header toolbar
      Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: headerCol.implicitHeight + 16
        Layout.topMargin: 3
        Layout.leftMargin: 3
        Layout.rightMargin: 3
        color: Theme.surface_container // qmllint disable unqualified
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
            text: VolumeControlTranslator.Tr("VolumeControl")
            color: Theme.primary // qmllint disable unqualified
            font.pixelSize: 16
            font.weight: Font.Bold
          }

          // Tab bar
          Item {
            id: tabBar
            Layout.fillWidth: true
            height: 40

            property int currentTab: 0

            // Tab buttons
            Row {
              anchors.fill: parent
              spacing: 0

              Repeater {
                model: [VolumeControlTranslator.Tr("Output"),
                        VolumeControlTranslator.Tr("Input"),
                        VolumeControlTranslator.Tr("Stream")
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
                      ? Theme.primary // qmllint disable unqualified
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
              width: 48
              height: 3
              radius: 1.5
              anchors.bottom: parent.bottom
              color: Theme.primary // qmllint disable unqualified

              x: tabBar.currentTab * (tabBar.width / 3) +
                 (tabBar.width / 3 - width) / 2

              Behavior on x {
                NumberAnimation {
                  duration: 360
                  easing.type: Easing.OutBack
                  easing.overshoot: 1.8
                }
              }
            }

            // Swipe gesture lives on the tab bar — no slider conflicts here.
            // Moving left → next tab, moving right → previous tab.
            DragHandler {
              id: swipeDrag
              target: null
              xAxis.enabled: true
              yAxis.enabled: false

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

      // Tab content area
      Item {
        id: tabContent
        Layout.fillWidth: true
        Layout.topMargin: 2
        clip: true

        // Height tracks the active page, cross-fades between tabs
        implicitHeight: tabBar.currentTab === 0 ? outputPage.implicitHeight
                      : tabBar.currentTab === 1 ? inputPage.implicitHeight
                      :                           streamPage.implicitHeight // qmllint disable unqualified
        Behavior on implicitHeight {
          NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
        }

        // Live x of the sliding row — driven by animation or direct drag
        property real _liveX: 0
        // Guards against double-animation when swipe updates tabBar.currentTab
        property bool _swipeSnapping: false

        // Snap animation — same feel as the indicator pill
        NumberAnimation {
          id: snapAnim
          target: tabContent
          property: "_liveX"
          duration: 360
          easing.type: Easing.OutBack
          easing.overshoot: 1.8
        }

        // Respond to tab-button clicks (not triggered during swipe)
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

        // All three pages side by side; x driven by _liveX
        Item {
          x: tabContent._liveX
          width: tabContent.width * 3

          VolumePage {
            id: outputPage
            x: 0
            width: tabContent.width

            deviceLabel: VolumeControlTranslator.Tr("OutputDevice") // qmllint disable unqualified
            devices: VolumeHandler.sinks // qmllint disable unqualified

            onDeviceSelected: (name) => VolumeHandler.setDefaultSink(name) // qmllint disable unqualified
            onDeviceVolumeChanged: (index, vol) =>
              VolumeHandler.setSinkVolume(index, vol) // qmllint disable unqualified
          }

          VolumePage {
            id: inputPage
            x: tabContent.width
            width: tabContent.width

            deviceLabel: VolumeControlTranslator.Tr("InputDevice") // qmllint disable unqualified
            devices: VolumeHandler.sources // qmllint disable unqualified

            onDeviceSelected: (name) => VolumeHandler.setDefaultSource(name) // qmllint disable unqualified
            onDeviceVolumeChanged: (index, vol) =>
              VolumeHandler.setSourceVolume(index, vol) // qmllint disable unqualified
          }

          StreamPage {
            id: streamPage
            x: tabContent.width * 2
            width: tabContent.width

            playbackStreams:  VolumeHandler.sinkInputs    // qmllint disable unqualified
            recordingStreams: VolumeHandler.sourceOutputs // qmllint disable unqualified

            onPlaybackVolumeChanged:  (index, vol)   => VolumeHandler.setSinkInputVolume(index, vol)    // qmllint disable unqualified
            onRecordingVolumeChanged: (index, vol)   => VolumeHandler.setSourceOutputVolume(index, vol) // qmllint disable unqualified
            onPlaybackMuteToggled:    (index, muted) => VolumeHandler.setSinkInputMute(index, muted)    // qmllint disable unqualified
            onRecordingMuteToggled:   (index, muted) => VolumeHandler.setSourceOutputMute(index, muted) // qmllint disable unqualified
          }
        }
      }

      // Bottom spacer inside frame
      Item { Layout.preferredHeight: 8 }
    }
  }
}
