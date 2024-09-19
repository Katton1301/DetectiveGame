#include"pbr_shader.hpp"

static void getMainPBRShader( HShader * const pShader )
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

PBRShader::PBRShader()
{
    initMainShader();
    initEquirectangularToCubemapShader();
    initIrradianceShader();
    initPrefilterShader();
    initBRDFShader();
    initBackShader();
}

void PBRShader::initMainShader()
{
    getMainPBRShader(&m_pbrShader);
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

void PBRShader::initEquirectangularToCubemapShader()
{
    getEquirectangularToCubemapShader(&m_equirectangularToCubemap);
}

void PBRShader::initIrradianceShader()
{
    getIrradianceShader(&m_irradianceShader);
}

void PBRShader::initPrefilterShader()
{
    getPrefilterShader(&m_prefilterShader);
}

void PBRShader::initBRDFShader()
{
    getBRDFShader(&m_brdfShader);
}

void PBRShader::initBackShader()
{
    getBackShader(&m_backgroundShader);
}
HShader PBRShader::MainShader() const
{
    return m_pbrShader;
}

HShader PBRShader::EtCShader() const
{
    return m_equirectangularToCubemap;
}

HShader PBRShader::IrradianceShader() const
{
    return m_irradianceShader;
}

HShader PBRShader::PrefilterShader() const
{
    return m_prefilterShader;
}

HShader PBRShader::BRDFShader() const
{
    return m_brdfShader;
}

HShader PBRShader::BackShader() const
{
    return m_backgroundShader;
}
