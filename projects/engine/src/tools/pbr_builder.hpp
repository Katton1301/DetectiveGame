#pragma once
#include <shaders/shader_collection.hpp>
#include <textures/texture.hpp>
#include <common/vertices.hpp>
#include <camera/camera.hpp>
#include <vector>

void DrawSphere( PBRTextures const& _textures, TSphereVertices const & _sphereVertices, glm::mat4 const & _model );

