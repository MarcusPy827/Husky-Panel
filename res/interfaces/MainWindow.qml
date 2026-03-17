// MainWindow.qml
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// LayerShell 通过 C++ 侧设置，QML 这边只管内容
Window {
    id: root
    visible: false                      // shown from C++ after LayerShell is configured
    width: Screen.width
    height: 32                          // inclusive_zone_height_

    // ── 黑色底板（panel_container） ──
    Rectangle {
        anchors.fill: parent
        color: "#000000"

        // ── base_layer ──
        Rectangle {
            id: baseLayer
            anchors.fill: parent
            color: Theme.status_bar_bg   // GetAppBackground → GetSurfaceContainer()
            topLeftRadius:  8
            topRightRadius: 8
            // bottom radius 保持 0，QML 默认就是 0

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin:  8
                    rightMargin: 8
                }
                spacing: 6

                // ── slot_left ──
                RowLayout {
                    spacing: 4
                    Layout.alignment: Qt.AlignVCenter
                }

                // ── spacer ──
                Item { Layout.fillWidth: true }

                // ── slot_middle（暂空） ──
                RowLayout {
                    spacing: 4
                    Layout.alignment: Qt.AlignVCenter
                }

                // ── spacer ──
                Item { Layout.fillWidth: true }

                // ── slot_right ──
                RowLayout {
                    spacing: 4
                    Layout.alignment: Qt.AlignVCenter
                }
            }
        }
    }
}