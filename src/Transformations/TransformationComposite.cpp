// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/TransformationComposite.h"

TransformationComposite::StaticMatrix::StaticMatrix(const glm::mat4& matrix) : matrix(matrix) {}

glm::mat4 TransformationComposite::StaticMatrix::getMatrix() const {
    return matrix;
}

void TransformationComposite::StaticMatrix::multiply(const glm::mat4& other) {
    matrix = matrix * other;
}

void TransformationComposite::addStatic(const Transformation& transformation) {
    const glm::mat4 matrix = transformation.getMatrix();

    if (!children.empty() && !ownedStatic.empty() && children.back() == ownedStatic.back().get()) {
        ownedStatic.back()->multiply(matrix);
        return;
    }

    ownedStatic.push_back(std::make_unique<StaticMatrix>(matrix));
    children.push_back(ownedStatic.back().get());
}

void TransformationComposite::addDynamic(Transformation* transformation) {
    children.push_back(transformation);
}

glm::mat4 TransformationComposite::getMatrix() const {
    glm::mat4 matrix(1.0f);
    for (const Transformation* child : children) {
        matrix = matrix * child->getMatrix();
    }

    return matrix;
}

std::string TransformationComposite::describe() const
{
    std::string text;
    
    for (const Transformation* child : children) {
        if (!text.empty())
            text += ' ';
        text += dynamic_cast<const StaticMatrix*>(child) ? 'S' : 'D';
    }

    return text;
}

void TransformationComposite::add(Transformation* transformation) {
    addDynamic(transformation);
}

std::size_t TransformationComposite::size() const {
    return children.size();
}
