/*
 * ConfigPanelSystemTrayPage.qml
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

  // Extract a human-readable app name from a D-Bus service name.
  // e.g. "org.fcitx.Fcitx5" → "Fcitx5"
  // Returns "" when the service is a D-Bus unique name (":1.391") or
  // when the extracted segment is purely numeric.
  function appNameFromService(svc) {
    var base = svc.split('/')[0]              // strip object path
    if (/^:?\d+\.\d+$/.test(base)) return "" // D-Bus unique name, e.g. ":1.391"
    var parts = base.split('.')
    if (parts.length < 2) return base
    var last = parts[parts.length - 1]
    last = last.replace(/-\d+(-\d+)*$/, '')  // strip trailing -PID-N
    if (last.length === 0 || /^\d+$/.test(last)) return "" // purely numeric → useless
    return last
  }

  // Choose the best display label for a tray icon delegate.
  function displayLabel(tooltip, title, app_id, real_service) {
    if (tooltip.length > 0) return tooltip
    if (title.length  > 0) return title
    // Skip generic Electron / Chrome frame IDs
    if (app_id.length > 0 && !app_id.startsWith("chrome_status_icon"))
      return app_id
    var name = appNameFromService(real_service)
    return name.length > 0 ? name : real_service
  }

  Flickable {
    id: flickable
    anchors.fill: parent
    anchors.margins: 24
    contentHeight: pageColumn.implicitHeight
    clip: true
    boundsBehavior: Flickable.DragAndOvershootBounds

    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AlwaysOff }

    Column {
      id: pageColumn
      width: parent.width
      spacing: 8

      // Page title
      Text {
        text: ConfigPanelTranslator.Tr("SystemTray") // qmllint disable unqualified
        font.pixelSize: 22
        font.weight: Font.Medium
        color: Theme.surface_variant_fg // qmllint disable unqualified
        bottomPadding: 8
      }

      // Section header
      Text {
        text: ConfigPanelTranslator.Tr("ExpandIconConfig") // qmllint disable unqualified
        font.pixelSize: 14
        font.weight: Font.Medium
        color: Theme.primary // qmllint disable unqualified
        topPadding: 4
        bottomPadding: 4
      }

      // Decide whether the "Expanding icon" on the left hand side
      Rectangle {
        width: pageColumn.width
        height: 56
        radius: 12
        color: Theme.surface_container // qmllint disable unqualified

        RowLayout {
          anchors {
            fill: parent
            leftMargin: 16
            rightMargin: 16
          }
          spacing: 12

          // Util desc
          Text {
            Layout.fillWidth: true
            text: ConfigPanelTranslator.Tr("SetExpandIconOnLeftHandSide") // qmllint disable unqualified
            font.pixelSize: 16
            color: Theme.surface_variant_fg // qmllint disable unqualified
            elide: Text.ElideRight
            Layout.alignment: Qt.AlignVCenter
          }

          // Switch
          Item {
            id: switchExpandLeft
            implicitWidth: 52
            implicitHeight: 32
            Layout.alignment: Qt.AlignVCenter

            readonly property bool checked: TrayHandler.expandIconOnLeft // qmllint disable unqualified

            property bool isHovered: false
            property bool isPressed: false

            readonly property real handleSize:
              isPressed ? 28 : (checked ? 24 : 16)
            readonly property real handleCenterX: checked ? 36 : 16
            readonly property color handleColor:
              checked
                ? ((isHovered || isPressed) ? Theme.primary_container   // qmllint disable unqualified
                                            : Theme.surface_bg)         // qmllint disable unqualified
                : Theme.surface_variant_fg                              // qmllint disable unqualified
            readonly property color stateLayerColor: {
              var c = checked ? Theme.primary : Theme.surface_fg // qmllint disable unqualified
              if (isPressed) return Qt.rgba(c.r, c.g, c.b, 0.10)
              if (isHovered) return Qt.rgba(c.r, c.g, c.b, 0.08)
              return "transparent"
            }

            Rectangle {
              anchors.fill: parent
              radius: 16
              color: switchExpandLeft.checked
                ? Theme.primary                 // qmllint disable unqualified
                : Theme.surface_container_high  // qmllint disable unqualified
              border.width: switchExpandLeft.checked ? 0 : 2
              border.color: Theme.surface_variant_fg // qmllint disable unqualified
              Behavior on color        { ColorAnimation { duration: 200; easing.type: Easing.OutCubic } }
              Behavior on border.width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
            }

            Rectangle {
              width: 40; height: 40; radius: 20
              anchors.verticalCenter: parent.verticalCenter
              x: switchExpandLeft.handleCenterX - 20
              color: switchExpandLeft.stateLayerColor
              Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
              Behavior on color { ColorAnimation  { duration: 150; easing.type: Easing.OutCubic } }
            }

            Rectangle {
              width: switchExpandLeft.handleSize
              height: width
              radius: width / 2
              anchors.verticalCenter: parent.verticalCenter
              x: switchExpandLeft.handleCenterX - switchExpandLeft.handleSize / 2
              color: switchExpandLeft.handleColor
              Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
              Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
              Behavior on color { ColorAnimation  { duration: 200; easing.type: Easing.OutCubic } }
            }

            MouseArea {
              anchors.fill: parent
              hoverEnabled: true
              onEntered:  switchExpandLeft.isHovered = true
              onExited:   { switchExpandLeft.isHovered = false; switchExpandLeft.isPressed = false }
              onPressed:  switchExpandLeft.isPressed  = true
              onReleased: switchExpandLeft.isPressed  = false
              onClicked:  TrayHandler.setExpandIconOnLeft(!switchExpandLeft.checked) // qmllint disable unqualified
            }
          }
        }
      }

      // Spacer
      Item {
        Layout.fillWidth: true
        height: 12
      }

      // Section header
      Text {
        text: ConfigPanelTranslator.Tr("FoldIcons") // qmllint disable unqualified
        font.pixelSize: 14
        font.weight: Font.Medium
        color: Theme.primary // qmllint disable unqualified
        topPadding: 4
        bottomPadding: 4
      }

      // Tray icon list
      Repeater {
        model: TrayHandler // qmllint disable unqualified
        delegate: Rectangle {
          id: trayDelegate
          required property string service
          required property string real_service
          required property string title
          required property string app_id
          required property string tooltip
          required property bool   is_visible
          required property bool   is_config_visible
          required property int    icon_revision
          required property int    icon_key

          width: pageColumn.width
          height: 56
          radius: 12
          color: Theme.surface_container // qmllint disable unqualified

          RowLayout {
            anchors {
              fill: parent
              leftMargin: 16
              rightMargin: 16
            }
            spacing: 12

            // Tray icon image
            Image {
              Layout.preferredWidth: 24
              Layout.preferredHeight: 24
              sourceSize: Qt.size(24, 24)
              source: (trayDelegate.icon_key >= 0)
                ? "image://trayicon/" + trayDelegate.icon_key + "/" + trayDelegate.icon_revision
                : ""
              fillMode: Image.PreserveAspectFit
              smooth: true
              cache: false
              Layout.alignment: Qt.AlignVCenter
            }

            // Service name / tooltip
            Text {
              Layout.fillWidth: true
              text: root.displayLabel(
                trayDelegate.tooltip, trayDelegate.title,
                trayDelegate.app_id,  trayDelegate.real_service)
              font.pixelSize: 16
              color: Theme.surface_variant_fg // qmllint disable unqualified
              elide: Text.ElideRight
              Layout.alignment: Qt.AlignVCenter
            }

            // Switch
            Item {
              id: switcherSE
              implicitWidth: 52
              implicitHeight: 32
              Layout.alignment: Qt.AlignVCenter

              // Model state (read-only, driven by TrayHandler)
              readonly property bool checked: trayDelegate.is_config_visible

              // Interaction state
              property bool isHovered: false
              property bool isPressed: false

              readonly property real handleSize:
                isPressed ? 28 : (checked ? 24 : 16)

              readonly property real handleCenterX: checked ? 36 : 16
              readonly property color handleColor:
                checked
                  ? ((isHovered || isPressed) ? Theme.primary_container    // qmllint disable unqualified
                                              : Theme.surface_bg)          // qmllint disable unqualified
                  : Theme.surface_variant_fg                               // qmllint disable unqualified

              readonly property color stateLayerColor: {
                var c = checked ? Theme.primary : Theme.surface_fg // qmllint disable unqualified
                if (isPressed)  return Qt.rgba(c.r, c.g, c.b, 0.10)
                if (isHovered)  return Qt.rgba(c.r, c.g, c.b, 0.08)
                return "transparent"
              }

              Rectangle {
                anchors.fill: parent
                radius: 16
                color: switcherSE.checked
                  ? Theme.primary                 // qmllint disable unqualified
                  : Theme.surface_container_high  // qmllint disable unqualified
                border.width: switcherSE.checked ? 0 : 2
                border.color: Theme.surface_variant_fg // qmllint disable unqualified

                Behavior on color        { ColorAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Behavior on border.width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
              }

              // State layer
              Rectangle {
                id: switchStateLayer
                width: 40; height: 40; radius: 20
                anchors.verticalCenter: parent.verticalCenter
                x: switcherSE.handleCenterX - 20
                color: switcherSE.stateLayerColor

                Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Behavior on color { ColorAnimation  { duration: 150; easing.type: Easing.OutCubic } }
              }

              // Handle
              Rectangle {
                id: switchHandle
                width: switcherSE.handleSize
                height: width
                radius: width / 2
                anchors.verticalCenter: parent.verticalCenter
                x: switcherSE.handleCenterX - switcherSE.handleSize / 2
                color: switcherSE.handleColor

                Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Behavior on color { ColorAnimation  { duration: 200; easing.type: Easing.OutCubic } }
              }

              MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered:  switcherSE.isHovered = true
                onExited:   { switcherSE.isHovered = false; switcherSE.isPressed = false }
                onPressed:  switcherSE.isPressed  = true
                onReleased: switcherSE.isPressed  = false
                onClicked:  TrayHandler.setConfigVisible( // qmllint disable unqualified
                  trayDelegate.service, !switcherSE.checked)
              }
            }
          }
        }
      }

      // Empty state
      Item {
        width: pageColumn.width
        height: 80
        visible: TrayHandler.rowCount() === 0 // qmllint disable unqualified

        Text {
          anchors.centerIn: parent
          text: "No tray icons active"
          font.pixelSize: 14
          color: Theme.surface_variant_fg // qmllint disable unqualified
          opacity: 0.5
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
