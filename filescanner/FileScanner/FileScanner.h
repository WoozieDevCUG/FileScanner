#ifndef FILESCANNER_H
#define FILESCANNER_H

#include <QtCore>
#include <functional>
#include <atomic>

typedef void (*ProgressCallback)(const char* file, qint64 size, void* userData);
typedef void (*CompleteCallback)(const QVector<QPair<QString, qint64>>* results, void* userData);

#ifdef FILESCANNER_LIBRARY
#define FILESCANNER_EXPORT Q_DECL_EXPORT
#else
#define FILESCANNER_EXPORT Q_DECL_IMPORT
#endif

class FILESCANNER_EXPORT FileScanner : public QObject
{
    Q_OBJECT
public:
    explicit FileScanner(QObject* parent = nullptr);
    ~FileScanner() override;  // 明确标记 override
    // 添加回调设置方法
    void setCallbacks(ProgressCallback progressCB,
        CompleteCallback completeCB,
        void* userData) {
        m_progressCB = progressCB;
        m_completeCB = completeCB;
        m_userData = userData;
    }

    void startScan(const QString& directory);
    void stopScan();
    QVector<QPair<QString, qint64>> getResults() const;

signals:
    void progressUpdated(const QString& file, qint64 size);
    void scanCompleted(const QVector<QPair<QString, qint64>>& results);

private:
    void scanDirectory(const QString& directory);

    std::atomic_bool m_stopRequested;
    QVector<QPair<QString, qint64>> m_results;
    mutable QMutex m_mutex;

    QThreadPool m_threadPool;
    // 添加回调成员变量
    ProgressCallback m_progressCB = nullptr;
    CompleteCallback m_completeCB = nullptr;
    void* m_userData = nullptr;
};

extern "C" {
    FILESCANNER_EXPORT FileScanner* createFileScanner(ProgressCallback progressCB,
        CompleteCallback completeCB,
        void* userData);
    FILESCANNER_EXPORT void destroyFileScanner(FileScanner* scanner);
    FILESCANNER_EXPORT void startFileScan(FileScanner* scanner, const wchar_t* directory);
    FILESCANNER_EXPORT void stopFileScan(FileScanner* scanner);
}
#endif // FILESCANNER_H