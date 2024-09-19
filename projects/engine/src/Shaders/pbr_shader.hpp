#pragma once
#include"shader.hpp"

class PBRShader
{
public:
    PBRShader();
    ~PBRShader() = default;
    HShader MainShader() const;
    HShader EtCShader() const;
    HShader IrradianceShader() const;
    HShader PrefilterShader() const;
    HShader BRDFShader() const;
    HShader BackShader() const;

private:
    void initMainShader();
    void initEquirectangularToCubemapShader();
    void initIrradianceShader();
    void initPrefilterShader();
    void initBRDFShader();
    void initBackShader();

private:
    HShader m_pbrShader = nullptr;
    HShader m_equirectangularToCubemap = nullptr;
    HShader m_irradianceShader = nullptr;
    HShader m_prefilterShader = nullptr;
    HShader m_brdfShader = nullptr;
    HShader m_backgroundShader = nullptr;
};
