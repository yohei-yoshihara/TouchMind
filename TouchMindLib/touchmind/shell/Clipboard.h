#ifndef TOUCHMIND_CLIPBOARD_H_
#define TOUCHMIND_CLIPBOARD_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace shell
{
#define _NUMBER_OF_CLIPBOARD_FORMATS 2

class Clipboard
{
public:
    static const size_t NUMBER_OF_CLIPBOARD_FORMATS;

private:
    touchmind::selection::SelectionManager *m_pSelectionManager;
    int m_touchMindClipboardFormatId;
    std::array<UINT, _NUMBER_OF_CLIPBOARD_FORMATS> m_pFormatPriorityList;
    touchmind::converter::NodeModelXMLEncoder *m_pNodeModelXMLEncoder;
    touchmind::converter::NodeModelXMLDecoder *m_pNodeModelXMLDecoder;

protected:
    HRESULT _InitializeClipboard();
    bool _CopyTEXT(HWND hWnd, const std::wstring &text);
    bool _PasteTEXT(HWND hWnd, std::wstring &text);
    bool _CopyXML(HWND hWnd, const std::wstring &xml);
    bool _PasteXML(HWND hWnd, std::wstring &xml);

public:
    static const wchar_t *CLIPBOARD_FORMAT_NAME;
    Clipboard();
    virtual ~Clipboard(void);
    void SetSelectionManager(touchmind::selection::SelectionManager *pSelectionManager) {
        m_pSelectionManager = pSelectionManager;
    }
    void SetNodeModelXMLEncoder(touchmind::converter::NodeModelXMLEncoder *pNodeModelXMLEncoder) {
        m_pNodeModelXMLEncoder = pNodeModelXMLEncoder;
    }
    void SetNodeModelXMLDecoder(touchmind::converter::NodeModelXMLDecoder *pNodeModelXMLDecoder) {
        m_pNodeModelXMLDecoder = pNodeModelXMLDecoder;
    }
    HRESULT CopyNodeModel(IN HWND hWnd, IN const std::shared_ptr<touchmind::model::node::NodeModel> &node);
    HRESULT PasteNodeModel(
        IN HWND hWnd,
        OUT std::shared_ptr<touchmind::model::node::NodeModel> &node,
        OUT std::vector<std::shared_ptr<model::link::LinkModel>> &links);
};

} // shell
} // touchmind

#endif // TOUCHMIND_CLIPBOARD_H_
