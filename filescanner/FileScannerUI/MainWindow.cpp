#include "MainWindow.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <windows.h>
#include <QVector>
#include <QPair>
#include <QString>
#include <QMetaType>

//Q_DECLARE_METATYPE(QPair<QString, qint64>)
//Q_DECLARE_METATYPE(QVector<QPair<QString, qint64>>)

// MainWindow.cpp
namespace {
    static void onProgressUpdate(const char* file, qint64 size, void* userData) {
        MainWindow* window = static_cast<MainWindow*>(userData);
        emit window->progressUpdated(QString::fromUtf8(file), size);  // 直接发射信号
    }

    static void onScanComplete(const QVector<QPair<QString, qint64>>* results, void* userData) {
        MainWindow* window = static_cast<MainWindow*>(userData);
        emit window->scanFinished(*results);  // 解引用指针，再 emit
    }
}


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // 连接信号槽（不再需要元类型注册）
    connect(this, &MainWindow::progressUpdated, this, &MainWindow::updateProgress);
    connect(this, &MainWindow::scanFinished, this, &MainWindow::showResults);
    //// 注册元类型，放在构造函数，调用一次即可
    //qRegisterMetaType<QPair<QString, qint64>>("QPair<QString, qint64>");
    //qRegisterMetaType<QVector<QPair<QString, qint64>>>("QVector<QPair<QString, qint64>>");

    if (!loadDLLFunctions()) {
        QMessageBox::critical(this, "Error", "Failed to load DLL functions");
        return;
    }

    m_scanner = m_createFunc(&onProgressUpdate, &onScanComplete, this);
    if (!m_scanner) {
        QMessageBox::critical(this, "Error", "Failed to create scanner instance");
        return;
    }

    setupUI();
    connectSignals();
}

MainWindow::~MainWindow()
{
    if (m_scanner && m_destroyFunc) {
        m_destroyFunc(m_scanner);
    }
    if (m_dllHandle) {
        FreeLibrary(static_cast<HMODULE>(m_dllHandle));
    }
}

bool MainWindow::loadDLLFunctions()
{
    m_dllHandle = LoadLibraryW(L"FileScanner.dll");
    if (!m_dllHandle) {
        return false;
    }

    m_createFunc = reinterpret_cast<CreateFunc>(
        GetProcAddress(static_cast<HMODULE>(m_dllHandle), "createFileScanner"));
    m_destroyFunc = reinterpret_cast<DestroyFunc>(
        GetProcAddress(static_cast<HMODULE>(m_dllHandle), "destroyFileScanner"));
    m_startScanFunc = reinterpret_cast<StartScanFunc>(
        GetProcAddress(static_cast<HMODULE>(m_dllHandle), "startFileScan"));
    m_stopScanFunc = reinterpret_cast<StopScanFunc>(
        GetProcAddress(static_cast<HMODULE>(m_dllHandle), "stopFileScan"));

    return m_createFunc && m_destroyFunc && m_startScanFunc && m_stopScanFunc;
}

void MainWindow::setupUI()
{
    setWindowTitle("File Scanner");
    resize(800, 600);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Directory selection section
    QGroupBox* dirGroup = new QGroupBox("Scan Directory", centralWidget);
    QHBoxLayout* dirLayout = new QHBoxLayout(dirGroup);

    m_directoryEdit = new QLineEdit(dirGroup);
    m_browseButton = new QPushButton("Browse...", dirGroup);
    dirLayout->addWidget(m_directoryEdit);
    dirLayout->addWidget(m_browseButton);

    // Buttons section
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_startButton = new QPushButton("Start Scan", centralWidget);
    m_stopButton = new QPushButton("Stop Scan", centralWidget);
    m_stopButton->setEnabled(false);
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_stopButton);

    // File list
    m_fileTree = new QTreeWidget(centralWidget);
    m_fileTree->setHeaderLabels({ "File Path", "Size (bytes)" });
    m_fileTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_fileTree->setSortingEnabled(true);

    // Status bar
    m_statusLabel = new QLabel("Ready", centralWidget);

    // Assemble main layout
    mainLayout->addWidget(dirGroup);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_fileTree);
    mainLayout->addWidget(m_statusLabel);

    setCentralWidget(centralWidget);
}

void MainWindow::connectSignals()
{
    connect(m_browseButton, &QPushButton::clicked, this, &MainWindow::onBrowseClicked);
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
}

void MainWindow::onBrowseClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Scan Directory",
        m_directoryEdit->text(),
        QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        m_directoryEdit->setText(dir);
    }
}

void MainWindow::onStartClicked()
{
    if (m_directoryEdit->text().isEmpty()) {
        m_statusLabel->setText("Please select directory first");
        return;
    }

    m_fileTree->clear();
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_statusLabel->setText("Scanning...");

    std::wstring wdir = m_directoryEdit->text().toStdWString();
    m_startScanFunc(m_scanner, wdir.c_str());
}

void MainWindow::onStopClicked()
{
    m_stopScanFunc(m_scanner);
    m_statusLabel->setText("Scan stopped");
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
}

void MainWindow::updateProgress(const QString& file, qint64 size)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(m_fileTree);
    item->setText(0, file);
    item->setText(1, QString::number(size));
    m_statusLabel->setText(QString("Scanning: %1").arg(file));
}

void MainWindow::showResults(const QVector<QPair<QString, qint64>>& results)
{
    qint64 totalSize = 0;
    for (const auto& pair : results) {
        QTreeWidgetItem* item = new QTreeWidgetItem(m_fileTree);
        item->setText(0, pair.first);
        item->setText(1, QString::number(pair.second));
        totalSize += pair.second;
    }

    m_statusLabel->setText(QString("Scan completed! Total %1 files, size: %2 bytes")
        .arg(results.count()).arg(totalSize));
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
}
