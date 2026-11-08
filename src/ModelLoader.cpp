// Author: Matt Hejtman, login: HEJ0094
#include "ModelLoader.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>

LoadedModel ModelLoader::load(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices);
    
    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Unable to load model " << path << ": " << importer.GetErrorString() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::size_t slash = path.find_last_of("/\\");
    const std::string directory = slash == std::string::npos ? "" : path.substr(0, slash + 1);

    LoadedModel result;
    result.boundsMin = glm::vec3(1e30f);
    result.boundsMax = glm::vec3(-1e30f);

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
        std::vector<float> data;
        data.reserve(static_cast<std::size_t>(mesh->mNumFaces) * 3 * 8);

        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            const aiFace& face = mesh->mFaces[f];

            if (face.mNumIndices != 3) {
                continue;
            }

            for (unsigned int k = 0; k < 3; ++k) {
                const unsigned int i = face.mIndices[k];
                const aiVector3D& p = mesh->mVertices[i];
                const aiVector3D n = mesh->HasNormals() ? mesh->mNormals[i] : aiVector3D(0.0f, 1.0f, 0.0f);
                const aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0.0f);
                data.insert(data.end(), {p.x, p.y, p.z, n.x, n.y, n.z, uv.x, uv.y});

                result.boundsMin = glm::min(result.boundsMin, glm::vec3(p.x, p.y, p.z));
                result.boundsMax = glm::max(result.boundsMax, glm::vec3(p.x, p.y, p.z));
            }
        }

        if (data.empty()) {
            continue;
        }

        ModelPart part;
        part.model = std::make_unique<Model>(data.data(), data.size(), 8);

        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texture;
        
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texture) == AI_SUCCESS) {
            std::string file = texture.C_Str();
            std::replace(file.begin(), file.end(), '\\', '/');
            part.texturePath = directory + file;
        }

        aiColor3D color(1.0f, 1.0f, 1.0f);

        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            part.diffuseColor = glm::vec3(color.r, color.g, color.b);
        }

        result.parts.push_back(std::move(part));
    }

    std::cout << "Loaded " << path << ": " << result.parts.size() << " parts, bounds (";
    std::cout << result.boundsMin.x << ", " << result.boundsMin.y << ", " << result.boundsMin.z << ") - (";
    std::cout << result.boundsMax.x << ", " << result.boundsMax.y << ", " << result.boundsMax.z << ")\n";
    
    return result;
}
