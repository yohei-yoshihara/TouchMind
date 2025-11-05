#include "pch.h"
#include "touchmind/model/node/NodeModel.h"

using namespace touchmind::model;
using namespace touchmind::model::node;
using namespace touchmind::text;

TEST(TestCaseName, TestName) {
  auto node = std::make_shared<touchmind::model::node::NodeModel>();
  node->SetId(1);
  EXPECT_EQ(node->GetId(), 1);
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}
