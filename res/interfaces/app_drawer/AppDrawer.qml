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
          Item {
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

          // Search bar
          TextField {
            id: searchBar
            Layout.fillWidth: true
            placeholderText: DrawerTranslator.Tr("SearchHere")
            color: Theme.surface_variant_fg
            placeholderTextColor: Qt.lighter(Theme.surface_variant_fg, 1.4)
            font.pixelSize: 16
            leftPadding: 24
            rightPadding: 52  // room for icon inside

            background: Rectangle {
              color: Theme.surface_container_high
              width: parent.width   // qmllint disable unqualified
              height: 48
              radius: 24
              y: (parent.height - height) / 2  // qmllint disable unqualified

              Rectangle {
                id: searchBarStateLayer
                anchors.fill: parent
                radius: parent.radius
                color: "transparent"
              }

              Canvas {
                id: searchBarRipple
                anchors.fill: parent
                opacity: 0

                property real centerX: width / 2
                property real centerY: height / 2
                property real rippleRadius: 0

                onPaint: {
                  var ctx = getContext("2d")
                  ctx.clearRect(0, 0, width, height)
                  ctx.save()
                  // Clip to pill shape
                  var r = 24
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
                  id: searchBarRippleAnim
                  NumberAnimation {
                    target: searchBarRipple; property: "rippleRadius"
                    from: 0; to: searchBarRipple.width * 0.75
                    duration: 400; easing.type: Easing.OutCubic
                  }
                  SequentialAnimation {
                    PropertyAction { target: searchBarRipple; property: "opacity"; value: 0.3 }
                    PauseAnimation { duration: 150 }
                    NumberAnimation {
                      target: searchBarRipple; property: "opacity"
                      from: 0.3; to: 0
                      duration: 250; easing.type: Easing.OutCubic
                    }
                  }
                }
              }
            }

            HoverHandler { id: searchBarHover }

            TapHandler {
              onActiveChanged: {
                if (active) {
                  searchBarStateLayer.color = Theme.state_layer_pressed
                  // point.position is the press location in TextField coordinates
                  var pos = point.position  // qmllint disable unqualified
                  if (pos.x <= searchBar.width - 52) {  // qmllint disable unqualified
                    var bgOffsetY = (searchBar.height - 48) / 2  // qmllint disable unqualified
                    searchBarRipple.centerX = pos.x  // qmllint disable unqualified
                    searchBarRipple.centerY = pos.y - bgOffsetY
                    searchBarRippleAnim.restart()  // qmllint disable unqualified
                  }
                } else {
                  searchBarStateLayer.color = searchBarHover.hovered  // qmllint disable unqualified
                    ? Theme.state_layer_hover : "transparent"
                }
              }
            }

            // Search / clear icon pinned to the right end of the search bar
            Item {
              width: 40
              height: 40
              anchors.right: parent.right          // qmllint disable unqualified
              anchors.rightMargin: 4
              anchors.verticalCenter: parent.verticalCenter  // qmllint disable unqualified
              z: 1

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

            onTextChanged: {
              root.searchText = text;
              if (text.length > 0 && root.currentCategoryId !== "search_apps") {
                root.currentCategoryId = "search_apps";
              }
            }
          }

          // Avatar button
          Image {
            id: avatarBtn
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            source: "image://useravatar/"
            sourceSize: Qt.size(48, 48)
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
              width: sidePaneColumn.width - 32
              height: 48
              radius: 24
              color: Theme.secondary_container

              Component.onCompleted: y = root.categoryIndexOf(root.currentCategoryId) * 54

              Connections {
                target: root
                function onCurrentCategoryIdChanged() {
                  indicatorMoveAnim.to = root.categoryIndexOf(root.currentCategoryId) * 54
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
              { icon: "bedtime", tooltip: StatusBarTranslator.Tr("Action_Sleep"), action: "suspend" },
              { icon: "ac_unit", tooltip: StatusBarTranslator.Tr("Action_Hibernate"), action: "hibernate" },
              { icon: "restart_alt", tooltip: StatusBarTranslator.Tr("Action_Reboot"), action: "reboot" },
              { icon: "power_settings_new", tooltip: StatusBarTranslator.Tr("Action_shut_down"), action: "powerOff" }
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
