[简体中文](./README.zh.md) | English

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
    A monolithic status bar for Linux desktop.
    <br />
    <!--
    <a style="display: none;" href="https://github.com/github_username/repo_name"><strong>Explore the docs »</strong></a>
    -->
    <img src="./docs/screenshots/screenshot.png" />
    <span>(Screenshot of HuskyPanel running on KDE Plasma 6 (Wayland session). Wallpaper by Santiago César)</span>
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
        <li><a href="#build">Building</a></li>
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
`HuskyPanel` is another shell bar panel for Linux Wayland session. This is an experimental project that uses Qt Quick (QML) to bring a Material Design 3 styled UI to your desktop environment.

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
* PulseAudio (libpulse)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started
Ensure that you have Qt version 6.5+ avaliable in your system. This bar uses a new API provided by Qt 6.5+ to aware the change in light/dark scheme. Without the proper version of Qt, the bar could NOT be complied.

Currently only Plasma 6 is supported, we recommend you to login a Plasma 6.5 session to use this bar. Wlroots WM support is planned.

### Building
Please read the build instructions: 
* For Arch-based distros: **[Arch Build Instructions](./docs/build_instructions/Arch.en.md)**.
* For Debian-based distros: **[Debian Build Instructions](./docs/build_instructions/Debian.en.md)**.
* For Fedora: **[Fedora Build Instructions](./docs/build_instructions/Fedora.en.md)**.


<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage
Most usage of this bar is pretty straight forward, you just click on the icons on the status bar and a flyout will pop out.

The only hidden menu is for system tray icons. You need to right click on the spare space on the status bar, there is an option called "Configure panel...", that will open a new window, inside there is a system tray icon config, you may choose any items of the tray icons that you would like to hide.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [X] Search bar
- [X] Clock
- [X] System tray
    - [X] Simple tray
    - [X] Icon folding
- [X] App indicator
    - [X] KWin support (via KWin script)
    - [X] Niri/Hyprland support
- [ ] Notification manager
- [X] Network manager
- [X] Battery manager
- [X] Volume manager
- [X] Bluetooth manager
- [X] App drawer

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing
Please read [CONTRIBUTING.md](CONTRIBUTING.md).

### Top contributors

<a href="https://github.com/MarcusPy827/Husky-Panel/contributors">
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
* [PAUVControl-Qt](https://github.com/lxqt/pavucontrol-qt.git)

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