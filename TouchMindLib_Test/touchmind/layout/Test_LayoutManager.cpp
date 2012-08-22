#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/layout/LayoutManager.h"
#include "touchmind/layout/ILayoutManager.h"
#include "touchmind/layout/ITextSizeProvider.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/selection/SelectionManager.h"
/*
using namespace touchmind;
using namespace touchmind::selection;
using namespace touchmind::model;
using namespace touchmind::model::node;
using namespace touchmind::layout;

namespace touchmind
{
namespace layout
{

class DummyTextSizeProvider : public ITextSizeProvider
{
public:
    virtual void Calculate(
        IN std::shared_ptr<NodeModel> node,
        OUT FLOAT *pWidth,
        OUT FLOAT *pHeight) {
        *pWidth = node->GetText().size() * 10.0f;
        *pHeight = 10.0f;
    }
};

class LayoutManagerTest : public ::testing::Test
{
protected:
    virtual void SetUp() {
        m_layoutManager = std::make_shared<layout::LayoutManager>();
        m_layoutManager->SetTextSizeProvider(&m_textSizeProvider);
    }

    virtual void TearDown() {
    }

    std::shared_ptr<touchmind::layout::LayoutManager> m_layoutManager;
    DummyTextSizeProvider m_textSizeProvider;
    SelectionManager m_selectionManager;
};

TEST_F( LayoutManagerTest, test001 )
{
    std::shared_ptr<NodeModel> node1(NodeModel::Create(&m_selectionManager));
    node1->SetText(L"123");
    m_layoutManager->SetModel(node1);
    m_layoutManager->Arrange();

    ASSERT_FLOAT_EQ(-20.0f, node1->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node1->GetY());
    ASSERT_FLOAT_EQ(40.0f, node1->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node1->GetHeight());

    m_layoutManager->SaveAsVML(L"c:\\temp\\test001.html");

    std::wofstream fout(L"c:\\temp\\test001.txt");
    node1->DumpAll(fout);
    fout.close();
}

TEST_F( LayoutManagerTest, test002 )
{
    std::shared_ptr<NodeModel> node1(NodeModel::Create(&m_selectionManager));
    node1->SetText(L"123");
    std::shared_ptr<NodeModel> node2(NodeModel::Create(&m_selectionManager));
    node2->SetText(L"123456");
    node1->AddChild(node2);
    m_layoutManager->SetModel(node1);
    m_layoutManager->Arrange();

    ASSERT_FLOAT_EQ(-20.0f, node1->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node1->GetY());
    ASSERT_FLOAT_EQ(40.0f, node1->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node1->GetHeight());

    ASSERT_FLOAT_EQ(60.0f, node2->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node2->GetY());
    ASSERT_FLOAT_EQ(70.0f, node2->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node2->GetHeight());

    m_layoutManager->SaveAsVML(L"c:\\temp\\test002.html");

    std::wofstream fout(L"c:\\temp\\test002.txt");
    node1->DumpAll(fout);
    fout.close();
}

TEST_F( LayoutManagerTest, test003 )
{
    std::shared_ptr<NodeModel> node1(NodeModel::Create(&m_selectionManager));
    node1->SetText(L"123");
    std::shared_ptr<NodeModel> node2(NodeModel::Create(&m_selectionManager));
    node2->SetText(L"123456");
    node1->AddChild(node2);
    std::shared_ptr<NodeModel> node3(NodeModel::Create(&m_selectionManager));
    node3->SetText(L"123456789");
    node2->AddChild(node3);
    m_layoutManager->SetModel(node1);
    m_layoutManager->Arrange();

    ASSERT_FLOAT_EQ(-20.0f, node1->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node1->GetY());
    ASSERT_FLOAT_EQ(40.0f, node1->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node1->GetHeight());

    ASSERT_FLOAT_EQ(60.0f, node2->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node2->GetY());
    ASSERT_FLOAT_EQ(70.0f, node2->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node2->GetHeight());

    ASSERT_FLOAT_EQ(170.0f, node3->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node3->GetY());
    ASSERT_FLOAT_EQ(100.0f, node3->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node3->GetHeight());

    m_layoutManager->SaveAsVML(L"c:\\temp\\test003.html");

    std::wofstream fout(L"c:\\temp\\test003.txt");
    node1->DumpAll(fout);
    fout.close();
}

TEST_F( LayoutManagerTest, test004 )
{
    std::shared_ptr<NodeModel> node1(NodeModel::Create(&m_selectionManager));
    node1->SetText(L"123");
    std::shared_ptr<NodeModel> node2(NodeModel::Create(&m_selectionManager));
    node2->SetText(L"123456");
    node1->AddChild(node2);
    std::shared_ptr<NodeModel> node3(NodeModel::Create(&m_selectionManager));
    node3->SetText(L"123456789");
    node1->AddChild(node3);
    std::shared_ptr<NodeModel> node4(NodeModel::Create(&m_selectionManager));
    node4->SetText(L"123456789012");
    node1->AddChild(node4);
    m_layoutManager->SetModel(node1);
    m_layoutManager->Arrange();

    m_layoutManager->SaveAsVML(L"c:\\temp\\test004.html");

    std::wofstream fout(L"c:\\temp\\test004.txt");
    node1->DumpAll(fout);
    fout.close();

    ASSERT_FLOAT_EQ(-20.0f, node1->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node1->GetY());
    ASSERT_FLOAT_EQ(40.0f, node1->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node1->GetHeight());

    ASSERT_FLOAT_EQ(60.0f, node2->GetX());
    ASSERT_FLOAT_EQ(-60.0f, node2->GetY());
    ASSERT_FLOAT_EQ(70.0f, node2->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node2->GetHeight());

    ASSERT_FLOAT_EQ(-160.0f, node3->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node3->GetY());
    ASSERT_FLOAT_EQ(100.0f, node3->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node3->GetHeight());

    ASSERT_FLOAT_EQ(60.0f, node4->GetX());
    ASSERT_FLOAT_EQ(40.0f, node4->GetY());
    ASSERT_FLOAT_EQ(130.0f, node4->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node4->GetHeight());
}

TEST_F( LayoutManagerTest, test005 )
{
    std::shared_ptr<NodeModel> node1(NodeModel::Create(&m_selectionManager));
    node1->SetText(L"123");
    std::shared_ptr<NodeModel> node2(NodeModel::Create(&m_selectionManager));
    node2->SetText(L"123456");
    node1->AddChild(node2);
    std::shared_ptr<NodeModel> node3(NodeModel::Create(&m_selectionManager));
    node3->SetText(L"123456789");
    node1->AddChild(node3);
    std::shared_ptr<NodeModel> node4(NodeModel::Create(&m_selectionManager));
    node4->SetText(L"123456789012");
    node1->AddChild(node4);
    std::shared_ptr<NodeModel> node5(NodeModel::Create(&m_selectionManager));
    node5->SetText(L"123456789012345");
    node1->AddChild(node5);
    m_layoutManager->SetModel(node1);
    m_layoutManager->Arrange();

    m_layoutManager->SaveAsVML(L"c:\\temp\\test005.html");

    std::wofstream fout(L"c:\\temp\\test005.txt");
    node1->DumpAll(fout);
    fout.close();

    ASSERT_FLOAT_EQ(-20.0f, node1->GetX());
    ASSERT_FLOAT_EQ(-10.0f, node1->GetY());
    ASSERT_FLOAT_EQ(40.0f, node1->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node1->GetHeight());

    ASSERT_FLOAT_EQ(60.0f, node2->GetX());
    ASSERT_FLOAT_EQ(-60.0f, node2->GetY());
    ASSERT_FLOAT_EQ(70.0f, node2->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node2->GetHeight());

    ASSERT_FLOAT_EQ(-160.0f, node3->GetX());
    ASSERT_FLOAT_EQ(-60.0f, node3->GetY());
    ASSERT_FLOAT_EQ(100.0f, node3->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node3->GetHeight());

    ASSERT_FLOAT_EQ(60.0f, node4->GetX());
    ASSERT_FLOAT_EQ(40.0f, node4->GetY());
    ASSERT_FLOAT_EQ(130.0f, node4->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node4->GetHeight());

    ASSERT_FLOAT_EQ(-220.0f, node5->GetX());
    ASSERT_FLOAT_EQ(40.0f, node5->GetY());
    ASSERT_FLOAT_EQ(160.0f, node5->GetWidth());
    ASSERT_FLOAT_EQ(20.0f, node5->GetHeight());
}

}
}
*/