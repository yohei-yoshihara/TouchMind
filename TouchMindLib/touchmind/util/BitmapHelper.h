#ifndef TOUCHMIND_UTIL_BITMAPHELPER_H_
#define TOUCHMIND_UTIL_BITMAPHELPER_H_

namespace touchmind {
  namespace util {

#ifndef ARGB
    typedef DWORD ARGB;
#endif

    class BitmapHelper {
    private:
      BitmapHelper() {
      }

    public:
      static HRESULT CreateBitmap(IN IWICImagingFactory *pWICFactory, IN UINT width, IN UINT height,
                                  OUT IWICBitmap **ppWICBitmap);
      static HRESULT CreateBitmapRenderTarget(IN IWICBitmap *pWICBitmap, IN ID2D1Factory *pD2DFactory,
                                              OUT ID2D1RenderTarget **ppRenderTarget);
      static HRESULT CreateBitmapFromWICBitmapSource(IN IWICBitmapSource *pBitmapSource, OUT HBITMAP *pHBitmap);
      static HRESULT CreateBitmapFromHBITMAP(IN IWICImagingFactory *pWICFactory, IN HBITMAP hBitmap,
                                             OUT IWICBitmap **ppWICBitmap);
    };

  } // util
} // touchmind

#endif // TOUCHMIND_UTIL_BITMAPHELPER_H_