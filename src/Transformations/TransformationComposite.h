// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/CustomMatrix.h"
#include "Transformations/Transformation.h"

#include <memory>
#include <string>
#include <vector>

class TransformationComposite : public Transformation {
public:
    void addStatic(const Transformation& transformation);
    void addDynamic(Transformation* transformation);

    glm::mat4 getMatrix() const override;

    std::size_t size() const;
    std::string describe() const;

private:
    bool isOwnedStatic(const Transformation* transformation) const;

    std::vector<Transformation*> children;
    std::vector<std::unique_ptr<CustomMatrix>> ownedStatic;
};
