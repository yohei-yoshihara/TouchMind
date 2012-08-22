#ifndef TOUCHMIND_MUI_MUI_H_
#define TOUCHMIND_MUI_MUI_H_

namespace touchmind
{
class MUI
{
private:
    MUI(void);
    ~MUI(void);
    static HMODULE s_hModule;

public:
    static HRESULT Initialize();
    static HMODULE GetHModule() {
        return s_hModule;
    }
    static const wchar_t * GetString(UINT uID);
};

} // touchmind

#endif // TOUCHMIND_MUI_MUI_H_