#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

class ZaborWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onSendMessage();
    void onToggleAutoUpdate();

private:
    void setupUi();
    void loadMessagesFromFile();
    void saveMessagesToFile();

    void generateNewMessage();
    void updateRandomMessage();

    QSet<QString> m_existingAuthors;
    QStringList m_dictionary;
    QStringList m_fontList;
    int m_newPerSec = 0;
    int m_updPerSec = 0;
    int m_nextAutoId = 1;

    ZaborWidget* zaborWidget;
    QLineEdit* editAuthor;
    QLineEdit* editText;
    QComboBox* comboFont;
    QLineEdit* editFontSize;
    QLineEdit* editX;
    QLineEdit* editY;
    QPushButton* btnSend;

    QLineEdit* editNewPerSec;
    QLineEdit* editUpdatesPerSec;
    QPushButton* btnAuto;

    QTimer* autoTimer;

    QElapsedTimer m_updatesTimer;
    qint64 m_updatesDone = 0;
};

#endif // MAINWINDOW_H
