/*
 * AppDrawerGridPage.qml
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

Item {
  id: root

  property var appModel: []
  signal appLaunched()

  Flickable {
    id: flickable
    anchors.fill: parent
    clip: true
    contentHeight: grid.y + grid.height + 16
    boundsBehavior: Flickable.DragAndOvershootBounds

    ScrollBar.vertical: ScrollBar {
      policy: ScrollBar.AsNeeded
    }

    GridView {
      id: grid
      x: 16
      y: 16
      width: flickable.width - 32
      height: contentHeight
      cellWidth: 112
      cellHeight: 112
      model: root.appModel
      interactive: false

      delegate: Item {
        id: delegateItem
        required property var modelData

        width: 112
        height: 112

        Rectangle {
          id: iconBg
          width: 64
          height: 64
          radius: 16
          color: Theme.surface_container_low
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: parent.top
          anchors.topMargin: 8

          Image {
            anchors {
              fill: parent
              margins: 8
            }
            source: delegateItem.modelData.icon_name
              ? ("image://icon/" + delegateItem.modelData.icon_name) : ""
            fillMode: Image.PreserveAspectFit
            smooth: true
          }
        }

        Text {
          width: 104
          anchors.top: iconBg.bottom
          anchors.topMargin: 6
          anchors.horizontalCenter: parent.horizontalCenter
          text: delegateItem.modelData.name
          font.pixelSize: 12
          color: Theme.surface_fg
          horizontalAlignment: Text.AlignHCenter
          elide: Text.ElideRight
          maximumLineCount: 2
          wrapMode: Text.WordWrap
        }

        Canvas {
          id: rippleContainer
          anchors.fill: parent

          property real pressX: width / 2
          property real pressY: height / 2
          property real rippleR: 0
          property real rippleOpacity: 0
          property bool rippleReleased: false

          onRippleRChanged: requestPaint()
          onRippleOpacityChanged: requestPaint()

          onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            if (rippleOpacity <= 0 || rippleR <= 0) return
            ctx.save()
            var r = 8, w = width, h = height
            ctx.beginPath()
            ctx.moveTo(r, 0); ctx.lineTo(w - r, 0)
            ctx.arcTo(w, 0, w, r, r)
            ctx.lineTo(w, h - r)
            ctx.arcTo(w, h, w - r, h, r)
            ctx.lineTo(r, h)
            ctx.arcTo(0, h, 0, h - r, r)
            ctx.lineTo(0, r)
            ctx.arcTo(0, 0, r, 0, r)
            ctx.closePath()
            ctx.clip()
            var c = Theme.surface_fg
            ctx.fillStyle = Qt.rgba(c.r, c.g, c.b, rippleOpacity)
            ctx.beginPath()
            ctx.arc(pressX, pressY, rippleR, 0, Math.PI * 2)
            ctx.fill()
            ctx.restore()
          }

          NumberAnimation {
            id: rippleExpandAnim
            target: rippleContainer; property: "rippleR"
            from: 0; to: 160
            duration: 500; easing.type: Easing.OutQuad
            onStopped: {
              if (rippleContainer.rippleReleased) rippleFadeAnim.restart()
            }
          }

          NumberAnimation {
            id: rippleFadeAnim
            target: rippleContainer; property: "rippleOpacity"
            to: 0; duration: 300; easing.type: Easing.OutQuad
          }

          MouseArea {
            anchors.fill: parent

            onPressed: function(mouse) {
              rippleContainer.rippleReleased = false
              rippleContainer.pressX = mouse.x
              rippleContainer.pressY = mouse.y
              rippleContainer.rippleOpacity = 0.12
              rippleFadeAnim.stop()
              rippleExpandAnim.restart()
            }

            onReleased: {
              rippleContainer.rippleReleased = true
              if (!rippleExpandAnim.running) rippleFadeAnim.restart()
            }

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
