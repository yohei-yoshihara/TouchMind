#ifndef TOUCHMIND_FILEASSOCIATION_H_
#define TOUCHMIND_FILEASSOCIATION_H_

namespace touchmind
{
namespace shell
{

class FileAssociation
{
protected:
    HRESULT RegSetString(HKEY hkey, PCWSTR pszSubKey, PCWSTR pszValue, PCWSTR pszData);
    HRESULT RegSetString(HKEY hKey, std::wstring &subkey, std::wstring &value, std::wstring &data);
    HRESULT RegisterProgId();
    HRESULT UnregisterProgId();
    HRESULT RegisterToHandleExt(const WCHAR *ext);
    HRESULT UnregisterToHandleExt(const WCHAR *ext);
public:
    static const WCHAR APPEXE[];
    static const WCHAR PROGID[];
    static PCWSTR const EXTENSIONS[];
    FileAssociation(void);
    ~FileAssociation(void);
    HRESULT RegisterToHandleFileTypes();
    bool AreFileTypesRegistered();
    HRESULT UnRegisterFileTypeHandlers();
};

} // shell
} // touchmind

#endif // TOUCHMIND_FILEASSOCIATION_H_