#ifndef TOUCHMIND_CONTROL_DWRITEEDITCONTROLDISPLAYATTRIBUTE_H_
#define TOUCHMIND_CONTROL_DWRITEEDITCONTROLDISPLAYATTRIBUTE_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace control {

    class DisplayAttributeProperties {
    private:
      std::vector<GUID> m_guid;
      std::vector<const GUID *> m_pGuid;

    public:
      DisplayAttributeProperties() {
      }

      void Add(REFGUID rguid) {
        if (!FindGuid(rguid)) {
          GUID *pBefore = (m_guid.size() == 0 ? nullptr : &m_guid[0]);
          m_guid.push_back(rguid);
          size_t n = m_guid.size() - 1;
          if (pBefore != &m_guid[0]) {
            // memory was reallocated
            for (size_t i = 0; i < n; ++i) {
              m_pGuid[i] = &m_guid[i];
            }
          }
          m_pGuid.push_back(&m_guid[n]);
          ASSERT(m_guid.size() == m_pGuid.size());
        }
      }

      bool FindGuid(REFGUID rguid) const {
        return std::any_of(m_guid.begin(), m_guid.end(), [&](REFGUID e) { return IsEqualGUID(e, rguid); });
      }

      size_t Count() const {
        return m_guid.size();
      }

      GUID *GetPropTable() {
        return &m_guid[0];
      }

      const GUID **GetPropTablePointer() {
        return &m_pGuid[0];
      }
    };

    class DWriteEditControlDisplayAttribute {
    private:
      ITfDisplayAttributeMgr *m_pDisplayAttributeMgr;
      ITfCategoryMgr *m_pCategoryMgr;

    public:
      DWriteEditControlDisplayAttribute(void);
      ~DWriteEditControlDisplayAttribute(void);
      void SetDisplayAttributeMgr(ITfDisplayAttributeMgr *pDisplayAttributeMgr) {
        m_pDisplayAttributeMgr = pDisplayAttributeMgr;
      }
      void SetCategoryMgr(ITfCategoryMgr *pCategoryMgr) {
        m_pCategoryMgr = pCategoryMgr;
      }
      HRESULT GetDisplayAttributeProperties(OUT DisplayAttributeProperties &displayAttributeProperties);
      HRESULT GetDisplayAttributeTrackPropertyRange(IN ITfContext *tfContext,
                                                    IN DisplayAttributeProperties &displayAttributeProperties,
                                                    OUT ITfReadOnlyProperty **ppProp);
      HRESULT GetDisplayAttributeData(IN TfEditCookie editCookie, IN ITfReadOnlyProperty *pProp, IN ITfRange *pRange,
                                      OUT TF_DISPLAYATTRIBUTE *pDisplayAttribute, OUT TfGuidAtom *pGuid);

      HRESULT GetAttributeColor(IN const TF_DA_COLOR &pdac, OUT COLORREF &colorref);
    };

  } // control
} // touchmind

#endif // TOUCHMIND_CONTROL_DWRITEEDITCONTROLDISPLAYATTRIBUTE_H_