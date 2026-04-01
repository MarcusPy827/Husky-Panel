/*
 * AppDrawer.qml
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
  opacity: 0
  transform: Translate { id: slideTransform; y: -root.slideOffset }

  property bool sidePaneVisible: true
  readonly property int slideOffset: 24
  signal closeAnimationFinished()
  signal confirmActionRequested(string action, string label)

  function open() {
    slideTransform.y = -root.slideOffset
    root.opacity = 0
    openAnim.restart()
  }

  function close() {
    sessionBtn.closeMenu()
    closeAnim.restart()
  }

  // Panel open animination
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

  property string currentCategoryId: "all_apps"
  property string searchText: ""

  // Incremented whenever the app list refreshes; used by category bindings to re-evaluate.
  property int appRevision: 0
  readonly property var watchAllApps: typeof AppProvider !== "undefined" ? AppProvider.allApps : [] // qmllint disable unqualified
  onWatchAllAppsChanged: appRevision++
  readonly property var categories: [
    { id: "all_apps", icon: "apps", label: DrawerTranslator.Tr("AllApps") },
    { id: "search_apps", icon: "search", label: DrawerTranslator.Tr("SearchApps") },
    { id: "multimedia", icon: "movie", label: DrawerTranslator.Tr("Multimedia") },
    { id: "development", icon: "construction", label: DrawerTranslator.Tr("Development") },
    { id: "education", icon: "school", label: DrawerTranslator.Tr("Education") },
    { id: "game", icon: "stadia_controller", label: DrawerTranslator.Tr("Games") },
    { id: "graphics", icon: "photo", label: DrawerTranslator.Tr("Graphics") },
    { id: "network", icon: "lan", label: DrawerTranslator.Tr("Network") },
    { id: "office", icon: "domain", label: DrawerTranslator.Tr("Office") },
    { id: "settings", icon: "settings", label: DrawerTranslator.Tr("Settings") },
    { id: "system", icon: "computer", label: DrawerTranslator.Tr("System") },
    { id: "utility", icon: "service_toolbox", label: DrawerTranslator.Tr("Utilities") }
  ]

  readonly property var categoryKeyMap: ({
    "multimedia": "AudioVideo",
    "development": "Development",
    "education": "Education",
    "game": "Game",
    "graphics": "Graphics",
    "network": "Network",
    "office": "Office",
    "settings": "Settings",
    "system": "System",
    "utility": "Utility"
  })

  function categoryIndexOf(catId) {
    for (var i = 0; i < categories.length; ++i) {
      if (categories[i].id === catId) return i;
    }
    return 0;
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

    // Consume all clicks within the drawer so they don't propagate to
    // the window-level background click-catcher and accidentally close it.
    MouseArea { anchors.fill: parent }

    ColumnLayout {
      anchors.fill: parent
      anchors.margins: 2
      spacing: 0

      // Title bar
      Rectangle {
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
          ToolButton {
            id: menuBtn
            font.family: "Material Symbols Rounded"
            font.pixelSize: 24
            text: "\ue5d2"  // "menu"
            onClicked: root.sidePaneVisible = !root.sidePaneVisible

            background: Rectangle { color: "transparent" }
            contentItem: Text {
              text: parent.text
              font: parent.font
              color: Theme.surface_variant_fg
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
            }
          }

          // Search bar
          TextField {
            id: searchBar
            Layout.fillWidth: true
            placeholderText: DrawerTranslator.Tr("SearchHere")
            color: Theme.surface_variant_fg
            placeholderTextColor: Qt.lighter(Theme.surface_variant_fg, 1.4)
            font.pixelSize: 16

            background: Rectangle {
              color: "transparent"
            }

            onTextChanged: {
              root.searchText = text;
              if (text.length > 0 && root.currentCategoryId !== "search_apps") {
                root.currentCategoryId = "search_apps";
              }
            }
          }

          // Search / clear icon button
          Item {
            implicitWidth: 40
            implicitHeight: 40

            Rectangle {
              id: searchIconBg
              anchors.fill: parent
              radius: 20
              color: "transparent"

              Rectangle {
                id: searchIconStateLayer
                anchors.fill: parent
                radius: parent.radius
                color: "transparent"
              }

              Canvas {
                id: searchIconRipple
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
                  id: searchIconRippleAnim
                  NumberAnimation {
                    target: searchIconRipple; property: "rippleRadius"
                    from: 0; to: searchIconBg.width * 1.25
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: searchIconRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: searchIconRipple; property: "opacity"
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
              text: searchBar.text.length > 0 ? "close" : "search"
              color: Theme.surface_variant_fg
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
              anchors.fill: parent
              hoverEnabled: true
              onEntered: searchIconStateLayer.color = Theme.state_layer_hover
              onExited:  searchIconStateLayer.color = "transparent"
              onPressed: (mouse) => {
                searchIconStateLayer.color = Theme.state_layer_pressed
                searchIconRipple.centerX = mouse.x
                searchIconRipple.centerY = mouse.y
                searchIconRippleAnim.restart()
              }
              onReleased: searchIconStateLayer.color =
                containsMouse ? Theme.state_layer_hover : "transparent"
              onClicked: { if (searchBar.text.length > 0) searchBar.text = "" }
            }
          }

          // Avatar button
          Image {
            id: avatarBtn
            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
            source: "image://useravatar/"
            sourceSize: Qt.size(32, 32)
            fillMode: Image.PreserveAspectFit
            smooth: true

            ToolTip.text: typeof UserInfoProvider !== "undefined" ? UserInfoProvider.userName : ""
            ToolTip.visible: avatarHoverArea.containsMouse
            ToolTip.delay: 500

            MouseArea {
              id: avatarHoverArea
              anchors.fill: parent
              hoverEnabled: true
              acceptedButtons: Qt.NoButton
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
          // Keep visible during animation; hide only when fully collapsed so
          // the Flickable inside doesn't steal keyboard focus while invisible.
          visible: root.sidePaneVisible || Layout.preferredWidth > 0
          Layout.preferredWidth: root.sidePaneVisible ? 220 : 0
          Layout.fillHeight: true
          color: Theme.surface_container_low

          Behavior on Layout.preferredWidth {
            NumberAnimation { duration: 200; easing.type: Easing.InOutCubic }
          }

          Flickable {
            id: sidePaneFlickable
            anchors.fill: parent
            anchors.topMargin: 16
            anchors.bottomMargin: 16
            anchors.leftMargin: 0
            anchors.rightMargin: 0

            contentHeight: sidePaneColumn.height
            clip: true
            boundsBehavior: Flickable.DragAndOvershootBounds

            ScrollBar.vertical: ScrollBar {
              policy: ScrollBar.AsNeeded
            }

            // Sliding highlight pill — rendered before Column so it sits behind items
            Rectangle {
              id: sidePaneIndicator
              x: 16
              // Each item is 48px tall with 6px spacing → stride = 54px
              y: root.categoryIndexOf(root.currentCategoryId) * 54
              width: sidePaneColumn.width - 32
              height: 48
              radius: 24
              color: Theme.secondary_container

              Behavior on y {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
              }
            }

            Column {
              id: sidePaneColumn
              width: parent.width
              padding: 16
              topPadding: 0
              spacing: 6

              Repeater {
                model: root.categories
                delegate: AppDrawerSidePaneItem {
                  width: sidePaneColumn.width - 32
                  icon: modelData.icon
                  label: modelData.label
                  isActive: root.currentCategoryId === modelData.id
                  onClicked: {
                    root.currentCategoryId = modelData.id;
                  }
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

          // Intercept wheel events before Flickable's built-in handler
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

        // Stacked pages
        Rectangle {
          Layout.fillWidth: true
          Layout.fillHeight: true
          color: Theme.surface_container_low

          StackLayout {
            id: contentStack
            anchors.fill: parent
            currentIndex: root.categoryIndexOf(root.currentCategoryId)

            // Index 0: All Apps (grid)
            AppDrawerGridPage {
              appModel: typeof AppProvider !== "undefined"
                ? AppProvider.allApps : []
              onAppLaunched: root.close()
            }

            // Index 1: Search page (list)
            AppDrawerSearchPage {
              appModel: typeof AppProvider !== "undefined"
                ? AppProvider.allApps : []
              filterText: root.searchText
              onAppLaunched: root.close()
            }

            // Indices 2..11: Category pages
            Repeater {
              // Skip first two entries (all_apps, search_apps)
              model: root.categories.slice(2)
              delegate: AppDrawerGridPage {
                appModel: {
                  var catKey = root.categoryKeyMap[modelData.id];
                  if (typeof AppProvider !== "undefined" && catKey && root.appRevision >= 0) { // qmllint disable unqualified
                    return AppProvider.appsByCategory(catKey);
                  }
                  return [];
                }
                onAppLaunched: root.close()
              }
            }
          }
        }
      }

      // Bottom tool bar
      Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 64
        color: Theme.surface_container
        radius: 12

        Rectangle {
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.top: parent.top
          height: 12
          color: parent.color
        }

        RowLayout {
          anchors.fill: parent
          anchors.leftMargin: 16
          anchors.rightMargin: 16
          spacing: 32

          Repeater {
            model: [
              { icon: "bedtime",             tooltip: qsTr("Sleep"),      action: "suspend" },
              { icon: "ac_unit",             tooltip: qsTr("Hibernate"),  action: "hibernate" },
              { icon: "restart_alt",         tooltip: qsTr("Reboot"),     action: "reboot" },
              { icon: "power_settings_new",  tooltip: qsTr("Shut down"),  action: "powerOff" }
            ]

            delegate: AppDrawerIconButton {
              materialIcon: modelData.icon
              toolTipText: modelData.tooltip
              onClicked: root.confirmActionRequested(modelData.action, modelData.tooltip)
            }
          }

          Item { Layout.fillWidth: true }  // spacer

          // Session FAB
          AppDrawerSessionBtn {
            id: sessionBtn
            onConfirmActionRequested: (action, label) => root.confirmActionRequested(action, label)
          }
        }
      }
    }
  }
}
