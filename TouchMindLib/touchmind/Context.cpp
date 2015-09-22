#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "resource.h"
#include "Context.h"
#include "touchmind/animation/AnimationManagerEventHandler.h"
#include "touchmind/animation/IAnimationStatusChangedListener.h"

using namespace DirectX;

#define TOUCHMIND_CONTEXT_DEBUG

/*static*/
const D3D10_INPUT_ELEMENT_DESC touchmind::Context::s_InputLayout[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
};

/*static*/
const touchmind::Vertex touchmind::Context::s_VertexArray[] = {{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
                                                               {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},
                                                               {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
                                                               {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)}};

/*static*/
const SHORT touchmind::Context::s_FacesIndexArray[] = {3, 1, 0, 2, 1, 3};

const UINT touchmind::Context::sc_msaaSampleCount = 4;

void *touchmind::Context::operator new(size_t size) {
  return _aligned_malloc(size, 16);
}

void touchmind::Context::operator delete(void *p) {
  _aligned_free(p);
}

touchmind::Context::Context(void)
    : m_hwnd(nullptr)
    , m_pD2DFactory(nullptr)
    , m_pWICFactory(nullptr)
    , m_pDWriteFactory(nullptr)
    // The following variables are created by CreateDeviceResources
    , m_pDevice(nullptr)
    , m_pSwapChain(nullptr)
    // The following variables are created by CreateD3DDeviceResources
    , m_pRasterizerState(nullptr)
    , m_pDefaultEffect(nullptr)
    , m_pTechniqueNoRef(nullptr)
    , m_pWorldVariableNoRef(nullptr)
    , m_pViewVariableNoRef(nullptr)
    , m_pProjectionVariableNoRef(nullptr)
    , m_pDiffuseVariableNoRef(nullptr)
    , m_ViewMatrix()
    , m_pVertexLayout(nullptr)
    // The following variables are created by RecreateSizedResources
    , m_pBackBufferRenderTargetView(nullptr)
    , m_pDepthStencil(nullptr)
    , m_pDepthStencilView(nullptr)
    , m_ProjectionMatrix()
    , m_pD2DTexture2D(nullptr)
    , m_pD2DTexture2DResourceView(nullptr)
    , m_pD2DTexture2DRenderTarget(nullptr)
    // For user application
    , m_pRenderEventListener(nullptr)
    , m_pVertexBuffer(nullptr)
    , m_pFacesIndexBuffer(nullptr)
    // for MSAA
    , m_msaaSampleCount(1)
    , m_msaaQuality(0)
    // Animation
    , m_pAnimationManager(nullptr)
    , m_pAnimationTimer(nullptr)
    , m_pTransitionLibrary(nullptr)
    , m_animationStatusChangedListener(nullptr) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
  LOG_LEAVE;
#endif
}

touchmind::Context::~Context(void) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  // The following variables are created by CreateD3DDeviceResources
  SafeRelease(&m_pRasterizerState);
  SafeRelease(&m_pDefaultEffect);
  SafeRelease(&m_pVertexLayout);

  // The following variables are created by RecreateSizedResources
  SafeRelease(&m_pBackBufferRenderTargetView);
  SafeRelease(&m_pDepthStencil);
  SafeRelease(&m_pDepthStencilView);
  SafeRelease(&m_pD2DTexture2D);
  SafeRelease(&m_pD2DTexture2DResourceView);
  SafeRelease(&m_pD2DTexture2DRenderTarget);

  // The following variables are created by CreateVertexAndIndexBuffer
  SafeRelease(&m_pVertexBuffer);
  SafeRelease(&m_pFacesIndexBuffer);

  // Release factories
  SafeRelease(&m_pD2DFactory);
  SafeRelease(&m_pWICFactory);
  SafeRelease(&m_pDWriteFactory);

  // The following variables are created by CreateDeviceResources
  SafeRelease(&m_pSwapChain);
  SafeRelease(&m_pDevice);
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE;
#endif
}

void touchmind::Context::DiscardDeviceResources() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  if (m_pRenderEventListener != nullptr) {
    m_pRenderEventListener->DiscardDeviceResources();
  }
  // The following variables are created by CreateD3DDeviceResources
  SafeRelease(&m_pRasterizerState);
  SafeRelease(&m_pDefaultEffect);
  SafeRelease(&m_pVertexLayout);
  // The following variables are created by RecreateSizedResources
  SafeRelease(&m_pBackBufferRenderTargetView);
  SafeRelease(&m_pDepthStencil);
  SafeRelease(&m_pDepthStencilView);
  SafeRelease(&m_pD2DTexture2D);
  SafeRelease(&m_pD2DTexture2DResourceView);
  SafeRelease(&m_pD2DTexture2DRenderTarget);
  // The following variables are created by CreateVertexAndIndexBuffer
  SafeRelease(&m_pVertexBuffer);
  SafeRelease(&m_pFacesIndexBuffer);
  // The following variables are created by CreateDeviceResources
  SafeRelease(&m_pSwapChain);
  SafeRelease(&m_pDevice);
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE;
#endif
}

HRESULT touchmind::Context::CreateDeviceResources() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr = S_OK;
  RECT rcClient;
  ID3D10Device1 *pDevice = nullptr;
  IDXGIDevice *pDXGIDevice = nullptr;
  IDXGIAdapter *pAdapter = nullptr;
  IDXGIFactory *pDXGIFactory = nullptr;
  IDXGISurface *pSurface = nullptr;

  if (m_hwnd == nullptr) {
    LOG(SEVERITY_LEVEL_WARN) << L"HWND has not been set.";
    return S_FALSE;
  }

  GetClientRect(m_hwnd, &rcClient);

  UINT nWidth = std::abs(rcClient.right - rcClient.left);
  UINT nHeight = std::abs(rcClient.bottom - rcClient.top);

  if (!m_pDevice) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
    LOG(SEVERITY_LEVEL_DEBUG) << L"Creating D3D Device.";
#endif
#ifdef _DEBUG
    // GMA950 doesn't work with D3D10_CREATE_DEVICE_DEBUG
    UINT nDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT | D3D10_CREATE_DEVICE_DEBUG;
// UINT nDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#else
    UINT nDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#endif
    hr = _CreateD3DDevice(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nDeviceFlags, &pDevice);

    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_INFO) << L"Creating hardware support D3D10 device was failed. Try D3D10_DRIVER_TYPE_WARP.";
      hr = _CreateD3DDevice(nullptr, D3D10_DRIVER_TYPE_WARP, nDeviceFlags, &pDevice);
      if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_FATAL) << L"Creating D3D10 device was failed.";
        return hr;
      }
    }

    if (SUCCEEDED(hr)) {
      hr = pDevice->QueryInterface(&m_pDevice);
    }
    if (SUCCEEDED(hr)) {
      hr = pDevice->QueryInterface(&pDXGIDevice);
    }
    if (SUCCEEDED(hr)) {
      hr = pDXGIDevice->GetAdapter(&pAdapter);
    }
    if (SUCCEEDED(hr)) {
      hr = pAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory));
    }
    if (SUCCEEDED(hr)) {
      DXGI_SWAP_CHAIN_DESC swapDesc;
      ::ZeroMemory(&swapDesc, sizeof(swapDesc));

      swapDesc.BufferDesc.Width = nWidth;
      swapDesc.BufferDesc.Height = nHeight;
      swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      swapDesc.BufferDesc.RefreshRate.Numerator = 60;
      swapDesc.BufferDesc.RefreshRate.Denominator = 1;
      swapDesc.SampleDesc.Count = m_msaaSampleCount;
      swapDesc.SampleDesc.Quality = m_msaaQuality;
      swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapDesc.BufferCount = 1;
      swapDesc.OutputWindow = m_hwnd;
      swapDesc.Windowed = TRUE;

      hr = pDXGIFactory->CreateSwapChain(m_pDevice, &swapDesc, &m_pSwapChain);
#ifdef DEBUG_GPU_RESOURCE
      LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IDXGISwapChain = [" << std::hex << m_pSwapChain << L"]" << std::dec;
#endif
      if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_FATAL) << L"Creating swap chain was failed.";
        return hr;
      }
    }

    if (SUCCEEDED(hr)) {
      hr = _CreateD3DDeviceResources();
    }
    if (SUCCEEDED(hr)) {
      hr = _RecreateSizedResources(nWidth, nHeight);
    }
    if (SUCCEEDED(hr)) {
      if (m_pRenderEventListener != nullptr) {
        m_pRenderEventListener->CreateD2DResources(this, m_pD2DFactory, m_pD2DTexture2DRenderTarget);
      }
    }
  }

  SafeRelease(&pDevice);
  SafeRelease(&pDXGIDevice);
  SafeRelease(&pAdapter);
  SafeRelease(&pDXGIFactory);
  SafeRelease(&pSurface);
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::_CreateD3DDeviceResources() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr = S_OK;

  D3D10_RASTERIZER_DESC rsDesc;
  rsDesc.AntialiasedLineEnable = FALSE;
  rsDesc.CullMode = D3D10_CULL_NONE;
  rsDesc.DepthBias = 0;
  rsDesc.DepthBiasClamp = 0;
  rsDesc.DepthClipEnable = TRUE;
  rsDesc.FillMode = D3D10_FILL_SOLID;
  rsDesc.FrontCounterClockwise = FALSE;
  rsDesc.MultisampleEnable = FALSE;
  rsDesc.ScissorEnable = FALSE;
  rsDesc.SlopeScaledDepthBias = 0;

  hr = m_pDevice->CreateRasterizerState(&rsDesc, &m_pRasterizerState);
#ifdef DEBUG_GPU_RESOURCE
  LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10RasterizerState = [" << std::hex << m_pRasterizerState << L"]"
                           << std::dec;
#endif
  if (SUCCEEDED(hr)) {
    m_pDevice->RSSetState(m_pRasterizerState);
    m_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  }
  if (SUCCEEDED(hr)) {
    hr = CreateVertexAndIndexBuffer();
  }
  if (SUCCEEDED(hr)) {
    hr = m_pRenderEventListener->CreateD3DResources(this, m_pDevice);
  }
  if (SUCCEEDED(hr)) {
    hr = LoadResourceShader(m_pDevice, MAKEINTRESOURCE(IDR_PIXEL_SHADER), &m_pDefaultEffect);
  }
  if (SUCCEEDED(hr)) {
    m_pTechniqueNoRef = m_pDefaultEffect->GetTechniqueByName("Render");
    hr = m_pTechniqueNoRef ? S_OK : E_FAIL;
  }
  if (SUCCEEDED(hr)) {
    m_pWorldVariableNoRef = m_pDefaultEffect->GetVariableByName("World")->AsMatrix();
    hr = m_pWorldVariableNoRef ? S_OK : E_FAIL;
  }
  if (SUCCEEDED(hr)) {
    m_pViewVariableNoRef = m_pDefaultEffect->GetVariableByName("View")->AsMatrix();
    hr = m_pViewVariableNoRef ? S_OK : E_FAIL;

    if (SUCCEEDED(hr)) {
      // XMFLOAT3 Eye(0.0f, 2.0f, -6.0f);
      XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -6.0f, 0.f);
      XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
      XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
      m_ViewMatrix = XMMatrixLookAtLH(Eye, At, Up);
      m_pViewVariableNoRef->SetMatrix((float *)&m_ViewMatrix);
    }
  }
  if (SUCCEEDED(hr)) {
    m_pDiffuseVariableNoRef = m_pDefaultEffect->GetVariableByName("txDiffuse")->AsShaderResource();
    hr = m_pDiffuseVariableNoRef ? S_OK : E_FAIL;
  }
  if (SUCCEEDED(hr)) {
    m_pProjectionVariableNoRef = m_pDefaultEffect->GetVariableByName("Projection")->AsMatrix();
    hr = m_pProjectionVariableNoRef ? S_OK : E_FAIL;
  }
  if (SUCCEEDED(hr)) {
    UINT numElements = ARRAYSIZE(s_InputLayout);

    D3D10_PASS_DESC PassDesc;
    m_pTechniqueNoRef->GetPassByIndex(0)->GetDesc(&PassDesc);

    hr = m_pDevice->CreateInputLayout(s_InputLayout, numElements, PassDesc.pIAInputSignature,
                                      PassDesc.IAInputSignatureSize, &m_pVertexLayout);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10InputLayout = [" << std::hex << m_pVertexLayout << L"]"
                             << std::dec;
#endif
    if (SUCCEEDED(hr)) {
      m_pDevice->IASetInputLayout(m_pVertexLayout);
    }
  }

#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::_RecreateSizedResources(UINT nWidth, UINT nHeight) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER_ARG(L"width = " << nWidth << L", height = " << nHeight);
#endif
  HRESULT hr = S_OK;
  IDXGISurface *pBackBuffer = nullptr;
  ID3D10Resource *pBackBufferResource = nullptr;
  ID3D10RenderTargetView *viewList[1] = {nullptr};

  m_pDevice->OMSetRenderTargets(1, viewList, nullptr);
  SafeRelease(&m_pBackBufferRenderTargetView);

  hr = m_pSwapChain->ResizeBuffers(1, nWidth, nHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
  if (FAILED(hr)) {
    LOG(SEVERITY_LEVEL_FATAL) << L"resize the swap chain failed. hr = " << hr;
  }

  if (SUCCEEDED(hr)) {
    D3D10_TEXTURE2D_DESC texDesc;
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    texDesc.CPUAccessFlags = 0;
    texDesc.Format = DXGI_FORMAT_D16_UNORM;
    texDesc.Height = nHeight;
    texDesc.Width = nWidth;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D10_USAGE_DEFAULT;

    SafeRelease(&m_pDepthStencil);
    hr = m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pDepthStencil);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10Texture2D = [" << std::hex << m_pDepthStencil << L"]"
                             << std::dec;
#endif
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_FATAL) << L"creating a depth stencil texture was failed. hr = " << hr;
    }
  }

  if (SUCCEEDED(hr)) {
    hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBufferResource));
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_FATAL) << L"getting a buffer from the swap chain was failed. hr = " << hr;
    }
  }
  if (SUCCEEDED(hr)) {
    D3D10_RENDER_TARGET_VIEW_DESC renderDesc;
    renderDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    renderDesc.ViewDimension = (m_msaaQuality == 0 ? D3D10_RTV_DIMENSION_TEXTURE2D : D3D10_RTV_DIMENSION_TEXTURE2DMS);
    renderDesc.Texture2D.MipSlice = 0;

    SafeRelease(&m_pBackBufferRenderTargetView);
    hr = m_pDevice->CreateRenderTargetView(pBackBufferResource, &renderDesc, &m_pBackBufferRenderTargetView);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10RenderTargetView = [" << std::hex
                             << m_pBackBufferRenderTargetView << L"]" << std::dec;
#endif
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_FATAL) << L"creating a render taget view was failed. hr = " << hr;
    }
  }
  if (SUCCEEDED(hr)) {
    D3D10_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
    depthViewDesc.Format = DXGI_FORMAT_D16_UNORM;
    depthViewDesc.ViewDimension
        = (m_msaaQuality == 0 ? D3D10_DSV_DIMENSION_TEXTURE2D : D3D10_DSV_DIMENSION_TEXTURE2DMS);
    depthViewDesc.Texture2D.MipSlice = 0;

    SafeRelease(&m_pDepthStencilView);
    hr = m_pDevice->CreateDepthStencilView(m_pDepthStencil, &depthViewDesc, &m_pDepthStencilView);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10DepthStencilView = [" << std::hex << m_pDepthStencilView << L"]"
                             << std::dec;
#endif
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_FATAL) << L"creating depth stencil view was failed. hr = " << hr;
    }
  }
  if (SUCCEEDED(hr)) {
    viewList[0] = m_pBackBufferRenderTargetView;
    // don't need depth stencil view
    // m_pDevice->OMSetRenderTargets(1, viewList, m_pDepthStencilView);
    m_pDevice->OMSetRenderTargets(1, viewList, nullptr);

    D3D10_VIEWPORT viewport;
    viewport.Width = nWidth;
    viewport.Height = nHeight;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    m_pDevice->RSSetViewports(1, &viewport);

    hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_FATAL) << L"getting a buffer from the swap chain was failed. hr = " << hr;
    }
  }

  if (SUCCEEDED(hr)) {
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH((float)XM_PI * 0.24f, // fovy
                                                  nWidth / (float)nHeight, // aspect
                                                  0.1f, // zn
                                                  100.0f // zf
                                                  );
    hr = m_pProjectionVariableNoRef->SetMatrix((float *)&m_ProjectionMatrix);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_FATAL) << L"setting a projection matrix was failed. hr = " << hr;
    }
  }

  if (SUCCEEDED(hr)) {
    D3D10_TEXTURE2D_DESC texDesc;
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.Width = nWidth;
    texDesc.Height = nHeight;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D10_USAGE_DEFAULT;

    SafeRelease(&m_pD2DTexture2D);
    hr = m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pD2DTexture2D);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10Texture2D = [" << std::hex << m_pD2DTexture2D << L"]"
                             << std::dec;
#endif
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_FATAL) << L"creating a texture2D for Direct2D was failed. hr = " << hr;
    }

    D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    if (SUCCEEDED(hr)) {
      SafeRelease(&m_pD2DTexture2DResourceView);
      hr = m_pDevice->CreateShaderResourceView(m_pD2DTexture2D, nullptr, &m_pD2DTexture2DResourceView);
#ifdef DEBUG_GPU_RESOURCE
      LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10ShaderResourceView = [" << std::hex
                               << m_pD2DTexture2DResourceView << L"]" << std::dec;
#endif
      if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_FATAL) << L"creating a shader resource view for Direct2D was failed. hr = " << hr;
      }
    }

    IDXGISurface *pDxgiSurface1 = nullptr;
    if (SUCCEEDED(hr)) {
      hr = m_pD2DTexture2D->QueryInterface(&pDxgiSurface1);
    }

    FLOAT dpiX;
    FLOAT dpiY;
    m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX,
        dpiY);

    if (SUCCEEDED(hr)) {
      SafeRelease(&m_pD2DTexture2DRenderTarget);
      hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(pDxgiSurface1, &props, &m_pD2DTexture2DRenderTarget);
#ifdef DEBUG_GPU_RESOURCE
      LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1RenderTarget = [" << std::hex << m_pD2DTexture2DRenderTarget
                               << L"]" << std::dec;
#endif
      if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_FATAL) << L"creating a DXGI surface render target for Direct2D was failed. hr = " << hr;
      }
    }
    SafeRelease(&pDxgiSurface1);
  }

  SafeRelease(&pBackBuffer);
  SafeRelease(&pBackBufferResource);
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::OnRender() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr = S_OK;
  hr = CreateDeviceResources();
  if (SUCCEEDED(hr)) {
    m_WorldMatrix = XMMatrixRotationY(0.0f);

    DXGI_SWAP_CHAIN_DESC swapDesc;
    hr = m_pSwapChain->GetDesc(&swapDesc);

    if (SUCCEEDED(hr)) {
      m_pDevice->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH, 1, 0);
      float backgroundColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
      m_pDevice->ClearRenderTargetView(m_pBackBufferRenderTargetView, backgroundColor);

      if (m_pD2DTexture2DRenderTarget != nullptr && m_pRenderEventListener != nullptr) {
        m_pRenderEventListener->Render2D(this, m_pD2DTexture2DRenderTarget);
      }

      if (SUCCEEDED(hr)) {
        m_pWorldVariableNoRef->SetMatrix((float *)&m_WorldMatrix);

        SetIndexBuffer();

        if (m_pRenderEventListener != nullptr) {
          m_pRenderEventListener->PrepareRender3D(this, m_pDevice);
        }
        m_pDiffuseVariableNoRef->SetResource(m_pD2DTexture2DResourceView);
        m_pTechniqueNoRef->GetPassByIndex(0)->Apply(0);

        m_pDevice->DrawIndexed(ARRAYSIZE(s_FacesIndexArray), 0, 0);
        if (m_pRenderEventListener != nullptr) {
          m_pRenderEventListener->Render3D(this, m_pDevice);
        }

        if (m_pD2DTexture2DRenderTarget != nullptr && m_pRenderEventListener != nullptr) {
          m_pRenderEventListener->AfterRender3D(this, m_pD2DTexture2DRenderTarget);
        }
        if (SUCCEEDED(hr)) {
          hr = m_pSwapChain->Present(1, 0);
        }
      }
    }
  }

  if (hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_REMOVED || hr == D2DERR_RECREATE_TARGET) {
    hr = S_OK;

    DiscardDeviceResources();
  }
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

void touchmind::Context::OnResize(UINT width, UINT height) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  if (!m_pDevice) {
    CreateDeviceResources();
  } else {
    _RecreateSizedResources(width, height);
  }
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE;
#endif
}

HRESULT touchmind::Context::_CreateD3DDevice(IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE driverType, UINT flags,
                                             ID3D10Device1 **ppDevice) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr = S_OK;

  static const D3D10_FEATURE_LEVEL1 levelAttempts[] = {
      D3D10_FEATURE_LEVEL_10_1, D3D10_FEATURE_LEVEL_10_0, D3D10_FEATURE_LEVEL_9_3,
      D3D10_FEATURE_LEVEL_9_2,  D3D10_FEATURE_LEVEL_9_1,
  };

  for (UINT level = 0; level < ARRAYSIZE(levelAttempts); ++level) {
    ID3D10Device1 *pDevice = nullptr;
    hr = D3D10CreateDevice1(pAdapter, driverType, nullptr, flags, levelAttempts[level], D3D10_1_SDK_VERSION, &pDevice);

    if (SUCCEEDED(hr)) {
      *ppDevice = pDevice;
      pDevice = nullptr;
      break;
    }
  }

#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::LoadResourceBitmap(ID2D1RenderTarget *pRenderTarget, IWICImagingFactory *pIWICFactory,
                                               PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth,
                                               UINT destinationHeight, ID2D1Bitmap **ppBitmap) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr = S_OK;
  IWICBitmapDecoder *pDecoder = nullptr;
  IWICBitmapFrameDecode *pSource = nullptr;
  IWICStream *pStream = nullptr;
  IWICFormatConverter *pConverter = nullptr;
  IWICBitmapScaler *pScaler = nullptr;

  HRSRC imageResHandle = nullptr;
  HGLOBAL imageResDataHandle = nullptr;
  void *pImageFile = nullptr;
  DWORD imageFileSize = 0;

  imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);
  hr = imageResHandle ? S_OK : E_FAIL;
  if (SUCCEEDED(hr)) {
    imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);
    hr = imageResDataHandle ? S_OK : E_FAIL;
  }
  if (SUCCEEDED(hr)) {
    pImageFile = LockResource(imageResDataHandle);
    hr = pImageFile ? S_OK : E_FAIL;
  }
  if (SUCCEEDED(hr)) {
    imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
    hr = imageFileSize ? S_OK : E_FAIL;
  }
  if (SUCCEEDED(hr)) {
    hr = pIWICFactory->CreateStream(&pStream);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IWICStream = [" << std::hex << pStream << L"]" << std::dec;
#endif
  }
  if (SUCCEEDED(hr)) {
    hr = pStream->InitializeFromMemory(reinterpret_cast<BYTE *>(pImageFile), imageFileSize);
  }
  if (SUCCEEDED(hr)) {
    hr = pIWICFactory->CreateDecoderFromStream(pStream, nullptr, WICDecodeMetadataCacheOnLoad, &pDecoder);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IWICBitmapDecoder = [" << std::hex << pDecoder << L"]" << std::dec;
#endif
  }
  if (SUCCEEDED(hr)) {
    hr = pDecoder->GetFrame(0, &pSource);
  }
  if (SUCCEEDED(hr)) {
    hr = pIWICFactory->CreateFormatConverter(&pConverter);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IWICFormatConverter = [" << std::hex << pConverter << L"]" << std::dec;
#endif
  }
  if (SUCCEEDED(hr)) {
    if (destinationWidth != 0 || destinationHeight != 0) {
      UINT originalWidth, originalHeight;
      hr = pSource->GetSize(&originalWidth, &originalHeight);
      if (SUCCEEDED(hr)) {
        if (destinationWidth == 0) {
          FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
          destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
        } else if (destinationHeight == 0) {
          FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
          destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
        }

        hr = pIWICFactory->CreateBitmapScaler(&pScaler);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IWICBitmapScaler = [" << std::hex << pScaler << L"]" << std::dec;
#endif
        if (SUCCEEDED(hr)) {
          hr = pScaler->Initialize(pSource, destinationWidth, destinationHeight, WICBitmapInterpolationModeCubic);
          if (SUCCEEDED(hr)) {
            hr = pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f,
                                        WICBitmapPaletteTypeMedianCut);
          }
        }
      }
    } else {
      hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f,
                                  WICBitmapPaletteTypeMedianCut);
    }
  }
  if (SUCCEEDED(hr)) {
    hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, ppBitmap);
#ifdef DEBUG_OUTPUT_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1Bitmap = [" << std::hex << *ppBitmap << L"]" << std::dec;
#endif
  }

  SafeRelease(&pDecoder);
  SafeRelease(&pSource);
  SafeRelease(&pStream);
  SafeRelease(&pConverter);
  SafeRelease(&pScaler);

#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::LoadResourceShader(ID3D10Device *pDevice, PCWSTR pszResource, ID3D10Effect **ppShader) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr;

  HRSRC hResource = ::FindResource(HINST_THISCOMPONENT, pszResource, RT_RCDATA);
  hr = hResource ? S_OK : E_FAIL;

  if (SUCCEEDED(hr)) {
    HGLOBAL hResourceData = ::LoadResource(HINST_THISCOMPONENT, hResource);
    hr = hResourceData ? S_OK : E_FAIL;

    if (SUCCEEDED(hr)) {
      LPVOID pData = ::LockResource(hResourceData);
      hr = pData ? S_OK : E_FAIL;

      if (SUCCEEDED(hr)) {
        hr = ::D3D10CreateEffectFromMemory(pData, ::SizeofResource(HINST_THISCOMPONENT, hResource), 0, pDevice, nullptr,
                                           ppShader);
      }
    }
  }

#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::CreateDeviceIndependentResources() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr;

  D2D1_FACTORY_OPTIONS factoryOptions;
#ifdef _DEBUG
  factoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
  factoryOptions.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif
  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factoryOptions, &m_pD2DFactory);
#ifdef DEBUG_GPU_RESOURCE
  LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1Factory = [" << std::hex << m_pD2DFactory << L"]" << std::dec;
#endif
  if (SUCCEEDED(hr)) {
    // Create a WIC factory
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
                          reinterpret_cast<void **>(&m_pWICFactory));
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IWICImageFactory = [" << std::hex << m_pWICFactory << L"]" << std::dec;
#endif
  }
  if (SUCCEEDED(hr)) {
    // Create DWrite factory
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory),
                             reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IDWriteFactory = [" << std::hex << m_pDWriteFactory << L"]"
                             << std::dec;
#endif
  }

  if (SUCCEEDED(hr)) {
    hr = _InitializeAnimation();
  }

  if (SUCCEEDED(hr) && m_pRenderEventListener != nullptr) {
    hr = m_pRenderEventListener->CreateDeviceIndependentResources(this, m_pD2DFactory, m_pWICFactory, m_pDWriteFactory);
  }

#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::CreateTexture2D(UINT width, UINT height, OUT ID3D10Texture2D **ppTexture2D) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr;
  D3D10_TEXTURE2D_DESC texDescForSource;
  texDescForSource.ArraySize = 1;
  texDescForSource.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
  texDescForSource.CPUAccessFlags = 0;
  texDescForSource.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  texDescForSource.Width = width;
  texDescForSource.Height = height;
  texDescForSource.MipLevels = 1;
  texDescForSource.MiscFlags = 0;
  texDescForSource.SampleDesc.Count = 1;
  texDescForSource.SampleDesc.Quality = 0;
  texDescForSource.Usage = D3D10_USAGE_DEFAULT;
  hr = m_pDevice->CreateTexture2D(&texDescForSource, nullptr, ppTexture2D);
#ifdef DEBUG_OUTPUT_GPU_RESOURCE
  LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10Texture2D = [" << std::hex << *ppTexture2D << L"]" << std::dec;
#endif

#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::CreateD2DRenderTargetFromTexture2D(ID3D10Texture2D *pTexure2D,
                                                               ID2D1RenderTarget **ppRenderTarget) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr = S_OK;
  IDXGISurface *pDxgiSurface1 = nullptr;
  hr = pTexure2D->QueryInterface(&pDxgiSurface1);

  D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
      D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
  if (SUCCEEDED(hr)) {
    hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(pDxgiSurface1, &props, ppRenderTarget);
#ifdef DEBUG_OUTPUT_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1RenderTarget = [" << std::hex << *ppRenderTarget << L"]"
                             << std::dec;
#endif
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"failed to create ID2D1RenderTarget, hr = " << hr;
    }
  }
  SafeRelease(&pDxgiSurface1);

#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::CreateVertexAndIndexBuffer() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr;
  D3D10_BUFFER_DESC bd;
  bd.Usage = D3D10_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(s_VertexArray);
  bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags = 0;
  D3D10_SUBRESOURCE_DATA InitData;
  InitData.pSysMem = s_VertexArray;

  hr = m_pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
#ifdef DEBUG_GPU_RESOURCE
  LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10Buffer = [" << std::hex << m_pVertexBuffer << L"]" << std::dec;
#endif
  if (SUCCEEDED(hr)) {
    // Set vertex buffer
    UINT stride = sizeof(touchmind::Vertex);
    UINT offset = 0;
    ID3D10Buffer *pVertexBuffer = m_pVertexBuffer;

    m_pDevice->IASetVertexBuffers(0, // StartSlot
                                  1, // NumBuffers
                                  &pVertexBuffer, &stride, &offset);
  }
  if (SUCCEEDED(hr)) {
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(s_FacesIndexArray);
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = s_FacesIndexArray;

    hr = m_pDevice->CreateBuffer(&bd, &InitData, &m_pFacesIndexBuffer);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID3D10Buffer = [" << std::hex << m_pFacesIndexBuffer << L"]"
                             << std::dec;
#endif
  }
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE;
#endif
  return hr;
}

void touchmind::Context::SetIndexBuffer() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  m_pDevice->IASetIndexBuffer(m_pFacesIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE;
#endif
}

void touchmind::Context::PushStates() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  D3DStates d3dState;
  m_pDevice->OMGetRenderTargets(1, &d3dState.pRenderTargetViews, &d3dState.pDepthStencilView);
  UINT numViewports = 1;
  m_pDevice->RSGetViewports(&numViewports, &d3dState.viewport);
  m_vD3DStates.push_back(d3dState);
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE;
#endif
}

void touchmind::Context::PopStates() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  D3DStates d3dState = m_vD3DStates[m_vD3DStates.size() - 1];

  m_pDevice->OMSetRenderTargets(1, &d3dState.pRenderTargetViews, d3dState.pDepthStencilView);
  SafeRelease(&d3dState.pRenderTargetViews);
  SafeRelease(&d3dState.pDepthStencilView);
  m_pDevice->RSSetViewports(1, &d3dState.viewport);

  m_vD3DStates.pop_back();
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE;
#endif
}

HRESULT touchmind::Context::CreateD2DSharedBitmapFromTexture2D(ID2D1RenderTarget *pRenderTarget,
                                                               ID3D10Texture2D *pTexure2D, ID2D1Bitmap **ppBitmap) {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr = S_OK;
  IDXGISurface *pDxgiSurface = nullptr;
  hr = pTexure2D->QueryInterface(&pDxgiSurface);
  if (SUCCEEDED(hr)) {
    D2D1_BITMAP_PROPERTIES bitmapProp;
    bitmapProp.dpiX = 96;
    bitmapProp.dpiY = 96;
    bitmapProp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
    hr = pRenderTarget->CreateSharedBitmap(__uuidof(IDXGISurface), pDxgiSurface, &bitmapProp, ppBitmap);
#ifdef DEBUG_OUTPUT_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1Bitmap = [" << std::hex << *ppBitmap << L"]" << std::dec;
#endif
    SafeRelease(&pDxgiSurface);
  }
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}

HRESULT touchmind::Context::_InitializeAnimation() {
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_ENTER;
#endif
  HRESULT hr = S_OK;
  hr = CoCreateInstance(CLSID_UIAnimationManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pAnimationManager));
  if (SUCCEEDED(hr)) {
    hr = CoCreateInstance(CLSID_UIAnimationTimer, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pAnimationTimer));
  }
  if (SUCCEEDED(hr)) {
    hr = CoCreateInstance(CLSID_UIAnimationTransitionLibrary, nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&m_pTransitionLibrary));
  }
  IUIAnimationTimerUpdateHandler *pTimerUpdateHandler = nullptr;
  if (SUCCEEDED(hr)) {
    hr = m_pAnimationManager->QueryInterface(IID_PPV_ARGS(&pTimerUpdateHandler));
  }
  if (SUCCEEDED(hr)) {
    hr = m_pAnimationTimer->SetTimerUpdateHandler(
        pTimerUpdateHandler, UI_ANIMATION_IDLE_BEHAVIOR_CONTINUE); // UI_ANIMATION_IDLE_BEHAVIOR_DISABLE
    pTimerUpdateHandler->Release();
  }
  IUIAnimationManagerEventHandler *pAnimationManagerEventHandler = nullptr;
  if (SUCCEEDED(hr)) {
    hr = touchmind::animation::AnimationManagerEventHandler::CreateInstance(m_animationStatusChangedListener,
                                                                            &pAnimationManagerEventHandler);
  }
  if (SUCCEEDED(hr)) {
    hr = m_pAnimationManager->SetManagerEventHandler(pAnimationManagerEventHandler);
    pAnimationManagerEventHandler->Release();
  }

  m_pAnimationTimer->Enable();
#ifdef TOUCHMIND_CONTEXT_DEBUG
  LOG_LEAVE_HRESULT(hr);
#endif
  return hr;
}
