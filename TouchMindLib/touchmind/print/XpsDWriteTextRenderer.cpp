#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/util/Utils.h"
#include "XpsDWriteTextRenderer.h"
#include <Strsafe.h>

touchmind::print::XpsDWriteTextRenderer::XpsDWriteTextRenderer()
    : m_refCount(1)
    , m_pXpsFactory(nullptr)
    , m_pXpsCanvas(nullptr)
    , m_pXpsResources(nullptr)
    , m_pXpsDictionary(nullptr) {
}

touchmind::print::XpsDWriteTextRenderer::XpsDWriteTextRenderer(IN IXpsOMObjectFactory *xpsFactory)
    : m_refCount(1)
    , m_pXpsFactory(xpsFactory)
    , m_pXpsCanvas(nullptr)
    , m_pXpsResources(nullptr)
    , m_pXpsDictionary(nullptr) {
  m_pXpsFactory->AddRef();
}

touchmind::print::XpsDWriteTextRenderer::~XpsDWriteTextRenderer(void) {
  DiscardResources();
}

void touchmind::print::XpsDWriteTextRenderer::DiscardResources() {
  for (auto &kv : m_fontMap) {
    kv.second->Release();
  }
  m_fontMap.clear();
  SafeRelease(&m_pXpsFactory);
}

// IUnknown interface
STDMETHODIMP touchmind::print::XpsDWriteTextRenderer::QueryInterface(REFIID riid, void **ppvObject) {
  if (ppvObject == nullptr) {
    return E_POINTER;
  }
  *ppvObject = nullptr;

  if (IsEqualGUID(riid, __uuidof(IUnknown)) || IsEqualGUID(riid, __uuidof(IDWritePixelSnapping))
      || IsEqualGUID(riid, __uuidof(IDWriteTextRenderer))) {
    IUnknown *pUnk = static_cast<IUnknown *>(this);
    pUnk->AddRef();
    *ppvObject = pUnk;
    return S_OK;
  }
  return E_NOINTERFACE;
}

ULONG touchmind::print::XpsDWriteTextRenderer::AddRef() {
  return ++m_refCount;
}

ULONG touchmind::print::XpsDWriteTextRenderer::Release() {
  ULONG newCount = --m_refCount;
  if (newCount == 0)
    delete this;
  return newCount;
}

// IDWritePixelSnapping interface
STDMETHODIMP
touchmind::print::XpsDWriteTextRenderer::IsPixelSnappingDisabled(void * /* clientDrawingContext */, BOOL *isDisabled) {
  *isDisabled = FALSE;
  return S_OK;
}

STDMETHODIMP touchmind::print::XpsDWriteTextRenderer::GetCurrentTransform(void * /* clientDrawingContext */,
                                                                          DWRITE_MATRIX *transform) {
  transform->m11 = transform->m22 = 1.0;
  transform->m12 = transform->m21 = 0.0;
  transform->dx = transform->dy = 0.0;
  return S_OK;
}

STDMETHODIMP touchmind::print::XpsDWriteTextRenderer::GetPixelsPerDip(void * /* clientDrawingContext */,
                                                                      FLOAT *pixelsPerDip) {
  *pixelsPerDip = FLOAT(1.0);
  return S_OK;
}

// IDWriteTextRenderer interface
STDMETHODIMP
touchmind::print::XpsDWriteTextRenderer::DrawGlyphRun(void *clientDrawingContext, FLOAT baselineOriginX,
                                                      FLOAT baselineOriginY, DWRITE_MEASURING_MODE measuringMode,
                                                      IN const DWRITE_GLYPH_RUN *glyphRun,
                                                      IN const DWRITE_GLYPH_RUN_DESCRIPTION *glyphRunDescription,
                                                      IUnknown *clientDrawingEffect) {
  UNREFERENCED_PARAMETER(clientDrawingContext);
  UNREFERENCED_PARAMETER(measuringMode);

  HRESULT hr = S_OK;

  DWRITE_FONT_FACE_TYPE fontFaceType = glyphRun->fontFace->GetType();

  if (fontFaceType != DWRITE_FONT_FACE_TYPE_CFF && fontFaceType != DWRITE_FONT_FACE_TYPE_TRUETYPE
      && fontFaceType != DWRITE_FONT_FACE_TYPE_TRUETYPE_COLLECTION) {
    hr = S_OK;
  } else {
    const FLOAT positionScale = 100.0f / glyphRun->fontEmSize;

    IXpsOMGlyphsEditor *xpsGlyphsEditor = nullptr;
    XPS_GLYPH_INDEX *glyphIndexVector = nullptr;
    PWSTR pszUnicodeString = nullptr;

    IXpsOMFontResource *fontResource = nullptr;
    if (SUCCEEDED(hr)) {
      hr = FindOrCreateFontResource(glyphRun->fontFace, &fontResource);
    }

    IXpsOMGlyphs *xpsGlyphs = nullptr;
    if (SUCCEEDED(hr)) {
      hr = m_pXpsFactory->CreateGlyphs(fontResource, &xpsGlyphs);
    }

    IXpsOMVisualCollection *canvasVisuals = nullptr;
    if (SUCCEEDED(hr)) {
      hr = m_pXpsCanvas->GetVisuals(&canvasVisuals);
    }

    if (SUCCEEDED(hr)) {
      hr = canvasVisuals->Append(xpsGlyphs);
    }

    if (SUCCEEDED(hr)) {
      XPS_POINT glyphsBaselineOrigin = {baselineOriginX, baselineOriginY};
      hr = xpsGlyphs->SetOrigin(&glyphsBaselineOrigin);
    }

    if (SUCCEEDED(hr)) {
      hr = xpsGlyphs->SetFontRenderingEmSize(glyphRun->fontEmSize);
    }
    // TODO: ‹N“®’¼Œã‚¾‚ÆclientDrawingEffect‚ªnullptr‚É‚È‚éB
    if (SUCCEEDED(hr) && clientDrawingEffect != nullptr) {
      D2D1_COLOR_F colorf;
      std::wstring brushKey;
      hr = LookupBrush(clientDrawingEffect, &colorf, brushKey);
      if (SUCCEEDED(hr)) {
        hr = xpsGlyphs->SetFillBrushLookup(brushKey.c_str());
      }
    }

    if (SUCCEEDED(hr) && fontFaceType == DWRITE_FONT_FACE_TYPE_TRUETYPE_COLLECTION) {
      hr = xpsGlyphs->SetFontFaceIndex((SHORT)glyphRun->fontFace->GetIndex());
    }

    if (SUCCEEDED(hr)) {
      DWRITE_FONT_SIMULATIONS dwriteFontSim = glyphRun->fontFace->GetSimulations();

      if (dwriteFontSim & DWRITE_FONT_SIMULATIONS_BOLD) {
        if (dwriteFontSim & DWRITE_FONT_SIMULATIONS_OBLIQUE) {
          hr = xpsGlyphs->SetStyleSimulations(XPS_STYLE_SIMULATION_BOLDITALIC);
        } else {
          hr = xpsGlyphs->SetStyleSimulations(XPS_STYLE_SIMULATION_BOLD);
        }
      } else {
        if (dwriteFontSim & DWRITE_FONT_SIMULATIONS_OBLIQUE) {
          hr = xpsGlyphs->SetStyleSimulations(XPS_STYLE_SIMULATION_ITALIC);
        }
      }
    }

    if (SUCCEEDED(hr)) {
      hr = xpsGlyphs->GetGlyphsEditor(&xpsGlyphsEditor);
    }

    if (SUCCEEDED(hr)) {
      glyphIndexVector = (XPS_GLYPH_INDEX *)CoTaskMemAlloc(glyphRun->glyphCount * sizeof(XPS_GLYPH_INDEX));
      if (!glyphIndexVector) {
        hr = E_OUTOFMEMORY;
      }
    }

    if (SUCCEEDED(hr)) {
      for (UINT32 i = 0; i < glyphRun->glyphCount; ++i) {
        glyphIndexVector[i].index = glyphRun->glyphIndices[i];
        glyphIndexVector[i].advanceWidth = FLOAT(glyphRun->glyphAdvances[i] * positionScale);
        glyphIndexVector[i].horizontalOffset
            = (glyphRun->glyphOffsets != nullptr) ? FLOAT(glyphRun->glyphOffsets[i].advanceOffset * positionScale) : 0;
        glyphIndexVector[i].verticalOffset
            = (glyphRun->glyphOffsets != nullptr) ? FLOAT(glyphRun->glyphOffsets[i].ascenderOffset * positionScale) : 0;
      }

      hr = xpsGlyphsEditor->SetGlyphIndices((UINT32)glyphRun->glyphCount, glyphIndexVector);
    }

    if (SUCCEEDED(hr)) {
      hr = xpsGlyphsEditor->SetIsSideways(glyphRun->isSideways);
    }

    if (SUCCEEDED(hr)) {
      hr = xpsGlyphsEditor->SetBidiLevel(glyphRun->bidiLevel);
    }

    if (SUCCEEDED(hr) && glyphRunDescription->string && glyphRunDescription->stringLength > 0) {
      pszUnicodeString = (PWSTR)CoTaskMemAlloc((glyphRunDescription->stringLength + 1) * sizeof(WCHAR));
      if (!pszUnicodeString) {
        hr = E_OUTOFMEMORY;
      }

      if (SUCCEEDED(hr)) {
        hr = StringCchCopyN(pszUnicodeString, glyphRunDescription->stringLength + 1, glyphRunDescription->string,
                            glyphRunDescription->stringLength);
      }

      if (SUCCEEDED(hr)) {
        hr = xpsGlyphsEditor->SetUnicodeString(pszUnicodeString);
      }

      if (SUCCEEDED(hr) && glyphRunDescription->clusterMap) {
        std::vector<XPS_GLYPH_MAPPING> glyphMapping;
        hr = ClusterMapToMappingArray(glyphRunDescription->clusterMap, glyphRunDescription->stringLength,
                                      glyphRun->glyphCount, glyphMapping);

        if (SUCCEEDED(hr) && glyphMapping.size() > 0) {
          hr = xpsGlyphsEditor->SetGlyphMappings(static_cast<UINT32>(glyphMapping.size()), glyphMapping.data());
        }
      }
    }

    if (SUCCEEDED(hr)) {
      hr = xpsGlyphsEditor->ApplyEdits();
    }

    if (pszUnicodeString) {
      CoTaskMemFree(pszUnicodeString);
      pszUnicodeString = nullptr;
    }
    if (glyphIndexVector) {
      CoTaskMemFree(glyphIndexVector);
      glyphIndexVector = nullptr;
    }
    SafeRelease(&fontResource);
    SafeRelease(&xpsGlyphs);
    SafeRelease(&canvasVisuals);
    SafeRelease(&xpsGlyphsEditor);
  }

  return hr;
}

STDMETHODIMP touchmind::print::XpsDWriteTextRenderer::DrawUnderline(void *clientDrawingContext, FLOAT baselineOriginX,
                                                                    FLOAT baselineOriginY,
                                                                    DWRITE_UNDERLINE const *underline,
                                                                    IUnknown *clientDrawingEffect) {
  UNREFERENCED_PARAMETER(clientDrawingContext);

  XPS_POINT begin, end;

  begin.x = baselineOriginX;
  begin.y = baselineOriginY + underline->offset;
  end.x = baselineOriginX + underline->width;
  end.y = baselineOriginY + underline->offset;

  return AddLinePath(&begin, &end, underline->thickness, clientDrawingEffect);
}

STDMETHODIMP
touchmind::print::XpsDWriteTextRenderer::DrawStrikethrough(void *clientDrawingContext, FLOAT baselineOriginX,
                                                           FLOAT baselineOriginY,
                                                           DWRITE_STRIKETHROUGH const *strikethrough,
                                                           IUnknown *clientDrawingEffect) {
  UNREFERENCED_PARAMETER(clientDrawingContext);
  XPS_POINT begin, end;

  begin.x = baselineOriginX;
  begin.y = baselineOriginY + strikethrough->offset;
  end.x = baselineOriginX + strikethrough->width;
  end.y = baselineOriginY + strikethrough->offset;

  return AddLinePath(&begin, &end, strikethrough->thickness, clientDrawingEffect);
}

STDMETHODIMP
touchmind::print::XpsDWriteTextRenderer::DrawInlineObject(void * /* clientDrawingContext */, FLOAT /* originX */,
                                                          FLOAT /* originY */, IDWriteInlineObject * /* inlineObject */,
                                                          BOOL /* isSideways */, BOOL /* isRightToLeft */,
                                                          IUnknown * /* clientDrawingEffect */
                                                          ) {
  return S_OK;
}

HRESULT
touchmind::print::XpsDWriteTextRenderer::ClusterMapToMappingArray(IN const UINT16 *clusterMap, IN UINT32 mapLen,
                                                                  IN UINT32 glyphsArrayLen,
                                                                  OUT std::vector<XPS_GLYPH_MAPPING> glyphMapping) {
  HRESULT hr = S_OK;
  UINT32 i = 0;
  UINT32 mapPos = 0;
  while (mapPos < mapLen) {
    UINT32 codePointRangeLen = 1;
    UINT32 glyphIndexRangeLen = 1;
    while (mapPos + codePointRangeLen < mapLen && clusterMap[mapPos + codePointRangeLen] == clusterMap[mapPos]) {
      codePointRangeLen++;
    }
    if (mapPos + codePointRangeLen == mapLen) {
      glyphIndexRangeLen = glyphsArrayLen - clusterMap[mapPos];
    } else {
      glyphIndexRangeLen = clusterMap[mapPos + codePointRangeLen] - clusterMap[mapPos];
    }

    if (codePointRangeLen > 1 || glyphIndexRangeLen > 1) {
      XPS_GLYPH_MAPPING mappingEntry;
      mappingEntry.unicodeStringStart = mapPos;
      mappingEntry.unicodeStringLength = UINT16(codePointRangeLen);
      mappingEntry.glyphIndicesStart = clusterMap[mapPos];
      mappingEntry.glyphIndicesLength = UINT16(glyphIndexRangeLen);
      glyphMapping.push_back(mappingEntry);
      i++;
    }

    mapPos += codePointRangeLen;
  }
  return hr;
}

HRESULT touchmind::print::XpsDWriteTextRenderer::LookupBrush(IN IUnknown *drawingEffect, OUT D2D1_COLOR_F *pColor,
                                                             OUT std::wstring &brushKey) {
  HRESULT hr = S_OK;
  ID2D1SolidColorBrush &effect = *reinterpret_cast<ID2D1SolidColorBrush *>(drawingEffect);
  D2D1_COLOR_F colorf = effect.GetColor();
  COLORREF colorref;
  touchmind::ColorFToColorref(colorf, &colorref);
  if (m_brushKeyMap.count(colorref) > 0) {
    brushKey = m_brushKeyMap[colorref];
  } else {
    wchar_t buf[1024];
    swprintf_s(buf, 1024, L"textBrush_%06x", colorref);
    brushKey = buf;

    XPS_COLOR xpsColor;
    xpsColor.colorType = XPS_COLOR_TYPE_SRGB;
    xpsColor.value.sRGB.alpha = 0xff;
    xpsColor.value.sRGB.red = GetRValue(colorref);
    xpsColor.value.sRGB.green = GetGValue(colorref);
    xpsColor.value.sRGB.blue = GetBValue(colorref);
    IXpsOMSolidColorBrush *pSolidBrush = nullptr;
    hr = m_pXpsFactory->CreateSolidColorBrush(&xpsColor, nullptr, &pSolidBrush);
    if (SUCCEEDED(hr)) {
      hr = m_pXpsDictionary->Append(brushKey.c_str(), pSolidBrush);
    }
    if (SUCCEEDED(hr)) {
      m_brushKeyMap[colorref] = brushKey;
    }
    SafeRelease(&pSolidBrush);
  }
  *pColor = colorf;
  return hr;
}

HRESULT touchmind::print::XpsDWriteTextRenderer::AddLinePath(const XPS_POINT *beginPoint, const XPS_POINT *endPoint,
                                                             FLOAT thickness, IUnknown *clientDrawingEffect) {
  HRESULT hr = S_OK;

  IXpsOMVisualCollection *canvasVisuals = nullptr;
  IXpsOMPath *linePath = nullptr;
  IXpsOMGeometry *lineGeom = nullptr;
  IXpsOMGeometryFigureCollection *geomFigures = nullptr;
  IXpsOMGeometryFigure *lineFigure = nullptr;

  if (SUCCEEDED(hr)) {
    hr = m_pXpsFactory->CreatePath(&linePath);
  }
  if (SUCCEEDED(hr)) {
    hr = m_pXpsCanvas->GetVisuals(&canvasVisuals);
  }
  if (SUCCEEDED(hr)) {
    hr = canvasVisuals->Append(linePath);
  }

  if (SUCCEEDED(hr)) {
    D2D1_COLOR_F colorf;
    std::wstring brushKey;
    hr = LookupBrush(clientDrawingEffect, &colorf, brushKey);
    if (SUCCEEDED(hr)) {
      hr = linePath->SetStrokeBrushLookup(brushKey.c_str());
    }
  }
  if (SUCCEEDED(hr)) {
    hr = linePath->SetSnapsToPixels(TRUE);
  }
  if (SUCCEEDED(hr)) {
    hr = linePath->SetStrokeThickness(thickness);
  }

  if (SUCCEEDED(hr)) {
    hr = m_pXpsFactory->CreateGeometry(&lineGeom);
  }
  if (SUCCEEDED(hr)) {
    hr = linePath->SetGeometryLocal(lineGeom);
  }

  if (SUCCEEDED(hr)) {
    hr = m_pXpsFactory->CreateGeometryFigure(beginPoint, &lineFigure);
  }
  if (SUCCEEDED(hr)) {
    hr = lineGeom->GetFigures(&geomFigures);
  }
  if (SUCCEEDED(hr)) {
    hr = geomFigures->Append(lineFigure);
  }

  if (SUCCEEDED(hr)) {
    XPS_SEGMENT_TYPE segmType = XPS_SEGMENT_TYPE_LINE;
    FLOAT segmentData[2] = {endPoint->x, endPoint->y};
    BOOL segmStroke = TRUE;

    hr = lineFigure->SetSegments(1, 2, &segmType, segmentData, &segmStroke);
  }

  if (SUCCEEDED(hr)) {
    hr = lineFigure->SetIsClosed(FALSE);
  }
  if (SUCCEEDED(hr)) {
    hr = lineFigure->SetIsFilled(FALSE);
  }

  SafeRelease(&canvasVisuals);
  SafeRelease(&linePath);
  SafeRelease(&lineGeom);
  SafeRelease(&geomFigures);
  SafeRelease(&lineFigure);

  return hr;
}

HRESULT touchmind::print::XpsDWriteTextRenderer::CreateInstance(IN IXpsOMObjectFactory *xpsFactory,
                                                                OUT XpsDWriteTextRenderer **ppXpsDWriteTextRenderer) {
  HRESULT hr = S_OK;

  XpsDWriteTextRenderer *pResult = nullptr;

  pResult = new (std::nothrow) XpsDWriteTextRenderer(xpsFactory);
  if (!pResult) {
    hr = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hr)) {
    *ppXpsDWriteTextRenderer = pResult;
    (*ppXpsDWriteTextRenderer)->AddRef();
  }

  if (pResult) {
    pResult->Release();
    pResult = nullptr;
  }
  return hr;
}

HRESULT touchmind::print::XpsDWriteTextRenderer::FindOrCreateFontResource(IDWriteFontFace *fontFace,
                                                                          IXpsOMFontResource **ppXpsFontResource) {
  HRESULT hr = S_OK;

  *ppXpsFontResource = nullptr;

  UINT32 numFiles = 1;
  IDWriteFontFile *fontFile = nullptr;
  hr = fontFace->GetFiles(&numFiles, &fontFile);

  if (SUCCEEDED(hr)) {
    if (numFiles != 1) {
      hr = E_UNEXPECTED;
    }
  }

  IUnknown *pUnknown = nullptr;
  if (SUCCEEDED(hr)) {
    hr = fontFile->QueryInterface(__uuidof(IUnknown), reinterpret_cast<void **>(&pUnknown));
  }

  UINT_PTR fontFileKey = 0;
  if (SUCCEEDED(hr)) {
    fontFileKey = UINT_PTR(pUnknown);
    SafeRelease(&pUnknown);

    if (m_fontMap.count(fontFileKey) > 0) {
      *ppXpsFontResource = m_fontMap[fontFileKey];
      (*ppXpsFontResource)->AddRef();
    }
  }

  if (*ppXpsFontResource) {
    // nothing to do
  } else {
    IOpcPartUri *fontPartUri = nullptr;

    UINT64 readOffset = 0;

    wchar_t tempFileName[MAX_PATH];
    wchar_t tempPathBuffer[MAX_PATH];
    IStream *fontResourceStream = nullptr;
    if (SUCCEEDED(hr)) {
      if (SUCCEEDED(hr)) {
        DWORD ret = GetTempPath(MAX_PATH, tempPathBuffer);
        if (ret > MAX_PATH || ret == 0) {
          LOG(SEVERITY_LEVEL_ERROR) << L"GetTempPath failed";
          hr = E_FAIL;
        }
      }
      if (SUCCEEDED(hr)) {
        UINT ret = GetTempFileName(tempPathBuffer, _TEMPFILE_PREFIX, 0, tempFileName);
        if (ret == 0) {
          LOG(SEVERITY_LEVEL_ERROR) << L"GetTempFileName failed";
          hr = E_FAIL;
        }
      }

      if (SUCCEEDED(hr)) {
        hr = SHCreateStreamOnFile(tempFileName, STGM_READWRITE, &fontResourceStream);
        if (SUCCEEDED(hr)) {
          std::wstring _tempFileName(tempFileName);
          m_temporaryFileNames.push_back(_tempFileName);
        } else {
          DWORD lastError = GetLastError();
          LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"SHCreateStreamOnFile", lastError));
        }
      }
    }

    const void *fontFileRef = nullptr;
    UINT32 fontFileRefSize = 0;
    if (SUCCEEDED(hr)) {
      hr = fontFile->GetReferenceKey(&fontFileRef, &fontFileRefSize);
    }

    IDWriteFontFileLoader *fontFileLoader = nullptr;
    if (SUCCEEDED(hr)) {
      hr = fontFile->GetLoader(&fontFileLoader);
    }

    IDWriteFontFileStream *fontFileStream = nullptr;
    if (SUCCEEDED(hr)) {
      hr = fontFileLoader->CreateStreamFromKey(fontFileRef, fontFileRefSize, &fontFileStream);
      SafeRelease(&fontFileLoader);
    }

    UINT64 bytesLeft = 0;
    if (SUCCEEDED(hr)) {
      hr = fontFileStream->GetFileSize(&bytesLeft);
    }

    if (SUCCEEDED(hr)) {
      while (bytesLeft && SUCCEEDED(hr)) {
        const void *fragment = nullptr;
        PVOID fragmentContext = nullptr;
        bool bFragmentContextAcquired = false;
        UINT64 readSize = std::min(bytesLeft, (UINT64)16384);

        hr = fontFileStream->ReadFileFragment(&fragment, readOffset, readSize, &fragmentContext);

        if (SUCCEEDED(hr)) {
          bFragmentContextAcquired = true;
          hr = fontResourceStream->Write(fragment, ULONG(readSize), nullptr);
        }

        if (SUCCEEDED(hr)) {
          bytesLeft -= readSize;
          readOffset += readSize;
        }

        if (bFragmentContextAcquired) {
          fontFileStream->ReleaseFileFragment(fragmentContext);
        }
      }
      SafeRelease(&fontFileStream);
    }

    if (SUCCEEDED(hr)) {
      LARGE_INTEGER liZero = {0};
      hr = fontResourceStream->Seek(liZero, STREAM_SEEK_SET, nullptr);
    }

    if (SUCCEEDED(hr)) {
      hr = GenerateNewFontPartUri(&fontPartUri);
    }

    IXpsOMFontResource *pNewFontResource = nullptr;
    if (SUCCEEDED(hr)) {
      hr = m_pXpsFactory->CreateFontResource(fontResourceStream, XPS_FONT_EMBEDDING_OBFUSCATED, fontPartUri,
                                             FALSE, // isObfSourceStream
                                             &pNewFontResource);
    }

    IXpsOMFontResourceCollection *fontResources = nullptr;
    if (SUCCEEDED(hr)) {
      hr = m_pXpsResources->GetFontResources(&fontResources);
    }

    if (SUCCEEDED(hr)) {
      hr = fontResources->Append(pNewFontResource);
    }

    if (SUCCEEDED(hr)) {
      m_fontMap[fontFileKey] = pNewFontResource;
      m_fontMap[fontFileKey]->AddRef();

      *ppXpsFontResource = pNewFontResource;
      (*ppXpsFontResource)->AddRef();
    }

    SafeRelease(&fontResourceStream);
    SafeRelease(&fontFileLoader);
    SafeRelease(&fontFileStream);
    SafeRelease(&fontPartUri);
    SafeRelease(&pNewFontResource);
    SafeRelease(&fontResources);
  }

  SafeRelease(&fontFile);
  SafeRelease(&pUnknown);

  return hr;
}

HRESULT touchmind::print::XpsDWriteTextRenderer::GenerateNewFontPartUri(IOpcPartUri **ppPartUri) {
  HRESULT hr = S_OK;
  GUID guid;
  WCHAR guidString[128] = {0};
  WCHAR uriString[256] = {0};

  hr = CoCreateGuid(&guid);

  if (SUCCEEDED(hr)) {
    hr = StringFromGUID2(guid, guidString, ARRAYSIZE(guidString));
  }

  if (SUCCEEDED(hr)) {
    hr = StringCchCopy(uriString, ARRAYSIZE(uriString), L"/Resources/Fonts/");
  }

  if (SUCCEEDED(hr)) {
    hr = StringCchCatN(uriString, ARRAYSIZE(uriString), guidString + 1, wcslen(guidString) - 2);
  }

  if (SUCCEEDED(hr)) {
    hr = StringCchCat(uriString, ARRAYSIZE(uriString), L".odttf");
  }

  if (SUCCEEDED(hr)) {
    hr = m_pXpsFactory->CreatePartUri(uriString, ppPartUri);
  }

  return hr;
}

void touchmind::print::XpsDWriteTextRenderer::DeleteTemporaryFiles() {
  for (auto &fileName : m_temporaryFileNames) {
    DeleteFile(fileName.c_str());
  }
  m_temporaryFileNames.clear();
}
