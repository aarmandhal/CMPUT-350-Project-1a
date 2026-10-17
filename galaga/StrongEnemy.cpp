#include "StrongEnemy.h"

StrongEnemy::StrongEnemy(CMPUT350::Point2D loc) : Enemy(loc, CMPUT350::Colors::yellow, 2, 150) {
    this->mFormationLocation = loc;
    this->mAttackPath.controlPoints = {
        loc,
        loc + CMPUT350::Point2D(90, 70),
        loc + CMPUT350::Point2D(-90, 170),
        loc + CMPUT350::Point2D(0, 300),
    };
    this->mDesiredPixelSpeed = 3.0f;
}