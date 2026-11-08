// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

#include <memory>
#include <string>
#include <vector>

class TransformationComposite : public Transformation {
public:
    void addStatic(const Transformation& transformation);
    void addDynamic(Transformation* transformation);
    void add(Transformation* transformation) { addDynamic(transformation); }

    glm::mat4 getMatrix() const override;

    std::size_t size() const { return children.size(); }
    std::string describe() const;

private:
    class StaticMatrix : public Transformation {
    public:
        explicit StaticMatrix(const glm::mat4& matrix) : matrix(matrix) {}
        glm::mat4 getMatrix() const override { return matrix; }
        void multiply(const glm::mat4& other) { matrix = matrix * other; }

    private:
        glm::mat4 matrix;
    };

    std::vector<Transformation*> children;
    std::vector<std::unique_ptr<StaticMatrix>> ownedStatic;
};
