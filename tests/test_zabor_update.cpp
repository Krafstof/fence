#include <gtest/gtest.h>
#include "../zaborwidget.h"

TEST(ZaborWidgetTests, UpdateMessage)
{
    ZaborWidget widget;

    Message m;
    m.author = "Alice";
    m.text = "Hello";
    widget.addOrUpdateMessage(m);

    const Message* before = widget.getMessage("Alice");
    ASSERT_NE(before, nullptr);
    EXPECT_EQ(before->text, "Hello");

    m.text = "Updated text";
    widget.addOrUpdateMessage(m);

    const Message* after = widget.getMessage("Alice");
    ASSERT_NE(after, nullptr);
    EXPECT_EQ(after->text, "Updated text");
}
