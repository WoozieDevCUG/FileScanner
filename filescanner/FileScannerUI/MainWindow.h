#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>
#include <QPair>
#include <QString>

typedef void (*CompleteCallback)(const QVector<QPair<QString, qint64>>* results, void* userData);

struct FileScanner;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:  // ע�����������public slots����֤ QMetaObject::invokeMethod ��������
    void updateProgress(const QString& file, qint64 size);
    void showResults(const QVector<QPair<QString, qint64>>& results);

private slots:
    void onBrowseClicked();
    void onStartClicked();
    void onStopClicked();

signals:
    void progressUpdated(QString file, qint64 size);
    void scanFinished(QVector<QPair<QString, qint64>> results);

private:
    void setupUI();
    void connectSignals();
    bool loadDLLFunctions();

    // UI��Ա
    QLabel* m_statusLabel = nullptr;
    QTreeWidget* m_fileTree = nullptr;
    QLineEdit* m_directoryEdit = nullptr;
    QPushButton* m_browseButton = nullptr;
    QPushButton* m_startButton = nullptr;
    QPushButton* m_stopButton = nullptr;

    // DLL��س�Ա
    void* m_dllHandle = nullptr;
    FileScanner* m_scanner = nullptr;

    // ����ָ�����ͣ�ע�����Ҫƥ��DLL��createFileScanner�Ķ���
    using ProgressCallback = void(*)(const char*, qint64, void*);
    using CreateFunc = FileScanner * (*)(ProgressCallback, CompleteCallback, void*);
    using DestroyFunc = void(*)(FileScanner*);
    using StartScanFunc = void(*)(FileScanner*, const wchar_t*);
    using StopScanFunc = void(*)(FileScanner*);

    // ����ָ��ʵ��
    CreateFunc m_createFunc = nullptr;
    DestroyFunc m_destroyFunc = nullptr;
    StartScanFunc m_startScanFunc = nullptr;
    StopScanFunc m_stopScanFunc = nullptr;
};

#endif // MAINWINDOW_H
