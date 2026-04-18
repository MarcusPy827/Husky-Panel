/*
 * ConfigPanelAboutPage.qml
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

Item {
  id: root

  Column {
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      margins: 24
    }
    spacing: 8

    // Page title
    Text {
      text: ConfigPanelTranslator.Tr("AboutSystem")  // qmllint disable unqualified
      font.pixelSize: 22
      font.weight: Font.Medium
      color: Theme.surface_variant_fg  // qmllint disable unqualified
      bottomPadding: 8
    }

    // Section header
    Text {
      text: ConfigPanelTranslator.Tr("PanelVersion")  // qmllint disable unqualified
      font.pixelSize: 14
      font.weight: Font.Medium
      color: Theme.primary  // qmllint disable unqualified
      topPadding: 4
      bottomPadding: 4
    }

    // Shaded box
    Rectangle {
      width: parent.width
      height: boxColumn.implicitHeight + 24
      radius: 12
      color: Theme.surface_container  // qmllint disable unqualified

      Column {
        id: boxColumn
        anchors {
          left: parent.left
          right: parent.right
          verticalCenter: parent.verticalCenter
          leftMargin: 16
          rightMargin: 16
        }
        spacing: 4

        Text {
          text: "Husky Panel"
          font.pixelSize: 14
          font.bold: true
          color: Theme.surface_fg  // qmllint disable unqualified
        }

        Text {
          text: "RELEASE 1.0.0"
          font.pixelSize: 18
          color: Theme.surface_variant_fg  // qmllint disable unqualified
        }
      }
    }
  }
}
