#pragma once
#include <shaders/shader.hpp>
#include <camera/camera_interface.hpp>

class TShaderCollection
{
public:
    static TShaderCollection const & Instance();

    void updateCameraMatrix( std::shared_ptr<ICamera> const & _cameraCptr ) const;

    HShader MainShader() const;
    HShader EtCShader() const;
    HShader IrradianceShader() const;
    HShader PrefilterShader() const;
    HShader BRDFShader() const;
    HShader BackShader() const;

private:
    TShaderCollection();
    ~TShaderCollection() = default;
    void initMainShader();
    void initEquirectangularToCubemapShader();
    void initIrradianceShader();
    void initPrefilterShader();
    void initBRDFShader();
    void initBackShader();

    TShaderCollection(TShaderCollection const&) = delete;
    TShaderCollection& operator= (TShaderCollection const&) = delete;

private:
    HShader m_pbrShader = nullptr;
    HShader m_equirectangularToCubemap = nullptr;
    HShader m_irradianceShader = nullptr;
    HShader m_prefilterShader = nullptr;
    HShader m_brdfShader = nullptr;
    HShader m_backgroundShader = nullptr;
};
