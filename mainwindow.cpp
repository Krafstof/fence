#include "mainwindow.h"
#include "zaborwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), autoTimer(new QTimer(this))
{
    m_dictionary = {
        "Привет!", "Здесь был я", "Тест", "Сообщение", "Qt — круто",
        "Автомат", "Обновление", "Случайный текст", "Hello", "Hi there",
        "Был тут", "Забор", "Смотрите!", "Новое сообщение", "Всем привет"
    };

    m_fontList = {"Arial", "Courier New", "Times New Roman", "Verdana", "Helvetica"};

    setupUi();
    loadMessagesFromFile();

    connect(btnSend, &QPushButton::clicked, this, &MainWindow::onSendMessage);
    connect(btnAuto, &QPushButton::clicked, this, &MainWindow::onToggleAutoUpdate);
    connect(autoTimer, &QTimer::timeout, this, [this]()
    {
        int total = m_newPerSec + m_updPerSec;
        if (total == 0) return;

        if (m_existingAuthors.isEmpty())
        {
            generateNewMessage();
            return;
        }

        int r = QRandomGenerator::global()->bounded(total);
        if (r < m_newPerSec)
        {
            generateNewMessage();
        }
        else
        {
            updateRandomMessage();
        }
    });

    m_updatesDone = 0;
    m_updatesTimer.start();
}

void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    zaborWidget = new ZaborWidget(this);
    zaborWidget->setMinimumSize(600, 400);
    zaborWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    editAuthor = new QLineEdit;
    editText = new QLineEdit;
    editFontSize = new QLineEdit("12");
    editX = new QLineEdit("10");
    editY = new QLineEdit("10");

    comboFont = new QComboBox;
    QStringList fonts = {"Arial", "Courier New", "Times New Roman", "Verdana", "Helvetica"};
    comboFont->addItems(fonts);

    btnSend = new QPushButton("Отправить сообщение");

    editNewPerSec = new QLineEdit("0");
    editUpdatesPerSec = new QLineEdit("0");
    btnAuto = new QPushButton("Запустить автообновление");

    QFormLayout *form = new QFormLayout;
    form->addRow("Автор:", editAuthor);
    form->addRow("Текст:", editText);
    form->addRow("Шрифт:", comboFont);
    form->addRow("Размер шрифта:", editFontSize);
    form->addRow("X:", editX);
    form->addRow("Y:", editY);
    form->addWidget(btnSend);

    QHBoxLayout *autoLayout = new QHBoxLayout;
    autoLayout->addWidget(new QLabel("Новых/сек:"));
    autoLayout->addWidget(editNewPerSec);
    autoLayout->addWidget(new QLabel("Обновлений/сек:"));
    autoLayout->addWidget(editUpdatesPerSec);
    autoLayout->addWidget(btnAuto);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(zaborWidget);
    mainLayout->addLayout(form);
    mainLayout->addLayout(autoLayout);

    central->setLayout(mainLayout);
    setWindowTitle("Забор");
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveMessagesToFile();
    event->accept();
}

void MainWindow::onSendMessage()
{
    Message msg;
    msg.author = editAuthor->text().trimmed();
    msg.text = editText->text();
    msg.fontName = comboFont->currentText();
    msg.fontSize = editFontSize->text().toDouble();
    msg.x = editX->text().toInt();
    msg.y = editY->text().toInt();
    msg.color = Qt::black;

    if (msg.author.isEmpty() || msg.text.isEmpty()) return;

    zaborWidget->addOrUpdateMessage(msg);
}

void MainWindow::onToggleAutoUpdate()
{
    bool wasRunning = autoTimer->isActive();

    if (wasRunning)
    {
        autoTimer->stop();
        btnAuto->setText("Запустить автообновление");
    }
    else
    {
        bool ok1, ok2;
        m_newPerSec = editNewPerSec->text().toInt(&ok1);
        m_updPerSec = editUpdatesPerSec->text().toInt(&ok2);

        if (!ok1 || !ok2 || m_newPerSec < 0 || m_updPerSec < 0)
        {
            QMessageBox::warning(this, "Ошибка", "Введите корректные неотрицательные числа.");
            return;
        }

        int total = m_newPerSec + m_updPerSec;
        if (total == 0)
        {
            QMessageBox::information(this, "Информация", "Укажите хотя бы одно ненулевое значение.");
            return;
        }

        int intervalMs = 1000 / total;
        autoTimer->setInterval(intervalMs);
        autoTimer->start();
        btnAuto->setText("Остановить автообновление");
    }
}

void MainWindow::generateNewMessage()
{
    Message msg;
    msg.author = QString("AutoUser_%1").arg(m_nextAutoId++, 3, 10, QChar('0')); // unique ids for auto mode
    msg.text = m_dictionary[QRandomGenerator::global()->bounded(m_dictionary.size())];
    msg.fontName = m_fontList[QRandomGenerator::global()->bounded(m_fontList.size())];
    msg.fontSize = 10.0 + QRandomGenerator::global()->bounded(31);
    msg.x = QRandomGenerator::global()->bounded(qMax(1, zaborWidget->width() - 100));
    msg.y = QRandomGenerator::global()->bounded(qMax(1, zaborWidget->height() - 30));

    int r = QRandomGenerator::global()->bounded(200);
    int g = QRandomGenerator::global()->bounded(200);
    int b = QRandomGenerator::global()->bounded(200);
    msg.color = QColor(r, g, b);

    m_existingAuthors.insert(msg.author);
    zaborWidget->addOrUpdateMessage(msg);
    m_updatesDone++;

    if (m_updatesDone % 500 == 0)
    {
        double sec = m_updatesTimer.elapsed() / 1000.0;
        qDebug() << "AUTO RATE:" << (m_updatesDone / sec) << "updates/sec";
    }

}

void MainWindow::updateRandomMessage()
{
    if (m_existingAuthors.isEmpty()) return;

    int index = QRandomGenerator::global()->bounded(m_existingAuthors.size());
    auto it = m_existingAuthors.begin();
    std::advance(it, index);
    QString author = *it;

    Message msg;
    msg.author = author;
    msg.text = m_dictionary[QRandomGenerator::global()->bounded(m_dictionary.size())];
    msg.fontName = m_fontList[QRandomGenerator::global()->bounded(m_fontList.size())];
    msg.fontSize = 10.0 + QRandomGenerator::global()->bounded(31);
    msg.x = QRandomGenerator::global()->bounded(qMax(1, zaborWidget->width() - 100));
    msg.y = QRandomGenerator::global()->bounded(qMax(1, zaborWidget->height() - 30));

    int r = QRandomGenerator::global()->bounded(200);
    int g = QRandomGenerator::global()->bounded(200);
    int b = QRandomGenerator::global()->bounded(200);
    msg.color = QColor(r, g, b);

    zaborWidget->addOrUpdateMessage(msg);

    m_updatesDone++;

    if (m_updatesDone % 500 == 0)
    {
        double sec = m_updatesTimer.elapsed() / 1000.0;
        qDebug() << "AUTO RATE:" << (m_updatesDone / sec) << "updates/sec";
    }

}

void MainWindow::loadMessagesFromFile()
{
    QFile file("messages.json");
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    if (!root.contains("messages") || !root["messages"].isArray()) return;

    QJsonArray messagesArray = root["messages"].toArray();
    for (const QJsonValue &value : messagesArray)
    {
        QJsonObject msgObj = value.toObject();
        Message msg;
        msg.author = msgObj["author"].toString().trimmed();
        msg.text = msgObj["text"].toString();
        msg.fontName = msgObj["font"].toString();
        msg.fontSize = msgObj["size"].toDouble();
        msg.x = msgObj["left"].toInt();
        msg.y = msgObj["top"].toInt();

        QString colorHex = msgObj["color"].toString();
        if (colorHex.startsWith("#")) colorHex = colorHex.mid(1);
        msg.color = QColor("#" + colorHex);

        if (!msg.author.isEmpty())
        {
            zaborWidget->addOrUpdateMessage(msg);
        }
    }
}

void MainWindow::saveMessagesToFile()
{
    QJsonArray messagesArray;
    for (const Message &msg : zaborWidget->messages())
    {
        QJsonObject msgObj;
        msgObj["author"] = msg.author;
        msgObj["text"] = msg.text;
        msgObj["font"] = msg.fontName;
        msgObj["size"] = msg.fontSize;
        msgObj["left"] = msg.x;
        msgObj["top"] = msg.y;

        QString colorHex = msg.color.name(QColor::HexRgb).mid(1);
        msgObj["color"] = colorHex;

        messagesArray.append(msgObj);
    }

    QJsonObject root;
    root["messages"] = messagesArray;
    QJsonDocument doc(root);

    QFile file("messages.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(doc.toJson());
        file.close();
    }
}
