简体中文 | [English](./README.md)

VERSION 1.0.0

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
![Static Badge](https://img.shields.io/badge/Qt-Target%20version%206.10.2-green?style=flat-square&logo=qt&link=qt.io) ![Static Badge](https://img.shields.io/badge/C%2B%2B%20standard-17-blue?style=flat-square&logo=c%2B%2B&link=https%3A%2F%2Fen.cppreference.com%2Fw%2Fcpp%2F17.html) ![Static Badge](https://img.shields.io/badge/Material%20Symbols%20version-4.0.0-yellow?style=flat-square&logo=materialdesign&color=%236750A4&link=https%3A%2F%2Ffonts.google.com%2Ficons) ![Static Badge](https://img.shields.io/badge/Version-0.9_alpha-yellow?style=flat-square) ![Static Badge](https://img.shields.io/badge/Made%20with-Love-red?style=flat-square)



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
    为Linux桌面端准备的整体式状态栏
    <br />
    <!--
    <a style="display: none;" href="https://github.com/github_username/repo_name"><strong>Explore the docs »</strong></a>
    -->
    <img src="./docs/screenshots/screenshot.png" />
    <span><span>(程序截图，在KDE Plasma6（Wayland会话）下截取。壁纸由Santiago César创作)</span></span>
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
        <li><a href="#构建">构建</a></li>
      </ul>
    </li>
    <li><a href="#如何使用">如何使用</a></li>
    <li><a href="#里程碑">里程碑</a></li>
    <li><a href="#贡献者">贡献者</a></li>
    <li><a href="#许可证">许可证</a></li>
    <li><a href="#联系我们">联系我们</a></li>
    <li><a href="#特别鸣谢">特别鸣谢</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## 关于项目
`HuskyPanel`是又一款Linux Wayland会话上的Shell状态栏，这是一个使用Qt Quick（QML）构建的实验性项目，目的是为您的桌面环境带来Material Design 3风格的状态栏与面板。

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
* PulseAudio (libpulse)

<p align="right">(<a href="#readme-top">回到顶部</a>)</p>



<!-- GETTING STARTED -->
## 快速上手
### 构建
请阅读如下构建指南：
* 对于基于Archlinux的发行版: **[在基于Arch的发行版上构建](./docs/build_instructions/Arch.zh.md)**.
* 对于基于Debian的发行版: **[在基于Debian的发行版上构建](./docs/build_instructions/Debian.zh.md)**.
* 对于Fedora: **[在基于Fedora上构建](./docs/build_instructions/Fedora.zh.md)**.

<p align="right">(<a href="#readme-top">返回顶部</a>)</p>



<!-- USAGE EXAMPLES -->
## 如何使用
(*文档正在施工中...*)

<p align="right">(<a href="#readme-top">返回顶部</a>)</p>



<!-- ROADMAP -->
## 里程碑

- [X] 搜索框
- [X] 时钟
- [X] 系统托盘
    - [X] 简易托盘
    - [X] 图标折叠
- [X] 应用程序指示器
    - [X] KWin支持（通过KWin脚本）
    - [X] Niri/Hyprland支持
- [ ] 通知管理器
- [X] 网络管理器
- [X] 电量管理器
- [X] 音量管理器
- [X] 蓝牙管理器
- [X] 应用抽屉

<p align="right">(<a href="#readme-top">返回顶部</a>)</p>



<!-- CONTRIBUTING -->
## 贡献者
请参阅[CONTRIBUTING.md](CONTRIBUTING.md).

### 最活跃的贡献者

<a href="https://github.com/MarcusPy827/Husky-Panel/contributors">
  <img src="https://contrib.rocks/image?repo=MarcusPy827/Husky-Panel" alt="contrib.rocks image" />
</a>

<p align="right">(<a href="#readme-top">返回顶部</a>)</p>


<!-- LICENSE -->
## 许可证
本项目以GNU GENERAL PUBLIC LICENSE Version 3许可发行，请参阅`COPYING`以获取更多许可证信息。

对于所有集成的第三方库的许可证，请参阅`lib/3rdparty/VERSION.md`。

<p align="right">(<a href="#readme-top">返回顶部</a>)</p>



<!-- CONTACT -->
## 联系我们
请善用Issue功能。

<p align="right">(<a href="#readme-top">返回顶部</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## 特别鸣谢
### 第三方库作者
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

(*要获取完整的第三方库信息，请参阅[这里](./lib/3rdparty/VERSION.md)*)

### 模板与参考
* [Best-README-Template](https://github.com/othneildrew/Best-README-Template.git)
* [LXQt-Panel](https://github.com/lxqt/lxqt-panel.git)
* [PAUVControl-Qt](https://github.com/lxqt/pavucontrol-qt.git)

<p align="right">(<a href="#readme-top">返回顶部</a>)</p>



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