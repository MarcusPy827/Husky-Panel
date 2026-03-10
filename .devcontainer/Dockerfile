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
        which \
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
RUN pip3 install --root-user-action=ignore cpplint

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

# ── GPG runtime directory (needed for keyboxd in containers) ─────────────────
RUN sudo mkdir -p /run/user/${USER_UID} \
    && sudo chown ${USERNAME}:${USERNAME} /run/user/${USER_UID} \
    && sudo chmod 700 /run/user/${USER_UID}
ENV XDG_RUNTIME_DIR=/run/user/1000

# ── Colourful bash prompt & aliases ──────────────────────────────────────────
RUN echo '\n\
# Colour prompt\n\
export PS1="\\[\\033[01;32m\\]\\u@\\h\\[\\033[00m\\]:\\[\\033[01;34m\\]\\w\\[\\033[00m\\]\\$ "\n\
# Colour ls / grep\n\
alias ls="ls --color=auto"\n\
alias grep="grep --color=auto"\n\
' >> ~/.bashrc

WORKDIR /workspace
