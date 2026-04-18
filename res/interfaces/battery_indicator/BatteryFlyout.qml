/*
 * BatteryFlyout.qml
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
    color: Theme.surface_bg        // qmllint disable unqualified
    border.color: Theme.primary    // qmllint disable unqualified
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
        Layout.preferredHeight: headerRow.implicitHeight + 24
        Layout.topMargin: 3
        Layout.leftMargin: 3
        Layout.rightMargin: 3
        color: Theme.surface_container // qmllint disable unqualified
        topLeftRadius: 10
        topRightRadius: 10

        RowLayout {
          id: headerRow
          anchors {
            left: parent.left; right: parent.right
            top: parent.top
            leftMargin: 16; rightMargin: 16; topMargin: 12
          }
          spacing: 8

          Text {
            text: BatteryTranslator.Tr("Battery")
            color: Theme.primary // qmllint disable unqualified
            font.pixelSize: 16
            font.weight: Font.Bold
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
            Layout.preferredHeight: 32
          }
        }
      }

      // Builtin Battery
      Item {
        Layout.fillWidth: true
        implicitHeight: builtinCol.implicitHeight
        visible: BatteryHandler.has_builtin_battery // qmllint disable unqualified

        Column {
          id: builtinCol
          anchors.left: parent.left
          anchors.right: parent.right
          spacing: 0

          Item { width: 1; height: 12 }

          Text {
            x: 16
            text: BatteryTranslator.Tr("BuiltInBattery")
            color: Theme.primary // qmllint disable unqualified
            font.pixelSize: 12
            font.weight: Font.Medium
          }

          Item { width: 1; height: 6 }

          Flickable {
            x: 8
            width: parent.width - 16
            implicitHeight: Math.min(builtinItems.implicitHeight, 200)
            contentHeight: builtinItems.implicitHeight
            clip: true

            Column {
              id: builtinItems
              width: parent.width
              spacing: 6

              Repeater {
                model: BatteryHandler.builtin_batteries // qmllint disable unqualified

                delegate: Rectangle {
                  id: builtinEntry
                  required property var modelData

                  width: builtinItems.width
                  height: builtinRow.implicitHeight + 20
                  radius: 12
                  color: Theme.surface_container // qmllint disable unqualified

                  RowLayout {
                    id: builtinRow
                    anchors {
                      left: parent.left; right: parent.right
                      verticalCenter: parent.verticalCenter
                      leftMargin: 14; rightMargin: 14
                    }
                    spacing: 12

                    Text {
                      readonly property int pct:
                        builtinEntry.modelData["percentage"] ?? 0
                      text: builtinEntry.modelData["is_charging"]
                        ? "battery_android_full"
                        : (pct <= 20  ? "battery_android_alert"
                          : pct <= 50 ? "battery_android_frame_3"
                          :             "battery_android_frame_6")
                      color: Theme.primary // qmllint disable unqualified
                      font.pixelSize: 24
                      font.family: "Material Symbols Rounded"
                      Layout.alignment: Qt.AlignVCenter
                    }

                    Text {
                      text: builtinEntry.modelData["name"] ?? ""
                      font.pixelSize: 14
                      font.weight: Font.Medium
                      color: Theme.surface_fg // qmllint disable unqualified
                      Layout.fillWidth: true
                      elide: Text.ElideRight
                    }

                    ColumnLayout {
                      spacing: 2
                      Layout.alignment: Qt.AlignVCenter

                      RowLayout {
                        spacing: 0
                        Text {
                          text: BatteryTranslator.Tr("EstimatedTime")
                          font.pixelSize: 12
                          color: Theme.surface_variant_fg // qmllint disable unqualified
                        }

                        Text {
                          text: builtinEntry.modelData["time_remaining"] ?? "Unknown"
                          font.pixelSize: 12
                          color: Theme.surface_fg // qmllint disable unqualified
                        }

                        Text {
                          text: "  ·  "
                          font.pixelSize: 12
                          color: Theme.surface_variant_fg // qmllint disable unqualified
                        }

                        Text {
                          text: (builtinEntry.modelData["percentage"] ?? 0) + "%"
                          font.pixelSize: 12
                          color: Theme.surface_fg // qmllint disable unqualified
                        }
                      }

                      RowLayout {
                        spacing: 0
                        Text {
                          text: BatteryTranslator.Tr("Health")
                          font.pixelSize: 12
                          color: Theme.surface_variant_fg // qmllint disable unqualified
                        }

                        Text {
                          text: builtinEntry.modelData["health"] ?? "Unknown"
                          font.pixelSize: 12
                          color: Theme.surface_fg // qmllint disable unqualified
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

      // Performance Profile
      Item {
        Layout.fillWidth: true
        implicitHeight: perfCol.implicitHeight
        z: 10

        Column {
          id: perfCol
          anchors.left: parent.left
          anchors.right: parent.right
          spacing: 0

          Item { width: 1; height: 12 }

          Text {
            x: 16
            text: BatteryTranslator.Tr("PerformanceProfile")
            color: Theme.primary  // qmllint disable unqualified
            font.pixelSize: 12
            font.weight: Font.Medium
          }

          Item { width: 1; height: 6 }

          // Dropdown
          Item {
            id: profileSelector
            x: 8
            width: parent.width - 16
            height: 52
            z: 10

            property bool expanded: false
            readonly property var options: ["ECO", "Balanced", "Performance"]

            function labelFor(key) {
              if (key === "ECO")         return BatteryTranslator.Tr("ECO")         // qmllint disable unqualified
              if (key === "Balanced")    return BatteryTranslator.Tr("Balanced")    // qmllint disable unqualified
              if (key === "Performance") return BatteryTranslator.Tr("Performance") // qmllint disable unqualified
              return key
            }

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
                  readonly property string icon: {
                    var p = BatteryHandler.performance_profile // qmllint disable unqualified
                    if (p === "Performance") return "speed"
                    if (p === "ECO") return "nest_eco_leaf"
                    return "electrical_services"
                  }
                  text: icon
                  font.family: "Material Symbols Rounded"
                  font.pixelSize: 20
                  color: Theme.primary  // qmllint disable unqualified
                }

                Text {
                  Layout.fillWidth: true
                  text: profileSelector.labelFor(BatteryHandler.performance_profile)  // qmllint disable unqualified
                  color: Theme.surface_fg  // qmllint disable unqualified
                  font.pixelSize: 14
                  verticalAlignment: Text.AlignVCenter
                }

                Text {
                  id: chevron
                  text: "expand_more"
                  font.family: "Material Symbols Rounded"
                  font.pixelSize: 18
                  color: Theme.surface_variant_fg  // qmllint disable unqualified

                  transform: Rotation {
                    origin.x: chevron.width / 2
                    origin.y: chevron.height / 2
                    angle: profileSelector.expanded ? 180 : 0
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
                onClicked: profileSelector.expanded = !profileSelector.expanded
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
              color: Theme.surface_container  // qmllint disable unqualified
              border.color: Theme.primary  // qmllint disable unqualified
              border.width: 1
              clip: true

              visible: opacity > 0
              opacity: profileSelector.expanded ? 1.0 : 0.0
              Behavior on opacity {
                NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
              }

              property real slideOffset: profileSelector.expanded ? 0 : -8
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
                  model: profileSelector.options

                  delegate: Rectangle {
                    id: dropItem
                    required property string modelData

                    width: dropCol.width
                    height: 44
                    radius: 8
                    clip: true
                    color: BatteryHandler.performance_profile === dropItem.modelData  // qmllint disable unqualified
                      ? Theme.primary_container  // qmllint disable unqualified
                      : "transparent"

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

                    RowLayout {
                      anchors {
                        left: parent.left; right: parent.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: 12; rightMargin: 12
                      }
                      spacing: 8

                      Text {
                        readonly property string icon: {
                          if (dropItem.modelData === "Performance") return "speed"
                          if (dropItem.modelData === "ECO") return "nest_eco_leaf"
                          return "electrical_services"
                        }
                        text: icon
                        font.family: "Material Symbols Rounded"
                        font.pixelSize: 18
                        color: BatteryHandler.performance_profile === dropItem.modelData  // qmllint disable unqualified
                          ? Theme.primary  // qmllint disable unqualified
                          : Theme.surface_variant_fg  // qmllint disable unqualified
                      }

                      Text {
                        Layout.fillWidth: true
                        text: profileSelector.labelFor(dropItem.modelData)
                        font.pixelSize: 13
                        color: BatteryHandler.performance_profile === dropItem.modelData  // qmllint disable unqualified
                          ? Theme.primary  // qmllint disable unqualified
                          : Theme.surface_fg  // qmllint disable unqualified
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                      }
                    }

                    MouseArea {
                      anchors.fill: parent
                      onPressed: (mouse) => {
                        itemRipple.cx = mouse.x
                        itemRipple.cy = mouse.y
                        itemRippleAnim.restart()
                      }
                      onClicked: {
                        BatteryHandler.setPerformanceProfile(dropItem.modelData)  // qmllint disable unqualified
                        profileSelector.expanded = false
                      }
                    }
                  }
                }
              }
            }
          }

        }
      }

      // Prevent system sleep
      Item {
        Layout.fillWidth: true
        implicitHeight: sleepCol.implicitHeight

        Column {
          id: sleepCol
          anchors.left: parent.left
          anchors.right: parent.right
          spacing: 0

          Item { width: 1; height: 12 }

          Text {
            x: 16
            text: BatteryTranslator.Tr("PreventSystemSleep")
            color: Theme.primary  // qmllint disable unqualified
            font.pixelSize: 12
            font.weight: Font.Medium
          }

          Item { width: 1; height: 6 }

          Rectangle {
            x: 8
            width: parent.width - 16
            height: sleepRow.implicitHeight + 20
            radius: 12
            color: Theme.surface_container  // qmllint disable unqualified

            RowLayout {
              id: sleepRow
              anchors {
                left: parent.left; right: parent.right
                verticalCenter: parent.verticalCenter
                leftMargin: 14; rightMargin: 14
              }
              spacing: 12

              Text {
                text: BatteryTranslator.Tr("PreventSystemAutoSleepNLock")
                font.pixelSize: 13
                color: Theme.surface_fg  // qmllint disable unqualified
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
              }

              Item {
                id: sleepSwitch
                implicitWidth: 52
                implicitHeight: 32
                Layout.alignment: Qt.AlignVCenter

                readonly property bool checked: BatteryHandler.is_sleep_inhibited  // qmllint disable unqualified
                property bool isHovered: false
                property bool isPressed: false

                readonly property real handleSize: isPressed ? 28 : (checked ? 24 : 16)
                readonly property real handleCenterX: checked ? 36 : 16
                readonly property color handleColor:
                  checked
                    ? ((isHovered || isPressed) ? Theme.primary_container  // qmllint disable unqualified
                        : Theme.surface_bg)  // qmllint disable unqualified
                    : Theme.surface_variant_fg  // qmllint disable unqualified
                readonly property color stateLayerColor: {
                  var c = checked ? Theme.primary : Theme.surface_fg // qmllint disable unqualified
                  if (isPressed) return Qt.rgba(c.r, c.g, c.b, 0.10)
                  if (isHovered) return Qt.rgba(c.r, c.g, c.b, 0.08)
                  return "transparent"
                }

                Rectangle {
                  anchors.fill: parent
                  radius: 16
                  color: sleepSwitch.checked
                    ? Theme.primary  // qmllint disable unqualified
                    : Theme.surface_container_high  // qmllint disable unqualified
                  border.width: sleepSwitch.checked ? 0 : 2
                  border.color: Theme.surface_variant_fg // qmllint disable unqualified
                  Behavior on color { ColorAnimation { duration: 200; easing.type: Easing.OutCubic } }
                  Behavior on border.width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                }

                Rectangle {
                  width: 40; height: 40; radius: 20
                  anchors.verticalCenter: parent.verticalCenter
                  x: sleepSwitch.handleCenterX - 20
                  color: sleepSwitch.stateLayerColor
                  Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                  Behavior on color { ColorAnimation  { duration: 150; easing.type: Easing.OutCubic } }
                }

                Rectangle {
                  width: sleepSwitch.handleSize; height: width; radius: width / 2
                  anchors.verticalCenter: parent.verticalCenter
                  x: sleepSwitch.handleCenterX - sleepSwitch.handleSize / 2
                  color: sleepSwitch.handleColor
                  Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                  Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                  Behavior on color { ColorAnimation  { duration: 200; easing.type: Easing.OutCubic } }
                }

                MouseArea {
                  anchors.fill: parent
                  hoverEnabled: true
                  onEntered:  sleepSwitch.isHovered = true
                  onExited:   { sleepSwitch.isHovered = false; sleepSwitch.isPressed = false }
                  onPressed:  sleepSwitch.isPressed = true
                  onReleased: sleepSwitch.isPressed = false
                  onClicked:  BatteryHandler.setSleepInhibited(!sleepSwitch.checked)  // qmllint disable unqualified
                }
              }
            }
          }
        }
      }

      // Other devices
      Item {
        Layout.fillWidth: true
        implicitHeight: externalCol.implicitHeight
        visible: BatteryHandler.external_batteries.length > 0  // qmllint disable unqualified

        Column {
          id: externalCol
          anchors.left: parent.left
          anchors.right: parent.right
          spacing: 0

          Item { width: 1; height: 12 }

          Text {
            x: 16
            text: BatteryTranslator.Tr("OtherDevices")
            color: Theme.primary  // qmllint disable unqualified
            font.pixelSize: 12
            font.weight: Font.Medium
          }

          Item { width: 1; height: 6 }

          Flickable {
            x: 8
            width: parent.width - 16
            implicitHeight: Math.min(externalItems.implicitHeight, 200)
            contentHeight: externalItems.implicitHeight
            clip: true

            Column {
              id: externalItems
              width: parent.width
              spacing: 6

              Repeater {
                model: BatteryHandler.external_batteries  // qmllint disable unqualified

                delegate: Rectangle {
                  id: extEntry
                  required property var modelData

                  width: externalItems.width
                  height: extRow.implicitHeight + 20
                  radius: 12
                  color: Theme.surface_container  // qmllint disable unqualified

                  RowLayout {
                    id: extRow
                    anchors {
                      left: parent.left; right: parent.right
                      verticalCenter: parent.verticalCenter
                      leftMargin: 14; rightMargin: 14
                    }
                    spacing: 12

                    Text {
                      text: extEntry.modelData["icon"] ?? "battery_android_frame_question"
                      color: Theme.primary  // qmllint disable unqualified
                      font.pixelSize: 22
                      font.family: "Material Symbols Rounded"
                      Layout.alignment: Qt.AlignVCenter
                    }

                    Text {
                      text: extEntry.modelData["name"] ?? ""
                      font.pixelSize: 14
                      font.weight: Font.Medium
                      color: Theme.surface_fg  // qmllint disable unqualified
                      Layout.fillWidth: true
                      elide: Text.ElideRight
                    }

                    Text {
                      text: (extEntry.modelData["percentage"] ?? 0) + "%"
                      font.pixelSize: 14
                      color: Theme.surface_variant_fg  // qmllint disable unqualified
                    }
                  }
                }
              }
            }
          }
        }
      }

      // Bottom spacer
      Item { Layout.preferredHeight: 8 }
    }
  }
}
