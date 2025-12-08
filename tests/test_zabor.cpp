#include <gtest/gtest.h>
#include "../zaborwidget.h"

TEST(ZaborWidgetTest, AddFirstMessage)
{
    ZaborWidget w;
    w.useOptimized = true;

    Message m;
    m.author = "test";
    m.text = "Hello";
    m.fontName = "Arial";
    m.fontSize = 12;
    m.x = 10;
    m.y = 10;
    m.color = Qt::black;

    w.addOrUpdateMessage(m);

    ASSERT_EQ(w.messages().size(), 1);
    EXPECT_EQ(w.messages()[0].text, "Hello");
}

TEST(ZaborWidgetTest, UpdateMessageByAuthor)
{
    ZaborWidget w;
    w.useOptimized = true;

    Message m;
    m.author = "u1";
    m.text = "First";
    m.fontName = "Arial";
    m.fontSize = 12;
    m.x = 10;
    m.y = 10;
    m.color = Qt::black;

    w.addOrUpdateMessage(m);

    m.text = "Updated";
    w.addOrUpdateMessage(m);

    ASSERT_EQ(w.messages().size(), 1);
    EXPECT_EQ(w.messages()[0].text, "Updated");
}
