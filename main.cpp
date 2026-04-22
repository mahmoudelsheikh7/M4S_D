#include <QApplication>
#include <QMainWindow>
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QProcess>
#include <QProcessEnvironment>
#include <QClipboard>
#include <QGroupBox>
#include <QScrollBar>
#include <QFont>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QStandardPaths>
#include <QSettings>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QFrame>
#include <QSizePolicy>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QProgressBar>
#include <QStatusBar>
#include <QSplitter>
#include <QColor>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QGuiApplication>
#include <QScreen>
#include <functional>

static QString appDataDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

static QString binDir()
{
    return appDataDir() + "/bin";
}

static QString exeSuffix()
{
#ifdef Q_OS_WIN
    return QStringLiteral(".exe");
#else
    return QString();
#endif
}

static QString findInPath(const QString &name)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString pathVar = env.value(QStringLiteral("PATH"));
#ifdef Q_OS_WIN
    const QChar sep = ';';
#else
    const QChar sep = ':';
#endif
    for (const QString &dir : pathVar.split(sep, Qt::SkipEmptyParts)) {
        QString candidate = dir + "/" + name + exeSuffix();
        if (QFile::exists(candidate))
            return candidate;
    }
    return {};
}

static QString findTool(const QString &name)
{
    QString local = binDir() + "/" + name + exeSuffix();
    if (QFile::exists(local))
        return local;

    if (name == QLatin1String("demucs")) {
#ifndef Q_OS_WIN
        QString pipxPath = QDir::homePath() + "/.local/bin/demucs";
        if (QFile::exists(pipxPath))
            return pipxPath;
#else
        QString venvPath = appDataDir() + "/venv/Scripts/demucs.exe";
        if (QFile::exists(venvPath))
            return venvPath;
#endif
    }

    return findInPath(name);
}

static void makeDarkPalette(QApplication &app)
{
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette p;
    p.setColor(QPalette::Window,          QColor(28,  28,  30 ));
    p.setColor(QPalette::WindowText,      QColor(235, 235, 245));
    p.setColor(QPalette::Base,            QColor(18,  18,  20 ));
    p.setColor(QPalette::AlternateBase,   QColor(38,  38,  42 ));
    p.setColor(QPalette::ToolTipBase,     QColor(50,  50,  55 ));
    p.setColor(QPalette::ToolTipText,     QColor(235, 235, 245));
    p.setColor(QPalette::Text,            QColor(235, 235, 245));
    p.setColor(QPalette::Button,          QColor(44,  44,  48 ));
    p.setColor(QPalette::ButtonText,      QColor(235, 235, 245));
    p.setColor(QPalette::BrightText,      Qt::red);
    p.setColor(QPalette::Link,            QColor(10,  132, 255));
    p.setColor(QPalette::Highlight,       QColor(10,  132, 255));
    p.setColor(QPalette::HighlightedText, Qt::black);
    p.setColor(QPalette::Disabled, QPalette::Text,       QColor(100, 100, 110));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(100, 100, 110));
    app.setPalette(p);
}

// ─────────────────────────────────────────────────────────────────────────────
// SetupWorker — downloads yt-dlp and (Windows) ffmpeg into binDir() once
// ─────────────────────────────────────────────────────────────────────────────

class SetupWorker : public QObject
{
    Q_OBJECT
public:
    explicit SetupWorker(QObject *parent = nullptr);
    void run();

signals:
    void message(const QString &text, const QString &color);
    void finished(bool success);

private slots:
    void onYtdlpReply(QNetworkReply *reply);
    void onFfmpegZipReply(QNetworkReply *reply);
    void onExtractFinished(int code, QProcess::ExitStatus st);

private:
    void downloadYtdlp();
    void downloadFfmpegWin();
    void extractFfmpegZip(const QString &zipPath);
    void complete(bool ok);

    QNetworkAccessManager *m_nam = nullptr;
    bool m_needFfmpeg = false;
    QString m_zipTemp;
};

SetupWorker::SetupWorker(QObject *parent) : QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
}

void SetupWorker::run()
{
    QDir().mkpath(binDir());

    m_needFfmpeg = findTool("ffmpeg").isEmpty();
    bool needYtdlp = findTool("yt-dlp").isEmpty();

    if (!needYtdlp && !m_needFfmpeg) {
        emit message("  [OK] All tools already present — no download needed.", "#66bb6a");
        emit finished(true);
        return;
    }

    if (needYtdlp)
        downloadYtdlp();
    else if (m_needFfmpeg)
        downloadFfmpegWin();
}

void SetupWorker::downloadYtdlp()
{
    emit message("  [↓] Downloading yt-dlp…", "#80cbc4");
#ifdef Q_OS_WIN
    QUrl url("https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe");
#else
    QUrl url("https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp");
#endif
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply] { onYtdlpReply(reply); });
    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 r, qint64 t) {
        if (t > 0)
            emit message(QString("  [↓] yt-dlp  %1/%2 KB")
                .arg(r/1024).arg(t/1024), "#80cbc4");
    });
}

void SetupWorker::onYtdlpReply(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        emit message("  [ERR] yt-dlp download failed: " + reply->errorString(), "#ef5350");
        emit finished(false);
        return;
    }

    QString dest = binDir() + "/yt-dlp" + exeSuffix();
    QFile f(dest);
    if (!f.open(QIODevice::WriteOnly)) {
        emit message("  [ERR] Cannot write: " + dest, "#ef5350");
        emit finished(false);
        return;
    }
    f.write(reply->readAll());
    f.close();
#ifndef Q_OS_WIN
    f.setPermissions(f.permissions() | QFileDevice::ExeOwner | QFileDevice::ExeGroup | QFileDevice::ExeOther);
#endif
    emit message("  [OK] yt-dlp saved → " + dest, "#66bb6a");

    if (m_needFfmpeg)
        downloadFfmpegWin();
    else
        complete(true);
}

void SetupWorker::downloadFfmpegWin()
{
#ifndef Q_OS_WIN
    emit message("  [WW] ffmpeg not found. Install it via your package manager.", "#ffa726");
    complete(true);
    return;
#else
    emit message("  [↓] Downloading ffmpeg for Windows…", "#80cbc4");
    QUrl url("https://github.com/yt-dlp/FFmpeg-Builds/releases/download/latest/"
             "ffmpeg-master-latest-win64-gpl.zip");
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply] { onFfmpegZipReply(reply); });
    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 r, qint64 t) {
        if (t > 0)
            emit message(QString("  [↓] ffmpeg.zip  %1/%2 MB")
                .arg(r/1048576).arg(t/1048576), "#80cbc4");
    });
#endif
}

void SetupWorker::onFfmpegZipReply(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        emit message("  [ERR] ffmpeg download failed: " + reply->errorString(), "#ef5350");
        complete(false);
        return;
    }

    m_zipTemp = QDir::tempPath() + "/m4s_d_ffmpeg.zip";
    QFile f(m_zipTemp);
    if (!f.open(QIODevice::WriteOnly)) {
        emit message("  [ERR] Cannot write temp zip.", "#ef5350");
        complete(false);
        return;
    }
    f.write(reply->readAll());
    f.close();
    emit message("  [→] Extracting ffmpeg…", "#80cbc4");
    extractFfmpegZip(m_zipTemp);
}

void SetupWorker::extractFfmpegZip(const QString &zipPath)
{
    QString extractDir = binDir() + "/_ffx";
    QDir().mkpath(extractDir);

    auto *proc = new QProcess(this);
    QStringList args;
#ifdef Q_OS_WIN
    args << "-NoProfile" << "-NonInteractive" << "-Command"
         << QString("Expand-Archive -Force -Path '%1' -DestinationPath '%2'")
                .arg(zipPath, extractDir);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, proc, extractDir, zipPath](int code, QProcess::ExitStatus st) {
        proc->deleteLater();
        QFile::remove(zipPath);
        onExtractFinished(code, st);
        Q_UNUSED(extractDir);
    });
    proc->start("powershell.exe", args);
#else
    Q_UNUSED(proc);
    Q_UNUSED(extractDir);
    complete(true);
#endif
}

void SetupWorker::onExtractFinished(int code, QProcess::ExitStatus)
{
    QString extractDir = binDir() + "/_ffx";
    if (code != 0) {
        emit message("  [ERR] Extraction failed.", "#ef5350");
        QDir(extractDir).removeRecursively();
        complete(false);
        return;
    }

    QDirIterator it(extractDir, {"ffmpeg.exe"}, QDir::Files, QDirIterator::Subdirectories);
    if (it.hasNext()) {
        QString src = it.next();
        QString dst = binDir() + "/ffmpeg.exe";
        QFile::remove(dst);
        QFile::copy(src, dst);
        QDir(extractDir).removeRecursively();
        emit message("  [OK] ffmpeg.exe saved → " + dst, "#66bb6a");
        complete(true);
    } else {
        emit message("  [ERR] ffmpeg.exe not found inside archive.", "#ef5350");
        QDir(extractDir).removeRecursively();
        complete(false);
    }
}

void SetupWorker::complete(bool ok)
{
    emit finished(ok);
}

// ─────────────────────────────────────────────────────────────────────────────
// SettingsDialog
// ─────────────────────────────────────────────────────────────────────────────

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    QString downloadDir() const;
    QString device() const;

private:
    QLineEdit  *m_dirEdit   = nullptr;
    QComboBox  *m_devCombo  = nullptr;
};

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Settings — m4s d");
    setMinimumWidth(520);

    QSettings s;
    QString defDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + "/m4s_d";

    auto *root = new QVBoxLayout(this);
    root->setSpacing(14);

    auto *dirGroup = new QGroupBox("Download Directory");
    auto *dirRow   = new QHBoxLayout(dirGroup);
    m_dirEdit = new QLineEdit(s.value("downloadDir", defDir).toString());
    m_dirEdit->setMinimumWidth(300);
    auto *browseBtn = new QPushButton("Browse…");
    browseBtn->setFixedWidth(90);
    dirRow->addWidget(m_dirEdit, 1);
    dirRow->addWidget(browseBtn);

    auto *devGroup = new QGroupBox("Demucs Processing Device");
    auto *devRow   = new QHBoxLayout(devGroup);
    m_devCombo = new QComboBox;
    m_devCombo->addItem("cpu  —  Always safe (recommended)", "cpu");
    m_devCombo->addItem("cuda —  NVIDIA GPU (faster)",       "cuda");
    int devIdx = (s.value("device", "cpu").toString() == "cuda") ? 1 : 0;
    m_devCombo->setCurrentIndex(devIdx);
    devRow->addWidget(m_devCombo, 1);

    auto *noteLabel = new QLabel(
        "<small style='color:#aaa;'>The app auto-retries on CPU if CUDA fails.</small>");
    noteLabel->setTextFormat(Qt::RichText);

    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    root->addWidget(dirGroup);
    root->addWidget(devGroup);
    root->addWidget(noteLabel);
    root->addStretch();
    root->addWidget(btns);

    connect(browseBtn, &QPushButton::clicked, this, [this] {
        QString d = QFileDialog::getExistingDirectory(this, "Select Download Directory",
                                                      m_dirEdit->text());
        if (!d.isEmpty())
            m_dirEdit->setText(d);
    });

    connect(btns, &QDialogButtonBox::accepted, this, [this] {
        QSettings s;
        s.setValue("downloadDir", m_dirEdit->text());
        s.setValue("device", m_devCombo->currentData().toString());
        accept();
    });
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString SettingsDialog::downloadDir() const { return m_dirEdit->text(); }
QString SettingsDialog::device() const      { return m_devCombo->currentData().toString(); }

// ─────────────────────────────────────────────────────────────────────────────
// MainWindow
// ─────────────────────────────────────────────────────────────────────────────

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onStart();
    void onStop();
    void onPaste();
    void onOpenSettings();
    void onSetupDeps();

    void onYtdlpData();
    void onDownloadFinished(int code, QProcess::ExitStatus st);

    void onDemucsData();
    void onDemucsFinished(int code, QProcess::ExitStatus st);

    void onConvertData();
    void onConvertFinished(int code, QProcess::ExitStatus st);

private:
    void buildUi();
    void checkDeps();
    void appendLog(const QString &html, const QString &color = "#cccccc");
    void setRunning(bool running);
    QString outDir() const;
    QString currentDevice() const;
    void startDownload();
    void startDemucs(const QString &audioFile);
    void startConvert(const QString &vocalsWav, const QString &outM4a);
    void finishPipeline();

    QLineEdit   *m_urlEdit       = nullptr;
    QPushButton *m_pasteBtn      = nullptr;
    QPushButton *m_startBtn      = nullptr;
    QPushButton *m_stopBtn       = nullptr;
    QPushButton *m_settingsBtn   = nullptr;
    QPushButton *m_setupBtn      = nullptr;
    QComboBox   *m_formatCombo   = nullptr;
    QCheckBox   *m_noMusicChk    = nullptr;
    QTextEdit   *m_log           = nullptr;
    QProgressBar *m_progress     = nullptr;

    QProcess *m_proc = nullptr;

    QString m_ytdlpPath;
    QString m_ffmpegPath;
    QString m_demucsPath;

    QString m_downloadedFile;
    QString m_pendingDelete;
    QString m_pendingConvertOut;
    bool    m_cudaFailed = false;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QSettings s;
    setWindowTitle("m4s d  —  Universal Downloader & Vocal Extractor");
    setMinimumSize(760, 580);

    QRect scr = QGuiApplication::primaryScreen()->availableGeometry();
    resize(860, 640);
    move((scr.width() - width()) / 2, (scr.height() - height()) / 2);

    buildUi();
    checkDeps();
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *root = new QVBoxLayout(central);
    root->setSpacing(10);
    root->setContentsMargins(14, 14, 14, 10);

    auto *topRow = new QHBoxLayout;
    auto *titleLabel = new QLabel(
        "<span style='font-size:18px;font-weight:700;color:#80cbc4;'>m4s d</span>"
        "<span style='font-size:12px;color:#777;'>  v1.0  —  Universal Downloader + Vocal AI</span>");
    titleLabel->setTextFormat(Qt::RichText);
    m_settingsBtn = new QPushButton("⚙  Settings");
    m_setupBtn    = new QPushButton("⬇  Setup Tools");
    m_settingsBtn->setFixedWidth(110);
    m_setupBtn->setFixedWidth(120);
    topRow->addWidget(titleLabel, 1);
    topRow->addWidget(m_setupBtn);
    topRow->addWidget(m_settingsBtn);
    root->addLayout(topRow);

    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color:#333;");
    root->addWidget(sep);

    auto *inputGroup = new QGroupBox("Download");
    auto *inputGrid  = new QGridLayout(inputGroup);
    inputGrid->setSpacing(8);

    auto *urlLbl = new QLabel("URL:");
    urlLbl->setFixedWidth(42);
    m_urlEdit = new QLineEdit;
    m_urlEdit->setPlaceholderText("https://youtube.com/watch?v=…  (any yt-dlp-supported site)");
    m_pasteBtn = new QPushButton("📋 Paste");
    m_pasteBtn->setFixedWidth(80);

    auto *fmtLbl = new QLabel("Format:");
    fmtLbl->setFixedWidth(52);
    m_formatCombo = new QComboBox;
    m_formatCombo->addItem("🎵  Audio  (.m4a — default)", "audio");
    m_formatCombo->addItem("🎬  Video  (.mp4 — default)", "video");
    m_formatCombo->setFixedWidth(230);

    m_noMusicChk = new QCheckBox(
        "✨  No Music  —  AI vocal extraction (Demucs htdemucs_ft)  →  saves \"Title (no music).m4a\", deletes original");
    m_noMusicChk->setStyleSheet("color:#ce93d8;");

    inputGrid->addWidget(urlLbl,        0, 0);
    inputGrid->addWidget(m_urlEdit,     0, 1, 1, 3);
    inputGrid->addWidget(m_pasteBtn,    0, 4);
    inputGrid->addWidget(fmtLbl,        1, 0);
    inputGrid->addWidget(m_formatCombo, 1, 1);
    inputGrid->addWidget(m_noMusicChk,  2, 0, 1, 5);
    root->addWidget(inputGroup);

    auto *btnRow = new QHBoxLayout;
    m_startBtn = new QPushButton("▶   Start");
    m_stopBtn  = new QPushButton("■   Stop");
    auto *clearBtn = new QPushButton("Clear Log");
    m_startBtn->setFixedHeight(36);
    m_stopBtn->setFixedHeight(36);
    m_startBtn->setFixedWidth(130);
    m_stopBtn->setFixedWidth(100);
    clearBtn->setFixedWidth(90);
    m_startBtn->setStyleSheet(
        "QPushButton{background:#1a6b4a;color:#fff;font-weight:700;border-radius:4px;}"
        "QPushButton:hover{background:#25905f;}"
        "QPushButton:disabled{background:#333;color:#666;}");
    m_stopBtn->setStyleSheet(
        "QPushButton{background:#6b1a1a;color:#fff;font-weight:700;border-radius:4px;}"
        "QPushButton:hover{background:#902525;}"
        "QPushButton:disabled{background:#333;color:#666;}");
    m_stopBtn->setEnabled(false);
    btnRow->addWidget(m_startBtn);
    btnRow->addWidget(m_stopBtn);
    btnRow->addStretch();
    btnRow->addWidget(clearBtn);
    root->addLayout(btnRow);

    m_progress = new QProgressBar;
    m_progress->setRange(0, 0);
    m_progress->setVisible(false);
    m_progress->setFixedHeight(4);
    m_progress->setStyleSheet(
        "QProgressBar{border:none;background:#222;}"
        "QProgressBar::chunk{background:#80cbc4;}");
    root->addWidget(m_progress);

    auto *logGroup = new QGroupBox("Process Log");
    auto *logLayout = new QVBoxLayout(logGroup);
    logLayout->setContentsMargins(4, 4, 4, 4);
    m_log = new QTextEdit;
    m_log->setReadOnly(true);
    m_log->setFont(QFont("Monospace", 9));
    m_log->document()->setMaximumBlockCount(2000);
    m_log->setStyleSheet(
        "QTextEdit{background:#0e0e10;color:#ccc;border:1px solid #2a2a30;"
        "border-radius:4px;padding:4px;}");
    logLayout->addWidget(m_log);
    root->addWidget(logGroup, 1);

    statusBar()->showMessage("Ready");

    connect(m_pasteBtn,    &QPushButton::clicked, this, &MainWindow::onPaste);
    connect(m_startBtn,    &QPushButton::clicked, this, &MainWindow::onStart);
    connect(m_stopBtn,     &QPushButton::clicked, this, &MainWindow::onStop);
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::onOpenSettings);
    connect(m_setupBtn,    &QPushButton::clicked, this, &MainWindow::onSetupDeps);
    connect(clearBtn,      &QPushButton::clicked, m_log, &QTextEdit::clear);

    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx) {
        bool isAudio = (idx == 0);
        m_noMusicChk->setEnabled(isAudio && !m_demucsPath.isEmpty());
        if (!isAudio)
            m_noMusicChk->setChecked(false);
    });
}

void MainWindow::checkDeps()
{
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  m4s d — Self-Diagnostic", "#80cbc4");
    appendLog("  Data dir: " + appDataDir(), "#555");

    m_ytdlpPath  = findTool("yt-dlp");
    m_ffmpegPath = findTool("ffmpeg");
    m_demucsPath = findTool("demucs");

    auto check = [&](const QString &name, const QString &path) {
        if (!path.isEmpty())
            appendLog("  [OK]  " + name + "  →  " + path, "#66bb6a");
        else
            appendLog("  [!!]  " + name + "  NOT FOUND — click \"Setup Tools\" to download.", "#ef5350");
    };

    check("yt-dlp",  m_ytdlpPath);
    check("ffmpeg",  m_ffmpegPath);
    check("demucs",  m_demucsPath);

    bool demucsAvail = !m_demucsPath.isEmpty();
    m_noMusicChk->setEnabled(demucsAvail && m_formatCombo->currentIndex() == 0);

    if (!demucsAvail)
        appendLog("  [WW]  demucs not found — \"No Music\" feature disabled. "
                  "Run install_linux.sh or install_windows.ps1 to install.", "#ffa726");

    bool canDownload = !m_ytdlpPath.isEmpty();
    m_startBtn->setEnabled(canDownload);

    appendLog("  Ready — paste a URL and click Start.", "#80cbc4");
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
}

void MainWindow::appendLog(const QString &text, const QString &color)
{
    QString escaped = text.toHtmlEscaped();
    m_log->append(QString("<span style='color:%1;font-family:monospace;'>%2</span>")
                      .arg(color, escaped));
    m_log->verticalScrollBar()->setValue(m_log->verticalScrollBar()->maximum());
}

void MainWindow::setRunning(bool running)
{
    m_startBtn->setEnabled(!running && !m_ytdlpPath.isEmpty());
    m_stopBtn->setEnabled(running);
    m_urlEdit->setEnabled(!running);
    m_formatCombo->setEnabled(!running);
    m_noMusicChk->setEnabled(!running && !m_demucsPath.isEmpty()
                              && m_formatCombo->currentIndex() == 0);
    m_progress->setVisible(running);
    statusBar()->showMessage(running ? "Processing…" : "Ready");
}

QString MainWindow::outDir() const
{
    QSettings s;
    QString def = QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + "/m4s_d";
    return s.value("downloadDir", def).toString();
}

QString MainWindow::currentDevice() const
{
    QSettings s;
    return s.value("device", "cpu").toString();
}

void MainWindow::onPaste()
{
    QString text = QGuiApplication::clipboard()->text().trimmed();
    if (!text.isEmpty())
        m_urlEdit->setText(text);
}

void MainWindow::onOpenSettings()
{
    SettingsDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
        appendLog("  [→] Settings saved. Output dir: " + dlg.downloadDir(), "#80cbc4");
}

void MainWindow::onSetupDeps()
{
    m_setupBtn->setEnabled(false);
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  [Setup] Checking and downloading missing tools…", "#80cbc4");
    m_progress->setVisible(true);

    auto *worker = new SetupWorker(this);
    connect(worker, &SetupWorker::message,  this,
            [this](const QString &t, const QString &c) { appendLog(t, c); });
    connect(worker, &SetupWorker::finished, this, [this, worker](bool ok) {
        worker->deleteLater();
        m_progress->setVisible(false);
        m_setupBtn->setEnabled(true);
        if (ok) {
            appendLog("  [OK] Setup complete — re-running diagnostics…", "#66bb6a");
            checkDeps();
        } else {
            appendLog("  [ERR] Setup encountered errors. See above.", "#ef5350");
        }
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    });
    worker->run();
}

void MainWindow::onStop()
{
    if (m_proc && m_proc->state() != QProcess::NotRunning) {
        m_proc->kill();
        appendLog("  [--] Stopped by user.", "#ffa726");
    }
    setRunning(false);
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
}

void MainWindow::onStart()
{
    QString url = m_urlEdit->text().trimmed();
    if (url.isEmpty()) {
        QMessageBox::warning(this, "m4s d", "Please enter a URL.");
        return;
    }
    if (m_ytdlpPath.isEmpty()) {
        QMessageBox::critical(this, "m4s d", "yt-dlp not found. Click \"Setup Tools\" first.");
        return;
    }

    m_downloadedFile.clear();
    m_pendingDelete.clear();
    m_pendingConvertOut.clear();
    m_cudaFailed = false;

    QDir().mkpath(outDir());
    setRunning(true);
    startDownload();
}

void MainWindow::startDownload()
{
    QString url = m_urlEdit->text().trimmed();
    bool isAudio = (m_formatCombo->currentIndex() == 0);

    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog(QString("  Step 1 — yt-dlp  [%1]").arg(isAudio ? "audio → .m4a" : "video → .mp4"), "#80cbc4");
    appendLog("  URL: " + url, "#777");

    QStringList args;
    args << "--no-playlist" << "--no-warnings";

    if (isAudio) {
        args << "-f"  << "bestaudio[ext=m4a]/bestaudio/best"
             << "-x"
             << "--audio-format" << "m4a"
             << "--audio-quality" << "0";
    } else {
        args << "-f"  << "bestvideo[ext=mp4]+bestaudio[ext=m4a]/bestvideo+bestaudio/best"
             << "--merge-output-format" << "mp4";
    }

    args << "--newline"
         << "-o" << (outDir() + "/%(title)s.%(ext)s")
         << url;

    appendLog("  > yt-dlp " + args.join(" "), "#444");

    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead,
            this, &MainWindow::onYtdlpData);
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onDownloadFinished);
    m_proc->start(m_ytdlpPath, args);
}

void MainWindow::onYtdlpData()
{
    QString out = QString::fromLocal8Bit(m_proc->readAll());
    for (const QString &raw : out.split('\n', Qt::SkipEmptyParts)) {
        QString line = raw.trimmed();
        if (line.isEmpty()) continue;

        for (const QString &pfx : {
                 QStringLiteral("[download] Destination: "),
                 QStringLiteral("[ExtractAudio] Destination: ") }) {
            if (line.startsWith(pfx)) {
                m_downloadedFile = line.mid(pfx.length()).trimmed();
                break;
            }
        }
        if (line.startsWith("[Merger] Merging formats into \"")) {
            m_downloadedFile = line.mid(31);
            if (m_downloadedFile.endsWith('"'))
                m_downloadedFile.chop(1);
        }

        appendLog("  " + line, "#80cbc4");
    }
}

void MainWindow::onDownloadFinished(int code, QProcess::ExitStatus st)
{
    m_proc->deleteLater();
    m_proc = nullptr;

    if (code != 0 || st == QProcess::CrashExit) {
        appendLog("  [ERR] Download failed (exit " + QString::number(code) + ")", "#ef5350");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    appendLog("  [OK] Download complete.", "#66bb6a");

    if (m_downloadedFile.isEmpty()) {
        QDirIterator it(outDir(), QDir::Files, QDirIterator::NoIteratorFlags);
        QDateTime newest;
        while (it.hasNext()) {
            it.next();
            QFileInfo fi = it.fileInfo();
            if (fi.lastModified() > newest) {
                newest = fi.lastModified();
                m_downloadedFile = fi.absoluteFilePath();
            }
        }
    }

    appendLog("  [→] File: " + m_downloadedFile, "#aaa");

    bool noMusic = m_noMusicChk->isChecked() && !m_demucsPath.isEmpty()
                   && !m_downloadedFile.isEmpty();

    if (noMusic)
        startDemucs(m_downloadedFile);
    else
        finishPipeline();
}

void MainWindow::startDemucs(const QString &audioFile)
{
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  Step 2 — Demucs  (htdemucs_ft · shifts=2 · two-stems=vocals)", "#ce93d8");
    appendLog("  Input : " + audioFile, "#777");

    QString device = currentDevice();
    if (m_cudaFailed)
        device = "cpu";
    appendLog("  Device: " + device.toUpper(), "#777");

    QStringList args;
    args << "-n"              << "htdemucs_ft"
         << "--two-stems=vocals"
         << "--shifts=2"
         << "-d"              << device
         << "-o"              << outDir()
         << audioFile;

    appendLog("  > demucs " + args.join(" "), "#444");

    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead,
            this, &MainWindow::onDemucsData);
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onDemucsFinished);
    m_proc->start(m_demucsPath, args);
}

void MainWindow::onDemucsData()
{
    QString out = QString::fromLocal8Bit(m_proc->readAll());
    for (const QString &ln : out.split('\n', Qt::SkipEmptyParts)) {
        QString t = ln.trimmed();
        if (!t.isEmpty())
            appendLog("  " + t, "#ce93d8");
    }
}

void MainWindow::onDemucsFinished(int code, QProcess::ExitStatus st)
{
    QString inputFile = m_proc->arguments().last();
    m_proc->deleteLater();
    m_proc = nullptr;

    if (code != 0 || st == QProcess::CrashExit) {
        if (!m_cudaFailed && currentDevice() == "cuda") {
            m_cudaFailed = true;
            appendLog("  [WW] CUDA error — auto-retrying with CPU…", "#ffa726");
            startDemucs(inputFile);
            return;
        }
        appendLog("  [ERR] Demucs failed. Check installation.", "#ef5350");
        appendLog("        Tip: pipx inject demucs torchcodec", "#ff8a65");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    appendLog("  [OK] Vocal separation complete.", "#66bb6a");

    QFileInfo fi(inputFile);
    QString base      = fi.completeBaseName();
    QString vocalsWav = outDir() + "/htdemucs_ft/" + base + "/vocals.wav";
    QString noMusicM4a = outDir() + "/" + base + " (no music).m4a";

    if (!QFile::exists(vocalsWav)) {
        appendLog("  [ERR] Expected vocals.wav not found: " + vocalsWav, "#ef5350");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    m_pendingDelete     = m_downloadedFile;
    m_pendingConvertOut = noMusicM4a;

    startConvert(vocalsWav, noMusicM4a);
}

void MainWindow::startConvert(const QString &vocalsWav, const QString &outM4a)
{
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  Step 3 — ffmpeg  (vocals.wav → .m4a)", "#80cbc4");
    appendLog("  Output: " + QFileInfo(outM4a).fileName(), "#aaa");

    if (m_ffmpegPath.isEmpty()) {
        appendLog("  [ERR] ffmpeg not found — cannot convert. Install ffmpeg.", "#ef5350");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    QStringList args;
    args << "-y" << "-i" << vocalsWav
         << "-c:a" << "aac" << "-b:a" << "256k"
         << outM4a;

    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead,
            this, &MainWindow::onConvertData);
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onConvertFinished);
    m_proc->start(m_ffmpegPath, args);
}

void MainWindow::onConvertData()
{
    QString out = QString::fromLocal8Bit(m_proc->readAll());
    for (const QString &ln : out.split('\n', Qt::SkipEmptyParts)) {
        QString t = ln.trimmed();
        if (!t.isEmpty() && t.startsWith("size="))
            appendLog("  " + t, "#80cbc4");
    }
}

void MainWindow::onConvertFinished(int code, QProcess::ExitStatus st)
{
    m_proc->deleteLater();
    m_proc = nullptr;

    if (code != 0 || st == QProcess::CrashExit) {
        appendLog("  [ERR] Conversion failed (exit " + QString::number(code) + ")", "#ef5350");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    appendLog("  [OK] Saved: " + m_pendingConvertOut, "#66bb6a");

    if (!m_pendingDelete.isEmpty() && QFile::exists(m_pendingDelete)) {
        if (QFile::remove(m_pendingDelete))
            appendLog("  [→] Original deleted (space saved): " + m_pendingDelete, "#aaa");
    }

    QFileInfo fi(m_pendingDelete);
    QString demucsFolder = outDir() + "/htdemucs_ft/" + fi.completeBaseName();
    if (QDir(demucsFolder).exists()) {
        QDir(demucsFolder).removeRecursively();
        appendLog("  [→] Demucs temp folder removed.", "#aaa");
    }

    finishPipeline();
}

void MainWindow::finishPipeline()
{
    appendLog("  ✓  Done.", "#66bb6a");
    setRunning(false);
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    statusBar()->showMessage("Done  —  " + QFileInfo(
        m_pendingConvertOut.isEmpty() ? m_downloadedFile : m_pendingConvertOut).fileName());
}

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("m4s_d");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("m4s");
    app.setOrganizationDomain("github.com/mahmoudelsheikh7/M4S_D");
    makeDarkPalette(app);

    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
