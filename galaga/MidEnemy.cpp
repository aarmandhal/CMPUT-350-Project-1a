#include "MidEnemy.h"

MidEnemy::MidEnemy(CMPUT350::Point2D loc) : Enemy(loc, CMPUT350::Colors::magenta, 1, 80) {
    this->mFormationLocation = loc;
    this->mAttackPath.controlPoints = {
        loc,
        loc + CMPUT350::Point2D(-60, 90),
        loc + CMPUT350::Point2D(70, 180),
        loc + CMPUT350::Point2D(-20, 280),
    };
    this->mDesiredPixelSpeed = 4.0f;
}