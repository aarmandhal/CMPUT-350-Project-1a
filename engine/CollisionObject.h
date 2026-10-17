#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>
#include <vector>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

enum class ShapeType { Circle, Rect, Line };

struct ShapeUnion {
    ShapeType type;
    union {
        Circle circle;
        Rect rect;
        Line line;
    };
    ShapeUnion() : type(ShapeType::Circle), circle() {}
};

class CollisionObject : public GraphicsObject {
public:
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj) = 0;
    virtual const Rect& GetBounds() = 0;

    virtual const std::vector<ShapeUnion>& GetShapes() = 0;
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>&,
                                const Point2D& collisionPoint) {}
};

}  // namespace CMPUT350

#endif