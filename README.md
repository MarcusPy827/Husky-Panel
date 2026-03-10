[简体中文](./README.zh.md) | English

VERSION 0.9.3

<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a id="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
![Static Badge](https://img.shields.io/badge/Qt-Target%20version%206.10.2-green?style=flat-square&logo=qt&link=qt.io) ![Static Badge](https://img.shields.io/badge/C%2B%2B%20standard-20-blue?style=flat-square&logo=c%2B%2B&link=https%3A%2F%2Fen.cppreference.com%2Fw%2Fcpp%2F20.html) ![Static Badge](https://img.shields.io/badge/Material%20Symbols%20version-4.0.0-yellow?style=flat-square&logo=materialdesign&color=%236750A4&link=https%3A%2F%2Ffonts.google.com%2Ficons) ![Static Badge](https://img.shields.io/badge/Version-0.9_alpha-yellow?style=flat-square) ![Static Badge](https://img.shields.io/badge/Made%20with-Love-red?style=flat-square)



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <!--
  <a href="https://github.com/github_username/repo_name">
    <img style="display: none;" src="images/logo.png" alt="Logo" width="80" height="80">
  </a>
  -->

<h3 align="center">Husky Panel</h3>

  <p align="center">
    (WIP) Yet another bar for Linux on Wayland session...
    <br />
    <!--
    <a style="display: none;" href="https://github.com/github_username/repo_name"><strong>Explore the docs »</strong></a>
    -->
    <img src="./docs/screenshots/screenshot.png" />
    <span>(<i>Icon from <a href="https://github.com/vinceliuice/Colloid-icon-theme.git">Colloid Icon Theme</a>, which is NOT related, and NOT included with this project.</i>)</span>
    <br />
    <br />
    <!--
    <a style="display: none;"  href="https://github.com/github_username/repo_name">View Demo</a>
    <span style="display: none;" >&middot;</span>
    <a style="display: none;"  href="https://github.com/github_username/repo_name/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    <span style="display: none;" >&middot;</span>
    <a style="display: none;"  href="https://github.com/github_username/repo_name/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
    -->
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project
`HuskyPanel` is another shell bar panel for Linux Wayland session. This is an experimental project that trys to utilize QWidget to bring a Material design 3 styled UI to your desktop environment.

Currenty we are focusing on KWin/KDE Plasma 6.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With
* Qt 6.5+
* Layer-Shell-Qt
* Abseil
* Google Test
* Libdbusmenu
* Material-Color-Utilities
* QWindowKit
* KDE Framework 6
* KServices

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started
> ⚠️ **WARNING**: For the Debian-based distro part, I haven't finished writting the documentation yet, please ignore that part for now.

Ensure that you have Qt version 6.5+ avaliable in your system. This bar uses a new API provided by Qt 6.5+ to aware the change in light/dark scheme. Without the proper version of Qt, the bar could NOT be complied.

Currently only Plasma 6 is supported, we recommend you to login a Plasma 6.5 session to use this bar. Wlroots WM support is planned.

### Prerequisites
#### Building ECM (only needed when `USE_VENDORED_LIBS=ON`)

> **If you are using system `layer-shell-qt` and `libdbusmenu-lxqt` (the default), you can skip this step entirely** — the system ECM is sufficient.

ECM (Extra CMake Modules) is required by the vendored `layer-shell-qt`. The vendored version depends on a very new ECM, and many non-rolling distros do not meet the minimum version requirement. To solve this, we have vendored the ECM module as well. **If you pass `-DUSE_VENDORED_LIBS=ON`**, please build the ECM module first:

First, open a terminal, make sure you are **on project root**.

Then execute the following: 
```bash
chmod a+x ./scripts/configure_ecm.sh
./scripts/configure_ecm.sh
```

If you see the log output:
```
[ OK ] ECM should be built and available in "/home/marcus/Desktop/Repository/Private/husky-panel/build/ecm-build/" now.
```

... then ECM modules are ready to use for next steps!!

#### Install Dependencies
You will need to install the dependencies.

##### On Archlinux-based Distros
```bash
sudo pacman -S wayland wayland-protocols libxkbcommon layer-shell-qt libdbusmenu-lxqt
```

##### On Fedora
> ⚠️ **CRITICAL WARNING**: Fedora ships its own Qt build. Mixing Qt versions **will** cause ABI mismatches, which can corrupt your desktop session and potentially make your system unbootable into a graphical environment.

**Before installing any dependencies**, perform a full system upgrade and reboot to ensure all packages (especially Qt and KDE Frameworks) are at consistent versions:
```bash
sudo dnf upgrade --refresh
sudo reboot
```

Then install the dependencies:
```bash
sudo dnf install wayland-devel wayland-protocols-devel libxkbcommon-devel layer-shell-qt-devel libdbusmenu-lxqt-devel kf6-kservice-devel extra-cmake-modules qt6-qtbase-private-devel
```

> `qt6-qtbase-private-devel` is required because QWindowKit uses Qt private headers. This package only contains headers.

The KF6 and layer-shell-qt devel packages will automatically pull in the **matching** Qt 6 public development headers as dependencies — do **not** install `qt6-qtbase-devel` etc. by hand, as that may resolve to a different Qt version and break your Plasma session.

After installation, verify that all Qt packages are from the **same version**:
```bash
rpm -qa 'qt6*' --qf '%{NAME}-%{VERSION}\n' | awk -F- '{print $NF}' | sort -u
```
If more than one version is printed, your system is at risk of ABI breakage — run `sudo dnf distro-sync 'qt6*'` to fix it.

> Fedora packages `layer-shell-qt` and ECM from KDE Frameworks, so you generally do **not** need `-DUSE_VENDORED_LIBS=ON` or the manual ECM build step.

##### On OpenSUSE
```bash
sudo zypper in wayland-devel wayland-protocols-devel libxkbcommon-devel
```

##### On Debian-Based Distros
> ⚠️ **Note**: Ubuntu LTS releases (e.g. 24.04) ship Qt 6.4, which is **too old** — this project requires Qt 6.5+. You need at least **Ubuntu 24.10** or **Kubuntu 24.10** (or newer) to build. If you are on an LTS release, consider using [KDE Neon](https://neon.kde.org/) which ships up-to-date Qt and KDE Frameworks.

> Ubuntu / Kubuntu do **not** ship `layer-shell-qt` or `libdbusmenu-lxqt` development packages. You **must** use `-DUSE_VENDORED_LIBS=ON` and build ECM first (see the [Building ECM](#building-ecm-only-needed-when-use_vendored_libson) section above).

```bash
sudo apt install build-essential cmake \
    libwayland-dev wayland-protocols libxkbcommon-dev \
    qt6-base-private-dev qt6-base-dev qt6-wayland-dev qt6-tools-dev \
    libkf6service-dev extra-cmake-modules
```

Then configure with vendored libraries:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_VENDORED_LIBS=ON ..
```

##### Side Notes
You will also need to use a Wayland session, otherwise the bar will NOT attach properly to the top of the screen.

> ⚠️ **Note**: This panel is NOT compactiable with Mutter, hence there is no way to run this panel on GNOME.

Some third-party libraries (abseil, gtest, material-color-utilities, qwindowkit) are always vendored and do NOT need to be installed separately.

By default, `layer-shell-qt` and `libdbusmenu-lxqt` are linked from the **system**. If your distro does not ship these packages (or ships an incompatible version), you can fall back to the vendored copies by passing `-DUSE_VENDORED_LIBS=ON` at configure time:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_VENDORED_LIBS=ON ..
```

### Build & Installation
#### Build the Bar Itself
```bash
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
cmake --build
```

The process could lasts for minutes...

#### (Optional) Install to System
```bash
sudo cmake --install .
```
> **Note**: Currently HuskyPanel does NOT autostart itself after you login, you may want to add it to your destop environment's autostart settings MANUALLY.

#### (KWin Only) Install the Plugin
The KWin app-bridge script plugin is installed automatically by `cmake --install .` (see above). It is enabled by default via the `INSTALL_KWIN_PLUGIN` CMake option.

If you do **not** want the plugin, pass `-DINSTALL_KWIN_PLUGIN=OFF` at configure time.

After installation, navigate to *KDE Settings*, search "*KWin*", and enable the script named "*Husky-Panel App Bridge*" in the *KWin Script* section. You may need to restart KWin or log out and back in.

For more details about the plugin, read the README in `plugins/kde/app-bridge`.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage
(*Documentation work in progress...*)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [X] Search bar
- [X] Clock
- [ ] System tray
    - [X] Simple tray
    - [ ] Icon folding
- [ ] App indicator
    - [X] KWin support (via KWin script)
    - [ ] Niri/Hyprland support
- [ ] Notification manager
- [ ] Network manager
- [ ] Battery manager
- [ ] Volume manager
- [ ] Bluetooth manager
- [ ] App drawer (WIP)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing
Please read [CONTRIBUTING.md](CONTRIBUTING.md).

### Top contributors

<a href="https://github.com/github_username/MarcusPy827/Husky-Panel/contributors">
  <img src="https://contrib.rocks/image?repo=MarcusPy827/Husky-Panel" alt="contrib.rocks image" />
</a>

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- LICENSE -->
## License

Distributed under the GNU GENERAL PUBLIC LICENSE Version 3. See `COPYING` for more information.

For all the vendored libraries' version, please read `lib/3rdparty/VERSION.md`.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact
To contact me, please utilize the issue.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments
### Authors of Third Party Libraries Used
* **Layershell-Qt**: KDE.
* **Material Color Utility**: Material Foundation.
* **Abseil**: Google Inc.
* **Google Test**: Google Inc.
* **QWindowKit**: Stdware Collections.
* **Qmsetup**: Stdware Collections.
* **Syscmdline**: SineStriker.
* **libdbusmenu-lxqt**: lxqt.
* **Extra CMake Modules**: KDE.
* **KDE Frameworks 6**: KDE.

(*To get full third party library information, please refer [this document](./lib/3rdparty/VERSION.md)*)

### Template & Reference
* [Best-README-Template](https://github.com/othneildrew/Best-README-Template.git)
* [LXQt-Panel](https://github.com/lxqt/lxqt-panel.git)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/github_username/repo_name.svg?style=for-the-badge
[contributors-url]: https://github.com/github_username/repo_name/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/github_username/repo_name.svg?style=for-the-badge
[forks-url]: https://github.com/github_username/repo_name/network/members
[stars-shield]: https://img.shields.io/github/stars/github_username/repo_name.svg?style=for-the-badge
[stars-url]: https://github.com/github_username/repo_name/stargazers
[issues-shield]: https://img.shields.io/github/issues/github_username/repo_name.svg?style=for-the-badge
[issues-url]: https://github.com/github_username/repo_name/issues
[license-shield]: https://img.shields.io/github/license/github_username/repo_name.svg?style=for-the-badge
[license-url]: https://github.com/github_username/repo_name/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username
[product-screenshot]: images/screenshot.png
<!-- Shields.io badges. You can a comprehensive list with many more badges at: https://github.com/inttter/md-badges -->
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com 