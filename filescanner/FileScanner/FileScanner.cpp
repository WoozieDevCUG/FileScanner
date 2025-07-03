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

    // ɨ���ļ�
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo& fileInfo : files) {
        if (m_stopRequested.load()) return;

        qint64 size = fileInfo.size();
        QString filePath = fileInfo.absoluteFilePath();

        {
            QMutexLocker locker(&m_mutex);
            m_results.append(qMakePair(filePath, size));
        }

        // Qt �ź�֪ͨ
        emit progressUpdated(filePath, size);

        //  C ���ص�֪ͨ
        if (m_progressCB) {
            m_progressCB(filePath.toUtf8().constData(), size, m_userData);
        }
    }

    // �ݹ�ɨ����Ŀ¼
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& dirInfo : dirs) {
        if (m_stopRequested.load()) return;
        scanDirectory(dirInfo.absoluteFilePath());
    }

    //  ����ɨ����ɵĻص������������ٴ�����
    if (m_completeCB) {
        QVector<QPair<QString, qint64>> resultCopy;
        {
            QMutexLocker locker(&m_mutex);
            resultCopy = m_results;
        }

        m_completeCB(&resultCopy, m_userData);
    }

    // ���� Qt �źţ�ȷ��ֻ���������ò� emit��
    if (!m_stopRequested.load()) {
        emit scanCompleted(m_results);
    }
}


// DLL��������ʵ��
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