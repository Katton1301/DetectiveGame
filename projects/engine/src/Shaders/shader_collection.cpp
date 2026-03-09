#include <shaders/shader_collection.hpp>


static void getMainTShaderCollection( HShader * const pShader )
{
    if(!pShader)
    {
        return;
    }
    static HShader shader = new TShader(
#include"core/pbr_shader.vs"
        ,
#include"core/pbr_shader.frag"
        );
    *pShader = shader;
}

static void getEquirectangularToCubemapShader( HShader * const pShader )
{
    if(!pShader)
    {
        return;
    }
    static HShader shader = new TShader(
#include"core/cubemap_shader.vs"
        ,
#include"core/equirectangular_to_cubemap_shader.frag"
        );
    *pShader = shader;
}

static void getIrradianceShader( HShader * const pShader )
{
    if(!pShader)
    {
        return;
    }
    static HShader shader = new TShader(
#include"core/cubemap_shader.vs"
        ,
#include"core/irradiance_convolution.frag"
        );
    *pShader = shader;
}

static void getPrefilterShader( HShader * const pShader )
{
    if(!pShader)
    {
        return;
    }
    static HShader shader = new TShader(
#include"core/cubemap_shader.vs"
        ,
#include"core/prefilter_shader.frag"
        );
    *pShader = shader;
}

static void getBRDFShader( HShader * const pShader )
{
    if(!pShader)
    {
        return;
    }
    static HShader shader = new TShader(
#include"core/brdf_shader.vs"
        ,
#include"core/brdf_shader.frag"
        );
    *pShader = shader;
}

static void getBackShader( HShader * const pShader )
{
    if(!pShader)
    {
        return;
    }
    static HShader shader = new TShader(
#include"core/back_shader.vs"
        ,
#include"core/back_shader.frag"
        );
    *pShader = shader;
}


TShaderCollection const & TShaderCollection::Instance()
{
    static TShaderCollection singleton;
    return singleton;
}


TShaderCollection::TShaderCollection()
{
    initMainShader();
    initEquirectangularToCubemapShader();
    initIrradianceShader();
    initPrefilterShader();
    initBRDFShader();
    initBackShader();
}

void TShaderCollection::updateCameraMatrix( std::shared_ptr<ICamera> const & _cameraCptr ) const
{
    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    BackShader()->Use();
    BackShader()->setInt("environmentMap", 0);
    BackShader()->setMat4("projection", *_cameraCptr->ProjectionCptr());
    MainShader()->Use();
    MainShader()->setMat4("projection", *_cameraCptr->ProjectionCptr());
    MainShader()->setMat4("view", _cameraCptr->ViewMatrix());
    MainShader()->setVec3("camPos", _cameraCptr->Position() );
}

void TShaderCollection::initMainShader()
{
    getMainTShaderCollection(&m_pbrShader);
    MainShader()->Use();
    MainShader()->setInt("irradianceMap", 0);
    MainShader()->setInt("prefilterMap", 1);
    MainShader()->setInt("brdfLUT", 2);
    MainShader()->setInt("albedoMap", 3);
    MainShader()->setInt("normalMap", 4);
    MainShader()->setInt("metallicMap", 5);
    MainShader()->setInt("roughnessMap", 6);
    MainShader()->setInt("aoMap", 7);
}

void TShaderCollection::initEquirectangularToCubemapShader()
{
    getEquirectangularToCubemapShader(&m_equirectangularToCubemap);
}

void TShaderCollection::initIrradianceShader()
{
    getIrradianceShader(&m_irradianceShader);
}

void TShaderCollection::initPrefilterShader()
{
    getPrefilterShader(&m_prefilterShader);
}

void TShaderCollection::initBRDFShader()
{
    getBRDFShader(&m_brdfShader);
}

void TShaderCollection::initBackShader()
{
    getBackShader(&m_backgroundShader);
}
HShader TShaderCollection::MainShader() const
{
    return m_pbrShader;
}

HShader TShaderCollection::EtCShader() const
{
    return m_equirectangularToCubemap;
}

HShader TShaderCollection::IrradianceShader() const
{
    return m_irradianceShader;
}

HShader TShaderCollection::PrefilterShader() const
{
    return m_prefilterShader;
}

HShader TShaderCollection::BRDFShader() const
{
    return m_brdfShader;
}

HShader TShaderCollection::BackShader() const
{
    return m_backgroundShader;
}
