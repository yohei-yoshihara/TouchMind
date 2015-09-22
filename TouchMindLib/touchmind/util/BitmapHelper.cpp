#include "StdAfx.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/util/BitmapHelper.h"

HRESULT touchmind::util::BitmapHelper::CreateBitmap(IN IWICImagingFactory *pWICFactory, IN UINT width, IN UINT height,
                                                    OUT IWICBitmap **ppWICBitmap) {
  HRESULT hr = S_OK;
  hr = pWICFactory->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, ppWICBitmap);
  //#ifdef DEBUG_GPU_RESOURCE
  //    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IWICBitmap = [" << std::hex << *ppWICBitmap << L"]" << std::dec;
  //#endif
  return hr;
}

HRESULT touchmind::util::BitmapHelper::CreateBitmapRenderTarget(IN IWICBitmap *pWICBitmap, IN ID2D1Factory *pD2DFactory,
                                                                OUT ID2D1RenderTarget **ppRenderTarget) {
  HRESULT hr = S_OK;

  hr = pD2DFactory->CreateWicBitmapRenderTarget(
      pWICBitmap,
      D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
                                   D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0.0f,
                                   0.0f, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE),
      ppRenderTarget);
  //#ifdef DEBUG_GPU_RESOURCE
  //    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1RenderTarget = [" << std::hex << *ppRenderTarget << L"]" <<
  //    std::dec;
  //#endif
  return hr;
}

HRESULT touchmind::util::BitmapHelper::CreateBitmapFromWICBitmapSource(IN IWICBitmapSource *pBitmapSource,
                                                                       OUT HBITMAP *pHBitmap) {
  HRESULT hr = S_OK;

  UINT width = 0;
  UINT height = 0;

  void *pvImageBits = nullptr;

  WICPixelFormatGUID pixelFormat = {0};
  hr = pBitmapSource->GetPixelFormat(&pixelFormat);

  if (SUCCEEDED(hr)) {
    hr = (pixelFormat == GUID_WICPixelFormat32bppPBGRA) ? S_OK : E_FAIL;
  }

  if (SUCCEEDED(hr)) {
    hr = pBitmapSource->GetSize(&width, &height);
  }

  if (SUCCEEDED(hr)) {
    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -(LONG)height;
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;

    HDC hdcScreen = GetDC(nullptr);

    hr = hdcScreen ? S_OK : E_FAIL;

    if (SUCCEEDED(hr)) {
      if (pHBitmap) {
        DeleteObject(pHBitmap);
      }

      *pHBitmap = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, nullptr, 0);
      ReleaseDC(nullptr, hdcScreen);

      hr = *pHBitmap ? S_OK : E_FAIL;
    }
  }

  UINT cbStride = 0;
  if (SUCCEEDED(hr)) {
    hr = UIntMult(width, sizeof(ARGB), &cbStride);
  }

  UINT cbImage = 0;
  if (SUCCEEDED(hr)) {
    hr = UIntMult(cbStride, height, &cbImage);
  }

  if (SUCCEEDED(hr)) {
    hr = pBitmapSource->CopyPixels(nullptr, cbStride, cbImage, reinterpret_cast<BYTE *>(pvImageBits));
  }

  if (FAILED(hr)) {
    DeleteObject(*pHBitmap);
    *pHBitmap = nullptr;
  }

  return hr;
}

HRESULT touchmind::util::BitmapHelper::CreateBitmapFromHBITMAP(IN IWICImagingFactory *pWICFactory, IN HBITMAP hBitmap,
                                                               OUT IWICBitmap **ppWICBitmap) {
  HRESULT hr = pWICFactory->CreateBitmapFromHBITMAP(hBitmap, nullptr, WICBitmapUseAlpha, ppWICBitmap);
  //#ifdef DEBUG_GPU_RESOURCE
  //    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IWICBitmap = [" << std::hex << *ppWICBitmap << L"]" << std::dec;
  //#endif
  return hr;
}
