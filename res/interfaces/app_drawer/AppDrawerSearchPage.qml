/*
 * AppDrawerSearchPage.qml
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
import QtQuick.Layouts

Item {
  id: root

  property var appModel: []
  property string filterText: ""
  signal appLaunched()

  readonly property var filteredModel: {
    if (filterText.length === 0) return appModel
    const q = filterText.toLowerCase()
    return appModel.filter(function(item) {
      return item.name.toLowerCase().includes(q)
        || (item.generic_name
            && item.generic_name.toLowerCase().includes(q))
    })
  }

  Flickable {
    id: flickable
    anchors.fill: parent
    clip: true
    contentHeight: listColumn.implicitHeight + 16
    boundsBehavior: Flickable.DragAndOvershootBounds

    ScrollBar.vertical: ScrollBar {
      policy: ScrollBar.AsNeeded
    }

    Column {
      id: listColumn
      width: parent.width
      padding: 8
      spacing: 2

      Repeater {
        model: root.filteredModel

        delegate: Item {
          id: delegateItem
          required property var modelData

          width: listColumn.width - 16
          height: 56

          Rectangle {
            anchors.fill: parent
            radius: 8
            color: "transparent"
            clip: true

            Rectangle {
              id: searchStateLayer
              anchors.fill: parent
              radius: parent.radius
              color: "transparent"
            }

            RowLayout {
              anchors {
                fill: parent
                leftMargin: 12
                rightMargin: 12
              }
              spacing: 12

              Image {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                source: delegateItem.modelData.icon_name
                  ? ("image://icon/" + delegateItem.modelData.icon_name) : ""
                fillMode: Image.PreserveAspectFit
                smooth: true
              }

              Column {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                spacing: 2

                Text {
                  width: parent.width
                  text: delegateItem.modelData.name
                  font.pixelSize: 14
                  color: Theme.surface_fg
                  elide: Text.ElideRight
                }

                Text {
                  width: parent.width
                  text: delegateItem.modelData.generic_name || delegateItem.modelData.comment || ""
                  font.pixelSize: 12
                  color: Theme.surface_variant_fg
                  elide: Text.ElideRight
                  visible: text.length > 0
                }
              }
            }

            Rectangle {
              id: searchRipple
              property real cx: 0
              property real cy: 0
              property real r: 0
              x: cx - r; y: cy - r
              width: r * 2; height: r * 2
              radius: r
              color: Theme.state_layer_pressed
              opacity: 0

              ParallelAnimation {
                id: searchRippleAnim
                NumberAnimation {
                  target: searchRipple; property: "r"
                  from: 0; to: 300
                  duration: 400; easing.type: Easing.OutCubic
                }
                NumberAnimation {
                  target: searchRipple; property: "opacity"
                  from: 1; to: 0
                  duration: 400; easing.type: Easing.InCubic
                }
              }
            }

            MouseArea {
              anchors.fill: parent
              hoverEnabled: true

              onEntered:  searchStateLayer.color = Theme.state_layer_hover
              onExited:   searchStateLayer.color = "transparent"
              onPressed: (mouse) => {
                searchStateLayer.color = Theme.state_layer_pressed
                searchRipple.cx = mouse.x
                searchRipple.cy = mouse.y
                searchRippleAnim.restart()
              }
              onReleased: searchStateLayer.color =
                containsMouse ? Theme.state_layer_hover : "transparent"
              onClicked: {
                if (typeof AppLaunchProvider !== "undefined") {
                  AppLaunchProvider.launch(delegateItem.modelData.exec)
                  root.appLaunched()
                }
              }
            }
          }
        }
      }
    }
  }

  NumberAnimation {
    id: bounceAnim
    target: flickable
    property: "contentY"
    duration: 600
    easing.type: Easing.OutBack
    easing.overshoot: 0.5
  }

  // Intercept wheel events before Flickable's built-in handler
  MouseArea {
    anchors.fill: parent
    z: 1
    acceptedButtons: Qt.NoButton
    onWheel: function(wheel) {
      bounceAnim.stop()
      var maxY = Math.max(0, flickable.contentHeight - flickable.height)
      var newY = flickable.contentY - wheel.angleDelta.y * 3
      if (newY < 0) {
        flickable.contentY = Math.max(newY, -30)
        bounceAnim.to = 0
        bounceAnim.restart()
      } else if (newY > maxY) {
        flickable.contentY = Math.min(newY, maxY + 30)
        bounceAnim.to = maxY
        bounceAnim.restart()
      } else {
        flickable.contentY = newY
      }
      wheel.accepted = true
    }
  }
}
