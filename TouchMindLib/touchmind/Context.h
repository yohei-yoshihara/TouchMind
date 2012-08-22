#ifndef TOUCHMIND_CONTEXT_H_
#define TOUCHMIND_CONTEXT_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"

namespace touchmind
{

struct Vertex {
    D3DXVECTOR3 Pos;
    D3DXVECTOR2 Tex;
};

class D3DStates
{
public:
    D3DStates():
        pRenderTargetViews(nullptr),
        pDepthStencilView(nullptr),
        viewport()
    {}
    // OMGetRenderTargets (Currently only one render target view supported)
    ID3D10RenderTargetView *pRenderTargetViews;
    ID3D10DepthStencilView *pDepthStencilView;
    // RSGetViewports (Currently only one viewport supported)
    D3D10_VIEWPORT viewport;
};

class Context
{
private:
    HWND m_hwnd;
    // The following variables are created by CreateDeviceIndependentResources
    ID2D1Factory *m_pD2DFactory;
    IWICImagingFactory *m_pWICFactory;
    IDWriteFactory *m_pDWriteFactory;

    // The following variables are created by CreateDeviceResources
    ID3D10Device *m_pDevice;
    IDXGISwapChain *m_pSwapChain;

    // The following variables are created by CreateD3DDeviceResources
    ID3D10RasterizerState *m_pRasterizerState;
    ID3D10Effect *m_pDefaultEffect;
    ID3D10EffectTechnique *m_pTechniqueNoRef;
    ID3D10EffectMatrixVariable *m_pWorldVariableNoRef;
    ID3D10EffectMatrixVariable *m_pViewVariableNoRef;
    ID3D10EffectMatrixVariable *m_pProjectionVariableNoRef;
    ID3D10EffectShaderResourceVariable *m_pDiffuseVariableNoRef;
    D3DXMATRIX m_ViewMatrix;
    ID3D10InputLayout *m_pVertexLayout;

    // The following variables are created by RecreateSizedResources
    //ID2D1RenderTarget *m_pBackBufferRenderTarget;
    ID3D10RenderTargetView *m_pBackBufferRenderTargetView;
    ID3D10Texture2D *m_pDepthStencil;
    ID3D10DepthStencilView *m_pDepthStencilView;
    D3DXMATRIX m_ProjectionMatrix;
    // Add for D2D texture
    ID3D10Texture2D *m_pD2DTexture2D;
    ID3D10ShaderResourceView *m_pD2DTexture2DResourceView;
    ID2D1RenderTarget *m_pD2DTexture2DRenderTarget;

    // The following variables are updated by OnRender
    D3DXMATRIX m_WorldMatrix;

    // User Application
    IRenderEventListener *m_pRenderEventListener;

    ID3D10Buffer *m_pVertexBuffer;
    ID3D10Buffer *m_pFacesIndexBuffer;

    std::vector<D3DStates> m_vD3DStates;

    // for MSAA
    UINT m_msaaSampleCount;
    UINT m_msaaQuality;

    // ********** Animation **********
    CComPtr<IUIAnimationManager> m_pAnimationManager;
    CComPtr<IUIAnimationTimer> m_pAnimationTimer;
    CComPtr<IUIAnimationTransitionLibrary> m_pTransitionLibrary;
    touchmind::animation::IAnimationStatusChangedListener *m_animationStatusChangedListener;

protected:
    HRESULT _CreateD3DDevice(
        IDXGIAdapter *pAdapter,
        D3D10_DRIVER_TYPE driverType,
        UINT flags,
        ID3D10Device1 **ppDevice);
    HRESULT _CreateD3DDeviceResources();
    HRESULT _InitializeAnimation();

public:
    static const D3D10_INPUT_ELEMENT_DESC s_InputLayout[];
    static const touchmind::Vertex s_VertexArray[];
    static const SHORT s_FacesIndexArray[];
    static const UINT sc_msaaSampleCount;
    static HRESULT LoadResourceBitmap(
        ID2D1RenderTarget *pRenderTarget,
        IWICImagingFactory *pIWICFactory,
        PCWSTR resourceName,
        PCWSTR resourceType,
        UINT destinationWidth,
        UINT destinationHeight,
        ID2D1Bitmap **ppBitmap);
    static HRESULT LoadResourceShader(
        ID3D10Device *pDevice,
        PCWSTR pszResource,
        ID3D10Effect **ppShader);

    Context(void);
    virtual ~Context(void);
    void SetHWnd(HWND hwnd) {
        m_hwnd = hwnd;
    }
    HWND GetHWnd() const {
        return m_hwnd;
    }
    void SetRenderEventListener(
        IRenderEventListener *pRenderEventListener) {
        m_pRenderEventListener = pRenderEventListener;
    }
    ID3D10Device* GetD3D10Device() const {
        return m_pDevice;
    }
    ID2D1Factory* GetD2DFactory() const {
        return m_pD2DFactory;
    }
    IWICImagingFactory* GetWICImagingFactory() const {
        return m_pWICFactory;
    }
    IDWriteFactory* GetDWriteFactory() const {
        return m_pDWriteFactory;
    }
    ID2D1RenderTarget* GetD2DRenderTarget() const {
        return m_pD2DTexture2DRenderTarget;
    }
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources();
    HRESULT OnRender();
    void OnResize(UINT width, UINT height);
    HRESULT _RecreateSizedResources(UINT nWidth, UINT nHeight);
    void DiscardDeviceResources();
    HRESULT CreateTexture2D(
        UINT width,
        UINT height,
        OUT ID3D10Texture2D **ppTexture2D);
    HRESULT CreateD2DRenderTargetFromTexture2D(
        ID3D10Texture2D *pTexure2D,
        ID2D1RenderTarget **ppRenderTarget);
    HRESULT CreateD2DSharedBitmapFromTexture2D(
        ID2D1RenderTarget *pRenderTarget,
        ID3D10Texture2D *pTexure2D,
        ID2D1Bitmap **ppBitmap);
    HRESULT CreateVertexAndIndexBuffer();
    void SetIndexBuffer();
    void PushStates();
    void PopStates();

    // Animation
    void SetAnimationStatusChangedListener(touchmind::animation::IAnimationStatusChangedListener *animationStatusChangedListener) {
        m_animationStatusChangedListener = animationStatusChangedListener;
    }
    IUIAnimationManager* GetAnimationManager() const {
        return m_pAnimationManager;
    }
    IUIAnimationTimer* GetAnimationTimer() const {
        return m_pAnimationTimer;
    }
    IUIAnimationTransitionLibrary* GetAnimationTransitionLibrary() const {
        return m_pTransitionLibrary;
    }
};

class IRenderEventListener
{
public:
    virtual HRESULT CreateDeviceIndependentResources(
        Context *pContext,
        ID2D1Factory *pD2DFactory,
        IWICImagingFactory *pWICFactory,
        IDWriteFactory *pDWriteFactory) = 0;
    virtual HRESULT CreateD3DResources(
        Context *pContext,
        ID3D10Device *pDevice) = 0;
    virtual HRESULT CreateD2DResources(
        Context *pContext,
        ID2D1Factory *pD2DFactory,
        ID2D1RenderTarget *pRenderTarget) = 0;
    virtual HRESULT Render2D(
        Context *pContext,
        ID2D1RenderTarget *pRenderTarget) = 0;
    virtual HRESULT PrepareRender3D(
        Context *pContext,
        ID3D10Device *pDevice) = 0;
    virtual HRESULT Render3D(
        Context *pContext,
        ID3D10Device *pDevice) = 0;
    virtual HRESULT AfterRender3D(
        Context *pContext,
        ID2D1RenderTarget *pRenderTarget) = 0;
    virtual void DiscardDeviceResources() = 0;
};

} // touchmind

#endif // TOUCHMIND_CONTEXT_H_