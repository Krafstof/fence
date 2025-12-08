#include <gtest/gtest.h>
#include "../zaborwidget.h"

TEST(ZaborWidgetTests, MessageStartsInsideWidget)
{
    ZaborWidget widget;
    widget.resize(300, 200);

    Message m;
    m.author = "Alice";
    m.text = "Long long long message";
    m.x = 280;
    m.y = 50;

    widget.addOrUpdateMessage(m);

    QRect r = widget.getMessageRect("Alice");

    EXPECT_LT(r.left(), widget.width());
}

