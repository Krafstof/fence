#include "zaborwidget.h"
#include <QPainter>
#include <QElapsedTimer>

ZaborWidget::ZaborWidget(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
}

void ZaborWidget::rebuildMessageCache(Message &m)
{
    if (m.layout)
    {
        delete m.layout;
        m.layout = nullptr;
    }

    QFont font(m.fontName);
    font.setPointSizeF(m.fontSize);

    m.layout = new QTextLayout(m.text, font);
    QTextOption option;
    m.layout->setTextOption(option);

    m.layout->beginLayout();

    qreal maxWidth = width();
    qreal height = 0;

    while (true) {
        QTextLine line = m.layout->createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(maxWidth*2);
        line.setPosition(QPointF(0, height));
        height += line.height();
    }
    m.layout->endLayout();

    m.bounds = QRect(m.x, m.y, maxWidth, height);
}

void ZaborWidget::ensureBackbuffer()
{
    if (m_backbuffer.size() != size())
    {
        m_backbuffer = QImage(size(), QImage::Format_ARGB32_Premultiplied);
        m_backbuffer.fill(Qt::white);
        redrawRegion(rect());
    }
}

void ZaborWidget::redrawRegion(const QRect &r)
{
    if (m_backbuffer.isNull())
        return;

    QPainter p(&m_backbuffer);
    p.setRenderHint(QPainter::Antialiasing);

    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(r, Qt::white);

    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    for (const Message &m : m_messages)
    {
        if (!m.bounds.intersects(r))
            continue;

        p.save();
        p.translate(m.x, m.y);
        p.setPen(m.color);

        for (int i = 0; i < m.layout->lineCount(); ++i)
        {
            QTextLine line = m.layout->lineAt(i);
            line.draw(&p, QPointF(0, line.y()));
        }
        p.restore();
    }
}

void ZaborWidget::paintEventOld(QPainter &p, QPaintEvent *event)
{
    for (const auto &m : m_messages)
    {
        QFont font(m.fontName);
        font.setPointSizeF(m.fontSize);
        p.setFont(font);
        p.setPen(m.color);

        QFontMetrics fm(font);
        QRect r = fm.boundingRect(QRect(), Qt::TextWordWrap, m.text);
        r.moveTo(m.x, m.y);

        if (!event->region().intersects(r))
            continue;

        p.drawText(m.x, m.y, m.text);
    }
}

void ZaborWidget::addOrUpdateMessage(const Message &msg)
{
    QRect oldBounds;

    if (useOptimized == false) {
        int idx;
        if (m_authorIndex.contains(msg.author))
        {
            idx = m_authorIndex[msg.author];
            m_messages[idx] = msg;
        }
        else
        {
            m_authorIndex[msg.author] = m_messages.size();
            m_messages.append(msg);
        }
        update();
        return;
    }

    int idx;
    if (m_authorIndex.contains(msg.author))
    {
        idx = m_authorIndex[msg.author];
        oldBounds = m_messages[idx].bounds;
        m_messages[idx] = msg;
        rebuildMessageCache(m_messages[idx]);
    }
    else
    {
        m_authorIndex[msg.author] = m_messages.size();
        m_messages.append(msg);
        rebuildMessageCache(m_messages.last());
        idx = m_messages.size() - 1;
    }

    QRect dirty = oldBounds.united(m_messages[idx].bounds).adjusted(-2, -2, 2, 2);
    ensureBackbuffer();
    redrawRegion(dirty);
    update(dirty);
}

void ZaborWidget::clear()
{
    for (Message &m : m_messages)
        delete m.layout;

    m_messages.clear();
    m_authorIndex.clear();

    if (useOptimized)
    {
        ensureBackbuffer();
        m_backbuffer.fill(Qt::white);
    }

    update();
}

void ZaborWidget::paintEvent(QPaintEvent *event)
{
    if (!useOptimized)
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        paintEventOld(p, event);
        return;
    }

    ensureBackbuffer();

    QPainter p(this);
    for (const QRect& r : event->region())
        p.drawImage(r, m_backbuffer, r);
}

void ZaborWidget::resizeEvent(QResizeEvent *event)
{
    if (useOptimized)
        ensureBackbuffer();
    QWidget::resizeEvent(event);
}

bool ZaborWidget::hasMessage(const QString &author) const
{
    return m_authorIndex.contains(author);
}

const Message* ZaborWidget::getMessage(const QString &author) const
{
    if (!m_authorIndex.contains(author))
        return nullptr;
    return &m_messages[m_authorIndex[author]];
}

QRect ZaborWidget::getMessageRect(const QString &author) const
{
    if (!m_authorIndex.contains(author))
        return QRect();

    const Message& m = m_messages[m_authorIndex[author]];

    QRect rect = m.bounds;

    rect.moveTo(m.x, m.y);

    return rect;
}


