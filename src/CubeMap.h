// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <array>
#include <string>

class CubeMap {
public:
    explicit CubeMap(const std::array<std::string, 6>& paths);
    ~CubeMap();
    CubeMap(const CubeMap&) = delete;
    CubeMap& operator=(const CubeMap&) = delete;

    void bind(int unit) const;

private:
    GLuint id = 0;
};
