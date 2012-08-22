#ifndef TOUCHMIND_FILTER_GAUSSFILTER_H_
#define TOUCHMIND_FILTER_GAUSSFILTER_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace filter
{

class GaussFilter
{
private:
    static const int NUMBER_OF_WEIGHTS;
    static float gaussWeight[];
    static float tapOffsetX[];
    static float tapOffsetY[];

    CComPtr<ID3D10Effect> m_pGaussEffect;
    ID3D10EffectTechnique *m_pTechniqueNoRefForTexture;
    ID3D10EffectShaderResourceVariable *m_pDiffuseVariableNoRefForTexture;

protected:
    void _UpdateWeight(float dispersion);
    void _UpdateTapOffset(int width, int height);
    HRESULT _SetGaussParameters(int width, int height);

public:
    GaussFilter(void);
    virtual ~GaussFilter(void);
    void DiscardResources();
    HRESULT ApplyFilter(
        IN  touchmind::Context *pContext,
        IN  ID3D10Texture2D *pSourceTexture,
        OUT ID3D10Texture2D **ppOutputTexture);
    FLOAT GetOffset() const {
        return 10.0f;
    }
};

} // filter
} // touchmind

#endif // TOUCHMIND_FILTER_GAUSSFILTER_H_