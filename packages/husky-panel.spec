%global app_name    husky-panel
%global app_version 0.9.4

Name:           %{app_name}
Version:        %{app_version}
Release:        1%{?dist}
Summary:        (WIP) Yet another bar for Linux on Wayland session...
License:        GPL-3.0-only
URL:            https://github.com/MarcusPy827/Husky-Panel

Source0:        %{app_name}-%{app_version}.tar.gz

BuildRequires:  cmake >= 3.16
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig(wayland-client)
BuildRequires:  pkgconfig(wayland-protocols)
BuildRequires:  pkgconfig(xkbcommon)
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtbase-private-devel
BuildRequires:  qt6-qtwayland-devel
BuildRequires:  qt6-qttools-devel
BuildRequires:  layer-shell-qt-devel
BuildRequires:  libdbusmenu-lxqt-devel
BuildRequires:  kf6-kservice-devel
BuildRequires:  extra-cmake-modules
BuildRequires:  pkgconfig(libpulse)

# Optional: enables XCB/X11 session support
# BuildRequires:  libxcb-devel

Requires:       qt6-qtbase
Requires:       qt6-qtdeclarative
Requires:       qt6-qtwayland
Requires:       layer-shell-qt
Requires:       libdbusmenu-lxqt
Requires:       kf6-kservice
Requires:       pulseaudio-libs

%description
HuskyPanel is a shell bar panel for Linux Wayland sessions, built with Qt 6 and
QWidget to deliver a Material Design 3 styled UI. Currently targeting KDE
Plasma 6 / KWin.

%prep
%autosetup -n %{app_name}-%{app_version}

%build
%cmake -DCMAKE_BUILD_TYPE=Release -DINSTALL_KWIN_PLUGIN=ON
%cmake_build

%install
%cmake_install

%files
%license COPYING
%doc README.md CHANGELOG
%{_bindir}/HuskyPanel
%{_datadir}/kwin/scripts/marcus-app-bridge/

%changelog
* Wed Apr 1 2026 MarcusPy827 - 0.9.4-1
- Re-implemented all elements w/ QML.
- Made a brand new calendar flyout.
- Enhanced AppDrawer style.
- Added tons of animations to the status bar, application drawer, and calendar.

* Tue Mar 10 2026 MarcusPy827 - 0.9.3-1
- Initial RPM packaging
