// Author: Matěj Hejtman, login: HEJ0094
#include "Rendering/ModelLoader.h"
#include "Rendering/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>

// Material z .mtl. Ambient (Ka) se NEPREBIRA: Blender exportuje skoro vzdy Ka = 1,
// takze by model vypadal jako bez osvetleni. Ambient se nastavuje v kodu sceny.
static Material readMaterial(const aiMaterial* source) {
    Material material;

    aiColor3D specular(1.0f, 1.0f, 1.0f);
    if (source->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
        material.specular = glm::vec3(specular.r, specular.g, specular.b);
    }

    float shininess = 0.0f;
    if (source->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS && shininess >= 1.0f) {
        material.shininess = shininess;     // Ns < 1 by znamenalo odlesk pres cely povrch -> vychozi
    }

    return material;
}

LoadedModel ModelLoader::load(const std::string& path, bool mergeColors) {
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

    const VertexLayout layout = mergeColors ? VertexLayout::positionNormalUvColor() : VertexLayout::positionNormalUv();
    std::vector<float> merged;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiColor3D color(1.0f, 1.0f, 1.0f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        std::vector<float> data;
        data.reserve(static_cast<std::size_t>(mesh->mNumFaces) * 3 * layout.floatsPerVertex);

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
                if (mergeColors) {
                    data.insert(data.end(), {color.r, color.g, color.b});
                }

                result.boundsMin = glm::min(result.boundsMin, glm::vec3(p.x, p.y, p.z));
                result.boundsMax = glm::max(result.boundsMax, glm::vec3(p.x, p.y, p.z));
            }
        }

        if (data.empty()) {
            continue;
        }

        if (mergeColors) {
            merged.insert(merged.end(), data.begin(), data.end());
            continue;
        }

        ModelPart part;
        part.model = std::make_unique<Model>(data.data(), data.size(), layout);
        part.diffuseColor = glm::vec3(color.r, color.g, color.b);
        part.material = readMaterial(material);

        aiString texture;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texture) == AI_SUCCESS) {
            std::string file = texture.C_Str();
            std::replace(file.begin(), file.end(), '\\', '/');
            part.texturePath = directory + file;
        }

        result.parts.push_back(std::move(part));
    }

    if (mergeColors && !merged.empty()) {
        ModelPart part;
        part.model = std::make_unique<Model>(merged.data(), merged.size(), layout);
        result.parts.push_back(std::move(part));
    }

    std::cout << "Loaded " << path << ": " << result.parts.size() << " parts, bounds (";
    std::cout << result.boundsMin.x << ", " << result.boundsMin.y << ", " << result.boundsMin.z << ") - (";
    std::cout << result.boundsMax.x << ", " << result.boundsMax.y << ", " << result.boundsMax.z << ")\n";

    return result;
}
