// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <string>

struct TextureOptions {
    bool repeat = true;
    bool mipmaps = true;
};

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
    float getAspect() const;
    const std::string& getPath() const;

private:
    GLuint id = 0;
    int width = 0;
    int height = 0;
    std::string path;
};
