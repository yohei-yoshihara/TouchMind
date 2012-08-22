#ifndef TOUCHMIND_MODEL_NODEMODELXMLDECODER_H_
#define TOUCHMIND_MODEL_NODEMODELXMLDECODER_H_

#include "forwarddecl.h"

#ifndef __RENAME_MSXML
#define __RENAME_MSXML
#import "MSXML6.dll" rename_namespace(_T("MSXML"))
#endif
#include <msxml6.h>

namespace touchmind
{
namespace converter
{

class NodeModelXMLDecoder
{
private:
    struct LinkInfo {
        NODE_ID start;
        NODE_ID end;
        std::shared_ptr<model::link::LinkModel> link;
    };
    typedef std::vector<LinkInfo> LinkInfoList;
    touchmind::selection::SelectionManager *m_pSelectionManager;
    touchmind::model::MapModel *m_pMapModel;

protected:
    HRESULT _DecodeText(
        IN MSXML::IXMLDOMNodePtr pTextNode,
        OUT std::shared_ptr<touchmind::model::node::NodeModel> &node);
    HRESULT _DecodePath(
        IN MSXML::IXMLDOMNodePtr xmlPathNode,
        OUT std::shared_ptr<touchmind::model::node::NodeModel> &node );
    HRESULT _DecodeFontAttribute(
        IN MSXML::IXMLDOMNodePtr pFontAttributeNode,
        OUT std::shared_ptr<touchmind::model::node::NodeModel> &node);
    HRESULT _DecodeLink(
        IN MSXML::IXMLDOMNodePtr xmlLinkNode,
        OUT std::shared_ptr<touchmind::model::node::NodeModel> &node,
        std::shared_ptr<LinkInfoList> linkInfoList);

public:
    NodeModelXMLDecoder();
    virtual ~NodeModelXMLDecoder(void);
    void SetSelectionManager(touchmind::selection::SelectionManager *pSelectionManager) {
        m_pSelectionManager = pSelectionManager;
    }
    void SetMapModel(touchmind::model::MapModel *pMapModel) {
        m_pMapModel = pMapModel;
    }
    HRESULT Decode(
        IN MSXML::IXMLDOMNodePtr pElement,
        OUT std::shared_ptr<touchmind::model::node::NodeModel> &node,
        OUT std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> &links,
        IN bool keepOriginalId,
        IN bool keepOriginalPosition,
        std::shared_ptr<LinkInfoList> linkInfoList = nullptr,
        std::shared_ptr<std::map<NODE_ID, NODE_ID>> idMap = nullptr);
};

} // converter
} // touchmind

#endif // TOUCHMIND_MODEL_NODEMODELXMLDECODER_H_