#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/text/FontAttribute.h"

using namespace touchmind::model;
using namespace touchmind::model::node;
using namespace touchmind::text;
/*
class Test : public ::testing::Test
{
public:
    Test() {
    }

    virtual ~Test() {
    }

    virtual void SetUp() {
        node = std::make_shared<NodeModel>();
        ASSERT_TRUE(node->IsRepaintRequired());
        ASSERT_TRUE(node->IsSaveRequired());

        node->SetRepaintRequired(false);
        node->SetSaveRequired(false);

        ASSERT_FALSE(node->IsRepaintRequired());
        ASSERT_FALSE(node->IsSaveRequired());
    }

    virtual void TearDown() {
    }

    std::shared_ptr<NodeModel> node;
};

TEST_F(Test, id_001)
{
    node->SetId(node->GetId());
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    node->SetId(1);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());
}

TEST_F(Test, position_001)
{
    node->SetPosition(node->GetPosition());
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    node->SetPosition(touchmind::NODE_SIDE_LEFT);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());
}

TEST_F(Test, createdTime_001)
{
    node->SetCreatedTime(node->GetCreatedTime());
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    SYSTEMTIME time;
    node->SetCreatedTime(time);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());
}
/*
TEST_F(Test, createdTime_002)
{
    LONGLONG t = node->GetCreatedTimeAsJavaTime();
    node->SetCreatedTime(t);
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    LONGLONG time = 0;
    node->SetCreatedTime(time);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());
}
*/
/*
TEST_F(Test, parent_001)
{
    std::shared_ptr<NodeModel> parent(std::make_shared<NodeModel>());
    node->SetParent(parent);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->SetParent(parent);
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    std::shared_ptr<NodeModel> parent1(std::make_shared<NodeModel>());
    node->SetParent(parent1);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());
}

TEST_F(Test, AddChild_001)
{
    std::shared_ptr<NodeModel> child(std::make_shared<NodeModel>());
    node->AddChild(child);
    ASSERT_EQ(1, node->GetChildrenCount());
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->AddChild(child);
    ASSERT_EQ(1, node->GetChildrenCount());
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());
}

TEST_F(Test, RemoveChild_001)
{
    std::shared_ptr<NodeModel> child(std::make_shared<NodeModel>());
    node->AddChild(child);
    ASSERT_EQ(1, node->GetChildrenCount());
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    std::shared_ptr<NodeModel> child2(std::make_shared<NodeModel>());
    node->RemoveChild(child2);
    ASSERT_EQ(1, node->GetChildrenCount());
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->RemoveChild(child);
    ASSERT_EQ(0, node->GetChildrenCount());
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());
}

TEST_F(Test, RemoveAllChildren_001)
{
    std::shared_ptr<NodeModel> child(std::make_shared<NodeModel>());
    node->AddChild(child);
    ASSERT_EQ(1, node->GetChildrenCount());
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->RemoveAllChildren();
    ASSERT_EQ(0, node->GetChildrenCount());
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());
}

TEST_F(Test, collapsed_001)
{
    std::shared_ptr<NodeModel> child1(std::make_shared<NodeModel>());
    std::shared_ptr<NodeModel> child2(std::make_shared<NodeModel>());
    node->AddChild(child1);
    child1->AddChild(child2);
    ASSERT_FALSE(child1->IsCollapsed());

    child1->SetRepaintRequired(false);
    child1->SetSaveRequired(false);

    child1->SetCollapsed(true);
    ASSERT_TRUE(child1->IsRepaintRequired());
    ASSERT_FALSE(child1->IsSaveRequired());
}

TEST_F(Test, x_001)
{
    node->SetX(1.0f);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->SetX(1.0f);
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());
}

TEST_F(Test, y_001)
{
    node->SetY(1.0f);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->SetY(1.0f);
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());
}

TEST_F(Test, width_001)
{
    node->SetWidth(1.0f);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->SetWidth(1.0f);
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());
}

TEST_F(Test, height_001)
{
    node->SetHeight(1.0f);
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->SetHeight(1.0f);
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());
}

TEST_F(Test, fontAttribute_001)
{
    node->AddFontAttribute(FontAttribute());
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());
}

TEST_F(Test, backgroundcolor_001)
{
    node->SetBackgroundColor(D2D1::ColorF(1.0, 1.0, 1.0, 0.5));
    ASSERT_TRUE(node->IsRepaintRequired());
    ASSERT_TRUE(node->IsSaveRequired());

    node->SetRepaintRequired(false);
    node->SetSaveRequired(false);

    node->SetBackgroundColor(D2D1::ColorF(1.0, 1.0, 1.0, 0.5));
    ASSERT_FALSE(node->IsRepaintRequired());
    ASSERT_FALSE(node->IsSaveRequired());
}

*/