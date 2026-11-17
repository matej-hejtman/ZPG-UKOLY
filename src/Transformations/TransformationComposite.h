// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/CustomMatrix.h"
#include "Transformations/Transformation.h"

#include <memory>
#include <string>
#include <vector>

// Skladani transformaci (vysledek = prvni * druha * ...), da se vnorovat.
//   addStatic  ... matice se spocita hned a ulozi (po sobe jdouci staticke se spoji do jedne)
//   addDynamic ... ulozi se ukazatel, matice se pocita pri kazdem getMatrix() (animace, sdileni)
class TransformationComposite : public Transformation {
public:
    void addStatic(const Transformation& transformation);
    void addDynamic(Transformation* transformation);

    glm::mat4 getMatrix() const override;

    std::size_t size() const;
    std::string describe() const;   // napr. "S D S" (staticka / dynamicka cast)

private:
    bool isOwnedStatic(const Transformation* transformation) const;

    std::vector<Transformation*> children;
    std::vector<std::unique_ptr<CustomMatrix>> ownedStatic;
};
