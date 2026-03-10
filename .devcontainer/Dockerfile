# Fedora 43 development container for HuskyPanel
FROM fedora:43

# ── Build dependencies (mirrors packages/husky-panel.spec) ──────────────────
RUN dnf install -y \
        cmake \
        gcc-c++ \
        ninja-build \
        pkgconfig\(wayland-client\) \
        pkgconfig\(wayland-protocols\) \
        pkgconfig\(xkbcommon\) \
        qt6-qtbase-devel \
        qt6-qtbase-private-devel \
        qt6-qtwayland-devel \
        qt6-qttools-devel \
        layer-shell-qt-devel \
        libdbusmenu-lxqt-devel \
        kf6-kservice-devel \
        extra-cmake-modules \
    && dnf clean all

# ── Dev tools ────────────────────────────────────────────────────────────────
RUN dnf install -y \
        git \
        gdb \
        clang-tools-extra \
        rpm-build \
        python3 \
        python3-pip \
    && dnf clean all

# ── cpplint (Python) ────────────────────────────────────────────────────────
RUN pip3 install cpplint \
    && ln -sf "$(which cpplint)" /usr/local/bin/cpplint

# ── Non-root user (matches VS Code Remote default) ──────────────────────────
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=${USER_UID}
RUN groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME} \
    && dnf install -y sudo \
    && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME} \
    && dnf clean all

USER ${USERNAME}
WORKDIR /workspace
