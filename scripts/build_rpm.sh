#!/usr/bin/env bash
# ---------------------------------------------------------------
# Usage:
#   ./scripts/build_rpm.sh            # prepare only
#   ./scripts/build_rpm.sh --build    # prepare + rpmbuild
# ---------------------------------------------------------------
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
  log "$1" "\x1b[1;4m$2\x1b[0m: $3\x1b[0m"
}

bold() {
  # $1: content
  echo -e "\x1b[1m$1\x1b[0m"
}

# ======================== Variables ========================

APP_NAME="husky-panel"
APP_VERSION="0.9.3"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

CI_BUILD_DIR="${PROJECT_ROOT}/ci_build"
TARBALL_NAME="${APP_NAME}-${APP_VERSION}"

DO_BUILD=false
for arg in "$@"; do
  case "$arg" in
    --build) DO_BUILD=true ;;
    *)
      log "error" "Unknown argument: $arg"
      log "info" "Usage: $0 [--build]"
      exit 1
      ;;
  esac
done

# ======================== Banner ========================

bold "HuskyPanel RPM Build Helper"
echo "==========================="
log "info" "App: ${APP_NAME}  Version: ${APP_VERSION}"
if $DO_BUILD; then
  log "info" "Mode: prepare + build RPM"
else
  log "info" "Mode: prepare only (pass --build to also run rpmbuild)"
fi

# ======================== Clean ci_build ========================

log "info" "\n"
if [ -d "${CI_BUILD_DIR}" ]; then
  if [ "$(ls -A "${CI_BUILD_DIR}" 2>/dev/null)" ]; then
    log "warn" "Cleaning previous ci_build artifacts in \"${CI_BUILD_DIR}\"..."
    rm -rf "${CI_BUILD_DIR:?}"/*
    log "ok" "ci_build directory cleaned."
  fi
else
  mkdir -p "${CI_BUILD_DIR}"
  log "ok" "Created ci_build directory: ${CI_BUILD_DIR}"
fi

# ======================== Source Tarball ========================

log "info" "\n"
log "info" "Creating source tarball..."

if git -C "${PROJECT_ROOT}" rev-parse --is-inside-work-tree > /dev/null 2>&1; then
    # Use git archive for a clean, reproducible tarball.
    git -C "${PROJECT_ROOT}" archive \
        --format=tar.gz \
        --prefix="${TARBALL_NAME}/" \
        -o "${CI_BUILD_DIR}/${TARBALL_NAME}.tar.gz" \
        HEAD

    # git-archive does not include submodule contents.  If any submodules
    # exist we need to layer them into the tarball.
    if [[ -f "${PROJECT_ROOT}/.gitmodules" ]]; then
        log "info" "Packing submodules into tarball..."
        TMPDIR_SUB="$(mktemp -d)"
        trap 'rm -rf "${TMPDIR_SUB}"' EXIT

        mkdir -p "${TMPDIR_SUB}/${TARBALL_NAME}"
        tar xzf "${CI_BUILD_DIR}/${TARBALL_NAME}.tar.gz" -C "${TMPDIR_SUB}"

        git -C "${PROJECT_ROOT}" submodule foreach --quiet --recursive \
            "git archive --prefix=\"${TARBALL_NAME}/\${sm_path}/\" HEAD | tar xf - -C \"${TMPDIR_SUB}\""

        tar czf "${CI_BUILD_DIR}/${TARBALL_NAME}.tar.gz" -C "${TMPDIR_SUB}" "${TARBALL_NAME}"
        log "ok" "Submodules packed."
    fi
else
    log "warn" "Not inside a git repository — falling back to plain tar."
    tar czf "${CI_BUILD_DIR}/${TARBALL_NAME}.tar.gz" \
        --transform "s|^.|${TARBALL_NAME}|" \
        --exclude='.git' \
        --exclude='build' \
        --exclude='ci_build' \
        -C "${PROJECT_ROOT}" .
fi

log "ok" "Source tarball created: ${CI_BUILD_DIR}/${TARBALL_NAME}.tar.gz"

# ======================== Copy Spec ========================

log "info" "Copying spec file..."
SPEC_SRC="${PROJECT_ROOT}/packages/${APP_NAME}.spec"

if [ ! -f "${SPEC_SRC}" ]; then
  log "error" "Spec file not found at ${SPEC_SRC}"
  exit 1
fi

cp "${SPEC_SRC}" "${CI_BUILD_DIR}/"
log "ok" "Spec copied to ci_build/."

# ======================== Build RPM ========================

if $DO_BUILD; then
  log "info" "\n"
  log "info" "Setting up rpmbuild tree..."

  RPMBUILD_DIR="${CI_BUILD_DIR}/rpmbuild"
  mkdir -p "${RPMBUILD_DIR}"/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

  cp "${CI_BUILD_DIR}/${TARBALL_NAME}.tar.gz" "${RPMBUILD_DIR}/SOURCES/"
  cp "${CI_BUILD_DIR}/${APP_NAME}.spec"        "${RPMBUILD_DIR}/SPECS/"

  log "info" "Running rpmbuild... this may take a while."
  rpmbuild \
    --define "_topdir ${RPMBUILD_DIR}" \
    -ba "${RPMBUILD_DIR}/SPECS/${APP_NAME}.spec"

  log "ok" "RPM build finished!"
  log "info" "RPMs:"
  find "${RPMBUILD_DIR}/RPMS" -name '*.rpm' -exec echo "  {}" \;
  log "info" "SRPMs:"
  find "${RPMBUILD_DIR}/SRPMS" -name '*.rpm' -exec echo "  {}" \;
fi

# ======================== Summary ========================

log "info" "\n"
log "ok" "All done! Artifacts in ${CI_BUILD_DIR}/:"
ls -lh "${CI_BUILD_DIR}/" | tail -n +2

if ! $DO_BUILD; then
  log "info" "\n"
  log "info" "To build the RPM, either re-run with --build:"
  log "info" "  $0 --build"
  log "info" "Or manually:"
  log "info" "  rpmbuild -tb ${CI_BUILD_DIR}/${TARBALL_NAME}.tar.gz"
fi
