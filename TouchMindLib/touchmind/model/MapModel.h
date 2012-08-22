#ifndef TOUCHMIND_MODEL_MAPMODEL_H_
#define TOUCHMIND_MODEL_MAPMODEL_H_

#include "forwarddecl.h"

#ifndef __RENAME_MSXML
#define __RENAME_MSXML
#import "MSXML6.dll" rename_namespace(_T("MSXML"))
#endif
#include <msxml6.h>

namespace touchmind
{
namespace model
{

enum MapModelIOResult {
    MapModelIOResult_OK,
    MapModelIOResult_FileNotFound,
    MapModelIOResult_LoadFailed,
    MapModelIOResult_SaveFailed,
    MapModelIOResult_FileIOError,
    MapModelIOResult_FileFormatError,
    MapModelIOResult_InternalError
};

class IMapModelChangeListener
{
public:
    virtual void BeforeNew(MapModel *pMapModel) = 0;
    virtual void AfterNew(MapModel *pMapModel) = 0;
    virtual void BeforeOpen(MapModel *pMapModel) = 0;
    virtual void AfterOpen(MapModel *pMapModel) = 0;
    virtual void BeforeSave(MapModel *pMapModel) = 0;
    virtual void AfterSave(MapModel *pMapModel) = 0;
};

class MapModel
{
public:
    static std::wstring s_defaultRootNodeText;

private:
    class UndoContainer
    {
    public:
        UndoContainer() : root(), links() {}
        UndoContainer(
            std::shared_ptr<touchmind::model::node::NodeModel> _root,
            std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> _links) :
            root(_root), links(_links)
        {}
        std::shared_ptr<touchmind::model::node::NodeModel> root;
        std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> links;
    };

    bool m_initialized;
    std::wstring m_fileName;
    std::shared_ptr<touchmind::model::node::NodeModel> m_root;
    std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> m_links;
    std::list<UndoContainer> m_undo;
    touchmind::Configuration *m_pConfiguration;
    touchmind::selection::SelectionManager *m_pSelectionManager;
    std::vector<IMapModelChangeListener*> m_mapModelChangeListeners;
    touchmind::operation::FileOperation *m_pFileOperation;
    touchmind::converter::NodeModelXMLEncoder *m_pNodeModelXMLEncoder;
    touchmind::converter::NodeModelXMLDecoder *m_pNodeModelXMLDecoder;

protected:
    void _FireBeforeNewEvent();
    void _FireAfterNewEvent();
    void _FireBeforeOpenEvent();
    void _FireAfterOpenEvent();
    void _FireBeforeSaveEvent();
    void _FireAfterSaveEvent();
    int _GetNumericStartIndex(const std::wstring &text);
    MapModelIOResult _Open_Version_1_0(MSXML::IXMLDOMElementPtr tmmElement);
    MapModelIOResult _OpenFromFreeMind_ParseNodeList(
        std::shared_ptr<touchmind::model::node::NodeModel> parent,
        MSXML::IXMLDOMNodeListPtr pNodeList);
    MapModelIOResult _OpenFromFreeMind_ParseNode(
        std::shared_ptr<touchmind::model::node::NodeModel> node,
        MSXML::IXMLDOMNodePtr xmlNode);

public:
    static void SetDefaultRootNodeText(const std::wstring &defaultRootNodeText) {
        s_defaultRootNodeText = defaultRootNodeText;
    }

    MapModel(void);
    virtual ~MapModel(void);
    void SetConfiguration(touchmind::Configuration *pConfiguration) {
        m_pConfiguration = pConfiguration;
    }
    void SetSelectionManager(touchmind::selection::SelectionManager *selectionManager) {
        m_pSelectionManager = selectionManager;
    }
    void SetFileOperation(touchmind::operation::FileOperation *pFileOperation) {
        m_pFileOperation = pFileOperation;
    }
    void SetNodeModelXMLEncoder(touchmind::converter::NodeModelXMLEncoder *pNodeModelXMLEncoder) {
        m_pNodeModelXMLEncoder = pNodeModelXMLEncoder;
    }
    void SetNodeModelXMLDecoder(touchmind::converter::NodeModelXMLDecoder *pNodeModelXMLDecoder) {
        m_pNodeModelXMLDecoder = pNodeModelXMLDecoder;
    }
    void Initialize();

    // root node
    void SetRootNodeModel(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
    const std::shared_ptr<touchmind::model::node::NodeModel>& GetRootNodeModel() const;

    // link
    void AddLink(const std::shared_ptr<touchmind::model::link::LinkModel> &linkModel);
    void AddLinks(const std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> &links);
    void RemoveLink(const std::shared_ptr<touchmind::model::link::LinkModel> &linkModel);
    size_t GetNumberOfLinkModels() const {
        return m_links.size();
    }
    const std::shared_ptr<touchmind::model::link::LinkModel>& GetLinkModel(size_t index) const {
        return m_links[index];
    }
    void NormalizeLinks();
    void RenumberLinkIds();

    void New();
    void Clear();

    void BeginTransaction();
    void EndTransaction();
    bool Roleback();
    void _MakeDeepCopy(
        std::shared_ptr<touchmind::model::node::NodeModel> &root,
        std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> &links);
    std::vector<std::shared_ptr<touchmind::model::link::LinkModel>>& _GetLinks() {
        return m_links;
    }

    bool HasFileName() const {
        return m_fileName.length() > 0;
    }
    const std::wstring & GetFileName() const {
        return m_fileName;
    }
    MapModelIOResult Open(const std::wstring &fileName);
    MapModelIOResult OpenFromFreeMind(const std::wstring &fileName);
    MapModelIOResult Save();
    MapModelIOResult SaveAs(const std::wstring &fileName);

    void AddMapModelChangeListener(IMapModelChangeListener *pMapModelChangeListener);
    void RemoveMapModelChangeListener(IMapModelChangeListener *pMapModelChangeListener);
};
} // model
} // touchmind

#endif // TOUCHMIND_MODEL_MAPMODEL_H_