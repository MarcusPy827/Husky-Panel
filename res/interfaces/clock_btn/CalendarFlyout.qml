/*
 * CalendarFlyout.qml
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
  implicitHeight: layout.implicitHeight

  Behavior on implicitHeight {
    NumberAnimation {
      duration: 200
      easing.type: Easing.OutCubic
    }
  }

  readonly property int slideOffset: 24
  readonly property int headerH: 108
  readonly property int cellSize: 44
  readonly property int _rowCount: Math.ceil(
    ((new Date(_viewYear, _viewMonth, 1).getDay() + 6) % 7
      + new Date(_viewYear, _viewMonth + 1, 0).getDate()) / 7)

  transform: Translate {
    id: slideTransform
    y: -root.slideOffset
  }

  signal closeAnimationFinished()

  property var _today: new Date()
  property int _viewYear: new Date().getFullYear()
  property int _viewMonth: new Date().getMonth()

  function open() {
    root._today = new Date()
    root._viewYear = root._today.getFullYear()
    root._viewMonth = root._today.getMonth()
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

  // Main frame
  Rectangle {
    id: frame
    anchors.fill: parent
    radius: 12
    color: Theme.surface_bg
    border.color: Theme.primary
    border.width: 2
    layer.enabled: true
    clip: true

    MouseArea {
      anchors.fill: parent
    }

    ColumnLayout {
      id: layout
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: parent.top
      spacing: 6

      // Header frame
      Rectangle {
        id: calendar_flyout_header
        Layout.fillWidth: true
        Layout.preferredHeight: root.headerH
        Layout.topMargin: 3
        Layout.bottomMargin: 2
        Layout.leftMargin: 3
        Layout.rightMargin: 3
        color: Theme.surface_container
        topLeftRadius: 12
        topRightRadius: 12

        ColumnLayout {
          anchors.fill: parent
          anchors.topMargin: 12
          anchors.bottomMargin: 12
          anchors.leftMargin: 3
          anchors.rightMargin: 3

          // Today
          Text {
            Layout.fillWidth: true
            Layout.leftMargin: 9
            Layout.rightMargin: 9
            text: Qt.formatDate(root._today, StatusBarTranslator.Tr("yyyy-M-") + StatusBarTranslator.Tr("dd")) + " " + StatusBarTranslator.Tr(Qt.formatDate(root._today, "ddd"))
            color: Theme.primary
            font.pixelSize: 16
            font.bold: true
            horizontalAlignment: Text.AlignLeft
          }

          // Month navigation
          RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 9
            Layout.rightMargin: 9
            spacing: 0

            // Prev month
            Rectangle {
              implicitWidth: 32; implicitHeight: 32
              radius: 16
              color: prevMa.containsMouse ? Theme.state_layer_hover : "transparent"

              Canvas {
                id: prevRipple
                anchors.fill: parent
                opacity: 0

                property real centerX: width / 2
                property real centerY: height / 2
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
                  id: prevRippleAnim
                  NumberAnimation {
                    target: prevRipple; property: "rippleRadius"
                    from: 0
                    to: Math.sqrt(prevRipple.width * prevRipple.width + prevRipple.height * prevRipple.height)
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: prevRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: prevRipple; property: "opacity"
                      from: 0.3; to: 0
                      duration: 250; easing.type: Easing.OutCubic
                    }
                  }
                }
              }

              Text {
                anchors.centerIn: parent
                text: "chevron_left"
                color: Theme.surface_fg
                font.pixelSize: 16
                font.family: "Material Symbols Rounded"
              }

              MouseArea {
                id: prevMa
                anchors.fill: parent
                hoverEnabled: true
                onPressed: {
                  prevRipple.centerX = mouseX
                  prevRipple.centerY = mouseY
                  prevRippleAnim.restart()
                }
                onClicked: {
                  if (root._viewMonth === 0) {
                    root._viewMonth = 11
                    root._viewYear--
                  }
                  else root._viewMonth--
                }
              }
            }

            Item {
              Layout.fillWidth: true
            }

            Text {
              text: Qt.formatDate(new Date(root._viewYear, root._viewMonth, 1), StatusBarTranslator.Tr("yyyy-M"))
              color: Theme.surface_fg
              font.pixelSize: 16
              font.bold: true
              horizontalAlignment: Text.AlignHCenter
            }

            Item {
              Layout.fillWidth: true
            }

            // Next month
            Rectangle {
              implicitWidth: 32; implicitHeight: 32
              radius: 16
              color: nextMa.containsMouse ? Theme.state_layer_hover : "transparent"

              Canvas {
                id: nextRipple
                anchors.fill: parent
                opacity: 0

                property real centerX: width / 2
                property real centerY: height / 2
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
                  id: nextRippleAnim
                  NumberAnimation {
                    target: nextRipple; property: "rippleRadius"
                    from: 0
                    to: Math.sqrt(nextRipple.width * nextRipple.width + nextRipple.height * nextRipple.height)
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: nextRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: nextRipple; property: "opacity"
                      from: 0.3; to: 0
                      duration: 250; easing.type: Easing.OutCubic
                    }
                  }
                }
              }

              Text {
                anchors.centerIn: parent
                text: "chevron_right"
                color: Theme.surface_fg
                font.pixelSize: 16
                font.family: "Material Symbols Rounded"
              }

              MouseArea {
                id: nextMa
                anchors.fill: parent
                hoverEnabled: true
                onPressed: {
                  nextRipple.centerX = mouseX
                  nextRipple.centerY = mouseY
                  nextRippleAnim.restart()
                }
                onClicked: {
                  if (root._viewMonth === 11) {
                    root._viewMonth = 0
                    root._viewYear++
                  }
                  else root._viewMonth++
                }
              }
            }
          }

          // Weekday labels
          GridLayout {
            Layout.fillWidth: true
            columns: 7
            columnSpacing: 2
            rowSpacing: 0

            Repeater {
              model: [
                StatusBarTranslator.Tr("M"),
                StatusBarTranslator.Tr("T"),
                StatusBarTranslator.Tr("W"),
                StatusBarTranslator.Tr("Th"),
                StatusBarTranslator.Tr("F"),
                StatusBarTranslator.Tr("S"),
                StatusBarTranslator.Tr("Su")
              ]

              Text {
                required property string modelData
                required property int index

                Layout.fillWidth: true
                text: modelData
                color: (index >= 5) ? Theme.primary : Theme.surface_variant_fg
                font.pixelSize: 14
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
              }
            }
          }

        }
      }

      // Calendar grid
      GridLayout {
        Layout.fillWidth: true
        Layout.leftMargin: 6
        Layout.rightMargin: 6
        columns: 7
        columnSpacing: 2; rowSpacing: 2

        Repeater {
          model: root._rowCount * 7

          delegate: Item {
            id: dayCell
            required property int index

            Layout.fillWidth: true
            Layout.preferredHeight: root.cellSize

            readonly property int _offset: (new Date(root._viewYear, root._viewMonth, 1).getDay() + 6) % 7
            readonly property int _day:    dayCell.index - _offset + 1
            readonly property int _total:  new Date(root._viewYear, root._viewMonth + 1, 0).getDate()
            readonly property bool _valid: _day >= 1 && _day <= _total
            readonly property bool _isToday: _valid
                && root._today.getFullYear() === root._viewYear
                && root._today.getMonth()    === root._viewMonth
                && root._today.getDate()     === _day

            Rectangle {
              anchors.centerIn: parent
              width:  Math.min(dayCell.width, dayCell.height) - 2
              height: width
              radius: 8
              visible: dayCell._valid
              color: dayCell._isToday
                     ? Theme.primary
                     : (cellMa.containsMouse ? Theme.state_layer_hover : "transparent")

              Canvas {
                id: cellRipple
                anchors.fill: parent
                opacity: 0

                property real centerX: width / 2
                property real centerY: height / 2
                property real rippleRadius: 0

                onPaint: {
                  var ctx = getContext("2d")
                  ctx.clearRect(0, 0, width, height)
                  ctx.save()
                  var r = 8
                  ctx.beginPath()
                  ctx.moveTo(r, 0)
                  ctx.lineTo(width - r, 0)
                  ctx.arcTo(width, 0, width, r, r)
                  ctx.lineTo(width, height - r)
                  ctx.arcTo(width, height, width - r, height, r)
                  ctx.lineTo(r, height)
                  ctx.arcTo(0, height, 0, height - r, r)
                  ctx.lineTo(0, r)
                  ctx.arcTo(0, 0, r, 0, r)
                  ctx.closePath()
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
                  id: cellRippleAnim
                  NumberAnimation {
                    target: cellRipple; property: "rippleRadius"
                    from: 0
                    to: Math.sqrt(cellRipple.width * cellRipple.width + cellRipple.height * cellRipple.height)
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: cellRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: cellRipple; property: "opacity"
                      from: 0.3; to: 0
                      duration: 250; easing.type: Easing.OutCubic
                    }
                  }
                }
              }

              Text {
                anchors.centerIn: parent
                text: dayCell._day
                color: dayCell._isToday ? Theme.surface_bg : Theme.surface_fg
                font.pixelSize: 12
                font.bold: dayCell._isToday
              }

              MouseArea {
                id: cellMa
                anchors.fill: parent
                hoverEnabled: true
                enabled: dayCell._valid
                onPressed: {
                  cellRipple.centerX = mouseX
                  cellRipple.centerY = mouseY
                  cellRippleAnim.restart()
                }
              }
            }
          }
        }
      }

      Item {
        Layout.fillWidth: true
        Layout.preferredHeight: 6
      }
    }
  }
}
