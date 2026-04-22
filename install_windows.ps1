# ==============================================================================
#  m4s d — Windows Dependency Setup Script v1.0.0
#  GitHub : https://github.com/mahmoudelsheikh7/M4S_D
#
#  Run from PowerShell (as normal user, no admin needed):
#    Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
#    .\install_windows.ps1
#
#  What this script does:
#    1. Creates %LOCALAPPDATA%\m4s\m4s_d\bin\
#    2. Downloads yt-dlp.exe
#    3. Downloads and extracts ffmpeg.exe
#    4. Creates a Python venv and installs demucs + torchcodec
#    5. Prints build instructions for the Qt6 C++ app
# ==============================================================================

$ErrorActionPreference = "Stop"

# ─── Colour helpers ────────────────────────────────────────────────────────────

function Info   ($m) { Write-Host "  [INFO]  $m" -ForegroundColor Cyan    }
function Ok     ($m) { Write-Host "  [OK]    $m" -ForegroundColor Green   }
function Warn   ($m) { Write-Host "  [WARN]  $m" -ForegroundColor Yellow  }
function Err    ($m) { Write-Host "  [ERR]   $m" -ForegroundColor Red; exit 1 }
function Step   ($m) { Write-Host "`n━━━  $m  ━━━" -ForegroundColor Blue  }

# ─── Paths ─────────────────────────────────────────────────────────────────────

$AppData  = [System.Environment]::GetFolderPath("LocalApplicationData")
$DataDir  = Join-Path $AppData  "m4s\m4s_d"
$BinDir   = Join-Path $DataDir  "bin"
$VenvDir  = Join-Path $DataDir  "venv"
$TempDir  = Join-Path $env:TEMP "m4s_d_setup"

foreach ($d in @($BinDir, $TempDir)) {
    if (-not (Test-Path $d)) { New-Item -ItemType Directory -Path $d -Force | Out-Null }
}

Write-Host @"

  ███╗   ███╗██╗  ██╗███████╗    ██████╗
  ████╗ ████║██║  ██║██╔════╝    ██╔══██╗
  ██╔████╔██║███████║███████╗    ██║  ██║
  ██║╚██╔╝██║╚════██║╚════██║    ██║  ██║
  ██║ ╚═╝ ██║     ██║███████║    ██████╔╝
  ╚═╝     ╚═╝     ╚═╝╚══════╝    ╚═════╝

  m4s d — Windows Dependency Setup v1.0.0
  https://github.com/mahmoudelsheikh7/M4S_D

  Install dir : $DataDir

"@ -ForegroundColor Cyan

# ─── yt-dlp ────────────────────────────────────────────────────────────────────

Step "Downloading yt-dlp.exe"

$YtdlpDst = Join-Path $BinDir "yt-dlp.exe"

if (Test-Path $YtdlpDst) {
    Ok "yt-dlp.exe already present at $YtdlpDst"
} else {
    $YtdlpUrl = "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe"
    Info "Source: $YtdlpUrl"
    try {
        $wc = New-Object System.Net.WebClient
        $wc.DownloadFile($YtdlpUrl, $YtdlpDst)
        Ok "yt-dlp.exe saved → $YtdlpDst"
    } catch {
        Err "Failed to download yt-dlp.exe: $_"
    }
}

# ─── ffmpeg ────────────────────────────────────────────────────────────────────

Step "Downloading and extracting ffmpeg.exe"

$FfmpegDst = Join-Path $BinDir "ffmpeg.exe"

if (Test-Path $FfmpegDst) {
    Ok "ffmpeg.exe already present at $FfmpegDst"
} else {
    $FfmpegZipUrl = "https://github.com/yt-dlp/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl.zip"
    $FfmpegZip    = Join-Path $TempDir "ffmpeg.zip"
    $FfmpegExtract= Join-Path $TempDir "ffmpeg_extracted"

    Info "Source: $FfmpegZipUrl"
    Info "This may take a minute (~60 MB) …"
    try {
        $wc = New-Object System.Net.WebClient
        $wc.DownloadFile($FfmpegZipUrl, $FfmpegZip)
    } catch {
        Err "Failed to download ffmpeg zip: $_"
    }

    Info "Extracting archive …"
    if (Test-Path $FfmpegExtract) { Remove-Item $FfmpegExtract -Recurse -Force }
    Expand-Archive -Path $FfmpegZip -DestinationPath $FfmpegExtract -Force
    Remove-Item $FfmpegZip -Force

    $FfmpegExe = Get-ChildItem -Path $FfmpegExtract -Filter "ffmpeg.exe" -Recurse |
                 Select-Object -First 1
    if ($null -eq $FfmpegExe) {
        Err "ffmpeg.exe not found inside the downloaded archive."
    }
    Copy-Item $FfmpegExe.FullName -Destination $FfmpegDst -Force
    Remove-Item $FfmpegExtract -Recurse -Force
    Ok "ffmpeg.exe saved → $FfmpegDst"
}

# ─── Python + demucs ───────────────────────────────────────────────────────────

Step "Setting up Python venv + demucs"

$Python = ""
foreach ($candidate in @("python", "python3", "py")) {
    try {
        $ver = & $candidate --version 2>&1
        if ($ver -match "Python 3\.") {
            $Python = $candidate
            break
        }
    } catch {}
}

if ($Python -eq "") {
    Warn "Python 3 not found in PATH."
    Warn "Download from https://www.python.org/downloads/ and re-run this script."
    Warn "Demucs will NOT be available — the No Music feature will be disabled."
} else {
    Info "Python found: $Python  ($( & $Python --version 2>&1 ))"

    if (-not (Test-Path (Join-Path $VenvDir "Scripts\activate.ps1"))) {
        Info "Creating virtual environment at $VenvDir …"
        & $Python -m venv $VenvDir
    } else {
        Ok "Virtual environment already exists."
    }

    $Pip     = Join-Path $VenvDir "Scripts\pip.exe"
    $DemucsExe = Join-Path $VenvDir "Scripts\demucs.exe"

    if (-not (Test-Path $DemucsExe)) {
        Info "Installing demucs (downloads PyTorch — may take 5-15 min) …"
        & $Pip install --upgrade pip --quiet
        & $Pip install demucs --no-cache-dir
        Ok "demucs installed."
    } else {
        Ok "demucs already installed in venv."
    }

    Info "Injecting torchcodec (critical audio-save fix) …"
    try {
        & $Pip install torchcodec --no-cache-dir --quiet
        Ok "torchcodec injected."
    } catch {
        Warn "torchcodec injection failed. App will still work but may hit audio-save errors."
        Warn "Manual fix: $Pip install torchcodec"
    }

    if (Test-Path $DemucsExe) {
        # Copy demucs.exe stub into binDir so the Qt app finds it
        $DemucsLink = Join-Path $BinDir "demucs.exe"
        if (-not (Test-Path $DemucsLink)) {
            Copy-Item $DemucsExe -Destination $DemucsLink -Force
        }
        Ok "demucs.exe linked → $DemucsLink"
    }
}

# Clean up temp
if (Test-Path $TempDir) { Remove-Item $TempDir -Recurse -Force }

# ─── Build instructions ────────────────────────────────────────────────────────

Step "C++ / Qt6 Build Instructions"

Write-Host @"

  Dependencies are ready. To compile the Qt6 application:

  OPTION A — MSYS2 (Recommended for Windows):
  ─────────────────────────────────────────────
  1. Install MSYS2 from https://www.msys2.org/
  2. Open "MSYS2 MinGW 64-bit" terminal and run:

       pacman -S mingw-w64-x86_64-qt6-base ^
                 mingw-w64-x86_64-cmake ^
                 mingw-w64-x86_64-gcc ^
                 make

  3. Navigate to the source directory:

       cd /c/path/to/M4S_D
       mkdir build && cd build
       cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
       mingw32-make -j$(nproc)

  OPTION B — Qt6 Online Installer:
  ──────────────────────────────────
  1. Install Qt6 from https://www.qt.io/download-open-source
     (Select: Qt 6.x → Desktop → MinGW 64-bit)
  2. Open Qt Creator, open CMakeLists.txt, build with Release.

  The compiled m4s_d.exe will auto-detect the tools from:
    $BinDir

"@ -ForegroundColor White

# ─── Summary ───────────────────────────────────────────────────────────────────

Write-Host @"
  ╔══════════════════════════════════════════════════════════╗
  ║       m4s d — Windows Setup Complete!                    ║
  ╠══════════════════════════════════════════════════════════╣
  ║                                                          ║
  ║  yt-dlp.exe   →  $BinDir\yt-dlp.exe
  ║  ffmpeg.exe   →  $BinDir\ffmpeg.exe
  ║  demucs       →  $VenvDir\Scripts\demucs.exe
  ║                                                          ║
  ║  No Music pipeline:                                      ║
  ║    download → demucs → ffmpeg → "Title (no music).m4a"   ║
  ║    original file deleted automatically                   ║
  ║                                                          ║
  ╚══════════════════════════════════════════════════════════╝
"@ -ForegroundColor Green
