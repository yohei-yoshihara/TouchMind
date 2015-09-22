#ifndef TOUCHMIND_RIBBON_PROPERTYSET_H_
#define TOUCHMIND_RIBBON_PROPERTYSET_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace ribbon {

    class PropertySet : public IUISimplePropertySet {
    private:
      ULONG m_refCount;
      std::wstring m_label;
      CComPtr<IUIImage> m_image;
      int m_categoryId;
      int m_commandId;
      UI_COMMANDTYPE m_commandType;

    public:
      PropertySet();
      virtual ~PropertySet();

      // IUnknown interface
      STDMETHOD_(ULONG, AddRef)();
      STDMETHOD_(ULONG, Release)();
      STDMETHOD(QueryInterface)(REFIID iid, void **ppv);

      // IUISimplePropertySet interface
      STDMETHOD(GetValue)(REFPROPERTYKEY key, PROPVARIANT *ppropvar);

      void SetLabel(const std::wstring &label) {
        m_label = label;
      }
      const std::wstring &GetLabel() const {
        return m_label;
      }
      void SetImage(CComPtr<IUIImage> image) {
        m_image = image;
      }
      CComPtr<IUIImage> GetImage() const {
        return m_image;
      }
      void SetCategory(int categoryId) {
        m_categoryId = categoryId;
      }
      int GetCategoryId() const {
        return m_categoryId;
      }
      void SetCommandId(int commandId) {
        m_commandId = commandId;
      }
      int GetCommandId() const {
        return m_commandId;
      }
      void SetCommandType(UI_COMMANDTYPE commandType) {
        m_commandType = commandType;
      }
    };

  } // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_PROPERTYSET_H_