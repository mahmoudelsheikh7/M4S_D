#include <QApplication>
#include <QDirIterator>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QLabel>
#include <QFileDialog>
#include <QSettings>
#include <QProcess>
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QMessageBox>
#include <QTimer>
#include <QFont>
#include <QFontDatabase>
#include <QClipboard>
#include <QRegularExpression>
#include <QScrollBar>
#include <QFrame>
#include <QGroupBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStyleFactory>
#include <QGuiApplication>
#include <QScreen>
#include <QProgressBar>
#include <QSplitter>
#include <QSizePolicy>
#include <QShortcut>
#include <QKeySequence>
#include <QPalette>
#include <QColor>
#include <QPixmap>
#include <QIcon>
#include <QToolTip>
#include <QStatusBar>
#include <QThread>
#include <QEventLoop>
#include <QTemporaryFile>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <functional>
#include <memory>

struct Lang {
    QString appTitle;
    QString langBtn;
    QString themeBtn;
    QString setupBtn;
    QString settingsBtn;
    QString urlPlaceholder;
    QString pasteBtn;
    QString browseBtn;
    QString formatLabel;
    QString noMusicLabel;
    QString extremeLabel;
    QString startBtn;
    QString stopBtn;
    QString logLabel;
    QString clearLog;
    QString statusReady;
    QString statusRunning;
    QString statusDone;
    QString statusError;
    QString setupTitle;
    QString setupMsg;
    QString setupOk;
    QString toolsMissing;
    QString toolsOk;
    QString downloadingTools;
    QString extractingAudio;
    QString runningDemucs;
    QString convertingFormat;
    QString cleaningUp;
    QString doneSuccess;
    QString errorOccurred;
    QString selectFile;
    QString urlRequired;
    QString openOutputDir;
    QString outputDirLabel;
    QString browseDirBtn;
    QString cpuMode;
    QString gpuMode;
    QString deviceLabel;
};

static const Lang EN = {
    "m4s d  —  Universal Downloader & AI Vocal Extractor",
    "🌐 عربي",
    "🌙 Dark",
    "⚙ Setup Tools",
    "☰ Settings",
    "Paste URL here (YouTube, SoundCloud, 1000+ sites)…",
    "📋 Paste",
    "📂 Browse Local File",
    "Output Format",
    "🎤 No Music  (AI Vocal Extraction)",
    "⚡ Extreme Isolation  (shifts=4, slower)",
    "▶  Start",
    "■  Stop",
    "Real-time Log",
    "🗑 Clear",
    "Ready",
    "Running…",
    "Done ✓",
    "Error ✗",
    "Setup Tools",
    "Download and configure yt-dlp, ffmpeg, and demucs automatically?",
    "Start Setup",
    "⚠ Some tools are missing. Click 'Setup Tools' to install them.",
    "✓ All tools are ready.",
    "Downloading tools…",
    "Extracting audio…",
    "Running AI vocal extraction (demucs)…",
    "Converting to target format…",
    "Cleaning up temporary files…",
    "Done! File saved successfully.",
    "An error occurred. Check the log for details.",
    "Select Audio or Video File",
    "Please enter a URL or select a local file.",
    "📁 Open Output Folder",
    "Output Directory",
    "Browse…",
    "CPU",
    "GPU (CUDA)",
    "Device"
};

static const Lang AR = {
    "m4s d  —  محمّل شامل وعازل أصوات بالذكاء الاصطناعي",
    "🌐 English",
    "🌙 داكن",
    "⚙ إعداد الأدوات",
    "☰ الإعدادات",
    "الصق رابط هنا (يوتيوب، ساوندكلاود، 1000+ موقع)…",
    "📋 لصق",
    "📂 تصفح ملفاً محلياً",
    "صيغة الإخراج",
    "🎤 بدون موسيقى  (عزل الصوت بالذكاء الاصطناعي)",
    "⚡ عزل متطرف  (shifts=4، أبطأ)",
    "▶  ابدأ",
    "■  إيقاف",
    "السجل المباشر",
    "🗑 مسح",
    "جاهز",
    "جارٍ التشغيل…",
    "تمّ ✓",
    "خطأ ✗",
    "إعداد الأدوات",
    "تحميل وإعداد yt-dlp وffmpeg وdemucs تلقائياً؟",
    "بدء الإعداد",
    "⚠ بعض الأدوات مفقودة. اضغط 'إعداد الأدوات' لتثبيتها.",
    "✓ جميع الأدوات جاهزة.",
    "جارٍ تحميل الأدوات…",
    "جارٍ استخراج الصوت…",
    "جارٍ تشغيل عزل الصوت بالذكاء الاصطناعي (demucs)…",
    "جارٍ تحويل الصيغة…",
    "جارٍ حذف الملفات المؤقتة…",
    "تمّ! تم حفظ الملف بنجاح.",
    "حدث خطأ. راجع السجل للتفاصيل.",
    "اختر ملف صوت أو فيديو",
    "الرجاء إدخال رابط أو اختيار ملف محلي.",
    "📁 فتح مجلد الإخراج",
    "مجلد الإخراج",
    "تصفح…",
    "معالج",
    "GPU (CUDA)",
    "الجهاز"
};

static QString getDarkStyleSheet() {
    return R"(
QMainWindow, QWidget {
    background-color: #0d1117;
    color: #e6edf3;
    font-family: 'Segoe UI', 'DejaVu Sans', Arial, sans-serif;
    font-size: 13px;
}
QGroupBox {
    border: 1px solid #30363d;
    border-radius: 6px;
    margin-top: 8px;
    padding-top: 8px;
    background-color: #161b22;
    color: #e6edf3;
}
QGroupBox::title {
    color: #58a6ff;
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}
QPushButton {
    background-color: #21262d;
    color: #e6edf3;
    border: 1px solid #30363d;
    border-radius: 6px;
    padding: 6px 14px;
    font-weight: 500;
}
QPushButton:hover {
    background-color: #30363d;
    border-color: #58a6ff;
}
QPushButton:pressed {
    background-color: #161b22;
}
QPushButton#startBtn {
    background-color: #238636;
    color: #ffffff;
    border-color: #2ea043;
    font-weight: 700;
    font-size: 14px;
    padding: 8px 28px;
}
QPushButton#startBtn:hover {
    background-color: #2ea043;
}
QPushButton#stopBtn {
    background-color: #b91c1c;
    color: #ffffff;
    border-color: #dc2626;
    font-weight: 700;
    font-size: 14px;
    padding: 8px 28px;
}
QPushButton#stopBtn:hover {
    background-color: #dc2626;
}
QPushButton#langBtn, QPushButton#themeBtn {
    background-color: #1f2937;
    color: #60a5fa;
    border-color: #3b82f6;
    font-weight: 600;
}
QPushButton#langBtn:hover, QPushButton#themeBtn:hover {
    background-color: #2563eb;
    color: #ffffff;
}
QPushButton#setupBtn {
    background-color: #1e3a5f;
    color: #93c5fd;
    border-color: #3b82f6;
}
QPushButton#setupBtn:hover {
    background-color: #2563eb;
    color: #ffffff;
}
QLineEdit {
    background-color: #0d1117;
    color: #e6edf3;
    border: 1px solid #30363d;
    border-radius: 6px;
    padding: 8px 12px;
    font-size: 13px;
    selection-background-color: #1f6feb;
}
QLineEdit:focus {
    border-color: #58a6ff;
    background-color: #161b22;
}
QComboBox {
    background-color: #21262d;
    color: #e6edf3;
    border: 1px solid #30363d;
    border-radius: 6px;
    padding: 6px 12px;
    min-width: 140px;
}
QComboBox:hover {
    border-color: #58a6ff;
}
QComboBox::drop-down {
    border: none;
    width: 20px;
}
QComboBox QAbstractItemView {
    background-color: #21262d;
    color: #e6edf3;
    border: 1px solid #30363d;
    selection-background-color: #1f6feb;
}
QCheckBox {
    color: #e6edf3;
    spacing: 8px;
    font-size: 13px;
}
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #58a6ff;
    border-radius: 3px;
    background-color: #0d1117;
}
QCheckBox::indicator:checked {
    background-color: #1f6feb;
    border-color: #58a6ff;
}
QTextEdit {
    background-color: #010409;
    color: #7ee787;
    border: 1px solid #30363d;
    border-radius: 6px;
    font-family: 'Consolas', 'Cascadia Code', 'DejaVu Sans Mono', monospace;
    font-size: 12px;
    padding: 6px;
}
QLabel {
    color: #8b949e;
    font-size: 12px;
}
QLabel#sectionLabel {
    color: #58a6ff;
    font-size: 12px;
    font-weight: 600;
}
QLabel#statusLabel {
    color: #3fb950;
    font-size: 13px;
    font-weight: 600;
    padding: 4px 8px;
    background-color: #161b22;
    border: 1px solid #30363d;
    border-radius: 4px;
}
QFrame#separator {
    background-color: #30363d;
}
QScrollBar:vertical {
    background: #0d1117;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #30363d;
    border-radius: 4px;
    min-height: 20px;
}
QScrollBar::handle:vertical:hover {
    background: #58a6ff;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QProgressBar {
    background-color: #21262d;
    border: 1px solid #30363d;
    border-radius: 4px;
    height: 6px;
    text-align: center;
}
QProgressBar::chunk {
    background-color: #1f6feb;
    border-radius: 4px;
}
QToolTip {
    background-color: #21262d;
    color: #e6edf3;
    border: 1px solid #58a6ff;
    border-radius: 4px;
    padding: 4px 8px;
}
)";
}

static QString getLightStyleSheet() {
    return R"(
QMainWindow, QWidget {
    background-color: #ffffff;
    color: #24292f;
    font-family: 'Segoe UI', 'DejaVu Sans', Arial, sans-serif;
    font-size: 13px;
}
QGroupBox {
    border: 1px solid #d0d7de;
    border-radius: 6px;
    margin-top: 8px;
    padding-top: 8px;
    background-color: #f6f8fa;
    color: #24292f;
}
QGroupBox::title {
    color: #0969da;
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}
QPushButton {
    background-color: #f6f8fa;
    color: #24292f;
    border: 1px solid #d0d7de;
    border-radius: 6px;
    padding: 6px 14px;
    font-weight: 500;
}
QPushButton:hover {
    background-color: #eaeef2;
    border-color: #0969da;
}
QPushButton:pressed {
    background-color: #d0d7de;
}
QPushButton#startBtn {
    background-color: #1a7f37;
    color: #ffffff;
    border-color: #1a7f37;
    font-weight: 700;
    font-size: 14px;
    padding: 8px 28px;
}
QPushButton#startBtn:hover {
    background-color: #2ea043;
}
QPushButton#stopBtn {
    background-color: #cf222e;
    color: #ffffff;
    border-color: #cf222e;
    font-weight: 700;
    font-size: 14px;
    padding: 8px 28px;
}
QPushButton#stopBtn:hover {
    background-color: #a40e26;
}
QPushButton#langBtn, QPushButton#themeBtn {
    background-color: #dbeafe;
    color: #1d4ed8;
    border-color: #93c5fd;
    font-weight: 600;
}
QPushButton#langBtn:hover, QPushButton#themeBtn:hover {
    background-color: #2563eb;
    color: #ffffff;
}
QPushButton#setupBtn {
    background-color: #eff6ff;
    color: #1d4ed8;
    border-color: #93c5fd;
}
QPushButton#setupBtn:hover {
    background-color: #2563eb;
    color: #ffffff;
}
QLineEdit {
    background-color: #ffffff;
    color: #24292f;
    border: 1px solid #d0d7de;
    border-radius: 6px;
    padding: 8px 12px;
    font-size: 13px;
    selection-background-color: #0969da;
    selection-color: #ffffff;
}
QLineEdit:focus {
    border-color: #0969da;
    outline: none;
}
QComboBox {
    background-color: #f6f8fa;
    color: #24292f;
    border: 1px solid #d0d7de;
    border-radius: 6px;
    padding: 6px 12px;
    min-width: 140px;
}
QComboBox:hover { border-color: #0969da; }
QComboBox::drop-down { border: none; width: 20px; }
QComboBox QAbstractItemView {
    background-color: #ffffff;
    color: #24292f;
    border: 1px solid #d0d7de;
    selection-background-color: #0969da;
    selection-color: #ffffff;
}
QCheckBox {
    color: #24292f;
    spacing: 8px;
    font-size: 13px;
}
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #0969da;
    border-radius: 3px;
    background-color: #ffffff;
}
QCheckBox::indicator:checked {
    background-color: #0969da;
    border-color: #0969da;
}
QTextEdit {
    background-color: #f6f8fa;
    color: #1a7f37;
    border: 1px solid #d0d7de;
    border-radius: 6px;
    font-family: 'Consolas', 'Cascadia Code', 'DejaVu Sans Mono', monospace;
    font-size: 12px;
    padding: 6px;
}
QLabel { color: #57606a; font-size: 12px; }
QLabel#sectionLabel { color: #0969da; font-size: 12px; font-weight: 600; }
QLabel#statusLabel {
    color: #1a7f37;
    font-size: 13px;
    font-weight: 600;
    padding: 4px 8px;
    background-color: #f6f8fa;
    border: 1px solid #d0d7de;
    border-radius: 4px;
}
QFrame#separator { background-color: #d0d7de; }
QScrollBar:vertical {
    background: #f6f8fa;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #d0d7de;
    border-radius: 4px;
    min-height: 20px;
}
QScrollBar::handle:vertical:hover { background: #0969da; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QProgressBar {
    background-color: #d0d7de;
    border: none;
    border-radius: 4px;
    height: 6px;
}
QProgressBar::chunk { background-color: #0969da; border-radius: 4px; }
QToolTip {
    background-color: #ffffff;
    color: #24292f;
    border: 1px solid #0969da;
    border-radius: 4px;
    padding: 4px 8px;
}
)";
}

class DownloadWorker : public QThread {
    Q_OBJECT
public:
    QNetworkAccessManager *nam;
    QUrl url;
    QString destPath;
    qint64 totalBytes = 0;
    qint64 receivedBytes = 0;

    explicit DownloadWorker(QObject *parent = nullptr) : QThread(parent) {
        nam = new QNetworkAccessManager();
    }

    ~DownloadWorker() { delete nam; }

    void download(const QUrl &u, const QString &dest) {
        url = u; destPath = dest;
    }

    void run() override {
        QEventLoop loop;
        QNetworkReply *reply = nam->get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 r, qint64 t){
            receivedBytes = r; totalBytes = t;
            if (t > 0) emit progress((int)(r * 100 / t));
        });
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        if (reply->error() == QNetworkReply::NoError) {
            QFile f(destPath);
            if (f.open(QIODevice::WriteOnly)) { f.write(reply->readAll()); f.close(); }
            emit finished(true, "");
        } else {
            emit finished(false, reply->errorString());
        }
        reply->deleteLater();
    }

signals:
    void progress(int pct);
    void finished(bool ok, const QString &err);
};

class MainWindow : public QMainWindow {
    Q_OBJECT

    bool m_arabic = false;
    bool m_dark = true;
    bool m_running = false;
    QString m_localFile;
    QString m_outputDir;
    QString m_toolsDir;
    QString m_ytdlpPath;
    QString m_ffmpegPath;
    QString m_demucsPath;
    QString m_lastOutputFile;
    QProcess *m_proc = nullptr;
    QSettings m_settings;

    QPushButton *m_langBtn, *m_themeBtn, *m_setupBtn;
    QLineEdit *m_urlEdit;
    QPushButton *m_pasteBtn, *m_browseBtn;
    QComboBox *m_formatCombo, *m_deviceCombo;
    QCheckBox *m_noMusicChk, *m_extremeChk;
    QPushButton *m_startBtn, *m_stopBtn;
    QTextEdit *m_logEdit;
    QPushButton *m_clearLogBtn;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QLineEdit *m_outputDirEdit;
    QPushButton *m_openDirBtn, *m_browseDirBtn;
    QLabel *m_outputDirLabel, *m_deviceLabel, *m_formatLabel;

    const Lang &L() const { return m_arabic ? AR : EN; }

public:
    explicit MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent), m_settings("m4s", "m4s_d") {

        m_arabic = m_settings.value("lang", false).toBool();
        m_dark   = m_settings.value("dark", true).toBool();

#ifdef _WIN32
        m_toolsDir = QDir::fromNativeSeparators(
            qEnvironmentVariable("LOCALAPPDATA") + "/m4s/m4s_d/bin");
#else
        m_toolsDir = QDir::homePath() + "/.local/share/m4s_d/bin";
#endif
        QDir().mkpath(m_toolsDir);

        m_outputDir = m_settings.value("outputDir",
            QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).toString();

        buildUI();
        applyTheme();
        applyLang();
        detectTools();

        resize(820, 720);
        setAcceptDrops(true);
        centerWindow();
    }

    ~MainWindow() {
        if (m_proc) { m_proc->kill(); delete m_proc; }
    }

protected:
    void dragEnterEvent(QDragEnterEvent *e) override {
        if (e->mimeData()->hasUrls()) e->acceptProposedAction();
    }
    void dropEvent(QDropEvent *e) override {
        const auto urls = e->mimeData()->urls();
        if (urls.isEmpty()) return;
        const QUrl &u = urls.first();
        if (u.isLocalFile()) {
            m_localFile = u.toLocalFile();
            m_urlEdit->setText(m_localFile);
        } else {
            m_urlEdit->setText(u.toString());
        }
    }

private:
    void centerWindow() {
        QScreen *scr = QGuiApplication::primaryScreen();
        if (!scr) return;
        QRect sg = scr->availableGeometry();
        move(sg.center() - QPoint(width()/2, height()/2));
    }

    void buildUI() {
        QWidget *central = new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout *root = new QVBoxLayout(central);
        root->setSpacing(8);
        root->setContentsMargins(12, 10, 12, 10);

        QHBoxLayout *topBar = new QHBoxLayout();
        topBar->setSpacing(6);

        m_langBtn  = new QPushButton(this);  m_langBtn->setObjectName("langBtn");
        m_themeBtn = new QPushButton(this);  m_themeBtn->setObjectName("themeBtn");
        m_setupBtn = new QPushButton(this);  m_setupBtn->setObjectName("setupBtn");

        topBar->addWidget(m_langBtn);
        topBar->addWidget(m_themeBtn);
        topBar->addWidget(m_setupBtn);
        topBar->addStretch();

        m_statusLabel = new QLabel(this);
        m_statusLabel->setObjectName("statusLabel");
        topBar->addWidget(m_statusLabel);

        root->addLayout(topBar);

        QFrame *sep1 = new QFrame(this);
        sep1->setObjectName("separator");
        sep1->setFixedHeight(1);
        root->addWidget(sep1);

        QGroupBox *inputGroup = new QGroupBox(this);
        QVBoxLayout *inputV = new QVBoxLayout(inputGroup);
        inputV->setSpacing(6);

        QHBoxLayout *urlRow = new QHBoxLayout();
        m_urlEdit   = new QLineEdit(this);
        m_pasteBtn  = new QPushButton(this);
        m_browseBtn = new QPushButton(this);
        urlRow->addWidget(m_urlEdit, 1);
        urlRow->addWidget(m_pasteBtn);
        urlRow->addWidget(m_browseBtn);
        inputV->addLayout(urlRow);

        QHBoxLayout *optRow = new QHBoxLayout();
        optRow->setSpacing(10);

        m_formatLabel = new QLabel(this);
        m_formatLabel->setObjectName("sectionLabel");
        m_formatCombo = new QComboBox(this);
        m_formatCombo->addItems({"M4A (default)", "MP3 (VBR)", "WAV", "FLAC",
                                  "Video – Best", "Video – 4K", "Video – 1080p",
                                  "Video – 720p", "Video – 480p"});
        optRow->addWidget(m_formatLabel);
        optRow->addWidget(m_formatCombo);
        optRow->addSpacing(16);

        m_deviceLabel = new QLabel(this);
        m_deviceLabel->setObjectName("sectionLabel");
        m_deviceCombo = new QComboBox(this);
        m_deviceCombo->addItems({EN.cpuMode, EN.gpuMode});
        optRow->addWidget(m_deviceLabel);
        optRow->addWidget(m_deviceCombo);
        optRow->addStretch();
        inputV->addLayout(optRow);

        m_noMusicChk = new QCheckBox(this);
        m_extremeChk = new QCheckBox(this);
        m_extremeChk->setEnabled(false);
        connect(m_noMusicChk, &QCheckBox::toggled, m_extremeChk, &QCheckBox::setEnabled);

        QHBoxLayout *chkRow = new QHBoxLayout();
        chkRow->addWidget(m_noMusicChk);
        chkRow->addSpacing(20);
        chkRow->addWidget(m_extremeChk);
        chkRow->addStretch();
        inputV->addLayout(chkRow);

        root->addWidget(inputGroup);

        QGroupBox *outDirGroup = new QGroupBox(this);
        QHBoxLayout *outDirRow = new QHBoxLayout(outDirGroup);
        m_outputDirLabel = new QLabel(this);
        m_outputDirLabel->setObjectName("sectionLabel");
        m_outputDirEdit = new QLineEdit(m_outputDir, this);
        m_browseDirBtn  = new QPushButton(this);
        m_openDirBtn    = new QPushButton(this);
        outDirRow->addWidget(m_outputDirLabel);
        outDirRow->addWidget(m_outputDirEdit, 1);
        outDirRow->addWidget(m_browseDirBtn);
        outDirRow->addWidget(m_openDirBtn);
        root->addWidget(outDirGroup);

        QHBoxLayout *ctrlRow = new QHBoxLayout();
        ctrlRow->setSpacing(10);
        m_startBtn = new QPushButton(this); m_startBtn->setObjectName("startBtn");
        m_stopBtn  = new QPushButton(this); m_stopBtn->setObjectName("stopBtn");
        m_stopBtn->setEnabled(false);
        m_progressBar = new QProgressBar(this);
        m_progressBar->setRange(0, 0);
        m_progressBar->setFixedHeight(6);
        m_progressBar->setVisible(false);
        ctrlRow->addWidget(m_startBtn);
        ctrlRow->addWidget(m_stopBtn);
        ctrlRow->addWidget(m_progressBar, 1);
        root->addLayout(ctrlRow);

        QGroupBox *logGroup = new QGroupBox(this);
        QVBoxLayout *logV = new QVBoxLayout(logGroup);
        logV->setSpacing(4);
        QHBoxLayout *logTopRow = new QHBoxLayout();
        QLabel *logLbl = new QLabel(this); logLbl->setObjectName("sectionLabel");
        logLbl->setProperty("logTitle", true);
        m_clearLogBtn = new QPushButton(this);
        logTopRow->addWidget(logLbl);
        logTopRow->addStretch();
        logTopRow->addWidget(m_clearLogBtn);
        logV->addLayout(logTopRow);
        m_logEdit = new QTextEdit(this);
        m_logEdit->setReadOnly(true);
        m_logEdit->setMinimumHeight(160);
        logV->addWidget(m_logEdit);
        root->addWidget(logGroup, 1);

        wireSignals();

        m_urlEdit->setPlaceholderText(L().urlPlaceholder);
    }

    void wireSignals() {
        connect(m_langBtn,  &QPushButton::clicked, this, &MainWindow::toggleLang);
        connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::toggleTheme);
        connect(m_setupBtn, &QPushButton::clicked, this, &MainWindow::runSetup);
        connect(m_pasteBtn, &QPushButton::clicked, this, [this](){
            m_localFile.clear();
            m_urlEdit->setText(QGuiApplication::clipboard()->text().trimmed());
        });
        connect(m_browseBtn, &QPushButton::clicked, this, &MainWindow::browseFile);
        connect(m_startBtn,  &QPushButton::clicked, this, &MainWindow::startPipeline);
        connect(m_stopBtn,   &QPushButton::clicked, this, &MainWindow::stopPipeline);
        connect(m_clearLogBtn, &QPushButton::clicked, m_logEdit, &QTextEdit::clear);
        connect(m_browseDirBtn, &QPushButton::clicked, this, [this](){
            QString d = QFileDialog::getExistingDirectory(this, "", m_outputDirEdit->text());
            if (!d.isEmpty()) { m_outputDirEdit->setText(d); m_outputDir = d; m_settings.setValue("outputDir", d); }
        });
        connect(m_openDirBtn, &QPushButton::clicked, this, [this](){
            QString d = m_outputDirEdit->text();
            if (!d.isEmpty()) QProcess::startDetached("xdg-open", {d});
        });
        connect(m_outputDirEdit, &QLineEdit::textChanged, this, [this](const QString &t){
            m_outputDir = t; m_settings.setValue("outputDir", t);
        });
    }

    void applyTheme() {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        qApp->setStyleSheet(m_dark ? getDarkStyleSheet() : getLightStyleSheet());
        m_settings.setValue("dark", m_dark);
    }

    void applyLang() {
        const Lang &l = L();
        setWindowTitle(l.appTitle);
        m_langBtn->setText(l.langBtn);
        m_themeBtn->setText(m_dark ? "☀ Light" : "🌙 Dark");
        m_setupBtn->setText(l.setupBtn);
        m_pasteBtn->setText(l.pasteBtn);
        m_browseBtn->setText(l.browseBtn);
        m_noMusicChk->setText(l.noMusicLabel);
        m_extremeChk->setText(l.extremeLabel);
        m_startBtn->setText(l.startBtn);
        m_stopBtn->setText(l.stopBtn);
        m_clearLogBtn->setText(l.clearLog);
        m_openDirBtn->setText(l.openOutputDir);
        m_browseDirBtn->setText(l.browseDirBtn);
        m_outputDirLabel->setText(l.outputDirLabel);
        m_deviceLabel->setText(l.deviceLabel);
        m_formatLabel->setText(l.formatLabel);
        m_urlEdit->setPlaceholderText(l.urlPlaceholder);

        Qt::LayoutDirection dir = m_arabic ? Qt::RightToLeft : Qt::LeftToRight;
        qApp->setLayoutDirection(dir);

        m_settings.setValue("lang", m_arabic);
    }

    void toggleLang() {
        m_arabic = !m_arabic;
        applyLang();
    }

    void toggleTheme() {
        m_dark = !m_dark;
        applyTheme();
        m_themeBtn->setText(m_dark ? "☀ Light" : "🌙 Dark");
    }

    void detectTools() {
        auto findTool = [&](const QString &name) -> QString {
#ifdef _WIN32
            QString local = m_toolsDir + "/" + name + ".exe";
            if (QFile::exists(local)) return local;
#else
            QString local = m_toolsDir + "/" + name;
            if (QFile::exists(local)) return local;
#endif
            QString found = QStandardPaths::findExecutable(name);
            return found;
        };

        m_ytdlpPath  = findTool("yt-dlp");
        m_ffmpegPath = findTool("ffmpeg");

        QStringList demucsNames = {"demucs"};
        for (const QString &n : demucsNames) {
            QString p = QStandardPaths::findExecutable(n);
            if (!p.isEmpty()) { m_demucsPath = p; break; }
            QString lp = QDir::homePath() + "/.local/bin/" + n;
            if (QFile::exists(lp)) { m_demucsPath = lp; break; }
        }

        if (m_ytdlpPath.isEmpty() || m_ffmpegPath.isEmpty()) {
            setStatus(L().toolsMissing, "#f0883e");
        } else {
            setStatus(L().toolsOk, "#3fb950");
        }
    }

    void setStatus(const QString &msg, const QString &color = "#3fb950") {
        m_statusLabel->setText(msg);
        m_statusLabel->setStyleSheet(
            QString("color: %1; font-size:13px; font-weight:600; padding:4px 8px;"
                    "background-color:%2; border:1px solid %1; border-radius:4px;")
                .arg(color, m_dark ? "#161b22" : "#f6f8fa"));
    }

    void logMsg(const QString &msg, const QString &color = "") {
        QString html;
        QString ts = QDateTime::currentDateTime().toString("[hh:mm:ss] ");
        if (!color.isEmpty()) {
            html = QString("<span style='color:%1'>%2%3</span>").arg(color, ts, msg.toHtmlEscaped());
        } else {
            html = QString("<span>%1%2</span>").arg(ts, msg.toHtmlEscaped());
        }
        m_logEdit->append(html);
        QScrollBar *sb = m_logEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
    }

    void browseFile() {
        QString path = QFileDialog::getOpenFileName(
            this, L().selectFile, QDir::homePath(),
            "Audio/Video Files (*.mp3 *.m4a *.wav *.flac *.ogg *.opus *.mp4 *.mkv *.webm *.avi *.mov *.flv);;"
            "All Files (*)");
        if (!path.isEmpty()) {
            m_localFile = path;
            m_urlEdit->setText(path);
        }
    }

    void runSetup() {
#ifdef _WIN32
        QMessageBox::information(this, L().setupTitle,
            "Windows: Tools will be downloaded to:\n" + m_toolsDir +
            "\n\nThis may take a few minutes.");
        downloadWindowsTools();
#else
        QMessageBox::information(this, L().setupTitle,
            "Linux: Please run the installer script:\n\n"
            "  chmod +x install_linux.sh\n"
            "  ./install_linux.sh\n\n"
            "This installs Qt6, ffmpeg, yt-dlp, and demucs AI.");
#endif
    }

    void downloadWindowsTools() {
        QDir().mkpath(m_toolsDir);
        setStatus(L().downloadingTools, "#f0883e");
        m_progressBar->setVisible(true);

        auto *worker = new DownloadWorker(this);
        QString ytPath = m_toolsDir + "/yt-dlp.exe";
        worker->download(
            QUrl("https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe"),
            ytPath);

        connect(worker, &DownloadWorker::progress, m_progressBar, [this](int p){
            m_progressBar->setRange(0, 100);
            m_progressBar->setValue(p);
        });
        connect(worker, &DownloadWorker::finished, this, [this, worker, ytPath](bool ok, const QString &err){
            if (ok) {
                m_ytdlpPath = ytPath;
                logMsg("yt-dlp.exe downloaded.", "#58a6ff");
                downloadFfmpegWindows();
            } else {
                logMsg("Failed to download yt-dlp: " + err, "#f85149");
                setStatus(L().statusError, "#f85149");
            }
            worker->deleteLater();
        });
        worker->start();
    }

    void downloadFfmpegWindows() {
        QString zipPath = m_toolsDir + "/ffmpeg.zip";
        auto *worker = new DownloadWorker(this);
        worker->download(
            QUrl("https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl.zip"),
            zipPath);

        connect(worker, &DownloadWorker::progress, m_progressBar, [this](int p){
            m_progressBar->setRange(0, 100);
            m_progressBar->setValue(p);
        });
        connect(worker, &DownloadWorker::finished, this, [this, worker, zipPath](bool ok, const QString &err){
            if (ok) {
                logMsg("ffmpeg zip downloaded. Extracting…", "#58a6ff");
                extractFfmpegZip(zipPath);
            } else {
                logMsg("Failed to download ffmpeg: " + err, "#f85149");
                setStatus(L().statusError, "#f85149");
            }
            worker->deleteLater();
        });
        worker->start();
    }

    void extractFfmpegZip(const QString &zipPath) {
        QProcess *pz = new QProcess(this);
        pz->setWorkingDirectory(m_toolsDir);
        connect(pz, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, pz, zipPath](int code, QProcess::ExitStatus){
            if (code == 0) {
                QDir dir(m_toolsDir);
                QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                for (const QString &sub : subdirs) {
                    QString candidate = m_toolsDir + "/" + sub + "/bin/ffmpeg.exe";
                    if (QFile::exists(candidate)) {
                        QFile::copy(candidate, m_toolsDir + "/ffmpeg.exe");
                        m_ffmpegPath = m_toolsDir + "/ffmpeg.exe";
                        break;
                    }
                }
                QFile::remove(zipPath);
                logMsg("ffmpeg ready.", "#3fb950");
            } else {
                logMsg("Extraction failed. Try installing 7-Zip or use the manual method.", "#f85149");
            }
            m_progressBar->setVisible(false);
            detectTools();
            pz->deleteLater();
        });
        pz->start("powershell", {"-Command",
            QString("Expand-Archive -Path '%1' -DestinationPath '%2' -Force").arg(zipPath, m_toolsDir)});
    }

    bool isVideoFormat() const {
        QString fmt = m_formatCombo->currentText();
        return fmt.startsWith("Video");
    }

    QString getAudioFormat() const {
        QString fmt = m_formatCombo->currentText();
        if (fmt.startsWith("MP3"))   return "mp3";
        if (fmt.startsWith("WAV"))   return "wav";
        if (fmt.startsWith("FLAC"))  return "flac";
        return "m4a";
    }

    QString getVideoFormatArg() const {
        QString fmt = m_formatCombo->currentText();
        if (fmt.contains("4K"))    return "bestvideo[height<=2160]+bestaudio/best";
        if (fmt.contains("1080p")) return "bestvideo[height<=1080]+bestaudio/best";
        if (fmt.contains("720p"))  return "bestvideo[height<=720]+bestaudio/best";
        if (fmt.contains("480p"))  return "bestvideo[height<=480]+bestaudio/best";
        return "bestvideo+bestaudio/best";
    }

    void startPipeline() {
        QString input = m_urlEdit->text().trimmed();
        if (input.isEmpty()) {
            QMessageBox::warning(this, "m4s d", L().urlRequired);
            return;
        }
        if (m_ytdlpPath.isEmpty() || m_ffmpegPath.isEmpty()) {
            QMessageBox::warning(this, "m4s d", L().toolsMissing);
            return;
        }

        m_running = true;
        m_startBtn->setEnabled(false);
        m_stopBtn->setEnabled(true);
        m_progressBar->setRange(0, 0);
        m_progressBar->setVisible(true);
        setStatus(L().statusRunning, "#f0883e");
        m_lastOutputFile.clear();

        QDir().mkpath(m_outputDir);

        bool isLocal = QFile::exists(input);
        if (isLocal) {
            m_localFile = input;
        }

        if (isLocal) {
            QFileInfo fi(input);
            QStringList videoExts = {"mp4","mkv","webm","avi","mov","flv","wmv","ts","m2ts"};
            bool isVideo = videoExts.contains(fi.suffix().toLower());
            if (isVideo) {
                QString tmpAudio = m_outputDir + "/m4s_tmp_audio_" +
                    QString::number(QDateTime::currentMSecsSinceEpoch()) + ".wav";
                extractAudioFromVideo(input, tmpAudio, fi.completeBaseName());
            } else {
                if (m_noMusicChk->isChecked()) {
                    runDemucs(input, fi.completeBaseName(), false);
                } else {
                    convertAudio(input, fi.completeBaseName());
                }
            }
        } else {
            if (isVideoFormat()) {
                downloadVideo(input);
            } else {
                downloadAudio(input);
            }
        }
    }

    void downloadAudio(const QString &url) {
        logMsg("Downloading audio from: " + url, "#58a6ff");
        QString tmpl = m_outputDir + "/%(title)s.%(ext)s";
        QStringList args = {
            "--no-playlist",
            "-x",
            "--audio-format", "wav",
            "--audio-quality", "0",
            "--ffmpeg-location", QFileInfo(m_ffmpegPath).absolutePath(),
            "-o", tmpl,
            "--print", "after_move:filepath",
            url
        };

        runProcess(m_ytdlpPath, args, [this](int code, const QString &out){
            if (code == 0) {
                QString fp = out.trimmed().split('\n').last().trimmed();
                if (fp.isEmpty() || !QFile::exists(fp)) {
                    QDir d(m_outputDir);
                    QStringList wavFiles = d.entryList({"*.wav"}, QDir::Files, QDir::Time);
                    if (!wavFiles.isEmpty()) fp = m_outputDir + "/" + wavFiles.first();
                }
                if (!fp.isEmpty() && QFile::exists(fp)) {
                    QFileInfo fi(fp);
                    if (m_noMusicChk->isChecked()) {
                        runDemucs(fp, fi.completeBaseName(), true);
                    } else {
                        convertAudio(fp, fi.completeBaseName());
                    }
                } else {
                    logMsg("Could not determine output file path.", "#f85149");
                    finishPipeline(false);
                }
            } else {
                finishPipeline(false);
            }
        });
    }

    void downloadVideo(const QString &url) {
        logMsg("Downloading video from: " + url, "#58a6ff");
        QString tmpl = m_outputDir + "/%(title)s.%(ext)s";
        QStringList args = {
            "--no-playlist",
            "-f", getVideoFormatArg(),
            "--merge-output-format", "mp4",
            "--ffmpeg-location", QFileInfo(m_ffmpegPath).absolutePath(),
            "-o", tmpl,
            "--print", "after_move:filepath",
            url
        };
        runProcess(m_ytdlpPath, args, [this](int code, const QString &out){
            if (code == 0) {
                QString fp = out.trimmed().split('\n').last().trimmed();
                if (!fp.isEmpty() && QFile::exists(fp)) {
                    m_lastOutputFile = fp;
                    logMsg("Video saved: " + fp, "#3fb950");
                }
                finishPipeline(code == 0);
            } else {
                finishPipeline(false);
            }
        });
    }

    void extractAudioFromVideo(const QString &videoPath, const QString &audioOut, const QString &title) {
        logMsg(L().extractingAudio, "#58a6ff");
        QStringList args = {"-y", "-i", videoPath, "-vn", "-q:a", "0", "-map", "a", audioOut};
        runProcess(m_ffmpegPath, args, [this, audioOut, videoPath, title](int code, const QString &){
            if (code == 0) {
                if (m_noMusicChk->isChecked()) {
                    runDemucs(audioOut, title, true, videoPath);
                } else {
                    convertAudio(audioOut, title, videoPath);
                }
            } else {
                finishPipeline(false);
            }
        });
    }

    void runDemucs(const QString &audioPath, const QString &title,
                   bool deleteOriginal = false, const QString &extraDelete = "") {
        if (m_demucsPath.isEmpty()) {
            logMsg("demucs not found. Install it via: pipx install demucs", "#f85149");
            finishPipeline(false);
            return;
        }
        logMsg(L().runningDemucs, "#f0883e");

        QString device = (m_deviceCombo->currentIndex() == 1) ? "cuda" : "cpu";
        int shifts = m_extremeChk->isChecked() ? 4 : 2;
        double overlap = m_extremeChk->isChecked() ? 0.25 : 0.1;

        QString demucsOutDir = m_outputDir + "/m4s_demucs_tmp_" +
            QString::number(QDateTime::currentMSecsSinceEpoch());
        QDir().mkpath(demucsOutDir);

        QStringList args = {
            "-n", "htdemucs_ft",
            "--two-stems=vocals",
            "-d", device,
            "--shifts", QString::number(shifts),
            "--overlap", QString::number(overlap),
            "--out", demucsOutDir,
            audioPath
        };

        auto doDemucs = [this, args, audioPath, title, demucsOutDir, deleteOriginal, extraDelete](bool useCuda) {
            QStringList a = args;
            if (!useCuda) {
                int di = a.indexOf("cuda");
                if (di >= 0) a[di] = "cpu";
            }
            runProcess(m_demucsPath, a, [this, audioPath, title, demucsOutDir, deleteOriginal, extraDelete]
                       (int code, const QString &){
                if (code == 0) {
                    QString vocalsWav = findVocalsFile(demucsOutDir);
                    if (vocalsWav.isEmpty()) {
                        logMsg("Could not find vocals output from demucs.", "#f85149");
                        finishPipeline(false);
                        return;
                    }
                    convertVocals(vocalsWav, title, [this, audioPath, demucsOutDir, deleteOriginal, extraDelete](){
                        if (deleteOriginal) QFile::remove(audioPath);
                        if (!extraDelete.isEmpty()) QFile::remove(extraDelete);
                        removeDir(demucsOutDir);
                        finishPipeline(true);
                    });
                } else {
                    logMsg("demucs failed. Check log for details.", "#f85149");
                    removeDir(demucsOutDir);
                    finishPipeline(false);
                }
            });
        };

        if (device == "cuda") {
            logMsg("Trying CUDA acceleration…", "#58a6ff");
            doDemucs(true);
        } else {
            doDemucs(false);
        }
    }

    QString findVocalsFile(const QString &demucsOutDir) {
        QDirIterator it(demucsOutDir, {"vocals.wav","vocals.mp3","vocals.flac"},
                        QDir::Files, QDirIterator::Subdirectories);
        if (it.hasNext()) return it.next();
        return {};
    }

    void convertVocals(const QString &vocalsWav, const QString &title,
                       std::function<void()> onDone) {
        logMsg(L().convertingFormat, "#58a6ff");
        QString ext = getAudioFormat();
        QString safeName = title;
        safeName.replace(QRegularExpression(R"([\\/:*?"<>|])"), "_");
        QString outPath = m_outputDir + "/" + safeName + " (no music)." + ext;

        QStringList args = {"-y", "-i", vocalsWav};
        if (ext == "mp3") {
            args << "-codec:a" << "libmp3lame" << "-q:a" << "2";
        } else if (ext == "m4a") {
            args << "-codec:a" << "aac" << "-b:a" << "256k";
        } else if (ext == "flac") {
            args << "-codec:a" << "flac";
        } else {
            args << "-codec:a" << "pcm_s16le";
        }
        args << outPath;

        runProcess(m_ffmpegPath, args, [this, outPath, onDone](int code, const QString &){
            if (code == 0) {
                m_lastOutputFile = outPath;
                logMsg("Saved: " + outPath, "#3fb950");
                onDone();
            } else {
                logMsg("Conversion failed.", "#f85149");
                finishPipeline(false);
            }
        });
    }

    void convertAudio(const QString &audioPath, const QString &title,
                      const QString &deleteAfter = "") {
        logMsg(L().convertingFormat, "#58a6ff");
        QString ext = getAudioFormat();
        QString safeName = title;
        safeName.replace(QRegularExpression(R"([\\/:*?"<>|])"), "_");
        QString outPath = m_outputDir + "/" + safeName + "." + ext;

        bool sameFormat = audioPath.endsWith("." + ext, Qt::CaseInsensitive);
        if (sameFormat && deleteAfter.isEmpty()) {
            QFile::rename(audioPath, outPath);
            m_lastOutputFile = outPath;
            logMsg("Saved: " + outPath, "#3fb950");
            finishPipeline(true);
            return;
        }

        QStringList args = {"-y", "-i", audioPath};
        if (ext == "mp3") {
            args << "-codec:a" << "libmp3lame" << "-q:a" << "2";
        } else if (ext == "m4a") {
            args << "-codec:a" << "aac" << "-b:a" << "256k";
        } else if (ext == "flac") {
            args << "-codec:a" << "flac";
        } else {
            args << "-codec:a" << "pcm_s16le";
        }
        args << outPath;

        runProcess(m_ffmpegPath, args, [this, audioPath, outPath, deleteAfter](int code, const QString &){
            if (code == 0) {
                if (audioPath != outPath) QFile::remove(audioPath);
                if (!deleteAfter.isEmpty()) QFile::remove(deleteAfter);
                m_lastOutputFile = outPath;
                logMsg("Saved: " + outPath, "#3fb950");
                finishPipeline(true);
            } else {
                logMsg("Conversion failed.", "#f85149");
                finishPipeline(false);
            }
        });
    }

    void removeDir(const QString &path) {
        logMsg(L().cleaningUp, "#8b949e");
        QDir(path).removeRecursively();
    }

    void finishPipeline(bool success) {
        m_running = false;
        m_startBtn->setEnabled(true);
        m_stopBtn->setEnabled(false);
        m_progressBar->setVisible(false);
        if (success) {
            logMsg(L().doneSuccess, "#3fb950");
            setStatus(L().statusDone, "#3fb950");
        } else {
            logMsg(L().errorOccurred, "#f85149");
            setStatus(L().statusError, "#f85149");
        }
    }

    void stopPipeline() {
        if (m_proc) {
            m_proc->kill();
            m_proc->waitForFinished(3000);
        }
        logMsg("Stopped by user.", "#f0883e");
        finishPipeline(false);
    }

    void runProcess(const QString &prog, const QStringList &args,
                    std::function<void(int, const QString &)> onDone) {
        if (m_proc) {
            m_proc->kill();
            m_proc->deleteLater();
            m_proc = nullptr;
        }
        m_proc = new QProcess(this);

        QStringList env = QProcess::systemEnvironment();
        QString localBin = QDir::homePath() + "/.local/bin";
        for (int i = 0; i < env.size(); ++i) {
            if (env[i].startsWith("PATH=")) {
                env[i] = "PATH=" + localBin + ":" + env[i].mid(5);
                break;
            }
        }
        m_proc->setEnvironment(env);

        QString *accumulated = new QString();

        connect(m_proc, &QProcess::readyReadStandardOutput, this, [this, accumulated](){
            QString out = m_proc->readAllStandardOutput();
            *accumulated += out;
            for (const QString &line : out.split('\n')) {
                if (!line.trimmed().isEmpty()) logMsg(line.trimmed());
            }
        });
        connect(m_proc, &QProcess::readyReadStandardError, this, [this](){
            QString err = m_proc->readAllStandardError();
            for (const QString &line : err.split('\n')) {
                if (!line.trimmed().isEmpty()) logMsg(line.trimmed(), "#f0883e");
            }
        });

        connect(m_proc, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, onDone, accumulated](int code, QProcess::ExitStatus){
            QString out = *accumulated;
            delete accumulated;
            m_proc->deleteLater();
            m_proc = nullptr;
            if (m_running || code == 0) onDone(code, out);
        });

        logMsg("$ " + prog + " " + args.join(' '), "#58a6ff");
        m_proc->start(prog, args);
        if (!m_proc->waitForStarted(5000)) {
            logMsg("Failed to start: " + prog, "#f85149");
            delete accumulated;
            m_proc->deleteLater();
            m_proc = nullptr;
            onDone(-1, "");
        }
    }
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("m4s_d");
    app.setOrganizationName("m4s");
    app.setApplicationVersion("2.0.0");

    QFontDatabase::addApplicationFont(":/fonts/segoeui.ttf");

    MainWindow win;
    win.show();
    return app.exec();
}
