#ifndef TOUCHMIND_PRINT_XPSDWRITETEXTRENDERER_H_
#define TOUCHMIND_PRINT_XPSDWRITETEXTRENDERER_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace print
{

class XpsDWriteTextRenderer : public IDWriteTextRenderer
{
private:
    ULONG m_refCount;
    std::map<UINT_PTR, IXpsOMFontResource*> m_fontMap;
    IXpsOMObjectFactory* m_pXpsFactory;
    IXpsOMCanvas* m_pXpsCanvas;
    IXpsOMPartResources* m_pXpsResources;
    IXpsOMDictionary* m_pXpsDictionary;
    std::map<COLORREF, std::wstring> m_brushKeyMap;
    std::vector<std::wstring> m_temporaryFileNames;
protected:
    static HRESULT ClusterMapToMappingArray(
        IN const UINT16 *clusterMap,
        IN UINT32 mapLen,
        IN UINT32 glyphsArrayLen,
        OUT std::vector<XPS_GLYPH_MAPPING> glyphMapping);
    HRESULT FindOrCreateFontResource(IDWriteFontFace* fontFace, IXpsOMFontResource** ppXpsFontResource);
public:
    static HRESULT CreateInstance(
        IN IXpsOMObjectFactory* xpsFactory,
        OUT XpsDWriteTextRenderer** ppXpsDWriteTextRenderer);

    XpsDWriteTextRenderer();
    XpsDWriteTextRenderer(IN IXpsOMObjectFactory* xpsFactory);
    virtual ~XpsDWriteTextRenderer(void);
    void DiscardResources();

    void SetXpsCanvas(IXpsOMCanvas *pXpsCanvas) {
        m_pXpsCanvas = pXpsCanvas;
    }
    IXpsOMCanvas* GetXpsCanvas() const {
        return m_pXpsCanvas;
    }
    void SetXpsResources(IXpsOMPartResources *pXpsResources) {
        m_pXpsResources = pXpsResources;
    }
    IXpsOMPartResources* GetXpsResources() const {
        m_pXpsResources;
    }
    void SetXpsDictionary(IXpsOMDictionary *pXpsDictionary) {
        m_pXpsDictionary = pXpsDictionary;
    }
    IXpsOMDictionary* GetXpsDictionary() const {
        m_pXpsDictionary;
    }

    // IUnknown interface
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDWritePixelSnapping methods
    STDMETHOD(IsPixelSnappingDisabled)(
        void* clientDrawingContext,
        BOOL* isDisabled
    );

    STDMETHOD(GetCurrentTransform)(
        void* clientDrawingContext,
        DWRITE_MATRIX* transform
    );

    STDMETHOD(GetPixelsPerDip)(
        void* clientDrawingContext,
        FLOAT* pixelsPerDip
    );

    // IDWriteTextRenderer interface
    STDMETHOD(DrawGlyphRun)(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        DWRITE_GLYPH_RUN const* glyphRun,
        DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        IUnknown* clientDrawingEffect
    );

    STDMETHOD(DrawUnderline)(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_UNDERLINE const* underline,
        IUnknown* clientDrawingEffect
    );

    STDMETHOD(DrawStrikethrough)(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_STRIKETHROUGH const* strikethrough,
        IUnknown* clientDrawingEffect
    );

    STDMETHOD(DrawInlineObject)(
        void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect
    );
    HRESULT LookupBrush(IN IUnknown *drawingEffect, OUT D2D1_COLOR_F *pColor, OUT std::wstring & brushKey);
    HRESULT AddLinePath(
        const XPS_POINT *beginPoint,
        const XPS_POINT *endPoint,
        FLOAT thickness,
        IUnknown* clientDrawingEffect);
    //HRESULT CreateRootCanvasAndResources();
    HRESULT GenerateNewFontPartUri( IOpcPartUri** ppPartUri );
    void DeleteTemporaryFiles();
};

} // print
} // touchmind

#endif // TOUCHMIND_PRINT_XPSDWRITETEXTRENDERER_H_
