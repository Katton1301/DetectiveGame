#pragma once
#include"shader.hpp"

std::string loadShader( std::string const & shaderPath );
HShader createCustomShader( std::string const & vertexPath, std::string const & fragmentPath );
