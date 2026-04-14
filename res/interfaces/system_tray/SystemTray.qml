/*
 * SystemTray.qml
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
import QtQuick.Layouts

// Container for all active StatusNotifierItem icons. Renders a row of
// SystemTrayIcon delegates driven by the TrayHandler model, plus an overflow
// expand button that appears when there are hidden-but-active icons.
Item {
  id: root

  signal overflowClicked()

  Layout.fillHeight: true
  implicitWidth: row.implicitWidth

  // Center x of the visible overflow button, in this item's coordinate space.
  // Uses item.x + item.width/2 directly (row anchors.fill parent, so
  // child coordinates are identical to root's coordinate space) to keep
  // this property fully reactive when the RowLayout re-positions children.
  readonly property real overflowBtnCenterX: {
    if (TrayHandler.hiddenActiveCount > 0) { // qmllint disable unqualified
      if (TrayHandler.expandIconOnLeft) // qmllint disable unqualified
        return overflowLeftBtn.x + overflowLeftBtn.width / 2 // qmllint disable missing-property
      else
        return overflowRightBtn.x + overflowRightBtn.width / 2 // qmllint disable missing-property
    }
    return root.width / 2
  }

  RowLayout {
    id: row
    anchors.fill: parent
    spacing: 4

    // Left-side expand button (shown when expandIconOnLeft is enabled)
    SystemTrayExpandIcon {
      id: overflowLeftBtn
      // qmllint disable unqualified
      visible: TrayHandler.hiddenActiveCount > 0 && TrayHandler.expandIconOnLeft
      onClicked: root.overflowClicked()
    }

    Repeater {
      model: TrayHandler // qmllint disable unqualified
      delegate: SystemTrayIcon {}
    }

    // Right-side expand button (default position)
    SystemTrayExpandIcon {
      id: overflowRightBtn
      // qmllint disable unqualified
      visible: TrayHandler.hiddenActiveCount > 0 && !TrayHandler.expandIconOnLeft
      onClicked: root.overflowClicked()
    }
  }
}
