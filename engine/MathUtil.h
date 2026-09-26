#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    double Distance(const Point2D& other) const {
        // Euclidian Distance Equation
        // d = sqrt((x2 - x1)^2 + (y2 - y1)^2)
        // (x1, y1) -> this->x, this->y
        // (x2, y2) -> other.x, other.y
        return std::sqrt((other.x - this->x) * (other.x - this->x) +
                         (other.y - this->y) * (other.y - this->y));
    }
    Point2D operator+(const Point2D& other) const {
        // Add the two points together
        // (x1, y1) + (x2, y2) = (x1 + x2, y1 + y2)
        return {this->x + other.x, this->y + other.y};
    }
    Point2D operator+(const float& other) const {
        // Add the scalar to the point
        // (x, y) + s = (x + s, y + s)
        return {this->x + other, this->y + other};
    }
    Point2D operator-(const Point2D& other) const {
        // Subtract the two points
        // (x1, y1) - (x2, y2) = (x1 - x2, y1 - y2)
        return {this->x - other.x, this->y - other.y};
    }
    Point2D operator-(const float& other) const {
        // Subtract the scalar from the point
        // (x, y) - s = (x - s, y - s)
        return {this->x - other, this->y - other};
    }
    Point2D operator*(const float& scalar) const {
        // Multiply the scalar by the point
        // (x, y) * s = (x * s, y * s)
        return {this->x * scalar, this->y * scalar};
    }
    Point2D& operator+=(const float& scalar) {
        // Scalar Addition on self
        // (x, y) += s = (x + s, y + s)
        this->x += scalar;
        this->y += scalar;
        return *this;
    }
    Point2D& operator+=(const Point2D& other) {
        // Point Addition on self
        // (x1, y1) += (x2, y2) = (x1 + x2, y1 + y2)
        this->x += other.x;
        this->y += other.y;
        return *this;
    }
    Point2D& operator-=(const Point2D& other) {
        // Point Subtraction on self
        // (x1, y1) -= (x2, y2) = (x1 - x2, y1 - y2)
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }
    bool operator==(const Point2D& other) const {
        // Compare the two points
        // If (x1, y1) == (x2, y2) then return true else false
        if (this->x == other.x && this->y == other.y) return true;
        return false;
    }
    Point2D& operator*=(const int& scalar) {
        // Scalar Multiplication on self
        // (x, y) *= s = (x * s, y * s)
        this->x *= scalar;
        this->y *= scalar;
        return *this;
    }
    Point2D& operator/=(const int& scalar) {
        // Scalar Division on self
        // (x, y) /= s = (x / s, y / s)
        if (scalar != 0) {
            this->x /= scalar;
            this->y /= scalar;
        }
        return *this;
    }
    float operator*(const Point2D& other) const {
        // Dot Product
        // (x1, y1) . (x2, y2) = x1 * x2 + y1 * y2
        return this->x * other.x + this->y * other.y;
    }
    float Dot(Point2D b) const {
        // Dot Product
        // (x1, y1) . (x2, y2) = x1 * x2 + y1 * y2
        return this->x * b.x + this->y * b.y;
    }
    static float Dot(Point2D a, Point2D b) {
        // Dot Product
        // (x1, y1) . (x2, y2) = x1 * x2 + y1 * y2
        return a.x * b.x + a.y * b.y;
    }
    static float Cross(Point2D a, Point2D b) {
        // 2D Cross Product
        // (x1, y1) x (x2, y2) = x1 * y2 - y1 * x2
        return a.x * b.y - a.y * b.x;
    }
    void Normalize() {
        // Vector Normalization
        // Magnitude: ||v|| = sqrt(x^2 + y^2)
        // Normalized Vector: v' = v / ||v|| = (x/||v||, y/||v||)
        // Check for zero vector to prevent division by zero
        float magnitude = std::sqrt(this->x * this->x + this->y * this->y);
        if (magnitude != 0) {
            this->x /= magnitude;
            this->y /= magnitude;
        }
    }
};

static std::ostream& operator<<(std::ostream& os, const Point2D& p) {
    // TODO: write this code
    return os;
}

static Point2D operator*(float number, const Point2D& rhs) {
    // TODO: write this code
    return rhs;
}

struct Line {
    Point2D p1, p2;

    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    float Length() const {
        // TODO: write this code
        return 0;
    }
    Point2D ClosestPoint(const Point2D& p) const {
        // TODO: write this code
        return p;
    }
    bool Crosses(Line other, Point2D& crossingPoint) const {
        // TODO: write this code
        return false;
    }
};

static std::ostream& operator<<(std::ostream& os, const Line& l) {
    // TODO: write this code
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

struct Rect {
    Point2D topLeft;
    float width, height;

    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(top, left)), width(width), height(height) {}

    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    Rect& operator|=(const Rect& other) {
        // Finds the union of the two rectangles
        // New top-left is the min corner of the two rectangles
        // New bottom-right is the max corner of the two rectangles
        float x_min_union = std::min(this->topLeft.x, other.topLeft.x);
        float y_min_union = std::min(this->topLeft.y, other.topLeft.y);
        float x_max_union = std::max(this->topLeft.x + this->width, other.topLeft.x + other.width);
        float y_max_union =
            std::max(this->topLeft.y + this->height, other.topLeft.y + other.height);

        // Rebuild rectangle from the combined corners
        this->topLeft.x = x_min_union;
        this->topLeft.y = y_min_union;
        this->width = x_max_union - x_min_union;
        this->height = y_max_union - y_min_union;

        return *this;
    }
    Rect& operator|=(const Point2D& other) {
        // Finds the union of the rectangle and the point
        // New top-left is the min corner of the rectangle and the point
        // New bottom-right is the max corner of the rectangle and the point
        float x_min_union = std::min(this->topLeft.x, other.x);
        float y_min_union = std::min(this->topLeft.y, other.y);
        float x_max_union = std::max(this->topLeft.x + this->width, other.x);
        float y_max_union = std::max(this->topLeft.y + this->height, other.y);

        // Rebuild rectangle from the combined corners
        this->topLeft.x = x_min_union;
        this->topLeft.y = y_min_union;
        this->width = x_max_union - x_min_union;
        this->height = y_max_union - y_min_union;

        return *this;
    }
    Rect& operator|=(const Line& other) {
        // Finds the union of the rectangle and the line
        // New top-left is the min corner of the rectangle and the line
        // New bottom-right is the max corner of the rectangle and the line
        float x_min_union = std::min({this->topLeft.x, other.p1.x, other.p2.x});
        float y_min_union = std::min({this->topLeft.y, other.p1.y, other.p2.y});
        float x_max_union = std::max({this->topLeft.x + this->width, other.p1.x, other.p2.x});
        float y_max_union = std::max({this->topLeft.y + this->height, other.p1.y, other.p2.y});

        // Rebuild rectangle from the combined corners
        this->topLeft.x = x_min_union;
        this->topLeft.y = y_min_union;
        this->width = x_max_union - x_min_union;
        this->height = y_max_union - y_min_union;

        return *this;
    }
    Rect& operator&=(const Rect& other) {
        // Finds the intersection of the rectangle and the other rectangle
        // New top-left is the max corner of the two rectangles
        // New bottom-right is the min corner of the two rectangles
        float x_min_intersection = std::max(this->topLeft.x, other.topLeft.x);
        float y_min_intersection = std::max(this->topLeft.y, other.topLeft.y);
        float x_max_intersection =
            std::min(this->topLeft.x + this->width, other.topLeft.x + other.width);
        float y_max_intersection =
            std::min(this->topLeft.y + this->height, other.topLeft.y + other.height);

        // Rebuild rectangle from the combined corners
        this->topLeft.x = x_min_intersection;
        this->topLeft.y = y_min_intersection;
        this->width = x_max_intersection - x_min_intersection;
        this->height = y_max_intersection - y_min_intersection;

        return *this;
    }
    Rect& operator+=(const Point2D& other) {
        // Translates the rectangle by adding the other point to the top-left corner
        // New top-left is the old top-left plus the other point
        // Width and height remain the same
        this->topLeft.x += other.x;
        this->topLeft.y += other.y;
        return *this;
    }
    Rect operator+(const Point2D& other) const {
        // Returns a new rectangle that is the old rectangle plus the other point
        // New top-left is the old top-left plus the other point
        // Width and height remain the same
        return Rect(this->topLeft + other, this->width, this->height);
    }
    void Inset(int inset) {
        // Insets the rectangle by the given amount
        // New top-left is the old top-left plus the given amount
        // New width and height are the old width and height minus twice the given amount
        this->topLeft.x += inset;
        this->topLeft.y += inset;
        this->width -= 2 * inset;
        this->height -= 2 * inset;
    }
    bool IsInside(const Point2D& p) const {
        // Variables for the rectangle's boundaries
        float x_min = this->topLeft.x;
        float y_min = this->topLeft.y;
        float x_max = this->topLeft.x + this->width;
        float y_max = this->topLeft.y + this->height;

        // Return true if the point is inside the rectangle
        return p.x >= x_min && p.x <= x_max && p.y >= y_min && p.y <= y_max;
    }
};

static std::ostream& operator<<(std::ostream& os, const Rect& l) {
    // TODO: write this code
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
