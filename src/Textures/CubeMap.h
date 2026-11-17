// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <array>
#include <string>

// Cubemapa ze slozky se 6 obrazky: right, left, top, bottom, front, back (+ pripona).
// OpenGL vyzaduje, aby vsechny steny byly ctvercove a stejne velke, jinak je
// textura "neuplna" a skybox je tise cerny. Proto se to kontroluje.
// Kdyz se cubemapa nepodari nacist (chybi stena, spatny rozmer), pouzije se
// vychozi DEFAULT_DIRECTORY; kdyz selze i ta, program skonci.
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

    // cesty ke stenam v poradi GL_TEXTURE_CUBE_MAP_POSITIVE_X + i: +X, -X, +Y, -Y, +Z, -Z
    static std::array<std::string, 6> facePaths(const std::string& directory, const std::string& extension);

private:
    bool tryLoad(const std::string& directory, const std::string& extension);

    GLuint id = 0;
    int faceSize = 0;
};
