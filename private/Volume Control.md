# Volume Control Implemention
## Overview of This Project
Please refer to `../README.md`.

## Objective
You need to implement the volume control module. This consist a button in the status bar (Material 3 styled Qt Quick Component with ripples and animation effect) and the backend.

## The Button On Status Bar
You may refer to `../res/interfaces/wlan_indicator/WLANIndicator.qml` to see what a button is like. It consists of: 

1. The icon (use material symbol fonts, see the example QML above to see how to use the icon font).
2. The ripple, you may just copy the original one from the example.

## The Flyout
Refer to `../res/interfaces/clock_btn/CalendarFlyout.qml` for how to pop up the flyout and `../res/interfaces/system_tray/SystemTrayOverflowFlyout.qml` for how to handle flyout location. Your flyout shall look like this (you do NOT need to do the translation and **DO NOT USE `qsTr`**)

```xml
<flyout>
    <header>
        <title>Volume Control</title>
        <md3-tab-bar>
            <md3-tab>Output</md3-tab>
            <md3-tab>Input</md3-tab>
        </md3-tab-bar>
    </header>

    <content-area index="Output">
        <!-- Be sure to "../res/interfaces/config_panel/ConfigPanelSystemTrayPage.qml" for component styles -->
        <section-title>Output Device</section-title>
        <shaded-box>
            <md3-dropdown><!--Output device name --></md3-dropdown>
        </shaded-box>

        <br/>

        <section-title>Control per Device</section-title>
        <flickable-list>
            <shaded-box>
                <text><!-- Device name --></text>
                <horizontal-layout>
                    <slider />
                    <text><!-- Device volume --></text>
                </horizontal-layout>
            </shaded-box>
        </flickable-list>
    </content-area>

    <!-- <content-area index="input"> is not here because I am lazy.They are the same layout design, you STILL need to implement that page -->
</flyout>
```

## The Backend
You need to let that thing be functional, so you need to write the C++ backend. Write your code with Qt and consider Qt5 compactibility. Use Google-Styled C++.

I do have some modifications to the style guideline, see `../src/components/app_drawer/` for example.

Put the "pure" backend to `../src/info_server/volume_control`, and put the part that would be injected into the QML engine (a helper class that invokes the backend) to `../src/components/volume_control`.

You are NOT forced to start everything from stratch, we have vendored the pauvcontrol-qt library from LXQt for you to use. 

**NOTE**: The vendored library is located at `../lib/3rdparty/pauvcontrol-qt/` but it is not yet added to `CMakeLists.txt` so you may want to add that accordingly.

Addon: The volume icon shall be indicating the volume of active output device (that one user selected)

Addon #2: The volume icon font have four stages: volume_mute, volume_off, volume_down, volume_up, those are ranked by percentage.
