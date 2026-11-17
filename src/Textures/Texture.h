// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <string>

// Nastaveni textury. Vychozi = opakovani (UV > 1 dlazdicuje) + mipmapy.
// Pro HUD ikonky: repeat = false (CLAMP_TO_EDGE), mipmaps = false.
struct TextureOptions {
    bool repeat = true;
    bool mipmaps = true;
};

// 2D textura z obrazku (stb_image). Kdyz se obrazek nepodari nacist,
// pouzije se vychozi textura DEFAULT_PATH; kdyz chybi i ta, program skonci.
class Texture {
public:
    static constexpr const char* DEFAULT_PATH = "assets/textures/Default/default.png";

    explicit Texture(const std::string& path, const TextureOptions& options = TextureOptions());
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void bind(int unit) const;

    int getWidth() const;
    int getHeight() const;
    float getAspect() const;            // sirka / vyska
    const std::string& getPath() const; // skutecne nactena cesta (pri chybe DEFAULT_PATH)

private:
    GLuint id = 0;
    int width = 0;
    int height = 0;
    std::string path;
};
