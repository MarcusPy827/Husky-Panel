/*
 * ConfigPanelWindow.qml
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
import QtQuick.Window
import QWindowKit

Window {
  id: root
  width: 960
  height: 648
  color: "transparent"
  flags: Qt.Window | Qt.FramelessWindowHint

  function open() {
    root.show()
    configPanel.open()
  }

  function close() {
    configPanel.close()
  }

  Component.onCompleted: {
    windowAgent.setup(root)
    windowAgent.setTitleBar(configPanel.titleBarItem)
    windowAgent.setHitTestVisible(configPanel.menuBtnItem, true) // qmllint disable missing-property
    windowAgent.setHitTestVisible(configPanel.closeBtnItem, true) // qmllint disable missing-property
  }

  WindowAgent {
    id: windowAgent
  }

  ConfigPanel {
    id: configPanel
    anchors.fill: parent
    onCloseAnimationFinished: root.hide()
  }
}
