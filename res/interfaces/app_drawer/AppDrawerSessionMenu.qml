/*
 * AppDrawerSessionMenu.qml
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

Popup {
  id: root

  padding: 0
  closePolicy: Popup.CloseOnEscape

  enter: Transition {
    ParallelAnimation {
      NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 180; easing.type: Easing.OutCubic }
      NumberAnimation { property: "y";       from: root.y + 12; to: root.y; duration: 180; easing.type: Easing.OutCubic }
    }
  }

  exit: Transition {
    ParallelAnimation {
      NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 120; easing.type: Easing.InCubic }
      NumberAnimation { property: "y";       from: root.y; to: root.y + 12; duration: 120; easing.type: Easing.InCubic }
    }
  }

  signal confirmActionRequested(string action, string label)

  background: Rectangle {
    color: Theme.surface_container_low
    radius: 12
    border.color: Theme.primary
    border.width: 1
  }

  contentItem: Column {
    spacing: 0

    // Lock
    Item {
      width: 200
      height: 48
      clip: true

      Rectangle {
        id: lockStateLayer
        anchors.fill: parent
        topLeftRadius: 12
        topRightRadius: 12
        color: "transparent"
      }

      Rectangle {
        id: lockRipple
        property real cx: 0
        property real cy: 0
        property real r: 0
        x: cx - r; y: cy - r
        width: r * 2; height: r * 2
        radius: r
        color: Theme.state_layer_pressed
        opacity: 0

        ParallelAnimation {
          id: lockRippleAnim
          NumberAnimation {
            target: lockRipple; property: "r"
            from: 0; to: 160
            duration: 400; easing.type: Easing.OutCubic
          }
          NumberAnimation {
            target: lockRipple; property: "opacity"
            from: 1; to: 0
            duration: 400; easing.type: Easing.InCubic
          }
        }
      }

      Row {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 12

        Text {
          anchors.verticalCenter: parent.verticalCenter
          font.family: "Material Symbols Rounded"
          font.pixelSize: 20
          text: "lock"
          color: Theme.surface_fg
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: qsTr("Lock")
          font.pixelSize: 14
          color: Theme.surface_fg
        }
      }

      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered:  lockStateLayer.color = Theme.state_layer_hover
        onExited:   lockStateLayer.color = "transparent"
        onPressed: {
          lockRipple.cx = mouseX
          lockRipple.cy = mouseY
          lockRippleAnim.restart()
        }
        onClicked: {
          root.close()
          root.confirmActionRequested("lock", qsTr("Lock"))
        }
      }
    }

    // Logout
    Item {
      width: 200
      height: 48
      clip: true

      Rectangle {
        id: logoutStateLayer
        anchors.fill: parent
        color: "transparent"
      }

      Rectangle {
        id: logoutRipple
        property real cx: 0
        property real cy: 0
        property real r: 0
        x: cx - r; y: cy - r
        width: r * 2; height: r * 2
        radius: r
        color: Theme.state_layer_pressed
        opacity: 0

        ParallelAnimation {
          id: logoutRippleAnim
          NumberAnimation {
            target: logoutRipple; property: "r"
            from: 0; to: 160
            duration: 400; easing.type: Easing.OutCubic
          }
          NumberAnimation {
            target: logoutRipple; property: "opacity"
            from: 1; to: 0
            duration: 400; easing.type: Easing.InCubic
          }
        }
      }

      Row {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 12

        Text {
          anchors.verticalCenter: parent.verticalCenter
          font.family: "Material Symbols Rounded"
          font.pixelSize: 20
          text: "logout"
          color: Theme.surface_fg
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: qsTr("Log Out")
          font.pixelSize: 14
          color: Theme.surface_fg
        }
      }

      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered:  logoutStateLayer.color = Theme.state_layer_hover
        onExited:   logoutStateLayer.color = "transparent"
        onPressed: {
          logoutRipple.cx = mouseX
          logoutRipple.cy = mouseY
          logoutRippleAnim.restart()
        }
        onClicked: {
          root.close()
          root.confirmActionRequested("logout", qsTr("Log Out"))
        }
      }
    }

    // Switch User
    Item {
      width: 200
      height: 48
      clip: true

      Rectangle {
        id: switchUserStateLayer
        anchors.fill: parent
        bottomLeftRadius: 12
        bottomRightRadius: 12
        color: "transparent"
      }

      Rectangle {
        id: switchUserRipple
        property real cx: 0
        property real cy: 0
        property real r: 0
        x: cx - r; y: cy - r
        width: r * 2; height: r * 2
        radius: r
        color: Theme.state_layer_pressed
        opacity: 0

        ParallelAnimation {
          id: switchUserRippleAnim
          NumberAnimation {
            target: switchUserRipple; property: "r"
            from: 0; to: 160
            duration: 400; easing.type: Easing.OutCubic
          }
          NumberAnimation {
            target: switchUserRipple; property: "opacity"
            from: 1; to: 0
            duration: 400; easing.type: Easing.InCubic
          }
        }
      }

      Row {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 12

        Text {
          anchors.verticalCenter: parent.verticalCenter
          font.family: "Material Symbols Rounded"
          font.pixelSize: 20
          text: "switch_account"
          color: Theme.surface_fg
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: qsTr("Switch User")
          font.pixelSize: 14
          color: Theme.surface_fg
        }
      }

      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered:  switchUserStateLayer.color = Theme.state_layer_hover
        onExited:   switchUserStateLayer.color = "transparent"
        onPressed: {
          switchUserRipple.cx = mouseX
          switchUserRipple.cy = mouseY
          switchUserRippleAnim.restart()
        }
        onClicked: {
          root.close()
          root.confirmActionRequested("switchUser", qsTr("Switch User"))
        }
      }
    }
  }
}
