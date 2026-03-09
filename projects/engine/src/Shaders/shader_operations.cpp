#include"shader_operations.hpp"

std::string loadShader( std::string const & shaderPath )
{
    std::string shaderCode;
    std::ifstream shaderFile;
    // Удостоверимся, что ifstream объекты могут выкидывать исключения
    shaderFile.exceptions(std::ifstream::badbit);
    try
    {
        // Открываем файл
        shaderFile.open(shaderPath);
        std::stringstream shaderStream;
        // Считываем данные в потоки
        shaderStream << shaderFile.rdbuf();
        // Закрываем файл
        shaderFile.close();
        // Преобразовываем потоки в массив GLchar
        shaderCode = shaderStream.str();
    }
    catch(std::ifstream::failure & e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    return shaderCode;
}

HShader createCustomShader( std::string const & vertexPath, std::string const & fragmentPath )
{
    HShader customShader = nullptr;
    std::string vertextCode = loadShader(vertexPath);
    std::string fragmentCode = loadShader(fragmentPath);
    if(!vertextCode.empty() && !fragmentCode.empty())
    {
        customShader = new TShader(vertextCode, fragmentCode);
    }
    return customShader;
}

HShader createSimpleModelShader()
{
    static HShader simpleShader = nullptr;

    if (!simpleShader)
    {
        simpleShader = new TShader(
            #include"core/simple_model_shader.vs"
            ,
            #include"core/simple_model_shader.frag"
        );
    }

    return simpleShader;
}

HShader createMirrorShader()
{
    static HShader mirrorShader = nullptr;

    if (!mirrorShader)
    {
        mirrorShader = new TShader(
            #include"core/mirror_shader.vs"
                        ,
            #include"core/mirror_shader.frag"
        );
    }
    mirrorShader->setInt("reflectionTexture", 0);
    mirrorShader->setFloat("metallic", 1.0f);
    mirrorShader->setFloat("roughness", 0.02f);
    mirrorShader->setVec3("albedo", glm::vec3(1.0f, 1.0f, 1.0f));

    return mirrorShader;
}
