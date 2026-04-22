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

**Universal Downloader & AI Vocal Extractor**

[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-blue?style=flat-square)](https://github.com/mahmoudelsheikh7/M4S_D)
[![Language](https://img.shields.io/badge/Language-C++17%20%7C%20Qt6-green?style=flat-square&logo=qt)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)](LICENSE)
[![AI Model](https://img.shields.io/badge/AI-htdemucs__ft-purple?style=flat-square)](https://github.com/facebookresearch/demucs)
[![Bilingual](https://img.shields.io/badge/UI-Arabic%20%7C%20English-orange?style=flat-square)](https://github.com/mahmoudelsheikh7/M4S_D)
[![Offline](https://img.shields.io/badge/Tools-Offline%20First-red?style=flat-square)](https://github.com/mahmoudelsheikh7/M4S_D)

**→ [github.com/mahmoudelsheikh7/M4S_D](https://github.com/mahmoudelsheikh7/M4S_D)**

</div>

---

## What is m4s d?

**m4s d** is a production-grade **C++17 / Qt6** desktop application combining two powerful pipelines in a single, non-blocking GUI:

- **Universal Downloader** — uses `yt-dlp` to download from YouTube and **1,000+ other sites**, with full control over audio format (MP3, M4A, WAV, FLAC) and video quality (Best, 4K, 1080p, 720p, 480p).
- **Local File Processing** — browse any local video (`.mp4`, `.mkv`, `.avi`, `.mov`, etc.) or audio file and process it through the same pipeline without downloading anything.
- **"No Music" Vocal Extractor** — uses Meta's `demucs` AI (`htdemucs_ft` model) to isolate vocals, saves `Title (no music).[ext]`, and **automatically deletes** the original file.
- **Extreme Vocal Isolation** — optional `shifts=4` + `overlap=0.25` mode for maximum accuracy.

All tools (`yt-dlp`, `ffmpeg`, `demucs`) are downloaded **once** into a local data directory and reused on every launch — no repeated downloads, no internet required after setup.

---

## Features

| Feature | Details |
|---|---|
| 🌐 **Universal Download** | yt-dlp — YouTube, SoundCloud, Twitter/X, Instagram, TikTok, and 1,000+ more |
| 🎵 **Audio Formats** | MP3 · M4A (default) · WAV (lossless) · FLAC (lossless) |
| 🎬 **Video Quality** | Best · 4K/2160p · 1080p HD · 720p · 480p (all .mp4) |
| 📂 **Local Files** | Browse any .mp4/.mkv/.avi/.mov/audio file — video auto-extracts audio |
| ✨ **No Music Pipeline** | htdemucs_ft · shifts=2/4 · two-stems=vocals → zero music bleed |
| ⚡ **Extreme Mode** | shifts=4 + overlap=0.25 for maximum separation accuracy |
| 🗑️ **Auto Cleanup** | Deletes original + demucs temp files — keeps only `(no music).[ext]` |
| 🔄 **Format Conversion** | ffmpeg converts vocals.wav to any selected audio format automatically |
| 📦 **Offline First** | Tools downloaded once to local app data dir — reused forever |
| 🔄 **Auto Repair** | Startup diagnostic detects missing/broken tools, re-downloads if needed |
| 🌍 **Bilingual UI** | Arabic RTL / English LTR — one-click toggle, fully bidirectional |
| 🎨 **Dark / Light Theme** | Theme toggle persisted across sessions via QSettings |
| ⚙️ **Settings** | Browse download directory + CPU/CUDA device selector, all persisted |
| 🔇 **Non-blocking UI** | 100% QProcess-driven — the GUI never freezes |
| 🖥️ **Cross-Platform** | Windows (PowerShell setup) + Linux (Bash setup, 4 distros) |
| 🔄 **Auto CUDA Fallback** | CUDA failure silently retried on CPU — no user action needed |

---

## UI Overview

```
┌──────────────────────────────────────────────────────────────────┐
│  m4s d  v2.0                    [عربي] [☀] [⬇ Setup Tools] [⚙]  │
├──────────────────────────────────────────────────────────────────┤
│ ╔══ Download / Process ══════════════════════════════════════════╗ │
│ ║ URL: [https://youtube.com/watch?v=…               ] [📋 Paste] ║ │
│ ║      [📂 Browse File] [✕]  Local: MySong.mp3                   ║ │
│ ║ Format/Quality: [🎵 Audio — M4A (.m4a — default)  ▼]          ║ │
│ ║ ✨ No Music — AI vocal extraction → "Title (no music).m4a"     ║ │
│ ║ ⚡ Extreme Vocal Isolation — shifts=4, overlap=0.25            ║ │
│ ╚════════════════════════════════════════════════════════════════╝ │
│                                                                    │
│  [▶ Start]  [■ Stop]                              [Clear Log]     │
│  ════════════════════════════════════════════════════════          │
│ ╔══ Process Log ════════════════════════════════════════════════╗  │
│ ║  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━       ║  │
│ ║  m4s d — Self-Diagnostic                                      ║  │
│ ║  [OK]  yt-dlp  → ~/.local/share/m4s/m4s_d/bin/yt-dlp         ║  │
│ ║  [OK]  ffmpeg  → /usr/bin/ffmpeg                              ║  │
│ ║  [OK]  demucs  → ~/.local/bin/demucs                          ║  │
│ ║  Ready — paste a URL or browse a local file, then click Start ║  │
│ ╚═══════════════════════════════════════════════════════════════╝  │
└──────────────────────────────────────────────────────────────────┘
```

---

## Supported Formats

### Audio Output
| Format | Codec | Quality |
|---|---|---|
| **MP3** | libmp3lame | -q:a 0 (VBR best) |
| **M4A** | AAC | 256 kbps CBR |
| **WAV** | PCM / copy | Lossless |
| **FLAC** | FLAC | Lossless |

### Video Output
| Quality | Format | Notes |
|---|---|---|
| **Best** | .mp4 | Highest available resolution |
| **4K / 2160p** | .mp4 | height ≤ 2160 |
| **1080p HD** | .mp4 | height ≤ 1080 |
| **720p** | .mp4 | height ≤ 720 |
| **480p** | .mp4 | height ≤ 480 |

---

## The "No Music" Pipeline

```
URL Input  ─────────────────────────┐
                                    │
Local File ─────────────────────────┤
  │                                 │
  └──[Video?] → Step 0: ffmpeg     │
                 -i video.mp4       │
                 -vn -q:a 0 -map a  │
                 → extracted.wav    │
                         │          │
                         ▼          ▼
                    Step 1: yt-dlp (URL only)
                         -f bestaudio[ext=m4a]
                         -x --audio-format m4a
                         → "Song Title.m4a"
                                    │
                                    ▼
                    Step 2: demucs (if No Music ✓)
                         -n htdemucs_ft
                         --two-stems=vocals
                         --shifts=2|4
                         --overlap=0.1|0.25
                         -d cpu|cuda
                         → outdir/htdemucs_ft/Song Title/vocals.wav
                                    │
                                    ▼
                    Step 3: ffmpeg (format conversion)
                         -c:a aac -b:a 256k   (→ .m4a)
                         -c:a libmp3lame -q:a 0  (→ .mp3)
                         -c:a flac               (→ .flac)
                         copy                    (→ .wav)
                         → "Song Title (no music).m4a"

    Auto-Cleanup:
        ✗ Delete "Song Title.m4a"               ← original with music
        ✗ Delete outdir/htdemucs_ft/Song Title/ ← demucs temp
        ✗ Delete extracted.wav                  ← video audio extract
        ✓ Keep  "Song Title (no music).m4a"     ← vocals only
```

---

## Offline-First Dependency Manager

On every launch, the app checks for tools in priority order:

1. **Local data dir** — `~/.local/share/m4s/m4s_d/bin/` (Linux) or `%LOCALAPPDATA%\m4s\m4s_d\bin\` (Windows)
2. **System PATH** — system-installed tools used as fallback
3. **Pipx location** — `~/.local/bin/demucs` (Linux)
4. **Venv** — `%LOCALAPPDATA%\m4s\m4s_d\venv\Scripts\demucs.exe` (Windows)

Click **"⬇ Setup Tools"** in the app to download `yt-dlp` and `ffmpeg` automatically. Demucs requires the install script.

---

## Installation

### Linux (Recommended)

```bash
# 1. Clone the repository
git clone https://github.com/mahmoudelsheikh7/M4S_D.git
cd M4S_D

# 2. Make installer executable
chmod +x install_linux.sh

# 3. Run — do NOT use sudo (calls sudo internally where needed)
./install_linux.sh

# 4. Launch
m4s_d
```

The installer automatically:
1. Detects your package manager (`pacman` / `apt` / `dnf` / `zypper`)
2. Installs Qt6, CMake, build tools, `ffmpeg`, `yt-dlp`, `pipx`
3. Installs `demucs` via `pipx install demucs`
4. Injects `torchcodec` via `pipx inject demucs torchcodec` *(audio-save fix)*
5. Compiles the Qt6 C++17 application with CMake
6. Installs binary to `/usr/local/bin/m4s_d`
7. Creates a `.desktop` launcher

#### Supported Linux Distributions

| Distribution | Package Manager | Status |
|---|---|---|
| Arch Linux / Manjaro / EndeavourOS | `pacman` | ✅ Full support |
| Ubuntu / Debian / Linux Mint | `apt` | ✅ Full support |
| Fedora / RHEL / Rocky Linux | `dnf` | ✅ Full support |
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
3. Downloads and extracts `ffmpeg.exe` (static GPL build, ~60 MB)
4. Creates a Python venv and installs `demucs` + `torchcodec`
5. Prints build instructions for compiling the Qt6 app

**Building the app on Windows** (after setup script):

**Option A — MSYS2 (Recommended):**
```bash
# In MSYS2 MinGW 64-bit terminal:
pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-tools \
          mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc make
cd /c/path/to/M4S_D
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make -j$(nproc)
```

**Option B — Qt Creator:**
Install Qt6 from [qt.io](https://www.qt.io/download-open-source), open `CMakeLists.txt` in Qt Creator, and build in Release mode.

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
~/Music/m4s_d/                              ← configurable via ⚙ Settings
├── Song Title.mp4                          ← video download (no music mode off)
├── Song Title.mp3                          ← audio as MP3
├── Song Title.m4a                          ← audio as M4A (default)
├── Song Title.wav                          ← audio as WAV
├── Song Title.flac                         ← audio as FLAC
│
└── [No Music mode ON — only these kept:]
    ├── Song Title (no music).m4a           ← vocals isolated, M4A ✨
    ├── Song Title (no music).mp3           ← vocals isolated, MP3
    ├── Song Title (no music).wav           ← vocals isolated, WAV
    └── Song Title (no music).flac          ← vocals isolated, FLAC
         (original deleted · demucs temp deleted · extracted audio deleted)
```

---

## Bilingual UI

Click the **language button** (`عربي` / `EN`) in the toolbar to instantly toggle between:
- **English** — Left-to-Right layout
- **Arabic** — Full Right-to-Left bidirectional layout (`قSettingsLayout`)

Language preference is persisted via `QSettings` and restored on next launch.

---

## CUDA / GPU Notes

- Select **CPU (Always Safe)** in Settings for guaranteed compatibility.
- Select **CUDA** for 3–10× faster processing on NVIDIA RTX / A-series cards.
- If CUDA fails, the app **automatically retries on CPU** — no user action needed.
- **Extreme Mode** (`shifts=4`, `overlap=0.25`) available on both CPU and CUDA.

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
→ Click **"⬇ Setup Tools"** in the app — it downloads and saves `yt-dlp.exe` and `ffmpeg.exe` automatically.

**Format not available for a video:**
→ Try a lower quality setting (e.g., 1080p instead of 4K) — not all platforms offer all resolutions.

---

## Dependencies

| Tool | Role | Installed By |
|---|---|---|
| **Qt6** (Core, Gui, Widgets, Network) | GUI framework | System / Qt Installer |
| **CMake 3.16+** | Build system | System package manager |
| **yt-dlp** | Universal media downloader (1,000+ sites) | Install script / in-app Setup |
| **ffmpeg** | Audio conversion (WAV→MP3/M4A/FLAC), video audio extraction | Install script / in-app Setup |
| **Python 3** | Runtime for demucs | System package manager |
| **pipx** | Isolated Python environment | System package manager |
| **demucs** | AI vocal separation (htdemucs_ft) | `pipx install demucs` |
| **torchcodec** | Audio I/O fix (replaces torchaudio backend) | `pipx inject demucs torchcodec` |

---

## 💰 Support My Work

If **m4s d** has been useful to you — saved you time, improved your workflow, or just made you smile — please consider supporting its continued development. Every contribution, no matter the size, goes directly toward keeping this project free, open-source, and actively maintained.

**Donate via USDT (TRC20 — TRON Network):**

```
TLTRj89qEp1oPcEQjAsqiDWutd1ezd4W4y
```

> ⚠️ **Important:** Send **only USDT on the TRC20 (TRON) network** to this address.  
> Sending any other token or using a different network (e.g. ERC20, BEP20) will result in **permanent loss of funds**.

Thank you for your support — it means a lot! 🙏

---

## License

MIT License — see [LICENSE](LICENSE).

Underlying tools have their own licenses:
- **yt-dlp** — The Unlicense
- **demucs** — MIT (Meta AI Research)
- **ffmpeg** — LGPL v2.1+ / GPL v2+ (static builds: GPL v2+)
- **Qt6** — LGPL v3 / GPL v3 / Commercial

---

<div align="center">

Built with ❤️ using **C++17 · Qt6 · yt-dlp · ffmpeg · demucs htdemucs_ft**

[github.com/mahmoudelsheikh7/M4S_D](https://github.com/mahmoudelsheikh7/M4S_D)

</div>
