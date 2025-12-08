#include <gtest/gtest.h>
#include "../zaborwidget.h"

TEST(ZaborWidgetTests, AddMessage)
{
    ZaborWidget widget;

    Message m;
    m.author = "Alice";
    m.text = "Hello test";

    widget.addOrUpdateMessage(m);

    EXPECT_TRUE(widget.hasMessage("Alice"));

    const Message* stored = widget.getMessage("Alice");
    ASSERT_NE(stored, nullptr);
    EXPECT_EQ(stored->text, "Hello test");
}
