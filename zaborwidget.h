#ifndef ZABORWIDGET_H
#define ZABORWIDGET_H

#include <QWidget>
#include <QVector>
#include <QHash>
#include <QImage>
#include <QTextLayout>

struct Message {
    QString author;
    QString text;
    QString fontName;
    double fontSize;
    int x, y;
    QColor color;

    QRect bounds;
    QTextLayout *layout = nullptr;
};

class ZaborWidget : public QWidget {
    Q_OBJECT
public:
    explicit ZaborWidget(QWidget *parent = nullptr);

    void addOrUpdateMessage(const Message &msg);
    void clear();
    const QVector<Message>& messages() const { return m_messages; }

    bool hasMessage(const QString& author) const;
    const Message* getMessage(const QString& author) const;
    QRect getMessageRect(const QString& author) const;

    // переключатель старой/новой версии
    bool useOptimized = true;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QVector<Message> m_messages;
    QHash<QString,int> m_authorIndex;

    QImage m_backbuffer;

    void rebuildMessageCache(Message &m);
    void ensureBackbuffer();
    void redrawRegion(const QRect &r);

    void paintEventOld(QPainter &p, QPaintEvent *event);
};
#endif // ZABORWIDGET_H
