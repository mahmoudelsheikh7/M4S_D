#!/usr/bin/env bash
# ==============================================================================
#  m4s d — Linux Installer v1.0.0
#  GitHub : https://github.com/mahmoudelsheikh7/M4S_D
#  Supports: Arch (pacman) · Debian/Ubuntu (apt) · Fedora (dnf) · openSUSE (zypper)
# ==============================================================================
set -euo pipefail

RED='\033[0;31m'  GRN='\033[0;32m'  YLW='\033[1;33m'
BLU='\033[0;34m'  CYN='\033[0;36m'  BOLD='\033[1m'    NC='\033[0m'

info()    { echo -e "${CYN}[INFO]${NC}  $*"; }
success() { echo -e "${GRN}[OK]${NC}    $*"; }
warn()    { echo -e "${YLW}[WARN]${NC}  $*"; }
fail()    { echo -e "${RED}[ERR]${NC}   $*" >&2; exit 1; }
step()    { echo -e "\n${BOLD}${BLU}━━━  $*  ━━━${NC}"; }

print_banner() {
  cat <<'EOF'

  ███╗   ███╗██╗  ██╗███████╗    ██████╗
  ████╗ ████║██║  ██║██╔════╝    ██╔══██╗
  ██╔████╔██║███████║███████╗    ██║  ██║
  ██║╚██╔╝██║╚════██║╚════██║    ██║  ██║
  ██║ ╚═╝ ██║     ██║███████║    ██████╔╝
  ╚═╝     ╚═╝     ╚═╝╚══════╝    ╚═════╝

  m4s d — Universal Downloader + Vocal AI
  Linux Installer v1.0.0
  https://github.com/mahmoudelsheikh7/M4S_D

EOF
}

# ─── Detect package manager ───────────────────────────────────────────────────

detect_pm() {
  if   command -v pacman  &>/dev/null; then echo "pacman"
  elif command -v apt-get &>/dev/null; then echo "apt"
  elif command -v dnf     &>/dev/null; then echo "dnf"
  elif command -v zypper  &>/dev/null; then echo "zypper"
  else fail "No supported package manager found (pacman/apt/dnf/zypper)."; fi
}

# ─── System dependencies ──────────────────────────────────────────────────────

install_system_deps() {
  local pm="$1"
  step "Installing system dependencies via $pm"

  case "$pm" in
    pacman)
      sudo pacman -Sy --noconfirm \
        qt6-base qt6-tools cmake base-devel git \
        ffmpeg yt-dlp \
        python python-pipx
      ;;
    apt)
      sudo apt-get update -qq
      sudo apt-get install -y \
        qt6-base-dev qt6-tools-dev cmake build-essential git \
        ffmpeg \
        python3 python3-pip pipx
      if ! command -v yt-dlp &>/dev/null; then
        info "Installing yt-dlp upstream binary …"
        sudo curl -fsSL \
          https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp \
          -o /usr/local/bin/yt-dlp
        sudo chmod a+rx /usr/local/bin/yt-dlp
      fi
      ;;
    dnf)
      sudo dnf install -y \
        https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm \
        2>/dev/null || warn "RPM Fusion already enabled."
      sudo dnf install -y \
        qt6-qtbase-devel qt6-qttools-devel cmake gcc-c++ make git \
        ffmpeg yt-dlp \
        python3 python3-pip pipx
      ;;
    zypper)
      sudo zypper --non-interactive refresh
      sudo zypper --non-interactive install \
        qt6-base-devel qt6-tools-devel cmake gcc-c++ make git \
        ffmpeg yt-dlp \
        python3 python3-pip python3-pipx
      ;;
  esac
  success "System dependencies installed."
}

# ─── Demucs ───────────────────────────────────────────────────────────────────

install_demucs() {
  step "Installing demucs (AI vocal separation) via pipx"

  if ! command -v pipx &>/dev/null; then
    python3 -m pip install --user pipx || warn "pip install pipx failed — trying anyway."
  fi

  export PATH="$HOME/.local/bin:$PATH"
  pipx ensurepath --force 2>/dev/null || true

  info "Running: pipx install demucs  (downloads PyTorch — may take several minutes)"
  pipx install demucs --pip-args="--no-cache-dir" 2>&1 || {
    warn "pipx install failed. Trying upgrade path …"
    pipx upgrade demucs --pip-args="--no-cache-dir" 2>&1 || true
  }

  info "Running: pipx inject demucs torchcodec  (critical audio-save fix)"
  pipx inject demucs torchcodec --pip-args="--no-cache-dir" 2>&1 || {
    warn "torchcodec injection failed. The app will still work but may hit audio-save"
    warn "errors on some systems. Manual fix: pipx inject demucs torchcodec"
  }

  if command -v demucs &>/dev/null || "$HOME/.local/bin/demucs" --help &>/dev/null 2>&1; then
    success "demucs + torchcodec ready."
  else
    warn "demucs could not be confirmed. The app will warn at startup."
    warn "Manual install: pipx install demucs"
  fi
}

# ─── Build ─────────────────────────────────────────────────────────────────────

build_app() {
  local src_dir="$1"
  step "Building m4s_d with CMake"

  [[ -f "$src_dir/CMakeLists.txt" ]] || fail "CMakeLists.txt not found in $src_dir"
  [[ -f "$src_dir/main.cpp"       ]] || fail "main.cpp not found in $src_dir"

  local build_dir="$src_dir/build"
  mkdir -p "$build_dir"
  cd "$build_dir"

  cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -10
  make -j"$(nproc)" 2>&1

  [[ -f "m4s_d" ]] || fail "Build failed — binary not produced."
  success "Build succeeded."
  cd "$src_dir"
}

# ─── Install binary ────────────────────────────────────────────────────────────

install_binary() {
  local build_dir="$1/build"
  step "Installing binary to /usr/local/bin/m4s_d"
  sudo install -m 755 "$build_dir/m4s_d" /usr/local/bin/m4s_d
  success "Binary installed at /usr/local/bin/m4s_d"
}

# ─── Desktop entry ─────────────────────────────────────────────────────────────

create_desktop_entry() {
  step "Creating .desktop entry"
  sudo tee /usr/share/applications/m4s_d.desktop >/dev/null <<'DESK'
[Desktop Entry]
Version=1.0
Type=Application
Name=m4s d
GenericName=Universal Downloader & Vocal Extractor
Comment=Download media with yt-dlp and isolate vocals with Meta Demucs AI
Exec=m4s_d
Icon=applications-multimedia
Terminal=false
Categories=AudioVideo;Network;Utility;
Keywords=youtube;download;vocal;demucs;yt-dlp;m4a;mp4;ai;
StartupWMClass=m4s_d
DESK
  command -v update-desktop-database &>/dev/null && \
    sudo update-desktop-database /usr/share/applications/ 2>/dev/null || true
  success ".desktop entry created."
}

# ─── Success banner ────────────────────────────────────────────────────────────

print_success() {
  cat <<'MSG'

  ╔══════════════════════════════════════════════════════════╗
  ║           m4s d v1.0 — Installation Complete!            ║
  ╠══════════════════════════════════════════════════════════╣
  ║                                                          ║
  ║  Terminal :  m4s_d                                       ║
  ║  App Menu :  "m4s d"                                     ║
  ║  Output   :  ~/Music/m4s_d/   (configurable in-app)      ║
  ║                                                          ║
  ║  No Music :  demucs htdemucs_ft · shifts=2               ║
  ║              saves "Title (no music).m4a"                ║
  ║              deletes original to save disk space         ║
  ║                                                          ║
  ║  NOTE: If demucs is not found, add to PATH:              ║
  ║    echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
  ║    source ~/.bashrc                                      ║
  ║                                                          ║
  ╚══════════════════════════════════════════════════════════╝

MSG
}

# ─── Main ──────────────────────────────────────────────────────────────────────

main() {
  print_banner

  [[ "$EUID" -eq 0 ]] && fail "Do not run as root. The script uses sudo internally."

  local PM
  PM="$(detect_pm)"
  info "Package manager: $PM"

  install_system_deps "$PM"
  install_demucs

  # Determine source directory (where this script lives, or cwd)
  local SCRIPT_DIR
  SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

  build_app "$SCRIPT_DIR"
  install_binary "$SCRIPT_DIR"
  create_desktop_entry
  print_success
}

main "$@"
