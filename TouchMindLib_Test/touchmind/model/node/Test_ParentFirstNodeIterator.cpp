#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/node/iterator/ParentFirstNodeIterator.h"

using namespace touchmind::model::node;

void assertTreeParentFirst(NodeModel::reference parent)
{
    LARGE_INTEGER liBegin, liEnd, liFreq;
    QueryPerformanceFrequency(&liFreq);

    std::vector<touchmind::NODE_ID> v1;
    QueryPerformanceCounter(&liBegin);
    std::for_each(parent->Begin_ParentFirst(), parent->End_ParentFirst(), [&v1](NodeModel::reference e) {
        v1.push_back(e->GetId());
    });
    QueryPerformanceCounter(&liEnd);
    double time1 = (double)(liEnd.QuadPart - liBegin.QuadPart) / (double)liFreq.QuadPart * 1000.0;
    std::wcout << L"iterator = " << time1;

    std::vector<touchmind::NODE_ID> v2;
    QueryPerformanceCounter(&liBegin);
    parent->ApplyVisitor([&v2](NodeModel::reference e)->touchmind::VISITOR_RESULT {
        v2.push_back(e->GetId());
        return touchmind::VISITOR_RESULT_CONTINUE;
    }, true);
    QueryPerformanceCounter(&liEnd);
    double time2 = (double)(liEnd.QuadPart - liBegin.QuadPart) / (double)liFreq.QuadPart * 1000.0;
    std::wcout << L", visitor = " << time2;
    std::wcout << L", difference = " << (time1 / time2) << std::endl;

    ASSERT_EQ(v2.size(), v1.size());
    ASSERT_TRUE(std::equal(std::begin(v1), std::end(v1), std::begin(v2)));
}

// operator++

TEST(touchmind_model_node_ParentFirstNodeIterator, plus_rootOnly_001)
{
    std::shared_ptr<NodeModel> node(std::make_shared<NodeModel>());
    node->SetId(100);

    auto begin_it = node->Begin_ParentFirst();
    auto end_it = node->End_ParentFirst();

    ASSERT_EQ(100, (*begin_it)->GetId());
    ASSERT_FALSE(begin_it == end_it);
    auto it = ++begin_it;
    ASSERT_TRUE(begin_it == end_it);
    ASSERT_TRUE(it == end_it);
    it = ++begin_it;
    ASSERT_TRUE(begin_it == end_it);
    ASSERT_TRUE(it == end_it);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, plus_rootOnly_002)
{
    std::shared_ptr<NodeModel> node(std::make_shared<NodeModel>());
    node->SetId(100);

    auto begin_it = node->Begin_ParentFirst();
    auto end_it = node->End_ParentFirst();

    ASSERT_EQ(100, (*begin_it)->GetId());
    ASSERT_FALSE(begin_it == end_it);
    auto it = begin_it++;
    ASSERT_TRUE(begin_it == end_it);
    ASSERT_FALSE(it == end_it);
    it = begin_it++;
    ASSERT_TRUE(begin_it == end_it);
    ASSERT_TRUE(it == end_it);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, plus_oneChild_001)
{
    std::shared_ptr<NodeModel> parent(std::make_shared<NodeModel>());
    parent->SetId(1);
    std::shared_ptr<NodeModel> child(std::make_shared<NodeModel>());
    child->SetId(2);
    parent->AddChild(child);

    assertTreeParentFirst(parent);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, plus_twoChild_001)
{
    std::shared_ptr<NodeModel> parent(std::make_shared<NodeModel>());
    parent->SetId(1);
    std::shared_ptr<NodeModel> child1(std::make_shared<NodeModel>());
    child1->SetId(2);
    parent->AddChild(child1);
    std::shared_ptr<NodeModel> child2(std::make_shared<NodeModel>());
    child2->SetId(3);
    parent->AddChild(child2);

    assertTreeParentFirst(parent);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, plus_grandchild_001)
{
    std::shared_ptr<NodeModel> parent(std::make_shared<NodeModel>());
    parent->SetId(1);
    std::shared_ptr<NodeModel> child1(std::make_shared<NodeModel>());
    child1->SetId(2);
    parent->AddChild(child1);
    std::shared_ptr<NodeModel> child2(std::make_shared<NodeModel>());
    child2->SetId(3);
    child1->AddChild(child2);

    assertTreeParentFirst(parent);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, plus_two_children_have_two_children_001)
{
    std::shared_ptr<NodeModel> parent(std::make_shared<NodeModel>());
    parent->SetId(0);
    std::shared_ptr<NodeModel> child1(std::make_shared<NodeModel>());
    child1->SetId(1);
    parent->AddChild(child1);
    std::shared_ptr<NodeModel> grandchild11(std::make_shared<NodeModel>());
    grandchild11->SetId(11);
    child1->AddChild(grandchild11);
    std::shared_ptr<NodeModel> grandchild12(std::make_shared<NodeModel>());
    grandchild12->SetId(12);
    child1->AddChild(grandchild12);

    std::shared_ptr<NodeModel> child2(std::make_shared<NodeModel>());
    child2->SetId(2);
    parent->AddChild(child2);
    std::shared_ptr<NodeModel> grandchild21(std::make_shared<NodeModel>());
    grandchild21->SetId(21);
    child2->AddChild(grandchild21);
    std::shared_ptr<NodeModel> grandchild22(std::make_shared<NodeModel>());
    grandchild22->SetId(22);
    child2->AddChild(grandchild22);

    assertTreeParentFirst(parent);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, plus_two_children_have_two_children_002)
{
    std::shared_ptr<NodeModel> parent(std::make_shared<NodeModel>());
    parent->SetId(0);
    std::shared_ptr<NodeModel> child1(std::make_shared<NodeModel>());
    child1->SetId(1);
    parent->AddChild(child1);
    std::shared_ptr<NodeModel> grandchild11(std::make_shared<NodeModel>());
    grandchild11->SetId(11);
    child1->AddChild(grandchild11);
    std::shared_ptr<NodeModel> grandchild12(std::make_shared<NodeModel>());
    grandchild12->SetId(12);
    child1->AddChild(grandchild12);

    std::shared_ptr<NodeModel> child2(std::make_shared<NodeModel>());
    child2->SetId(2);
    parent->AddChild(child2);
    std::shared_ptr<NodeModel> grandchild21(std::make_shared<NodeModel>());
    grandchild21->SetId(21);
    child2->AddChild(grandchild21);
    std::shared_ptr<NodeModel> grandchild22(std::make_shared<NodeModel>());
    grandchild22->SetId(22);
    child2->AddChild(grandchild22);

    assertTreeParentFirst(parent);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, pattern_001)
{
    std::shared_ptr<NodeModel> parent(std::make_shared<NodeModel>());
    parent->SetId(0);
    std::shared_ptr<NodeModel> child1(std::make_shared<NodeModel>());
    child1->SetId(1);
    parent->AddChild(child1);
    std::shared_ptr<NodeModel> grandchild11(std::make_shared<NodeModel>());
    grandchild11->SetId(11);
    child1->AddChild(grandchild11);

    std::shared_ptr<NodeModel> child2(std::make_shared<NodeModel>());
    child2->SetId(2);
    parent->AddChild(child2);
    std::shared_ptr<NodeModel> grandchild21(std::make_shared<NodeModel>());
    grandchild21->SetId(21);
    child2->AddChild(grandchild21);
    std::shared_ptr<NodeModel> grandchild22(std::make_shared<NodeModel>());
    grandchild22->SetId(22);
    child2->AddChild(grandchild22);

    assertTreeParentFirst(parent);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, pattern_002)
{
    std::shared_ptr<NodeModel> parent(std::make_shared<NodeModel>());
    parent->SetId(0);
    std::shared_ptr<NodeModel> child1(std::make_shared<NodeModel>());
    child1->SetId(1);
    parent->AddChild(child1);
    std::shared_ptr<NodeModel> grandchild11(std::make_shared<NodeModel>());
    grandchild11->SetId(11);
    child1->AddChild(grandchild11);

    std::shared_ptr<NodeModel> child2(std::make_shared<NodeModel>());
    child2->SetId(2);
    parent->AddChild(child2);
    std::shared_ptr<NodeModel> grandchild21(std::make_shared<NodeModel>());
    grandchild21->SetId(21);
    child2->AddChild(grandchild21);
    std::shared_ptr<NodeModel> grandchild22(std::make_shared<NodeModel>());
    grandchild22->SetId(22);
    child2->AddChild(grandchild22);
    std::shared_ptr<NodeModel> grandgrandchild221(std::make_shared<NodeModel>());
    grandgrandchild221->SetId(221);
    grandchild22->AddChild(grandgrandchild221);

    assertTreeParentFirst(parent);
}

TEST(touchmind_model_node_ParentFirstNodeIterator, random_001)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

#ifdef _DEBUG
    for (size_t loop = 0; loop < 30; ++loop) {
#else
    for (size_t loop = 0; loop < 300; ++loop) {
#endif
        std::shared_ptr<NodeModel> root(std::make_shared<NodeModel>());
        root->SetId(0);
        auto current = root;
#ifdef _DEBUG
        for (size_t i = 0; i < static_cast<size_t>(std::rand() % 50 + 50); ++i) {
#else
        for (size_t i = 0; i < static_cast<size_t>(std::rand() % 500 + 500); ++i) {
#endif
            switch (std::rand() % 2) {
            case 0: {
                for (size_t j = 0; j < static_cast<size_t>(std::rand() % 10); ++j) {
                    auto parent = current->GetParent();
                    if (parent != nullptr) {
                        current = parent;
                    } else {
                        break;
                    }
                }
            }
            case 1: {
                auto node = std::make_shared<NodeModel>();
                node->SetId(static_cast<touchmind::NODE_ID>(i));
                current->AddChild(node);
                current = node;
            }
            break;
            }
        }
        /*
        wchar_t buf[4096];
        wsprintf(buf, L"c:\\temp\\ParentFirst_%d.txt", loop);
        std::wofstream of(buf);
        root->DumpAll(of);
        of.close();
        */
        assertTreeParentFirst(root);
    }
}