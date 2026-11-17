// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <array>
#include <string>

class CubeMap {
public:
    static constexpr const char* DEFAULT_DIRECTORY = "assets/textures/Skyboxes/skybox_default";
    static constexpr const char* DEFAULT_EXTENSION = ".png";

    CubeMap(const std::string& directory, const std::string& extension = DEFAULT_EXTENSION);
    ~CubeMap();
    CubeMap(const CubeMap&) = delete;
    CubeMap& operator=(const CubeMap&) = delete;

    void bind(int unit) const;

    int getFaceSize() const;

    static std::array<std::string, 6> facePaths(const std::string& directory, const std::string& extension);

private:
    bool tryLoad(const std::string& directory, const std::string& extension);

    GLuint id = 0;
    int faceSize = 0;
};
