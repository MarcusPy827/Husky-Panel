/*
 * ConfigPanel.qml
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
import QtQuick.Controls
import QtQuick.Layouts

Item {
  id: root
  opacity: 0
  transform: Translate { id: slideTransform; y: -root.slideOffset }

  property bool sidePaneVisible: true
  readonly property int slideOffset: 24
  readonly property Item titleBarItem: titleBarRect
  readonly property Item menuBtnItem: menuBtn
  readonly property Item closeBtnItem: closeBtn
  signal closeAnimationFinished()

  function open() {
    slideTransform.y = -root.slideOffset
    root.opacity = 0
    openAnim.restart()
  }

  function close() {
    closeAnim.restart()
  }

  // Panel open animation
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

  // Panel close animation
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

  property string currentSectionId: "system_tray"
  readonly property var sections: [
    { id: "system_tray", icon: "apps", label: ConfigPanelTranslator.Tr("SystemTray") },
    { id: "about", icon: "info", label: ConfigPanelTranslator.Tr("AboutSystem") }
  ]

  function sectionIndexOf(secId) {
    for (var i = 0; i < sections.length; ++i) {
      if (sections[i].id === secId) return i
    }
    return 0
  }

  // Main frame
  Rectangle {
    id: frame
    anchors.fill: parent
    radius: 12
    color: Theme.surface_bg
    border.color: Theme.primary
    border.width: 2
    layer.enabled: true

    MouseArea { anchors.fill: parent }

    ColumnLayout {
      anchors.fill: parent
      anchors.margins: 2
      spacing: 0

      // Title bar
      Rectangle {
        id: titleBarRect
        Layout.fillWidth: true
        Layout.preferredHeight: 64
        color: Theme.surface_container
        radius: 12

        Rectangle {
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.bottom: parent.bottom
          height: 12
          color: parent.color
        }

        RowLayout {
          anchors.fill: parent
          anchors.leftMargin: 16
          anchors.rightMargin: 16
          spacing: 8

          // Menu button
          Item {
            id: menuBtn
            implicitWidth: 48
            implicitHeight: 48

            Rectangle {
              id: menuBtnBg
              anchors.fill: parent
              radius: 24
              color: "transparent"

              Rectangle {
                id: menuBtnStateLayer
                anchors.fill: parent
                radius: parent.radius
                color: "transparent"
              }

              Canvas {
                id: menuBtnRipple
                anchors.fill: parent
                opacity: 0

                property real centerX: parent.width / 2
                property real centerY: parent.height / 2
                property real rippleRadius: 0

                onPaint: {
                  var ctx = getContext("2d")
                  ctx.clearRect(0, 0, width, height)
                  ctx.save()
                  ctx.beginPath()
                  ctx.arc(width / 2, height / 2, width / 2, 0, 2 * Math.PI)
                  ctx.clip()
                  var c = Theme.state_layer_pressed
                  ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
                  ctx.beginPath()
                  ctx.arc(centerX, centerY, rippleRadius, 0, 2 * Math.PI)
                  ctx.fill()
                  ctx.restore()
                }

                onOpacityChanged: requestPaint()
                onRippleRadiusChanged: requestPaint()

                ParallelAnimation {
                  id: menuBtnRippleAnim
                  NumberAnimation {
                    target: menuBtnRipple; property: "rippleRadius"
                    from: 0; to: menuBtnBg.width * 1.25
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: menuBtnRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: menuBtnRipple; property: "opacity"
                      from: 0.3; to: 0
                      duration: 250; easing.type: Easing.OutCubic
                    }
                  }
                }
              }
            }

            Text {
              anchors.centerIn: parent
              font.family: "Material Symbols Rounded"
              font.pixelSize: 24
              text: "\ue5d2"  // "menu"
              color: Theme.surface_variant_fg
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
              anchors.fill: parent
              hoverEnabled: true
              onEntered: menuBtnStateLayer.color = Theme.state_layer_hover
              onExited:  menuBtnStateLayer.color = "transparent"
              onPressed: (mouse) => {
                menuBtnStateLayer.color = Theme.state_layer_pressed
                menuBtnRipple.centerX = mouse.x
                menuBtnRipple.centerY = mouse.y
                menuBtnRippleAnim.restart()
              }
              onReleased: menuBtnStateLayer.color =
                containsMouse ? Theme.state_layer_hover : "transparent"
              onClicked: root.sidePaneVisible = !root.sidePaneVisible
            }
          }

          // Panel title
          Text {
            Layout.fillWidth: true
            text: ConfigPanelTranslator.Tr("Preferences")
            color: Theme.surface_variant_fg
            font.pixelSize: 18
            font.weight: Font.Medium
            verticalAlignment: Text.AlignVCenter
          }

          // Spacer
          Item {
            implicitHeight: 48
            Layout.fillWidth: true
          }

          // Close button
          Item {
            id: closeBtn
            implicitWidth: 48
            implicitHeight: 48

            Rectangle {
              id: closeBtnBg
              anchors.fill: parent
              radius: 24
              color: "transparent"

              Rectangle {
                id: closeBtnStateLayer
                anchors.fill: parent
                radius: parent.radius
                color: "transparent"
              }

              Canvas {
                id: closeBtnRipple
                anchors.fill: parent
                opacity: 0

                property real centerX: parent.width / 2
                property real centerY: parent.height / 2
                property real rippleRadius: 0

                onPaint: {
                  var ctx = getContext("2d")
                  ctx.clearRect(0, 0, width, height)
                  ctx.save()
                  ctx.beginPath()
                  ctx.arc(width / 2, height / 2, width / 2, 0, 2 * Math.PI)
                  ctx.clip()
                  var c = Theme.state_layer_pressed
                  ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, 1.0)
                  ctx.beginPath()
                  ctx.arc(centerX, centerY, rippleRadius, 0, 2 * Math.PI)
                  ctx.fill()
                  ctx.restore()
                }

                onOpacityChanged: requestPaint()
                onRippleRadiusChanged: requestPaint()

                ParallelAnimation {
                  id: closeBtnRippleAnim
                  NumberAnimation {
                    target: closeBtnRipple; property: "rippleRadius"
                    from: 0; to: closeBtnBg.width * 1.25
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: closeBtnRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: closeBtnRipple; property: "opacity"
                      from: 0.3; to: 0
                      duration: 250; easing.type: Easing.OutCubic
                    }
                  }
                }
              }
            }

            Text {
              anchors.centerIn: parent
              font.family: "Material Symbols Rounded"
              font.pixelSize: 24
              text: "\ue5cd"  // "close"
              color: Theme.surface_variant_fg
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
              anchors.fill: parent
              hoverEnabled: true
              onEntered: closeBtnStateLayer.color = Theme.state_layer_hover
              onExited:  closeBtnStateLayer.color = "transparent"
              onPressed: (mouse) => {
                closeBtnStateLayer.color = Theme.state_layer_pressed
                closeBtnRipple.centerX = mouse.x
                closeBtnRipple.centerY = mouse.y
                closeBtnRippleAnim.restart()
              }
              onReleased: closeBtnStateLayer.color =
                containsMouse ? Theme.state_layer_hover : "transparent"
              onClicked: root.close()
            }
          }
        }
      }

      // Main contents
      RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: 0

        // Side pane
        Rectangle {
          id: sidePaneContainer
          clip: true
          visible: root.sidePaneVisible || Layout.preferredWidth > 0
          Layout.preferredWidth: root.sidePaneVisible ? 220 : 0
          Layout.fillHeight: true
          color: Theme.surface_container_low
          bottomLeftRadius: 12

          Behavior on Layout.preferredWidth {
            NumberAnimation { duration: 200; easing.type: Easing.InOutCubic }
          }

          Flickable {
            id: sidePaneFlickable
            anchors.fill: parent
            anchors.topMargin: 16
            anchors.bottomMargin: 16
            contentHeight: sidePaneColumn.height
            clip: true
            boundsBehavior: Flickable.DragAndOvershootBounds

            ScrollBar.vertical: ScrollBar {
              policy: ScrollBar.AsNeeded
            }

            // Sliding highlight pill
            Rectangle {
              id: sidePaneIndicator
              x: 16
              width: sidePaneColumn.width - 32
              height: 48
              radius: 24
              color: Theme.secondary_container

              Component.onCompleted: y = root.sectionIndexOf(root.currentSectionId) * 54

              Connections {
                target: root
                function onCurrentSectionIdChanged() {
                  indicatorMoveAnim.to = root.sectionIndexOf(root.currentSectionId) * 54
                  indicatorAnim.restart()
                }
              }

              ParallelAnimation {
                id: indicatorAnim
                NumberAnimation {
                  id: indicatorMoveAnim
                  target: sidePaneIndicator; property: "y"
                  duration: 300; easing.type: Easing.OutCubic
                }
                SequentialAnimation {
                  NumberAnimation {
                    target: sidePaneIndicator; property: "height"
                    from: 48; to: 64
                    duration: 120; easing.type: Easing.OutCubic
                  }
                  NumberAnimation {
                    target: sidePaneIndicator; property: "height"
                    from: 64; to: 48
                    duration: 250; easing.type: Easing.OutCubic
                  }
                }
              }
            }

            Column {
              id: sidePaneColumn
              width: parent.width
              padding: 16
              topPadding: 0
              spacing: 6

              Repeater {
                model: root.sections
                delegate: ConfigPanelSidePaneItem {
                  required property var modelData
                  width: sidePaneColumn.width - 32
                  icon: modelData.icon
                  label: modelData.label
                  isActive: root.currentSectionId === modelData.id
                  onClicked: root.currentSectionId = modelData.id
                }
              }
            }
          }

          NumberAnimation {
            id: sidePaneBounceAnim
            target: sidePaneFlickable
            property: "contentY"
            duration: 600
            easing.type: Easing.OutBack
            easing.overshoot: 0.5
          }

          MouseArea {
            anchors.fill: parent
            z: 1
            acceptedButtons: Qt.NoButton
            onWheel: function(wheel) {
              sidePaneBounceAnim.stop()
              var maxY = Math.max(0, sidePaneFlickable.contentHeight - sidePaneFlickable.height)
              var newY = sidePaneFlickable.contentY - wheel.angleDelta.y * 3
              if (newY < 0) {
                sidePaneFlickable.contentY = Math.max(newY, -30)
                sidePaneBounceAnim.to = 0
                sidePaneBounceAnim.restart()
              } else if (newY > maxY) {
                sidePaneFlickable.contentY = Math.min(newY, maxY + 30)
                sidePaneBounceAnim.to = maxY
                sidePaneBounceAnim.restart()
              } else {
                sidePaneFlickable.contentY = newY
              }
              wheel.accepted = true
            }
          }
        }

        // Content area
        Rectangle {
          Layout.fillWidth: true
          Layout.fillHeight: true
          color: Theme.surface_container_low
          bottomRightRadius: 12

          Loader {
            anchors.fill: parent
            source: {
              switch (root.currentSectionId) {
                case "system_tray": return "ConfigPanelSystemTrayPage.qml"
                default: return ""
              }
            }
          }
        }
      }
    }
  }
}
