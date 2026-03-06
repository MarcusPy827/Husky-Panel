简体中文 | [English](./README.md)

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
    [正在施工中] Linux Wayland会话下又一个状态栏...
    <br />
    <!--
    <a style="display: none;" href="https://github.com/github_username/repo_name"><strong>Explore the docs »</strong></a>
    -->
    <img src="./docs/screenshots/screenshot.png" />
    <span>(<i>截图中图标来自 <a href="https://github.com/vinceliuice/Colloid-icon-theme.git">Colloid Icon Theme</a>，仅供展示，与本项目无关，相关内容不包含在本项目中。</i>)</span>
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
  <summary>目录</summary>
  <ol>
    <li>
      <a href="#关于项目">关于项目</a>
      <ul>
        <li><a href="#依赖">依赖</a></li>
      </ul>
    </li>
    <li>
      <a href="#快速上手">快速上手</a>
      <ul>
        <li><a href="#前置条件">前置条件</a></li>
        <li><a href="#installation">安装</a></li>
      </ul>
    </li>
    <li><a href="#usage">如何使用</a></li>
    <li><a href="#roadmap">里程碑</a></li>
    <li><a href="#contributing">贡献者</a></li>
    <li><a href="#license">许可证</a></li>
    <li><a href="#contact">联系我们</a></li>
    <li><a href="#acknowledgments">特别鸣谢</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## 关于项目
`HuskyPanel`是又一款Linux Wayland会话上的Shell状态栏，这是一个尝试使用QWidget的实验性项目，目的是为您的桌面环境带来Material Design 3风格的状态栏与面板。

当前我们主要支持的平台是KWin/KDE Plasma 6。

<p align="right">(<a href="#readme-top">回到顶部</a>)</p>



### 依赖
* Qt 6.5+
* Layer-Shell-Qt
* Abseil
* Google Test
* Libdbusmenu
* Material-Color-Utilities
* QWindowKit
* KDE Framework 6
* KServices

<p align="right">(<a href="#readme-top">回到顶部</a>)</p>



<!-- GETTING STARTED -->
## 快速上手
请确保本机有可用的Qt 6.5+，状态栏使用了此版本新引入的API以感知系统深色/浅色模式变化，如果Qt版本低于此版本，则此项目将无法编译。

当前本项目仅支持Plasma 6桌面，我们推荐的版本是Plasma 6.5。Wlroots系的窗口管理器支持在规划之中。

### 前置条件
#### 安装工具链
在构建本项目之前，请确保您已经安装好了如下必需的工具链：

在基于Archlinux的发行版上：
```bash
sudo pacman -S cmake base-devel
```

#### 构建ECM
ECM (Extra CMake Modules) 被我们的依赖`layer-shell-qt`所依赖。我们集成了一个最近版本的`layer-shell-qt`，然而这带来了一个问题：这个版本依赖的ECM版本非常新，截止我正在写这篇文档的时候很多非滚动发行的发行版都没有能满足最小要求版本的情况。为了解决这个问题，我集成了ECM模块，在构建本项目之前请先按照如下说明来构建ECM模块：

首先，请打开终端，确保自己的终端上的位置在**项目根目录上**。

然后执行：
```bash
chmod a+x ./scripts/configure_ecm.sh
./scripts/configure_ecm.sh
```

运行脚本的时候脚本可能会询问一些问题，这些问题如下：

```
[WARN] The build directory "/XXXXX/husky-panel/build/ecm-build" is NOT found in your system.
Do you want to create it?\n    (y/n) >> 
```

> 也有可能询问的目录是`"/XXXXX/husky-panel/build/ecm-install"`，其与`ecm-build`的区别是前者是安装目录，后者是构建目录。

此问题报告：构建/安装目录不存在，是否创建？

首先，请检查问题打印出来的目录`"/XXXXXX/husky-panel/build/ecm-build"`是否就是`"<项目根目录>/build/ecm-build"`，如果不是则代表您的终端不在项目根目录上，输入`n`并按下回车取消执行脚本。

> 如果询问的是`"/XXXXX/husky-panel/build/ecm-install"`，那就是确认请检查问题打印出来的目录`"/XXXXXX/husky-panel/build/ecm-install"`是否就是`"<项目根目录>/build/ecm-install"`

> 输入`y`或`n`之外的任何内容都会打断脚本。

如果上一步中打印目录就是`"<项目根目录>/build/ecm-build"`，您需要输入`y`，然后按回车，脚本会自动创建这个构建路径。

<hr/>

```
[WARN] The build directory "/XXXXXX/build/ecm-build" already exists and is NOT empty.
Do you want to clean it?\nI mean... to delete everything in it?\n    (y/n) >> 
```

> 也有可能询问的目录是`"/XXXXX/husky-panel/build/ecm-install"`，其与`ecm-build`的区别是前者是安装目录，后者是构建目录。

此问题报告：构建目录/安装已经存在且不为空，是否清空？
* 输入`y`，按下回车会清空这个目录下一切内容。
* 输入`n`，按下回车会跳过目录清理。
* 输入任何其他奇奇怪怪的东西也会跳过目录清理。

<hr/>

如果看到如下输出：
```
[ OK ] ECM should be built and available in "/home/marcus/Desktop/Repository/Private/husky-panel/build/ecm-build/" now.
```

...则证明您的ECM模块已经构建好了。

#### 安装依赖
您需要安装以下依赖：

在OpenSUSE上：
```bash
sudo zypper in wayland-devel \
  wayland-protocols-devel \
  kf6-extra-cmake-modules \
  libxkbcommon-devel
```

On Archlinux: 
```bash
sudo pacman -S wayland wayland-protocols extra-cmake-modules libxkbcommon
```

You will also need to use a Wayland session, otherwise the bar will NOT attach properly to the top of the screen.

Other third-party libraries has been vendored, so you do NOT need to install them.

### Installation
#### Build the Bar Itself & The Locale Files
```bash
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
cd ..
```

#### Copy the Locale Files
Copy `build/locales/HuskyPanel_zh_CN.qm` to `res/translations/HuskyPanel_zh_CN.qm`.

Then, you will need to re-compile the bar.
```bash
rm -rf build
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
cd ..
```

#### (KWin Only) Install the Plugin
Before installing, do read the README on `plugins/kde/app-bridge`.

```bash
cd ./plugins/kde/app-bridge/
chmod a+x ./install.sh
./install.sh
```

(To uninstall the plugin, please execute: )
```bash
cd ./plugins/kde/app-bridge/
chmod a+x ./uninstall.sh
./uninstall.sh
```

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

See the [open issues](https://github.com/github_username/repo_name/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing
Please read [CONTRIBUTING.md](CONTRIBUTING.md).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Top contributors:

<a href="https://github.com/github_username/MarcusPy827/Husky-Panel/contributors">
  <img src="https://contrib.rocks/image?repo=MarcusPy827/Husky-Panel" alt="contrib.rocks image" />
</a>



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
* [Best-README-Template](https://github.com/othneildrew/Best-README-Template.git)
* [LXQt-Panel](https://github.com/lxqt/lxqt-panel.git)
* All the author of the libraries we depended (see [HERE](./lib/3rdparty/VERSION.md) for a detailed list!)

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