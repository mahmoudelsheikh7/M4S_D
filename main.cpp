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
#include <QStandardItemModel>
#include <functional>

// ─────────────────────────────────────────────────────────────────────────────
// Filesystem helpers
// ─────────────────────────────────────────────────────────────────────────────

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

// ─────────────────────────────────────────────────────────────────────────────
// Theme palettes
// ─────────────────────────────────────────────────────────────────────────────

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

static void makeLightPalette(QApplication &app)
{
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette p;
    p.setColor(QPalette::Window,          QColor(242, 242, 247));
    p.setColor(QPalette::WindowText,      QColor(28,  28,  30 ));
    p.setColor(QPalette::Base,            QColor(255, 255, 255));
    p.setColor(QPalette::AlternateBase,   QColor(235, 235, 240));
    p.setColor(QPalette::ToolTipBase,     QColor(255, 255, 255));
    p.setColor(QPalette::ToolTipText,     QColor(28,  28,  30 ));
    p.setColor(QPalette::Text,            QColor(28,  28,  30 ));
    p.setColor(QPalette::Button,          QColor(210, 210, 215));
    p.setColor(QPalette::ButtonText,      QColor(28,  28,  30 ));
    p.setColor(QPalette::BrightText,      Qt::red);
    p.setColor(QPalette::Link,            QColor(0,   90,  200));
    p.setColor(QPalette::Highlight,       QColor(0,   90,  200));
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::Text,       QColor(150, 150, 160));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(150, 150, 160));
    app.setPalette(p);
}

// ─────────────────────────────────────────────────────────────────────────────
// Bilingual string table
// ─────────────────────────────────────────────────────────────────────────────

struct Strings {
    QString langBtnText;
    QString windowTitle;
    QString titleHtml;
    QString setupBtn;
    QString settingsBtn;
    QString inputGroupTitle;
    QString urlLabel;
    QString urlPlaceholder;
    QString pasteBtn;
    QString browseFileBtn;
    QString clearFileBtn;
    QString localFilePrefix;
    QString formatLabel;
    QStringList formatItems;
    QString noMusicLabel;
    QString extremeLabel;
    QString startBtn;
    QString stopBtn;
    QString clearLogBtn;
    QString logGroupTitle;
    QString statusReady;
    QString statusProcessing;
    QString statusDone;
    QString settingsTitle;
    QString settingsDirGroup;
    QString settingsBrowse;
    QString settingsDevGroup;
    QString settingsDevNote;
    QString settingsDevCpu;
    QString settingsDevCuda;
    QString msgNoInput;
    QString msgNoYtdlp;
    QString msgUrlEmpty;
    QString diagTitle;
    QString diagDataDir;
    QString diagReady;
};

static const Strings &getStrings(bool arabic)
{
    static const Strings en = {
        "عربي",
        "m4s d  v2.0  —  Universal Downloader & Vocal AI Extractor",
        "<span style='font-size:17px;font-weight:700;color:#80cbc4;'>m4s d</span>"
        "<span style='font-size:11px;color:#666;'>  v2.0  —  Universal Downloader + AI Vocal Extractor</span>",
        "⬇  Setup Tools",
        "⚙  Settings",
        "Download / Process",
        "URL:",
        "https://youtube.com/watch?v=…  (any yt-dlp supported site — 1,000+ platforms)",
        "📋 Paste",
        "📂 Browse File",
        "✕",
        "Local: ",
        "Format / Quality:",
        {
            "🎵  Audio — MP3  (.mp3)",
            "🎵  Audio — M4A  (.m4a — default)",
            "🎵  Audio — WAV  (.wav — lossless)",
            "🎵  Audio — FLAC  (.flac — lossless)",
            "─────────────────────────────────",
            "🎬  Video — Best Quality  (.mp4)",
            "🎬  Video — 4K / 2160p  (.mp4)",
            "🎬  Video — 1080p HD  (.mp4)",
            "🎬  Video — 720p  (.mp4)",
            "🎬  Video — 480p  (.mp4)"
        },
        "✨  No Music  —  AI vocal extraction (Demucs htdemucs_ft)  →  saves \"Title (no music).[ext]\", deletes original",
        "⚡  Extreme Vocal Isolation  —  shifts=4, overlap=0.25  (slower, maximum accuracy)",
        "▶   Start",
        "■   Stop",
        "Clear Log",
        "Process Log",
        "Ready",
        "Processing…",
        "Done",
        "Settings — m4s d",
        "Download Directory",
        "Browse…",
        "Demucs Processing Device",
        "<small style='color:#aaa;'>App auto-retries on CPU if CUDA fails — no action needed.</small>",
        "cpu  —  Always safe (recommended)",
        "cuda —  NVIDIA GPU (3–10× faster)",
        "Please enter a URL or browse a local file.",
        "yt-dlp not found. Click \"Setup Tools\" to install it.",
        "Please enter a URL.",
        "m4s d — Self-Diagnostic",
        "  Data dir: ",
        "  Ready — paste a URL or browse a local file, then click Start."
    };

    static const Strings ar = {
        "EN",
        "m4s d  v2.0  —  محمل من مواقع متعددة + استخلاص صوتي بالذكاء الاصطناعي",
        "<span style='font-size:17px;font-weight:700;color:#80cbc4;'>m4s d</span>"
        "<span style='font-size:11px;color:#666;'>  v2.0  —  محمل عالمي + استخلاص صوتي AI</span>",
        "⬇  تحميل الأدوات",
        "⚙  الإعدادات",
        "تحميل / معالجة",
        "الرابط:",
        "https://youtube.com/watch?v=…  (أي موقع يدعمه yt-dlp — أكثر من 1000 منصة)",
        "📋 لصق",
        "📂 ملف محلي",
        "✕",
        "محلي: ",
        "الصيغة / الجودة:",
        {
            "🎵  صوت — MP3",
            "🎵  صوت — M4A (افتراضي)",
            "🎵  صوت — WAV (بلا خسارة)",
            "🎵  صوت — FLAC (بلا خسارة)",
            "─────────────────────────────────",
            "🎬  فيديو — أفضل جودة (.mp4)",
            "🎬  فيديو — 4K / 2160p",
            "🎬  فيديو — 1080p HD",
            "🎬  فيديو — 720p",
            "🎬  فيديو — 480p"
        },
        "✨  بدون موسيقى — استخلاص صوتي بالذكاء الاصطناعي → يحفظ «العنوان (بدون موسيقى)» ويحذف الأصلي",
        "⚡  عزل صوتي متطرف — shifts=4، overlap=0.25 (أبطأ، دقة قصوى)",
        "▶   بدء",
        "■   إيقاف",
        "مسح السجل",
        "سجل المعالجة",
        "جاهز",
        "جاري المعالجة…",
        "تم",
        "الإعدادات — m4s d",
        "مجلد التحميل",
        "استعراض…",
        "جهاز معالجة Demucs",
        "<small style='color:#aaa;'>يعيد التطبيق المحاولة على المعالج تلقائياً إذا فشل CUDA.</small>",
        "cpu  —  آمن دائماً (موصى به)",
        "cuda —  معالج رسوميات NVIDIA (أسرع 3-10×)",
        "أدخل رابطاً أو اختر ملفاً محلياً.",
        ".yt-dlp غير موجود. اضغط «تحميل الأدوات» لتثبيته",
        "أدخل رابطاً.",
        "m4s d — التشخيص الذاتي",
        "  مجلد البيانات: ",
        "  جاهز — الصق رابطاً أو اختر ملفاً محلياً، ثم اضغط «بدء»."
    };

    return arabic ? ar : en;
}

// ─────────────────────────────────────────────────────────────────────────────
// SetupWorker  (offline-first one-time tool downloader)
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

    QNetworkAccessManager *m_nam  = nullptr;
    bool                   m_needFfmpeg = false;
    QString                m_zipTemp;
};

SetupWorker::SetupWorker(QObject *parent) : QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
}

void SetupWorker::run()
{
    QDir().mkpath(binDir());

    m_needFfmpeg  = findTool("ffmpeg").isEmpty();
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
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);
    auto *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, [this, reply] { onYtdlpReply(reply); });
    connect(reply, &QNetworkReply::downloadProgress,
            this, [this](qint64 r, qint64 t) {
        if (t > 0)
            emit message(QString("  [↓] yt-dlp  %1/%2 KB").arg(r/1024).arg(t/1024), "#80cbc4");
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
    emit message("  [WW] ffmpeg not found. Install via your system package manager.", "#ffa726");
    complete(true);
    return;
#else
    emit message("  [↓] Downloading ffmpeg for Windows…", "#80cbc4");
    QUrl url("https://github.com/yt-dlp/FFmpeg-Builds/releases/download/latest/"
             "ffmpeg-master-latest-win64-gpl.zip");
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);
    auto *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, [this, reply] { onFfmpegZipReply(reply); });
    connect(reply, &QNetworkReply::downloadProgress,
            this, [this](qint64 r, qint64 t) {
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
            this, [this, proc, zipPath](int code, QProcess::ExitStatus st) {
        proc->deleteLater();
        QFile::remove(zipPath);
        onExtractFinished(code, st);
    });
    proc->start("powershell.exe", args);
#else
    Q_UNUSED(proc)
    Q_UNUSED(extractDir)
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
    explicit SettingsDialog(bool arabic, QWidget *parent = nullptr);
    QString downloadDir() const;
    QString device() const;

private:
    QLineEdit *m_dirEdit  = nullptr;
    QComboBox *m_devCombo = nullptr;
};

SettingsDialog::SettingsDialog(bool arabic, QWidget *parent) : QDialog(parent)
{
    const Strings &S = getStrings(arabic);
    setWindowTitle(S.settingsTitle);
    setMinimumWidth(520);
    setLayoutDirection(arabic ? Qt::RightToLeft : Qt::LeftToRight);

    QSettings s;
    QString defDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + "/m4s_d";

    auto *root = new QVBoxLayout(this);
    root->setSpacing(14);

    auto *dirGroup = new QGroupBox(S.settingsDirGroup);
    auto *dirRow   = new QHBoxLayout(dirGroup);
    m_dirEdit = new QLineEdit(s.value("downloadDir", defDir).toString());
    m_dirEdit->setMinimumWidth(300);
    auto *browseBtn = new QPushButton(S.settingsBrowse);
    browseBtn->setFixedWidth(100);
    dirRow->addWidget(m_dirEdit, 1);
    dirRow->addWidget(browseBtn);

    auto *devGroup = new QGroupBox(S.settingsDevGroup);
    auto *devRow   = new QHBoxLayout(devGroup);
    m_devCombo = new QComboBox;
    m_devCombo->addItem(S.settingsDevCpu,  "cpu");
    m_devCombo->addItem(S.settingsDevCuda, "cuda");
    m_devCombo->setCurrentIndex(
        s.value("device", "cpu").toString() == "cuda" ? 1 : 0);
    devRow->addWidget(m_devCombo, 1);

    auto *noteLabel = new QLabel(S.settingsDevNote);
    noteLabel->setTextFormat(Qt::RichText);

    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    root->addWidget(dirGroup);
    root->addWidget(devGroup);
    root->addWidget(noteLabel);
    root->addStretch();
    root->addWidget(btns);

    connect(browseBtn, &QPushButton::clicked, this, [this, &S] {
        QString d = QFileDialog::getExistingDirectory(
            this, S.settingsDirGroup, m_dirEdit->text());
        if (!d.isEmpty())
            m_dirEdit->setText(d);
    });

    connect(btns, &QDialogButtonBox::accepted, this, [this] {
        QSettings s;
        s.setValue("downloadDir", m_dirEdit->text());
        s.setValue("device",     m_devCombo->currentData().toString());
        accept();
    });
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString SettingsDialog::downloadDir() const { return m_dirEdit->text(); }
QString SettingsDialog::device()      const { return m_devCombo->currentData().toString(); }

// ─────────────────────────────────────────────────────────────────────────────
// MainWindow — declaration
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
    void onBrowseLocalFile();
    void onClearLocalFile();
    void onOpenSettings();
    void onSetupDeps();
    void onToggleLanguage();
    void onToggleTheme();

    void onYtdlpData();
    void onDownloadFinished(int code, QProcess::ExitStatus st);

    void onExtractAudioData();
    void onExtractAudioFinished(int code, QProcess::ExitStatus st);

    void onDemucsData();
    void onDemucsFinished(int code, QProcess::ExitStatus st);

    void onConvertData();
    void onConvertFinished(int code, QProcess::ExitStatus st);

private:
    void buildUi();
    void populateFormatCombo();
    void checkDeps();
    void updateCheckboxState();
    void applyLanguage();
    void applyTheme();
    void appendLog(const QString &text, const QString &color = "#cccccc");
    void setRunning(bool running);
    void doCleanup();

    QString outDir() const;
    QString currentDevice() const;
    QString outputExtForAudio() const;
    bool    isAudioMode() const;

    void startDownload();
    void startLocalProcess();
    void startExtractAudio(const QString &videoFile);
    void startDirectConvert(const QString &inputFile);
    void startDemucs(const QString &audioFile);
    void startConvert(const QString &inputAudio, const QString &outputFile);
    void finishPipeline();

    // ── Toolbar widgets
    QLabel      *m_titleLabel  = nullptr;
    QPushButton *m_langBtn     = nullptr;
    QPushButton *m_themeBtn    = nullptr;
    QPushButton *m_setupBtn    = nullptr;
    QPushButton *m_settingsBtn = nullptr;

    // ── Input area
    QGroupBox   *m_inputGroup     = nullptr;
    QLabel      *m_urlLabel       = nullptr;
    QLineEdit   *m_urlEdit        = nullptr;
    QPushButton *m_pasteBtn       = nullptr;
    QPushButton *m_localFileBtn   = nullptr;
    QPushButton *m_clearFileBtn   = nullptr;
    QLabel      *m_localFileLabel = nullptr;
    QLabel      *m_fmtLabel       = nullptr;
    QComboBox   *m_formatCombo    = nullptr;
    QCheckBox   *m_noMusicChk     = nullptr;
    QCheckBox   *m_extremeChk     = nullptr;

    // ── Control area
    QPushButton  *m_startBtn  = nullptr;
    QPushButton  *m_stopBtn   = nullptr;
    QProgressBar *m_progress  = nullptr;

    // ── Log area
    QGroupBox *m_logGroup = nullptr;
    QTextEdit *m_log      = nullptr;

    // ── State
    bool    m_arabic    = false;
    bool    m_darkTheme = true;
    bool    m_cudaFailed = false;

    QString m_ytdlpPath;
    QString m_ffmpegPath;
    QString m_demucsPath;

    QString m_localFilePath;
    QString m_downloadedFile;
    QString m_extractedAudioFile;
    QString m_pendingDelete;
    QString m_pendingConvertOut;

    QProcess *m_proc = nullptr;
};

// ─────────────────────────────────────────────────────────────────────────────
// MainWindow — construction
// ─────────────────────────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QSettings s;
    m_arabic    = s.value("arabic",    false).toBool();
    m_darkTheme = s.value("darkTheme", true).toBool();

    resize(920, 680);
    QRect scr = QGuiApplication::primaryScreen()->availableGeometry();
    move((scr.width() - width()) / 2, (scr.height() - height()) / 2);
    setMinimumSize(780, 600);

    buildUi();
    applyLanguage();
    applyTheme();
    checkDeps();
}

// ─────────────────────────────────────────────────────────────────────────────
// buildUi  — creates widgets once; applyLanguage() fills text
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *root = new QVBoxLayout(central);
    root->setSpacing(10);
    root->setContentsMargins(14, 14, 14, 10);

    // ── Top bar ───────────────────────────────────────────────────────────────
    auto *topRow = new QHBoxLayout;

    m_titleLabel = new QLabel;
    m_titleLabel->setTextFormat(Qt::RichText);

    m_langBtn  = new QPushButton;
    m_themeBtn = new QPushButton;
    m_setupBtn = new QPushButton;
    m_settingsBtn = new QPushButton;

    for (auto *btn : {m_langBtn, m_themeBtn}) {
        btn->setFixedSize(54, 28);
        btn->setStyleSheet(
            "QPushButton{background:#333;color:#ccc;border-radius:4px;font-size:13px;}"
            "QPushButton:hover{background:#444;}");
    }
    m_setupBtn->setFixedWidth(128);
    m_settingsBtn->setFixedWidth(110);

    topRow->addWidget(m_titleLabel, 1);
    topRow->addWidget(m_langBtn);
    topRow->addWidget(m_themeBtn);
    topRow->addSpacing(8);
    topRow->addWidget(m_setupBtn);
    topRow->addWidget(m_settingsBtn);
    root->addLayout(topRow);

    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color:#333;");
    root->addWidget(sep);

    // ── Input group ───────────────────────────────────────────────────────────
    m_inputGroup = new QGroupBox;
    auto *inputGrid = new QGridLayout(m_inputGroup);
    inputGrid->setSpacing(8);
    inputGrid->setColumnStretch(1, 1);

    m_urlLabel = new QLabel;
    m_urlLabel->setFixedWidth(62);
    m_urlEdit  = new QLineEdit;
    m_pasteBtn = new QPushButton;
    m_pasteBtn->setFixedWidth(84);

    m_localFileBtn  = new QPushButton;
    m_clearFileBtn  = new QPushButton;
    m_localFileLabel = new QLabel;
    m_localFileLabel->setStyleSheet("color:#80cbc4;font-size:10px;");
    m_localFileLabel->setWordWrap(true);
    m_localFileLabel->setVisible(false);
    m_clearFileBtn->setFixedWidth(26);
    m_clearFileBtn->setVisible(false);
    m_clearFileBtn->setStyleSheet(
        "QPushButton{background:#4a1a1a;color:#ef5350;border-radius:3px;font-weight:700;}"
        "QPushButton:hover{background:#6b1a1a;}");
    m_localFileBtn->setFixedWidth(110);

    m_fmtLabel = new QLabel;
    m_fmtLabel->setFixedWidth(110);
    m_formatCombo = new QComboBox;
    m_formatCombo->setMinimumWidth(260);

    m_noMusicChk = new QCheckBox;
    m_noMusicChk->setStyleSheet("color:#ce93d8;");

    m_extremeChk = new QCheckBox;
    m_extremeChk->setStyleSheet("color:#ffb74d;");
    m_extremeChk->setEnabled(false);

    // Row 0: URL input
    inputGrid->addWidget(m_urlLabel,   0, 0);
    inputGrid->addWidget(m_urlEdit,    0, 1);
    inputGrid->addWidget(m_pasteBtn,   0, 2);

    // Row 1: Local file browse
    auto *localRow = new QHBoxLayout;
    localRow->addWidget(m_localFileBtn);
    localRow->addWidget(m_clearFileBtn);
    localRow->addWidget(m_localFileLabel, 1);
    inputGrid->addWidget(new QLabel, 1, 0);
    inputGrid->addLayout(localRow,     1, 1, 1, 2);

    // Row 2: Format
    inputGrid->addWidget(m_fmtLabel,      2, 0);
    inputGrid->addWidget(m_formatCombo,   2, 1, 1, 2);

    // Row 3: No Music
    inputGrid->addWidget(m_noMusicChk, 3, 0, 1, 3);

    // Row 4: Extreme
    inputGrid->addWidget(m_extremeChk, 4, 0, 1, 3);

    root->addWidget(m_inputGroup);

    // ── Button row ────────────────────────────────────────────────────────────
    auto *btnRow = new QHBoxLayout;
    m_startBtn = new QPushButton;
    m_stopBtn  = new QPushButton;
    auto *clearLogBtn = new QPushButton;

    m_startBtn->setFixedHeight(36);
    m_stopBtn->setFixedHeight(36);
    m_startBtn->setFixedWidth(140);
    m_stopBtn->setFixedWidth(110);
    clearLogBtn->setFixedWidth(96);

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
    btnRow->addWidget(clearLogBtn);
    root->addLayout(btnRow);

    m_progress = new QProgressBar;
    m_progress->setRange(0, 0);
    m_progress->setVisible(false);
    m_progress->setFixedHeight(4);
    m_progress->setStyleSheet(
        "QProgressBar{border:none;background:#222;}"
        "QProgressBar::chunk{background:#80cbc4;}");
    root->addWidget(m_progress);

    // ── Log ───────────────────────────────────────────────────────────────────
    m_logGroup = new QGroupBox;
    auto *logLayout = new QVBoxLayout(m_logGroup);
    logLayout->setContentsMargins(4, 4, 4, 4);
    m_log = new QTextEdit;
    m_log->setReadOnly(true);
    m_log->setFont(QFont("Monospace", 9));
    m_log->document()->setMaximumBlockCount(3000);
    logLayout->addWidget(m_log);
    root->addWidget(m_logGroup, 1);

    statusBar()->showMessage("Ready");

    // ── Connections ───────────────────────────────────────────────────────────
    connect(m_pasteBtn,     &QPushButton::clicked, this, &MainWindow::onPaste);
    connect(m_startBtn,     &QPushButton::clicked, this, &MainWindow::onStart);
    connect(m_stopBtn,      &QPushButton::clicked, this, &MainWindow::onStop);
    connect(m_settingsBtn,  &QPushButton::clicked, this, &MainWindow::onOpenSettings);
    connect(m_setupBtn,     &QPushButton::clicked, this, &MainWindow::onSetupDeps);
    connect(m_langBtn,      &QPushButton::clicked, this, &MainWindow::onToggleLanguage);
    connect(m_themeBtn,     &QPushButton::clicked, this, &MainWindow::onToggleTheme);
    connect(m_localFileBtn, &QPushButton::clicked, this, &MainWindow::onBrowseLocalFile);
    connect(m_clearFileBtn, &QPushButton::clicked, this, &MainWindow::onClearLocalFile);
    connect(clearLogBtn,    &QPushButton::clicked, m_log, &QTextEdit::clear);

    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) {
        if (m_formatCombo->currentData().toString() == "separator") {
            int cur = m_formatCombo->currentIndex();
            m_formatCombo->setCurrentIndex(cur > 4 ? 5 : 3);
            return;
        }
        updateCheckboxState();
    });

    connect(m_noMusicChk, &QCheckBox::stateChanged, this, [this]() {
        updateCheckboxState();
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// populateFormatCombo  — fills items in current language, restores selection
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::populateFormatCombo()
{
    static const QStringList dataKeys = {
        "audio_mp3", "audio_m4a", "audio_wav", "audio_flac",
        "separator",
        "video_best", "video_4k", "video_1080p", "video_720p", "video_480p"
    };

    QString savedKey = m_formatCombo->currentData().toString();
    if (savedKey.isEmpty()) {
        QSettings s;
        savedKey = s.value("lastFormat", "audio_m4a").toString();
    }

    m_formatCombo->blockSignals(true);
    m_formatCombo->clear();

    const Strings &S = getStrings(m_arabic);
    for (int i = 0; i < qMin(S.formatItems.size(), dataKeys.size()); ++i)
        m_formatCombo->addItem(S.formatItems[i], dataKeys[i]);

    auto *model = qobject_cast<QStandardItemModel *>(m_formatCombo->model());
    if (model) {
        QStandardItem *sepItem = model->item(4);
        if (sepItem) {
            sepItem->setEnabled(false);
            sepItem->setFlags(sepItem->flags() & ~Qt::ItemIsSelectable);
        }
    }

    m_formatCombo->blockSignals(false);

    int idx = m_formatCombo->findData(savedKey);
    m_formatCombo->setCurrentIndex(idx >= 0 ? idx : 1);
}

// ─────────────────────────────────────────────────────────────────────────────
// checkDeps  — on startup and after setup
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::checkDeps()
{
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    const Strings &S = getStrings(m_arabic);
    appendLog("  " + S.diagTitle, "#80cbc4");
    appendLog(S.diagDataDir + appDataDir(), "#555");

    m_ytdlpPath  = findTool("yt-dlp");
    m_ffmpegPath = findTool("ffmpeg");
    m_demucsPath = findTool("demucs");

    auto check = [&](const QString &name, const QString &path) {
        if (!path.isEmpty())
            appendLog("  [OK]  " + name + "  →  " + path, "#66bb6a");
        else
            appendLog("  [!!]  " + name + "  NOT FOUND — click \"Setup Tools\" to fix.", "#ef5350");
    };

    check("yt-dlp",  m_ytdlpPath);
    check("ffmpeg",  m_ffmpegPath);
    check("demucs",  m_demucsPath);

    if (m_demucsPath.isEmpty())
        appendLog("  [WW]  demucs not found — \"No Music\" disabled. Run install script.", "#ffa726");

    m_startBtn->setEnabled(!m_ytdlpPath.isEmpty());
    updateCheckboxState();

    appendLog(S.diagReady, "#80cbc4");
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
}

// ─────────────────────────────────────────────────────────────────────────────
// updateCheckboxState
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::updateCheckboxState()
{
    bool demucsAvail = !m_demucsPath.isEmpty();
    bool hasLocal    = !m_localFilePath.isEmpty();
    bool videoUrl    = m_formatCombo->currentData().toString().startsWith("video_") && !hasLocal;

    bool noMusicEnabled = demucsAvail && !videoUrl;
    m_noMusicChk->setEnabled(noMusicEnabled);
    if (!noMusicEnabled)
        m_noMusicChk->setChecked(false);

    bool extremeEnabled = noMusicEnabled && m_noMusicChk->isChecked();
    m_extremeChk->setEnabled(extremeEnabled);
    if (!extremeEnabled)
        m_extremeChk->setChecked(false);

    if (!m_startBtn->isEnabled() && !m_ytdlpPath.isEmpty() && hasLocal)
        m_startBtn->setEnabled(true);
}

// ─────────────────────────────────────────────────────────────────────────────
// applyLanguage  — sets all text from the string table
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::applyLanguage()
{
    const Strings &S = getStrings(m_arabic);

    qApp->setLayoutDirection(m_arabic ? Qt::RightToLeft : Qt::LeftToRight);
    setWindowTitle(S.windowTitle);

    m_titleLabel->setText(S.titleHtml);
    m_langBtn->setText(S.langBtnText);
    m_setupBtn->setText(S.setupBtn);
    m_settingsBtn->setText(S.settingsBtn);

    m_inputGroup->setTitle(S.inputGroupTitle);
    m_urlLabel->setText(S.urlLabel);
    m_urlEdit->setPlaceholderText(S.urlPlaceholder);
    m_pasteBtn->setText(S.pasteBtn);
    m_localFileBtn->setText(S.browseFileBtn);
    m_clearFileBtn->setText(S.clearFileBtn);
    m_fmtLabel->setText(S.formatLabel);

    m_noMusicChk->setText(S.noMusicLabel);
    m_extremeChk->setText(S.extremeLabel);

    m_startBtn->setText(S.startBtn);
    m_stopBtn->setText(S.stopBtn);

    m_logGroup->setTitle(S.logGroupTitle);

    populateFormatCombo();
    updateCheckboxState();
}

// ─────────────────────────────────────────────────────────────────────────────
// applyTheme
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::applyTheme()
{
    m_themeBtn->setText(m_darkTheme ? "☀" : "🌙");

    if (m_darkTheme) {
        makeDarkPalette(*qApp);
        m_log->setStyleSheet(
            "QTextEdit{background:#0e0e10;color:#ccc;border:1px solid #2a2a30;"
            "border-radius:4px;padding:4px;}");
        m_progress->setStyleSheet(
            "QProgressBar{border:none;background:#222;}"
            "QProgressBar::chunk{background:#80cbc4;}");
    } else {
        makeLightPalette(*qApp);
        m_log->setStyleSheet(
            "QTextEdit{background:#ffffff;color:#222;border:1px solid #ccc;"
            "border-radius:4px;padding:4px;}");
        m_progress->setStyleSheet(
            "QProgressBar{border:none;background:#ddd;}"
            "QProgressBar::chunk{background:#007aff;}");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Utility methods
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::appendLog(const QString &text, const QString &color)
{
    QString escaped = text.toHtmlEscaped();
    m_log->append(
        QString("<span style='color:%1;font-family:monospace;'>%2</span>")
            .arg(color, escaped));
    m_log->verticalScrollBar()->setValue(m_log->verticalScrollBar()->maximum());
}

void MainWindow::setRunning(bool running)
{
    bool canStart = !running && (!m_ytdlpPath.isEmpty() || !m_localFilePath.isEmpty());
    m_startBtn->setEnabled(canStart);
    m_stopBtn->setEnabled(running);
    m_urlEdit->setEnabled(!running);
    m_localFileBtn->setEnabled(!running);
    m_formatCombo->setEnabled(!running);
    m_progress->setVisible(running);
    const Strings &S = getStrings(m_arabic);
    statusBar()->showMessage(running ? S.statusProcessing : S.statusReady);
}

void MainWindow::doCleanup()
{
    if (!m_pendingDelete.isEmpty() && QFile::exists(m_pendingDelete)) {
        if (QFile::remove(m_pendingDelete))
            appendLog("  [→] Original deleted (space saved): "
                      + QFileInfo(m_pendingDelete).fileName(), "#aaa");
    }

    if (!m_extractedAudioFile.isEmpty() && QFile::exists(m_extractedAudioFile)) {
        QFile::remove(m_extractedAudioFile);
        appendLog("  [→] Temp extracted audio removed.", "#aaa");
    }

    QString base = !m_pendingDelete.isEmpty()
        ? QFileInfo(m_pendingDelete).completeBaseName()
        : (!m_extractedAudioFile.isEmpty()
           ? QFileInfo(m_extractedAudioFile).completeBaseName()
           : QString());

    if (!base.isEmpty()) {
        QString demucsFolder = outDir() + "/htdemucs_ft/" + base;
        if (QDir(demucsFolder).exists()) {
            QDir(demucsFolder).removeRecursively();
            appendLog("  [→] Demucs temp folder removed.", "#aaa");
        }
    }
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

QString MainWindow::outputExtForAudio() const
{
    QString fmt = m_formatCombo->currentData().toString();
    if (fmt == "audio_mp3")  return "mp3";
    if (fmt == "audio_wav")  return "wav";
    if (fmt == "audio_flac") return "flac";
    return "m4a";
}

bool MainWindow::isAudioMode() const
{
    return !m_formatCombo->currentData().toString().startsWith("video_");
}

// ─────────────────────────────────────────────────────────────────────────────
// Slot implementations
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::onPaste()
{
    QString text = QGuiApplication::clipboard()->text().trimmed();
    if (!text.isEmpty()) {
        m_urlEdit->setText(text);
        onClearLocalFile();
    }
}

void MainWindow::onBrowseLocalFile()
{
    QString filter =
        "Media Files (*.mp4 *.mkv *.avi *.mov *.webm *.flv *.wmv *.m4v "
        "*.mp3 *.m4a *.wav *.flac *.ogg *.aac *.opus *.wma);;"
        "All Files (*)";
    QString file = QFileDialog::getOpenFileName(this, "Browse Local Media File",
                                                outDir(), filter);
    if (file.isEmpty()) return;

    m_localFilePath = file;
    m_urlEdit->clear();

    const Strings &S = getStrings(m_arabic);
    m_localFileLabel->setText(S.localFilePrefix + QFileInfo(file).fileName());
    m_localFileLabel->setVisible(true);
    m_clearFileBtn->setVisible(true);
    m_startBtn->setEnabled(true);
    updateCheckboxState();
    appendLog("  [→] Local file: " + file, "#80cbc4");
}

void MainWindow::onClearLocalFile()
{
    m_localFilePath.clear();
    m_localFileLabel->setVisible(false);
    m_clearFileBtn->setVisible(false);
    updateCheckboxState();
    m_startBtn->setEnabled(!m_ytdlpPath.isEmpty());
}

void MainWindow::onOpenSettings()
{
    SettingsDialog dlg(m_arabic, this);
    if (dlg.exec() == QDialog::Accepted) {
        appendLog("  [→] Settings saved. Output: " + dlg.downloadDir(), "#80cbc4");
        QSettings s;
        s.sync();
    }
}

void MainWindow::onSetupDeps()
{
    m_setupBtn->setEnabled(false);
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  [Setup] Checking and downloading missing tools…", "#80cbc4");
    m_progress->setVisible(true);

    auto *worker = new SetupWorker(this);
    connect(worker, &SetupWorker::message,
            this, [this](const QString &t, const QString &c) { appendLog(t, c); });
    connect(worker, &SetupWorker::finished,
            this, [this, worker](bool ok) {
        worker->deleteLater();
        m_progress->setVisible(false);
        m_setupBtn->setEnabled(true);
        if (ok) {
            appendLog("  [OK] Setup complete — re-running diagnostics…", "#66bb6a");
            checkDeps();
        } else {
            appendLog("  [ERR] Setup had errors. See above.", "#ef5350");
        }
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    });
    worker->run();
}

void MainWindow::onToggleLanguage()
{
    m_arabic = !m_arabic;
    QSettings s;
    s.setValue("arabic", m_arabic);
    applyLanguage();
    appendLog(m_arabic
              ? "  [→] اللغة: عربي"
              : "  [→] Language: English",
              "#80cbc4");
}

void MainWindow::onToggleTheme()
{
    m_darkTheme = !m_darkTheme;
    QSettings s;
    s.setValue("darkTheme", m_darkTheme);
    applyTheme();
    appendLog(m_darkTheme
              ? "  [→] Theme: Dark"
              : "  [→] Theme: Light",
              "#80cbc4");
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
    const Strings &S = getStrings(m_arabic);
    bool hasLocal = !m_localFilePath.isEmpty();
    bool hasUrl   = !m_urlEdit->text().trimmed().isEmpty();

    if (!hasLocal && !hasUrl) {
        QMessageBox::warning(this, "m4s d", S.msgNoInput);
        return;
    }

    if (!hasLocal && m_ytdlpPath.isEmpty()) {
        QMessageBox::critical(this, "m4s d", S.msgNoYtdlp);
        return;
    }

    m_downloadedFile.clear();
    m_extractedAudioFile.clear();
    m_pendingDelete.clear();
    m_pendingConvertOut.clear();
    m_cudaFailed = false;

    QDir().mkpath(outDir());
    setRunning(true);

    QSettings s;
    s.setValue("lastFormat", m_formatCombo->currentData().toString());

    if (hasLocal)
        startLocalProcess();
    else
        startDownload();
}

// ─────────────────────────────────────────────────────────────────────────────
// Pipeline Step 1a — URL download via yt-dlp
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::startDownload()
{
    QString url = m_urlEdit->text().trimmed();
    QString fmt = m_formatCombo->currentData().toString();

    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  Step 1 — yt-dlp  [" + fmt + "]", "#80cbc4");
    appendLog("  URL: " + url, "#777");

    QStringList args;
    args << "--no-playlist" << "--no-warnings" << "--newline";

    if (fmt == "audio_mp3") {
        args << "-f" << "bestaudio"
             << "-x" << "--audio-format" << "mp3" << "--audio-quality" << "0";
    } else if (fmt == "audio_m4a") {
        args << "-f" << "bestaudio[ext=m4a]/bestaudio/best"
             << "-x" << "--audio-format" << "m4a" << "--audio-quality" << "0";
    } else if (fmt == "audio_wav") {
        args << "-f" << "bestaudio"
             << "-x" << "--audio-format" << "wav";
    } else if (fmt == "audio_flac") {
        args << "-f" << "bestaudio"
             << "-x" << "--audio-format" << "flac" << "--audio-quality" << "0";
    } else if (fmt == "video_4k") {
        args << "-f"
             << "bestvideo[height<=2160][ext=mp4]+bestaudio[ext=m4a]/"
                "bestvideo[height<=2160]+bestaudio/best"
             << "--merge-output-format" << "mp4";
    } else if (fmt == "video_1080p") {
        args << "-f"
             << "bestvideo[height<=1080][ext=mp4]+bestaudio[ext=m4a]/"
                "bestvideo[height<=1080]+bestaudio/best"
             << "--merge-output-format" << "mp4";
    } else if (fmt == "video_720p") {
        args << "-f"
             << "bestvideo[height<=720][ext=mp4]+bestaudio[ext=m4a]/"
                "bestvideo[height<=720]+bestaudio/best"
             << "--merge-output-format" << "mp4";
    } else if (fmt == "video_480p") {
        args << "-f"
             << "bestvideo[height<=480][ext=mp4]+bestaudio[ext=m4a]/"
                "bestvideo[height<=480]+bestaudio/best"
             << "--merge-output-format" << "mp4";
    } else {
        args << "-f"
             << "bestvideo[ext=mp4]+bestaudio[ext=m4a]/bestvideo+bestaudio/best"
             << "--merge-output-format" << "mp4";
    }

    args << "-o" << (outDir() + "/%(title)s.%(ext)s") << url;

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
        QDirIterator it(outDir(), QDir::Files);
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
                   && !m_downloadedFile.isEmpty() && isAudioMode();

    if (noMusic)
        startDemucs(m_downloadedFile);
    else
        finishPipeline();
}

// ─────────────────────────────────────────────────────────────────────────────
// Pipeline Step 1b — Local file processing
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::startLocalProcess()
{
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  Mode: Local File Processing", "#80cbc4");
    appendLog("  File: " + m_localFilePath, "#777");

    static const QStringList videoExts = {
        "mp4", "mkv", "avi", "mov", "webm", "flv", "wmv", "m4v", "ts", "mpg", "mpeg"
    };

    QString ext = QFileInfo(m_localFilePath).suffix().toLower();
    bool isVideo = videoExts.contains(ext);

    if (isVideo) {
        appendLog("  [→] Video file detected — extracting audio stream…", "#80cbc4");
        startExtractAudio(m_localFilePath);
    } else {
        m_downloadedFile = m_localFilePath;
        bool noMusic = m_noMusicChk->isChecked() && !m_demucsPath.isEmpty();
        if (noMusic)
            startDemucs(m_downloadedFile);
        else
            startDirectConvert(m_downloadedFile);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Pipeline Step 1c — Extract audio from local video (ffmpeg)
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::startExtractAudio(const QString &videoFile)
{
    if (m_ffmpegPath.isEmpty()) {
        appendLog("  [ERR] ffmpeg not found — cannot extract audio from video.", "#ef5350");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  Step — ffmpeg Audio Extraction", "#80cbc4");

    QFileInfo fi(videoFile);
    QString audioOut = outDir() + "/" + fi.completeBaseName() + "_extracted.wav";
    m_extractedAudioFile = audioOut;

    QStringList args;
    args << "-y" << "-i" << videoFile
         << "-vn" << "-q:a" << "0" << "-map" << "a"
         << audioOut;

    appendLog("  > ffmpeg -i [video] -vn -q:a 0 -map a [audio.wav]", "#444");

    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead,    this, &MainWindow::onExtractAudioData);
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onExtractAudioFinished);
    m_proc->start(m_ffmpegPath, args);
}

void MainWindow::onExtractAudioData()
{
    QString out = QString::fromLocal8Bit(m_proc->readAll());
    for (const QString &ln : out.split('\n', Qt::SkipEmptyParts)) {
        QString t = ln.trimmed();
        if (!t.isEmpty() && (t.startsWith("size=") || t.startsWith("frame=")))
            appendLog("  " + t, "#80cbc4");
    }
}

void MainWindow::onExtractAudioFinished(int code, QProcess::ExitStatus st)
{
    m_proc->deleteLater();
    m_proc = nullptr;

    if (code != 0 || st == QProcess::CrashExit) {
        appendLog("  [ERR] Audio extraction failed (exit " + QString::number(code) + ")", "#ef5350");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    appendLog("  [OK] Audio extracted: " + QFileInfo(m_extractedAudioFile).fileName(), "#66bb6a");
    m_downloadedFile = m_extractedAudioFile;

    bool noMusic = m_noMusicChk->isChecked() && !m_demucsPath.isEmpty();
    if (noMusic)
        startDemucs(m_extractedAudioFile);
    else
        startDirectConvert(m_extractedAudioFile);
}

// ─────────────────────────────────────────────────────────────────────────────
// Pipeline Step 2 — Demucs vocal separation
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::startDemucs(const QString &audioFile)
{
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");

    int shifts = m_extremeChk->isChecked() ? 4 : 2;
    QString overlapArg = m_extremeChk->isChecked() ? "--overlap=0.25" : "--overlap=0.1";

    appendLog(QString("  Step 2 — Demucs  (htdemucs_ft · shifts=%1 · two-stems=vocals)").arg(shifts),
              "#ce93d8");
    appendLog("  Input : " + audioFile, "#777");

    QString device = currentDevice();
    if (m_cudaFailed) device = "cpu";
    appendLog("  Device: " + device.toUpper(), "#777");

    QStringList args;
    args << "-n"               << "htdemucs_ft"
         << "--two-stems=vocals"
         << QString("--shifts=%1").arg(shifts)
         << overlapArg
         << "-d"               << device
         << "-o"               << outDir()
         << audioFile;

    appendLog("  > demucs " + args.join(" "), "#444");

    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead,    this, &MainWindow::onDemucsData);
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
    QString ext       = outputExtForAudio();
    QString noMusicOut = outDir() + "/" + base + " (no music)." + ext;

    if (!QFile::exists(vocalsWav)) {
        appendLog("  [ERR] Expected vocals.wav not found: " + vocalsWav, "#ef5350");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    m_pendingDelete     = m_downloadedFile;
    m_pendingConvertOut = noMusicOut;

    startConvert(vocalsWav, noMusicOut);
}

// ─────────────────────────────────────────────────────────────────────────────
// Pipeline Step 3a — Convert vocals.wav → selected format
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::startConvert(const QString &inputAudio, const QString &outputFile)
{
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");

    QString outExt = QFileInfo(outputFile).suffix().toLower();

    if (outExt == "wav") {
        appendLog("  Step 3 — WAV copy (no re-encoding needed)", "#80cbc4");
        appendLog("  Output: " + QFileInfo(outputFile).fileName(), "#aaa");
        bool ok = QFile::copy(inputAudio, outputFile);
        if (ok) {
            appendLog("  [OK] Saved: " + outputFile, "#66bb6a");
        } else {
            appendLog("  [ERR] Failed to copy WAV.", "#ef5350");
        }
        doCleanup();
        finishPipeline();
        return;
    }

    appendLog("  Step 3 — ffmpeg  (vocals.wav → ." + outExt + ")", "#80cbc4");
    appendLog("  Output: " + QFileInfo(outputFile).fileName(), "#aaa");

    if (m_ffmpegPath.isEmpty()) {
        appendLog("  [ERR] ffmpeg not found — cannot convert. Install ffmpeg.", "#ef5350");
        setRunning(false);
        appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
        return;
    }

    QStringList args;
    args << "-y" << "-i" << inputAudio;

    if (outExt == "mp3")       args << "-c:a" << "libmp3lame" << "-q:a" << "0";
    else if (outExt == "flac") args << "-c:a" << "flac";
    else                       args << "-c:a" << "aac" << "-b:a" << "256k";

    args << outputFile;

    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead,    this, &MainWindow::onConvertData);
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
    doCleanup();
    finishPipeline();
}

// ─────────────────────────────────────────────────────────────────────────────
// Pipeline Step 3b — Direct format conversion (no demucs path)
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::startDirectConvert(const QString &inputFile)
{
    QFileInfo fi(inputFile);
    QString outExt = outputExtForAudio();
    QString outputFile = outDir() + "/" + fi.completeBaseName() + "." + outExt;

    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
    appendLog("  Step — Format Conversion  (→ ." + outExt + ")", "#80cbc4");

    if (fi.suffix().toLower() == outExt
        && fi.absoluteFilePath() == QFileInfo(outputFile).absoluteFilePath()) {
        appendLog("  [OK] File already in target format at output dir.", "#66bb6a");
        if (!m_extractedAudioFile.isEmpty() && QFile::exists(m_extractedAudioFile))
            QFile::remove(m_extractedAudioFile);
        finishPipeline();
        return;
    }

    if (fi.suffix().toLower() == outExt) {
        QFile::copy(inputFile, outputFile);
        appendLog("  [OK] Copied → " + outputFile, "#66bb6a");
        if (!m_extractedAudioFile.isEmpty() && QFile::exists(m_extractedAudioFile)
            && m_extractedAudioFile != inputFile)
            QFile::remove(m_extractedAudioFile);
        m_pendingConvertOut = outputFile;
        finishPipeline();
        return;
    }

    if (m_ffmpegPath.isEmpty()) {
        appendLog("  [ERR] ffmpeg not found — cannot convert format.", "#ef5350");
        appendLog("        Install ffmpeg or use a matching format.", "#ff8a65");
        setRunning(false);
        return;
    }

    QStringList args;
    args << "-y" << "-i" << inputFile;
    if (outExt == "mp3")       args << "-c:a" << "libmp3lame" << "-q:a" << "0";
    else if (outExt == "flac") args << "-c:a" << "flac";
    else if (outExt == "wav")  args << "-c:a" << "pcm_s16le";
    else                       args << "-c:a" << "aac" << "-b:a" << "256k";
    args << outputFile;

    m_pendingConvertOut = outputFile;

    appendLog("  > ffmpeg -i [input] → " + outputFile, "#444");

    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead, this, &MainWindow::onConvertData);
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int code, QProcess::ExitStatus st) {
        m_proc->deleteLater();
        m_proc = nullptr;
        if (code != 0 || st == QProcess::CrashExit) {
            appendLog("  [ERR] Format conversion failed.", "#ef5350");
            setRunning(false);
            appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");
            return;
        }
        appendLog("  [OK] Saved: " + m_pendingConvertOut, "#66bb6a");
        if (!m_extractedAudioFile.isEmpty() && QFile::exists(m_extractedAudioFile))
            QFile::remove(m_extractedAudioFile);
        finishPipeline();
    });
    m_proc->start(m_ffmpegPath, args);
}

// ─────────────────────────────────────────────────────────────────────────────
// Pipeline end
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::finishPipeline()
{
    const Strings &S = getStrings(m_arabic);
    appendLog("  ✓  Done.", "#66bb6a");
    setRunning(false);
    appendLog("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "#444");

    QString finalFile = m_pendingConvertOut.isEmpty()
                        ? m_downloadedFile
                        : m_pendingConvertOut;
    statusBar()->showMessage(S.statusDone + "  —  " + QFileInfo(finalFile).fileName());
}

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("m4s_d");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("m4s");
    app.setOrganizationDomain("github.com/mahmoudelsheikh7/M4S_D");

    QSettings s;
    bool darkTheme = s.value("darkTheme", true).toBool();
    if (darkTheme) makeDarkPalette(app);
    else           makeLightPalette(app);

    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
