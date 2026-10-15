#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "GameObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

class GameContext;

class GraphicsObject : public GameObject {
public:
    virtual void RenderBackground(GameContext *context);
    virtual void RenderForeground(GameContext *context);

    // return the object's position
    virtual Point2D GetLocation() const = 0;

    // return the object's rotation in radians
    virtual float GetRotation() const = 0;

};

}  // namespace CMPUT350

#endif
