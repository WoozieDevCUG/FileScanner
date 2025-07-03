#include "FileScanner.h"
#include <QDir>
#include <QFileInfo>
#include <QtConcurrent>
#include <QMutexLocker>


FileScanner::FileScanner(QObject* parent) : QObject(parent), m_stopRequested(false)
{
    m_threadPool.setMaxThreadCount(QThread::idealThreadCount());
}

FileScanner::~FileScanner()
{
    stopScan();
    m_threadPool.waitForDone();
}

void FileScanner::startScan(const QString& directory)
{
    m_stopRequested.store(false);
    m_results.clear();

    QtConcurrent::run(&m_threadPool, [this, directory]() {
        scanDirectory(directory);
        if (!m_stopRequested.load()) {
            emit scanCompleted(m_results);
        }
        });
}

void FileScanner::stopScan()
{
    m_stopRequested.store(true);
}

QVector<QPair<QString, qint64>> FileScanner::getResults() const
{
    QMutexLocker locker(&m_mutex);
    return m_results;
}

void FileScanner::scanDirectory(const QString& directory)
{
    QDir dir(directory);
    if (!dir.exists()) return;

    // 扫描文件
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo& fileInfo : files) {
        if (m_stopRequested.load()) return;

        qint64 size = fileInfo.size();
        QString filePath = fileInfo.absoluteFilePath();

        {
            QMutexLocker locker(&m_mutex);
            m_results.append(qMakePair(filePath, size));
        }

        // Qt 信号通知
        emit progressUpdated(filePath, size);

        //  C 风格回调通知
        if (m_progressCB) {
            m_progressCB(filePath.toUtf8().constData(), size, m_userData);
        }
    }

    // 递归扫描子目录
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& dirInfo : dirs) {
        if (m_stopRequested.load()) return;
        scanDirectory(dirInfo.absoluteFilePath());
    }

    //  调用扫描完成的回调（深拷贝出结果再传出）
    if (m_completeCB) {
        QVector<QPair<QString, qint64>> resultCopy;
        {
            QMutexLocker locker(&m_mutex);
            resultCopy = m_results;
        }

        m_completeCB(&resultCopy, m_userData);
    }

    // 发送 Qt 信号（确保只有最外层调用才 emit）
    if (!m_stopRequested.load()) {
        emit scanCompleted(m_results);
    }
}


// DLL导出函数实现
// FileScanner.cpp
FileScanner* createFileScanner(ProgressCallback progressCB,
    CompleteCallback completeCB,
    void* userData) {
    FileScanner* scanner = new FileScanner();
    scanner->setCallbacks(progressCB, completeCB, userData);
    return scanner;
}

void destroyFileScanner(FileScanner* scanner)
{
    delete scanner;
}

void startFileScan(FileScanner* scanner, const wchar_t* directory)
{
    if (scanner) {
        scanner->startScan(QString::fromWCharArray(directory));
    }
}

void stopFileScan(FileScanner* scanner)
{
    if (scanner) {
        scanner->stopScan();
    }
}