#include "WeakEnemy.h"

WeakEnemy::WeakEnemy(CMPUT350::Point2D loc) : Enemy(loc, CMPUT350::Colors::cyan, 1, 50) {
    this->mFormationLocation = loc;
    this->mAttackPath.controlPoints = {
        loc,
        loc + CMPUT350::Point2D(40, 80),
        loc + CMPUT350::Point2D(-40, 160),
        loc + CMPUT350::Point2D(0, 260),
    };
    this->mDesiredPixelSpeed = 3.5f;
}