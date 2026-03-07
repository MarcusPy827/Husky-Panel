set -e

log() {
  # $1: level
  # $2: content

  case $1 in
    "info")
      echo -e "\x1b[39;1m[\x1b[34;1mINFO\x1b[39;1m]\x1b[0m $2\x1b[0m"
      ;;
    "warn")
      echo -e "\x1b[39;1m[\x1b[33;1mWARN\x1b[39;1m]\x1b[0m $2\x1b[0m"
      ;;
    "ok")
      echo -e "\x1b[39;1m[\x1b[32;1m OK \x1b[39;1m]\x1b[0m $2\x1b[0m"
      ;;
    "error")
      echo -e "\x1b[39;1m[\x1b[31;1mERROR\x1b[39;1m]\x1b[0m $2\x1b[0m"
      ;;
    *)
      echo -e "\x1b[39;1m[\x1b[36;1mDEBUG\x1b[39;1m]\x1b[0m $2\x1b[0m"
      ;;
  esac
}

log_tag() {
  # $1: level
  # $2: tag
  # $3: content
  log $1 "\x1b[1;4m$2\x1b[0m: $3\x1b[0m"
}

bold() {
  # $1: content
  echo -e "\x1b[1m$1\x1b[0m"
}

TARGET_DIR="build/ecm-build"
FULL_TARGET_DIR=$(readlink -f "$TARGET_DIR" 2>/dev/null || echo "$(pwd)/$TARGET_DIR")
INSTALL_DIR="build/ecm-install"
FULL_INSTALL_DIR=$(readlink -f "$INSTALL_DIR" 2>/dev/null || echo "$(pwd)/$INSTALL_DIR")

bold "ECM Configuration Helper"
echo "========================"

log "info" "This script will help build vendored ECM for HuskyPanel."
log "info" "The reason we vendor ECM is that the version of ECM required by HuskyPanel is usually newer than what we have in package manager if you have a non-rolling distro."
log "info" "Don't worry, the ECM we build here will NOT be installed to your system PATH."

log "info" "\n"
if [ ! -d "$TARGET_DIR" ]; then
    log "warn" "The build directory \"$FULL_TARGET_DIR\" is NOT found in your system."
    read -p "Do you want to create it?\n    (y/n) >> " confirm
    if [[ "$confirm" == [yY] || "$confirm" == [yY][eE][sS] ]]; then
        mkdir -p "$TARGET_DIR"
        log "ok" "Target directory should be created."
    else
        log "error" "Operation cancelled."
        exit 0
    fi
else
    if [ "$(ls -A "$TARGET_DIR")" ]; then
        log "warn" "The build directory \"$FULL_TARGET_DIR\" already exists and is NOT empty."
        read -p "Do you want to clean it?\nI mean... to delete everything in it?\n    (y/n) >> " confirm
        if [[ "$confirm" == [yY] || "$confirm" == [yY][eE][sS] ]]; then
            rm -rf "${TARGET_DIR:?}"/*
            log "ok" "Target directory should be cleaned."
        else
            log "info" "Existing contents will be retained."
        fi
    else
        log "info" "The build directory \"$FULL_TARGET_DIR\" already exists and is empty. No action needed."
    fi
fi

log "info" "\n"
if [ ! -d "$INSTALL_DIR" ]; then
    log "warn" "The install directory \"$FULL_INSTALL_DIR\" is NOT found in your system."
    read -p "Do you want to create it?\n    (y/n) >> " confirm
    if [[ "$confirm" == [yY] || "$confirm" == [yY][eE][sS] ]]; then
        mkdir -p "$INSTALL_DIR"
        log "ok" "Install directory should be created."
    else
        log "error" "Operation cancelled."
        exit 0
    fi
else
    if [ "$(ls -A "$INSTALL_DIR")" ]; then
        log "warn" "The install directory \"$FULL_INSTALL_DIR\" already exists and is NOT empty."
        read -p "Do you want to clean it?\nI mean... to delete everything in it?\n    (y/n) >> " confirm
        if [[ "$confirm" == [yY] || "$confirm" == [yY][eE][sS] ]]; then
            rm -rf "${INSTALL_DIR:?}"/*
            log "ok" "Install directory should be cleaned."
        else
            log "info" "Existing contents will be retained."
        fi
    else
        log "info" "The install directory \"$FULL_INSTALL_DIR\" already exists and is empty. No action needed."
    fi
fi

log "info" "\n"
log "info" "Now building ECM..."
log "info" "For the version of ECM vendored, please read \"lib/3rdparty/VERSIONS.md\"."
cmake -S lib/3rdparty/extra-cmake-modules \
    -B $TARGET_DIR \
    -DBUILD_TESTING=OFF \
    -DBUILD_DOCS=OFF \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
cmake --build $TARGET_DIR
cmake --install $TARGET_DIR

log "ok" "ECM should be built and available in \"$FULL_INSTALL_DIR/\" now."