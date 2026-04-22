# m4s d

<div align="center">

```
  ███╗   ███╗██╗  ██╗███████╗    ██████╗
  ████╗ ████║██║  ██║██╔════╝    ██╔══██╗
  ██╔████╔██║███████║███████╗    ██║  ██║
  ██║╚██╔╝██║╚════██║╚════██║    ██║  ██║
  ██║ ╚═╝ ██║     ██║███████║    ██████╔╝
  ╚═╝     ╚═╝     ╚═╝╚══════╝    ╚═════╝
```

**Universal Downloader & Vocal Extractor**

[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-blue?style=flat-square)](https://github.com/mahmoudelsheikh7/M4S_D)
[![Language](https://img.shields.io/badge/Language-C++17%20%7C%20Qt6-green?style=flat-square&logo=qt)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)](LICENSE)
[![AI Model](https://img.shields.io/badge/AI-htdemucs__ft-purple?style=flat-square)](https://github.com/facebookresearch/demucs)
[![Offline](https://img.shields.io/badge/Tools-Offline%20First-red?style=flat-square)](https://github.com/mahmoudelsheikh7/M4S_D)

**→ [github.com/mahmoudelsheikh7/M4S_D](https://github.com/mahmoudelsheikh7/M4S_D)**

</div>

---

## What is m4s d?

**m4s d** is a production-grade **C++17 / Qt6** desktop application that combines two powerful pipelines in a single, non-blocking GUI:

- **Universal Downloader** — uses `yt-dlp` to download from YouTube and **1,000+ other sites**, defaulting to `.m4a` for audio and `.mp4` for video.
- **"No Music" Vocal Extractor** — uses Meta's `demucs` AI (`htdemucs_ft` model) to isolate vocals, saves `Title (no music).m4a`, and **automatically deletes** the original file to save disk space.

All tools (`yt-dlp`, `ffmpeg`, `demucs`) are downloaded **once** into a local data directory and reused on every launch — no repeated downloads, no internet required after setup.

---

## Features

| Feature | Details |
|---|---|
| 🌐 **Universal Download** | yt-dlp — YouTube, SoundCloud, Twitter/X, Instagram, and 1,000+ more |
| 🎵 **Audio Default** | Forces `.m4a` (AAC best quality) — no manual format hunting |
| 🎬 **Video Default** | Forces `.mp4` (best quality with auto-merge) |
| ✨ **No Music Pipeline** | htdemucs_ft · shifts=2 · two-stems=vocals → zero music bleed |
| 🗑️ **Auto Cleanup** | Deletes original + demucs temp files — keeps only `(no music).m4a` |
| 📦 **Offline First** | Tools downloaded once to `AppData/m4s_d/bin/` — reused forever |
| 🔄 **Auto Repair** | Startup diagnostic detects missing/broken tools, re-downloads if needed |
| ⚙️ **Settings** | Browse download directory; CPU/CUDA selector — all persisted via `QSettings` |
| 🔇 **Non-blocking UI** | 100% `QProcess`-driven — the GUI never freezes |
| 🖥️ **Cross-Platform** | Windows (PowerShell setup) + Linux (Bash setup, 4 distros) |
| 🔄 **Auto CUDA Fallback** | CUDA failure silently retried on CPU — no user action needed |

---

## UI Overview

```
┌───────────────────────────────────────────────────────────┐
│  m4s d  v1.0 — Universal Downloader + Vocal AI  [⚙ Settings] [⬇ Setup Tools] │
├───────────────────────────────────────────────────────────┤
│ ╔══ Download ════════════════════════════════════════════╗ │
│ ║ URL: [https://youtube.com/watch?v=…          ] [Paste] ║ │
│ ║ Format: [🎵 Audio (.m4a — default)   ▼]               ║ │
│ ║ ✨ No Music — AI vocal extraction (demucs) →           ║ │
│ ║    saves "Title (no music).m4a", deletes original      ║ │
│ ╚════════════════════════════════════════════════════════╝ │
│                                                           │
│  [▶ Start]  [■ Stop]                      [Clear Log]    │
│ ────────────────────────────────────────────────────────  │
│ ╔══ Process Log ════════════════════════════════════════╗ │
│ ║  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━      ║ │
│ ║  m4s d — Self-Diagnostic                              ║ │
│ ║  [OK] yt-dlp  → ~/.local/share/m4s/m4s_d/bin/yt-dlp  ║ │
│ ║  [OK] ffmpeg  → /usr/bin/ffmpeg                       ║ │
│ ║  [OK] demucs  → ~/.local/bin/demucs                   ║ │
│ ║  Ready — paste a URL and click Start.                 ║ │
│ ╚═══════════════════════════════════════════════════════╝ │
└───────────────────────────────────────────────────────────┘
```

---

## The "No Music" Pipeline

```
URL Input
    │
    └── Step 1: yt-dlp
            -f bestaudio[ext=m4a]/bestaudio/best
            -x --audio-format m4a --audio-quality 0
            → "Song Title.m4a"
                    │
                    └── Step 2: demucs
                            -n htdemucs_ft
                            --two-stems=vocals
                            --shifts=2
                            -d cpu|cuda
                            → outdir/htdemucs_ft/Song Title/vocals.wav
                                    │
                                    └── Step 3: ffmpeg
                                            -c:a aac -b:a 256k
                                            → "Song Title (no music).m4a"

    Cleanup (automatic):
        ✗ Delete "Song Title.m4a"              (original with music)
        ✗ Delete outdir/htdemucs_ft/Song Title/ (demucs temp folder)
        ✓ Keep  "Song Title (no music).m4a"    (vocals only)
```

---

## Offline-First Dependency Manager

On every launch, the app checks for tools in this priority order:

1. **Local data directory** — `~/.local/share/m4s/m4s_d/bin/` (Linux) or `%LOCALAPPDATA%\m4s\m4s_d\bin\` (Windows)
2. **System PATH** — uses system-installed tools as fallback
3. **Pipx location** — `~/.local/bin/demucs` (Linux)
4. **Venv** — `%LOCALAPPDATA%\m4s\m4s_d\venv\Scripts\demucs.exe` (Windows)

If `yt-dlp` or `ffmpeg` are missing, click **"⬇ Setup Tools"** in the app — it will download and save them to the local data directory automatically, with no further setup needed on subsequent launches.

---

## Installation

### Linux (Recommended)

```bash
# 1. Clone the repository
git clone https://github.com/mahmoudelsheikh7/M4S_D.git
cd M4S_D

# 2. Make installer executable
chmod +x install_linux.sh

# 3. Run (do NOT use sudo — it calls sudo internally when needed)
./install_linux.sh
```

The installer automatically:
1. Detects your package manager (`pacman` / `apt` / `dnf` / `zypper`)
2. Installs Qt6, CMake, build tools, `ffmpeg`, `yt-dlp`, `pipx`
3. Installs `demucs` via `pipx install demucs`
4. Injects `torchcodec` via `pipx inject demucs torchcodec` *(prevents audio-save crashes)*
5. Compiles the Qt6 application with CMake
6. Installs binary to `/usr/local/bin/m4s_d`
7. Creates a `.desktop` launcher in your application menu

Launch after install:
```bash
m4s_d
# or open "m4s d" from your application menu
```

#### Supported Distributions

| Distribution | Package Manager | Status |
|---|---|---|
| Arch Linux / Manjaro / EndeavourOS | `pacman` | ✅ Full support |
| Ubuntu / Debian / Linux Mint | `apt` | ✅ Full support |
| Fedora / RHEL / Rocky | `dnf` | ✅ Full support |
| openSUSE Tumbleweed / Leap | `zypper` | ✅ Full support |

---

### Windows

```powershell
# 1. Clone the repository
git clone https://github.com/mahmoudelsheikh7/M4S_D.git
cd M4S_D

# 2. Allow script execution (one-time)
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned

# 3. Run the setup script
.\install_windows.ps1
```

The PowerShell script automatically:
1. Creates `%LOCALAPPDATA%\m4s\m4s_d\bin\`
2. Downloads `yt-dlp.exe` from the official GitHub release
3. Downloads and extracts `ffmpeg.exe` (static build)
4. Creates a Python venv and installs `demucs` + `torchcodec`
5. Prints build instructions for compiling the Qt6 app

**Building the app on Windows** (after running the setup script):

**Option A — MSYS2 (Recommended):**
```bash
# In MSYS2 MinGW 64-bit terminal:
pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc make
cd /c/path/to/M4S_D
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make -j$(nproc)
```

**Option B — Qt Creator:**
Install Qt6 from [qt.io](https://www.qt.io/download-open-source), open `CMakeLists.txt` in Qt Creator, and build with Release configuration.

---

## Manual Build (Linux)

```bash
# Arch
sudo pacman -S qt6-base qt6-tools cmake base-devel ffmpeg yt-dlp python-pipx

# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential ffmpeg pipx python3

# Install demucs with audio-save fix
pipx install demucs
pipx inject demucs torchcodec

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./m4s_d
```

---

## Output Structure

```
~/Music/m4s_d/                         ← configurable via Settings
├── Song Title.mp4                     ← video download (no music mode off)
├── Song Title.m4a                     ← audio download (no music mode off)
│
└── [No Music mode ON — only these files are kept:]
    └── Song Title (no music).m4a      ← vocals isolated ✨
        (original .m4a deleted · demucs temp folder deleted)
```

---

## CUDA / GPU Notes

- Select **CPU (Always Safe)** in Settings for guaranteed compatibility.
- Select **CUDA** for 3-10× faster processing on NVIDIA RTX / A-series cards.
- If CUDA fails (common on older Quadro/GTX cards), the app **automatically retries on CPU** — no user action needed.

---

## Troubleshooting

**`demucs: command not found` after install:**
```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc && source ~/.bashrc
```

**Demucs audio-save error (`torchaudio.backend.sox_io_backend`):**
```bash
pipx inject demucs torchcodec
```

**yt-dlp HTTP 403 error:**
```bash
yt-dlp -U   # or: pipx upgrade yt-dlp
```

**App shows all tools as missing on first Windows run:**
→ Click **"⬇ Setup Tools"** — it will download and save yt-dlp.exe and ffmpeg.exe automatically.

---

## Dependencies

| Tool | Role | Installed By |
|---|---|---|
| **Qt6** (Core, Gui, Widgets, Network) | GUI framework | System / Qt Installer |
| **CMake 3.16+** | Build system | System package manager |
| **yt-dlp** | Universal media downloader | install script / in-app Setup |
| **ffmpeg** | Audio conversion (WAV → M4A) | install script / in-app Setup |
| **Python 3** | Runtime for demucs | System package manager |
| **pipx** | Isolated Python environment | System package manager |
| **demucs** | AI vocal separation | `pipx install demucs` |
| **torchcodec** | Audio I/O fix (replaces torchaudio) | `pipx inject demucs torchcodec` |

---

## License

MIT License — see [LICENSE](LICENSE).

Underlying tools have their own licenses:
- **yt-dlp** — The Unlicense
- **demucs** — MIT (Meta AI Research)
- **Qt6** — LGPL v3 / GPL v3 / Commercial

---

<div align="center">

Built with ❤️ using **C++17 · Qt6 · yt-dlp · ffmpeg · demucs htdemucs_ft**

[github.com/mahmoudelsheikh7/M4S_D](https://github.com/mahmoudelsheikh7/M4S_D)

</div>
