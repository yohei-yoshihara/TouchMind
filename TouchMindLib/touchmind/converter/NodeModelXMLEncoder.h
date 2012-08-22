#ifndef TOUCHMIND_MODEL_NODEMODELXMLENCODER_H_
#define TOUCHMIND_MODEL_NODEMODELXMLENCODER_H_

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
class NodeModelXMLEncoder
{
public:
    NodeModelXMLEncoder(void);
    ~NodeModelXMLEncoder(void);

    HRESULT Encode(
        IN std::shared_ptr<touchmind::model::node::NodeModel> node,
        IN MSXML::IXMLDOMDocumentPtr pXMLDoc,
        OUT MSXML::IXMLDOMElementPtr &pElement);
};
} // converter
} // touchmind

#endif // TOUCHMIND_MODEL_NODEMODELXMLENCODER_H_
