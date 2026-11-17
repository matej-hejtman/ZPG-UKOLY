// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/TransformationComposite.h"

void TransformationComposite::addStatic(const Transformation& transformation) {
    const glm::mat4 matrix = transformation.getMatrix();

    if (!children.empty() && !ownedStatic.empty() && children.back() == ownedStatic.back().get()) {
        ownedStatic.back() = std::make_unique<CustomMatrix>(ownedStatic.back()->getMatrix() * matrix);
        children.back() = ownedStatic.back().get();
        return;
    }

    ownedStatic.push_back(std::make_unique<CustomMatrix>(matrix));
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

bool TransformationComposite::isOwnedStatic(const Transformation* transformation) const {
    for (const auto& owned : ownedStatic) {
        if (owned.get() == transformation) {
            return true;
        }
    }
    return false;
}

std::string TransformationComposite::describe() const {
    std::string text;

    for (const Transformation* child : children) {
        if (!text.empty()) {
            text += ' ';
        }
        text += isOwnedStatic(child) ? 'S' : 'D';
    }

    return text;
}

std::size_t TransformationComposite::size() const {
    return children.size();
}
