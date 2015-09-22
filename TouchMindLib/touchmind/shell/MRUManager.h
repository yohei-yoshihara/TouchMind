#ifndef TOUCHMIND_MRUMANAGER_H_
#define TOUCHMIND_MRUMANAGER_H_

namespace touchmind {
  namespace shell {

    class MRUManager {
    public:
      static const size_t RECENT_FILE_COUNT;

    private:
      std::vector<std::wstring> m_mruDisplayNameList;
      std::vector<std::wstring> m_mruPathList;

    public:
      MRUManager(void);
      ~MRUManager(void);
      HRESULT PopulateRibbonRecentItems(__deref_out PROPVARIANT *pvarValue);
      HRESULT GetSelectedItem(IN const PROPERTYKEY *key, IN const PROPVARIANT *ppropvarValue,
                              OUT std::wstring &displayName, OUT std::wstring &fullPath);
    };

  } // shell
} // touchmind

#endif // TOUCHMIND_MRUMANAGER_H_