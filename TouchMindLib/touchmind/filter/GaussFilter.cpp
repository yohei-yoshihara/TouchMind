#include "StdAfx.h"
#include "Resource.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/Context.h"
#include "touchmind/filter/GaussFilter.h"

const int touchmind::filter::GaussFilter::NUMBER_OF_WEIGHTS = 6;
float touchmind::filter::GaussFilter::gaussWeight[NUMBER_OF_WEIGHTS];
float touchmind::filter::GaussFilter::tapOffsetX[NUMBER_OF_WEIGHTS];
float touchmind::filter::GaussFilter::tapOffsetY[NUMBER_OF_WEIGHTS];

touchmind::filter::GaussFilter::GaussFilter(void) :
    m_pGaussEffect(nullptr),
    m_pTechniqueNoRefForTexture(nullptr),
    m_pDiffuseVariableNoRefForTexture(nullptr)
{
}

touchmind::filter::GaussFilter::~GaussFilter(void)
{
    DiscardResources();
}

void touchmind::filter::GaussFilter::DiscardResources()
{
}

void touchmind::filter::GaussFilter::_UpdateWeight(float dispersion)
{
    float total = 0;
    for (int i = 0; i < NUMBER_OF_WEIGHTS; ++i) {
        gaussWeight[i] = expf(-0.5f * (float)(i * i) / dispersion);
        if (i == 0) {
            total += gaussWeight[i];
        } else {
            total += /* 2.0f */ 3.0f * gaussWeight[i];
        }
    }
    for (int i = 0; i < NUMBER_OF_WEIGHTS; ++i) {
        gaussWeight[i] /= total;
    }
}

void touchmind::filter::GaussFilter::_UpdateTapOffset(int width, int height)
{
    for (int i = 0; i < NUMBER_OF_WEIGHTS; ++i) {
        tapOffsetX[i] = 1.0f / width * i * 1.0f; // 2.0f;
        tapOffsetY[i] = 1.0f / height * i * 1.0f; // 2.0f;
    }
}

HRESULT touchmind::filter::GaussFilter::_SetGaussParameters(int width, int height)
{
    HRESULT hr = S_OK;
    _UpdateWeight(5.0f * 5.0f);
    _UpdateTapOffset(width, height);

    ID3D10EffectScalarVariable* pVarWidth = m_pGaussEffect->GetVariableByName("WIDTH")->AsScalar();
    hr = pVarWidth != nullptr ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = pVarWidth->SetFloat((float) width);
    }

    ID3D10EffectScalarVariable* pVarHeight = m_pGaussEffect->GetVariableByName("HEIGHT")->AsScalar();
    hr = pVarHeight != nullptr ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = pVarHeight->SetFloat((float) height);
    }

    ID3D10EffectScalarVariable* pVarWeight = m_pGaussEffect->GetVariableByName("WEIGHT")->AsScalar();
    hr = pVarWeight != nullptr ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = pVarWeight->SetFloatArray(gaussWeight, 0, NUMBER_OF_WEIGHTS);
    }

    ID3D10EffectScalarVariable* pVarTapOffsetX = m_pGaussEffect->GetVariableByName("TAPOFFSET_X")->AsScalar();
    pVarTapOffsetX->SetFloatArray(tapOffsetX, 0, NUMBER_OF_WEIGHTS);
    ID3D10EffectScalarVariable* pVarTapOffsetY = m_pGaussEffect->GetVariableByName("TAPOFFSET_Y")->AsScalar();
    pVarTapOffsetY->SetFloatArray(tapOffsetY, 0, NUMBER_OF_WEIGHTS);

    return S_OK;
}

HRESULT touchmind::filter::GaussFilter::ApplyFilter(
    IN touchmind::Context* pContext,
    IN ID3D10Texture2D* pSourceTexture,
    OUT ID3D10Texture2D** ppOutputTexture)
{
    HRESULT hr = S_OK;

    ID3D10Device* pDevice = pContext->GetD3D10Device();

    pContext->PushStates();

    D3D10_TEXTURE2D_DESC sourceTextureDesc;
    pSourceTexture->GetDesc(&sourceTextureDesc);

    CComPtr<ID3D10ShaderResourceView> pSourceShaderRV = nullptr;
    CComPtr<ID3D10Texture2D> pTemporaryTexture = nullptr;
    CComPtr<ID3D10RenderTargetView> pTemporaryTextureRV = nullptr;
    CComPtr<ID3D10ShaderResourceView> pTemporaryShaderRV = nullptr;
    CComPtr<ID3D10RenderTargetView> pOutputTextureRV = nullptr;

    if (m_pGaussEffect == nullptr) {
        // Load pixel shader
        CHK_RES(m_pGaussEffect,
                pContext->LoadResourceShader(
                    pDevice,
                    MAKEINTRESOURCE(IDR_GAUSS_SHADER),
                    &m_pGaussEffect
                ));
        // Obtain the technique
        CHK_FATAL_NULL(m_pTechniqueNoRefForTexture = m_pGaussEffect->GetTechniqueByName("Shader"));
        CHK_FATAL_NULL(m_pDiffuseVariableNoRefForTexture = m_pGaussEffect->GetVariableByName("texDiffuse")->AsShaderResource());
    }
    CHK_HR(_SetGaussParameters(sourceTextureDesc.Width, sourceTextureDesc.Height));
    CHK_RES(pTemporaryTexture,
            pContext->CreateTexture2D(sourceTextureDesc.Width, sourceTextureDesc.Height, &pTemporaryTexture));
    CHK_HR(pContext->CreateTexture2D(sourceTextureDesc.Width, sourceTextureDesc.Height, ppOutputTexture));

    // <<< PASS 0 >>>
    D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    CHK_RES(pSourceShaderRV, pDevice->CreateShaderResourceView(pSourceTexture, nullptr, &pSourceShaderRV));

    D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
    rtDesc.Format = sourceTextureDesc.Format;
    rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
    rtDesc.Texture2D.MipSlice = 0;
    CHK_RES(pTemporaryTextureRV, pDevice->CreateRenderTargetView(pTemporaryTexture, &rtDesc, &pTemporaryTextureRV));

    pContext->SetIndexBuffer();
    m_pDiffuseVariableNoRefForTexture->SetResource(pSourceShaderRV);

    ID3D10RenderTargetView *viewList[1] = {pTemporaryTextureRV};
    pDevice->OMSetRenderTargets(1, viewList, nullptr);

    D3D10_VIEWPORT viewport;
    viewport.Width = sourceTextureDesc.Width;
    viewport.Height = sourceTextureDesc.Height;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    pDevice->RSSetViewports(1, &viewport);
    CHK_HR(m_pTechniqueNoRefForTexture->GetPassByIndex(0)->Apply(0));

    pDevice->DrawIndexed(6, 0, 0);
    pDevice->Flush();

    m_pDiffuseVariableNoRefForTexture->SetResource(nullptr);
    ID3D10ShaderResourceView* resViewList[1] = {nullptr};
    pDevice->PSSetShaderResources(0, 1, resViewList);
    viewList[0] = nullptr;
    pDevice->OMSetRenderTargets(1, viewList, nullptr);
    // <<< PASS 0 END >>>

    // <<< PASS 1 START >>>
    CHK_RES(pTemporaryShaderRV, pDevice->CreateShaderResourceView(pTemporaryTexture, nullptr, &pTemporaryShaderRV));
    CHK_RES(pOutputTextureRV, pDevice->CreateRenderTargetView(*ppOutputTexture, &rtDesc, &pOutputTextureRV));

    m_pDiffuseVariableNoRefForTexture->SetResource(pTemporaryShaderRV);

    viewList[0] = pOutputTextureRV;
    pDevice->OMSetRenderTargets(1, viewList, nullptr);

    CHK_HR(m_pTechniqueNoRefForTexture->GetPassByIndex(1)->Apply(0));

    pDevice->DrawIndexed(6, 0, 0);
    pDevice->Flush();

    m_pDiffuseVariableNoRefForTexture->SetResource(nullptr);
    pDevice->PSSetShaderResources(0, 1, resViewList);
    viewList[0] = nullptr;
    pDevice->OMSetRenderTargets(1, viewList, nullptr);
    // <<< PASS 1 END >>>
    /*
    SafeRelease(&pOutputTextureRV);
    SafeRelease(&pTemporaryShaderRV);
    SafeRelease(&pTemporaryTextureRV);
    SafeRelease(&pTemporaryTexture);
    SafeRelease(&pSourceShaderRV);
    SafeRelease(&pSourceTexture);
    */
    pContext->PopStates();
    return hr;
}
