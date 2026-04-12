/*
 * StreamPage.qml
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

// Per-application stream volume control.
// playbackStreams  = VolumeHandler.sinkInputs
// recordingStreams = VolumeHandler.sourceOutputs
Item {
  id: root

  property var playbackStreams:  []
  property var recordingStreams: []

  signal playbackVolumeChanged(int index, int vol)
  signal recordingVolumeChanged(int index, int vol)
  signal playbackMuteToggled(int index, bool muted)
  signal recordingMuteToggled(int index, bool muted)

  implicitHeight: pageCol.implicitHeight

  // Reusable stream-row component (sink input or source output)
  component StreamRow: Rectangle {
    id: row

    property var  stream:   ({})
    property bool isLast:   false

    signal volumeChanged(int vol)
    signal muteToggled(bool muted)

    width:  parent ? parent.width : 0
    height: 72
    radius: 12
    clip:   true
    color:  Theme.surface_container // qmllint disable unqualified

    RowLayout {
      anchors {
        fill: parent
        leftMargin: 12; rightMargin: 12
        topMargin: 10; bottomMargin: 10
      }
      spacing: 10

      // App icon
      Image {
        source: (row.stream.icon_name && row.stream.icon_name !== "")
          ? ("image://icon/" + row.stream.icon_name) : "image://icon/audio-card"
        opacity: row.stream.muted ? 0.4 : 1.0
        smooth: true
        Layout.preferredWidth: 32
        Layout.preferredHeight: 32
      }

      ColumnLayout {
        Layout.fillWidth: true
        spacing: 4

        // App name
        Text {
          Layout.fillWidth: true
          text: row.stream.name ?? ""
          color: row.stream.muted
            ? Theme.surface_variant_fg // qmllint disable unqualified
            : Theme.surface_fg // qmllint disable unqualified
          font.pixelSize: 13
          font.weight: Font.Medium
          elide: Text.ElideRight
        }

        RowLayout {
          Layout.fillWidth: true
          spacing: 6

          VolumeSlider {
            id: rowSlider
            Layout.fillWidth: true
            enabled: !row.stream.muted
            opacity:  row.stream.muted ? 0.4 : 1.0

            property int backendVolume: row.stream.volume ?? 0
            Component.onCompleted: value = backendVolume
            onBackendVolumeChanged: if (!pressed) value = backendVolume

            onMoved: (v) => row.volumeChanged(v)
          }

          Text {
            text: rowSlider.value + "%"
            color: Theme.surface_variant_fg // qmllint disable unqualified
            font.pixelSize: 12
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: 30
          }
        }
      }

      // Mute toggle button
      Text {
        text: row.stream.muted ? "volume_off" : "volume_up"
        font.family: "Material Symbols Rounded"
        font.pixelSize: 18
        color: row.stream.muted
          ? Theme.primary // qmllint disable unqualified
          : Theme.surface_variant_fg // qmllint disable unqualified
        Layout.preferredWidth: 24
        horizontalAlignment: Text.AlignHCenter

        MouseArea {
          anchors.fill: parent
          anchors.margins: -4
          onClicked: row.muteToggled(!row.stream.muted)
        }
      }
    }
  }

  // Main layout
  Column {
    id: pageCol
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: 0

    // Playback section
    Item { width: 1; height: 12 }

    Text {
      x: 16
      text: VolumeControlTranslator.Tr("Playback") // qmllint disable unqualified
      color: Theme.primary // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6 }

    Flickable {
      id: playbackFlickable
      x: 8
      width: parent.width - 16
      height: Math.min(playbackCol.implicitHeight, 3 * 78)
      contentHeight: playbackCol.implicitHeight
      clip: true
      boundsBehavior: Flickable.DragAndOvershootBounds

      NumberAnimation {
        id: playbackBounce
        target: playbackFlickable
        property: "contentY"
        duration: 450; easing.type: Easing.OutBounce
      }

      MouseArea {
        anchors.fill: parent
        z: 1
        acceptedButtons: Qt.NoButton
        onWheel: function(wheel) {
          playbackBounce.stop()
          var maxY = Math.max(0, playbackFlickable.contentHeight -
                                 playbackFlickable.height)
          var newY = playbackFlickable.contentY - wheel.angleDelta.y * 3
          if (newY < 0) {
            playbackFlickable.contentY = Math.max(newY, -30)
            playbackBounce.to = 0; playbackBounce.restart()
          } else if (newY > maxY) {
            playbackFlickable.contentY = Math.min(newY, maxY + 30)
            playbackBounce.to = maxY; playbackBounce.restart()
          } else {
            playbackFlickable.contentY = newY
          }
          wheel.accepted = true
        }
      }

      Column {
        id: playbackCol
        width: parent.width
        spacing: 6

        Repeater {
          model: root.playbackStreams

          StreamRow {
            required property var modelData

            stream: modelData
            width: playbackCol.width

            onVolumeChanged:  (v)       => root.playbackVolumeChanged(modelData.index, v)
            onMuteToggled:    (muted)   => root.playbackMuteToggled(modelData.index, muted)
          }
        }

        Item {
          width: playbackCol.width
          height: 48
          visible: root.playbackStreams.length === 0

          Text {
            anchors.centerIn: parent
            text: VolumeControlTranslator.Tr("NoActivePlayback") // qmllint disable unqualified
            font.pixelSize: 13
            color: Theme.surface_variant_fg // qmllint disable unqualified
            opacity: 0.5
          }
        }
      }
    }

    // Recording section
    Item { width: 1; height: 12 }

    Text {
      x: 16
      text: VolumeControlTranslator.Tr("Recording") // qmllint disable unqualified
      color: Theme.primary // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6 }

    Flickable {
      id: recordingFlickable
      x: 8
      width: parent.width - 16
      height: Math.min(recordingCol.implicitHeight, 3 * 78)
      contentHeight: recordingCol.implicitHeight
      clip: true
      boundsBehavior: Flickable.DragAndOvershootBounds

      NumberAnimation {
        id: recordingBounce
        target: recordingFlickable
        property: "contentY"
        duration: 450; easing.type: Easing.OutBounce
      }

      MouseArea {
        anchors.fill: parent
        z: 1
        acceptedButtons: Qt.NoButton
        onWheel: function(wheel) {
          recordingBounce.stop()
          var maxY = Math.max(0, recordingFlickable.contentHeight -
                                 recordingFlickable.height)
          var newY = recordingFlickable.contentY - wheel.angleDelta.y * 3
          if (newY < 0) {
            recordingFlickable.contentY = Math.max(newY, -30)
            recordingBounce.to = 0; recordingBounce.restart()
          } else if (newY > maxY) {
            recordingFlickable.contentY = Math.min(newY, maxY + 30)
            recordingBounce.to = maxY; recordingBounce.restart()
          } else {
            recordingFlickable.contentY = newY
          }
          wheel.accepted = true
        }
      }

      Column {
        id: recordingCol
        width: parent.width
        spacing: 6

        Repeater {
          model: root.recordingStreams

          StreamRow {
            required property var modelData

            stream: modelData
            width: recordingCol.width

            onVolumeChanged: (v)     => root.recordingVolumeChanged(modelData.index, v)
            onMuteToggled:   (muted) => root.recordingMuteToggled(modelData.index, muted)
          }
        }

        Item {
          width: recordingCol.width
          height: 48
          visible: root.recordingStreams.length === 0

          Text {
            anchors.centerIn: parent
            text: VolumeControlTranslator.Tr("NoActiveRecording") // qmllint disable unqualified
            font.pixelSize: 13
            color: Theme.surface_variant_fg // qmllint disable unqualified
            opacity: 0.5
          }
        }
      }
    }

    Item { width: 1; height: 8 }
  }
}
