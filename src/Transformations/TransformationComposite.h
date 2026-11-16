// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

#include <memory>
#include <string>
#include <vector>

class TransformationComposite : public Transformation {
public:
    void addStatic(const Transformation& transformation);
    void addDynamic(Transformation* transformation);
    void add(Transformation* transformation);

    glm::mat4 getMatrix() const override;

    std::size_t size() const;
    std::string describe() const;

private:
    class StaticMatrix : public Transformation {
    public:
        explicit StaticMatrix(const glm::mat4& matrix);
        glm::mat4 getMatrix() const override;
        void multiply(const glm::mat4& other);

    private:
        glm::mat4 matrix;
    };

    std::vector<Transformation*> children;
    std::vector<std::unique_ptr<StaticMatrix>> ownedStatic;
};
