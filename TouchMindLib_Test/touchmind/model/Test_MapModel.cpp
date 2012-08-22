#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/path/PathModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/operation/FileOperation.h"
#include "touchmind/selection/SelectionManager.h"

using namespace touchmind;
using namespace touchmind::model;
using namespace touchmind::model::node;
using namespace touchmind::model::link;
using namespace touchmind::operation;
using namespace touchmind::selection;

void Dump(std::shared_ptr<NodeModel> root, std::vector<std::shared_ptr<LinkModel>> links)
{
    std::wcout << L"***** DUMP START *****" << std::endl;

    root->ApplyVisitor([](const std::shared_ptr<NodeModel> &node)->VISITOR_RESULT {
        std::wcout << L"node[" << node->GetId() << L"] = " << node.get() << std::endl;
        std::wcout << L"  " << node->GetPathModel().get() << std::endl;
        return VISITOR_RESULT_CONTINUE;
    });
    for (auto it = std::begin(links); it != std::end(links); ++it) {
        std::wcout << L"link[" << (*it)->GetLinkId() << L"] = " << (*it).get() << std::endl;
        std::wcout << L"  " << (*it)->GetEdge(EDGE_ID_1).get() << std::endl;
        std::wcout << L"  " << (*it)->GetEdge(EDGE_ID_2).get() << std::endl;
    }

    std::wcout << L"***** DUMP END *****" << std::endl;
}

TEST(touchmind_model_MapModel, test_001)
{
    std::unique_ptr<operation::FileOperation> fileOperation(new operation::FileOperation());
    std::unique_ptr<SelectionManager> selectionManager(new SelectionManager());
    std::shared_ptr<MapModel> map =
        std::make_shared<MapModel>();
    map->SetSelectionManager(selectionManager.get());
    map->SetFileOperation(fileOperation.get());
    map->New();
    std::shared_ptr<NodeModel> node1 =
        NodeModel::Create(selectionManager.get());
    map->GetRootNodeModel()->AddChild(node1);
    std::shared_ptr<NodeModel> node2 =
        NodeModel::Create(selectionManager.get());
    map->GetRootNodeModel()->AddChild(node2);

    std::shared_ptr<LinkModel> link1 =
        LinkModel::Create(selectionManager.get());
    link1->SetNode(EDGE_ID_1, node1);
    link1->SetNode(EDGE_ID_2, node2);

    map->AddLink(link1);
    auto root = map->GetRootNodeModel();
    std::vector<std::shared_ptr<touchmind::model::link::LinkModel>>& links = map->_GetLinks();
    Dump(root, links);

    std::shared_ptr<NodeModel> copiedRoot;
    std::vector<std::shared_ptr<LinkModel>> copiedLinks;

    map->_MakeDeepCopy(copiedRoot, copiedLinks);
    Dump(copiedRoot, copiedLinks);
}
