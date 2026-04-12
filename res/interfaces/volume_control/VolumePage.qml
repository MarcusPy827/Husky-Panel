/*
 * VolumePage.qml
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

// Reusable page shared
// "devices" = list of sinks or sources
//             (has is_default, name, description, volume, muted, channels, index)
Item {
  id: root

  property string deviceLabel: "Output Device"
  property var    devices:     []

  signal deviceSelected(string name)
  signal deviceVolumeChanged(int index, int vol)

  implicitHeight: pageCol.implicitHeight

  Column {
    id: pageCol
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: 0

    // Section: I/O device selection
    Item { width: 1; height: 12 }

    Text {
      x: 16
      text: root.deviceLabel
      color: Theme.primary // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6 }

    // Device selector dropdown
    Item {
      id: deviceSelector
      x: 8
      width: parent.width - 16
      height: 52
      z: 10  // render above the device list Flickable below

      property bool expanded: false

      // Selector button
      Rectangle {
        id: selectorBtn
        anchors.left: parent.left
        anchors.right: parent.right
        height: 52
        radius: 12
        color: Theme.surface_container // qmllint disable unqualified
        clip: true

        Canvas {
          id: btnRipple
          anchors.fill: parent
          opacity: 0
          property real cx: width / 2
          property real cy: height / 2
          property real rad: 0

          onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.save()
            var r = selectorBtn.radius
            ctx.beginPath()
            ctx.moveTo(r, 0); ctx.lineTo(width - r, 0)
            ctx.arcTo(width, 0, width, r, r)
            ctx.lineTo(width, height - r)
            ctx.arcTo(width, height, width - r, height, r)
            ctx.lineTo(r, height)
            ctx.arcTo(0, height, 0, height - r, r)
            ctx.lineTo(0, r)
            ctx.arcTo(0, 0, r, 0, r)
            ctx.closePath(); ctx.clip()
            var c = Theme.primary // qmllint disable unqualified
            ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
            ctx.beginPath()
            ctx.arc(cx, cy, rad, 0, 2 * Math.PI)
            ctx.fill(); ctx.restore()
          }
          onOpacityChanged: requestPaint()
          onRadChanged:     requestPaint()

          ParallelAnimation {
            id: btnRippleAnim
            NumberAnimation {
              target: btnRipple; property: "rad"
              from: 0
              to: Math.sqrt(btnRipple.width * btnRipple.width +
                            btnRipple.height * btnRipple.height)
              duration: 380; easing.type: Easing.OutCubic
            }
            SequentialAnimation {
              PropertyAction  { target: btnRipple; property: "opacity"; value: 0.14 }
              PauseAnimation  { duration: 130 }
              NumberAnimation {
                target: btnRipple; property: "opacity"
                from: 0.14; to: 0
                duration: 220; easing.type: Easing.OutCubic
              }
            }
          }
        }

        RowLayout {
          anchors {
            fill: parent
            leftMargin: 14; rightMargin: 14
          }
          spacing: 8

          Text {
            Layout.fillWidth: true
            text: {
              for (var i = 0; i < root.devices.length; ++i) {
                if (root.devices[i].is_default)
                  return root.devices[i].description
              }
              return VolumeControlTranslator.Tr("NoDevice")
            }
            color: Theme.surface_fg // qmllint disable unqualified
            font.pixelSize: 14
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
          }

          Text {
            id: chevron
            text: "expand_more"
            font.family: "Material Symbols Rounded"
            font.pixelSize: 18
            color: Theme.surface_variant_fg // qmllint disable unqualified

            transform: Rotation {
              origin.x: chevron.width / 2
              origin.y: chevron.height / 2
              angle: deviceSelector.expanded ? 180 : 0
              Behavior on angle {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
              }
            }
          }
        }

        MouseArea {
          anchors.fill: parent
          onPressed: (mouse) => {
            btnRipple.cx = mouse.x
            btnRipple.cy = mouse.y
            btnRippleAnim.restart()
          }
          onClicked: deviceSelector.expanded = !deviceSelector.expanded
        }
      }

      // Dropdown panel
      Rectangle {
        id: dropPanel
        z: 10
        anchors.top: selectorBtn.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 4
        implicitHeight: dropCol.implicitHeight + 8
        radius: 12
        color: Theme.surface_container // qmllint disable unqualified
        border.color: Theme.primary // qmllint disable unqualified
        border.width: 1
        clip: true

        visible: opacity > 0
        opacity: deviceSelector.expanded ? 1.0 : 0.0
        Behavior on opacity {
          NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
        }

        property real slideOffset: deviceSelector.expanded ? 0 : -8
        Behavior on slideOffset {
          NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
        }
        transform: Translate { y: dropPanel.slideOffset }

        Column {
          id: dropCol
          anchors {
            left: parent.left; right: parent.right
            top: parent.top; margins: 4
          }
          spacing: 2

          Repeater {
            model: root.devices

            Rectangle {
              id: dropItem
              required property var modelData

              width: dropCol.width
              height: 44
              radius: 8
              clip: true
              color: dropItem.modelData.is_default
                ? Theme.primary_container // qmllint disable unqualified
                : "transparent"

              // Ripple canvas
              Canvas {
                id: itemRipple
                anchors.fill: parent
                opacity: 0
                property real cx: width / 2
                property real cy: height / 2
                property real rad: 0

                onPaint: {
                  var ctx = getContext("2d")
                  ctx.clearRect(0, 0, width, height)
                  ctx.save()
                  var r = dropItem.radius
                  ctx.beginPath()
                  ctx.moveTo(r, 0); ctx.lineTo(width - r, 0)
                  ctx.arcTo(width, 0, width, r, r)
                  ctx.lineTo(width, height - r)
                  ctx.arcTo(width, height, width - r, height, r)
                  ctx.lineTo(r, height)
                  ctx.arcTo(0, height, 0, height - r, r)
                  ctx.lineTo(0, r)
                  ctx.arcTo(0, 0, r, 0, r)
                  ctx.closePath(); ctx.clip()
                  var c = Theme.primary // qmllint disable unqualified
                  ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
                  ctx.beginPath()
                  ctx.arc(cx, cy, rad, 0, 2 * Math.PI)
                  ctx.fill(); ctx.restore()
                }
                onOpacityChanged: requestPaint()
                onRadChanged:     requestPaint()

                ParallelAnimation {
                  id: itemRippleAnim
                  NumberAnimation {
                    target: itemRipple; property: "rad"
                    from: 0
                    to: Math.sqrt(itemRipple.width * itemRipple.width +
                                  itemRipple.height * itemRipple.height)
                    duration: 340; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction  { target: itemRipple; property: "opacity"; value: 0.18 }
                    PauseAnimation  { duration: 120 }
                    NumberAnimation {
                      target: itemRipple; property: "opacity"
                      from: 0.18; to: 0
                      duration: 200; easing.type: Easing.OutCubic
                    }
                  }
                }
              }

              Text {
                anchors {
                  verticalCenter: parent.verticalCenter
                  left: parent.left; leftMargin: 12
                  right: parent.right; rightMargin: 12
                }
                text: dropItem.modelData.description
                color: dropItem.modelData.is_default
                  ? Theme.primary // qmllint disable unqualified
                  : Theme.surface_fg // qmllint disable unqualified
                font.pixelSize: 13
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
              }

              MouseArea {
                anchors.fill: parent
                onPressed: (mouse) => {
                  itemRipple.cx = mouse.x
                  itemRipple.cy = mouse.y
                  itemRippleAnim.restart()
                }
                onClicked: {
                  root.deviceSelected(dropItem.modelData.name)
                  deviceSelector.expanded = false
                }
              }
            }
          }
        }
      }
    }

    Item { width: 1; height: 12 }

    // Control per Device
    Text {
      x: 16
      text: VolumeControlTranslator.Tr("ControlPerDevice")
      color: Theme.primary // qmllint disable unqualified
      font.pixelSize: 12
      font.weight: Font.Medium
    }

    Item { width: 1; height: 6 }

    // Flickable device list (max 3 rows visible)
    Flickable {
      id: deviceListFlickable
      x: 8
      width: parent.width - 16
      height: Math.min(deviceListCol.implicitHeight, 3 * 76)
      contentHeight: deviceListCol.implicitHeight
      clip: true
      boundsBehavior: Flickable.DragAndOvershootBounds

      NumberAnimation {
        id: bounceAnim
        target: deviceListFlickable
        property: "contentY"
        duration: 450
        easing.type: Easing.OutBounce
      }

      MouseArea {
        anchors.fill: parent
        z: 1
        acceptedButtons: Qt.NoButton
        onWheel: function(wheel) {
          bounceAnim.stop()
          var maxY   = Math.max(0, deviceListFlickable.contentHeight -
                                   deviceListFlickable.height)
          var newY   = deviceListFlickable.contentY - wheel.angleDelta.y * 3
          if (newY < 0) {
            deviceListFlickable.contentY = Math.max(newY, -30)
            bounceAnim.to = 0
            bounceAnim.restart()
          } else if (newY > maxY) {
            deviceListFlickable.contentY = Math.min(newY, maxY + 30)
            bounceAnim.to = maxY
            bounceAnim.restart()
          } else {
            deviceListFlickable.contentY = newY
          }
          wheel.accepted = true
        }
      }

      Column {
        id: deviceListCol
        width: parent.width
        spacing: 6

        Repeater {
          model: root.devices

          Rectangle {
            id: deviceRow
            required property var modelData

            width: deviceListCol.width
            height: 68
            radius: 12
            color: Theme.surface_container // qmllint disable unqualified

            ColumnLayout {
              anchors {
                fill: parent
                leftMargin: 14; rightMargin: 14
                topMargin: 10; bottomMargin: 10
              }
              spacing: 6

              // Device name
              Text {
                Layout.fillWidth: true
                text: deviceRow.modelData.description
                color: Theme.surface_fg // qmllint disable unqualified
                font.pixelSize: 13
                font.weight: Font.Medium
                elide: Text.ElideRight
              }

              // Slider + percentage
              RowLayout {
                Layout.fillWidth: true
                spacing: 8

                VolumeSlider {
                  id: devSlider
                  Layout.fillWidth: true

                  // Track the backend value reactively without using a Binding
                  // element (which silently ignores JS writes when active in Qt 6).
                  property int backendVolume: deviceRow.modelData.volume

                  Component.onCompleted: value = backendVolume

                  onBackendVolumeChanged: {
                    if (!pressed) value = backendVolume
                  }

                  onMoved: (v) => root.deviceVolumeChanged(deviceRow.modelData.index, v)
                }

                Text {
                  text: devSlider.value + "%"
                  color: Theme.surface_variant_fg // qmllint disable unqualified
                  font.pixelSize: 12
                  horizontalAlignment: Text.AlignRight
                  Layout.preferredWidth: 34
                }
              }
            }
          }
        }

        // Empty state
        Item {
          width: deviceListCol.width
          height: 48
          visible: root.devices.length === 0

          Text {
            anchors.centerIn: parent
            text: VolumeControlTranslator.Tr("NoDevice")
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
