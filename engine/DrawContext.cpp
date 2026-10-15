#include "DrawContext.h"
#include <algorithm>
#include <cmath>

namespace CMPUT350 {

DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}


// stores the position of the object
void DrawContext::SetContextOffset(Point2D p) {
    mContextOffset = p;
}

// stores the objects rotation in radians
void DrawContext::SetContextRotation(float rotation) {
    mContextRotation = rotation;
}

// converts local coordinates to screen coordinates
Point2D DrawContext::Transform(Point2D p) {
    float cosine = std::cos(mContextRotation);
    float sine = std::sin(mContextRotation);

    float screenX = p.x * cosine - p.y * sine + mContextOffset.x;
    float screenY = p.x * sine + p.y * cosine + mContextOffset.y;

    return {screenX, screenY};
}

Point2D DrawContext::ReverseTransform(Point2D p) {
    // remove the position offset
    float x = p.x - mContextOffset.x;
    float y = p.y - mContextOffset.y;

    // calculate sin and cosine of the rotation
    float cosine = std::cos(mContextRotation);
    float sine = std::sin(mContextRotation);

    // undo the rotation and return the local point
    return {
        x * cosine + y * sine,
        -x * sine + y * cosine
    };
}

Rect DrawContext::Transform(Rect r) {
    // Transform all four corners of the rectangle
    Point2D topLeft = Transform(r.topLeft);

    Point2D topRight = Transform(Point2D{
        r.topLeft.x + r.width,
        r.topLeft.y
    });

    Point2D bottomLeft = Transform(Point2D{
        r.topLeft.x,
        r.topLeft.y + r.height
    });

    Point2D bottomRight = Transform(Point2D{
        r.topLeft.x + r.width,
        r.topLeft.y + r.height
    });

    // Find the smallest and largest X coordinates
    float minimumX = std::min({
        topLeft.x, topRight.x, bottomLeft.x, bottomRight.x
    });

    float maximumX = std::max({
        topLeft.x, topRight.x, bottomLeft.x, bottomRight.x
    });

    // Find the smallest and largest Y coordinates
    float minimumY = std::min({
        topLeft.y, topRight.y, bottomLeft.y, bottomRight.y
    });

    float maximumY = std::max({
        topLeft.y, topRight.y, bottomLeft.y, bottomRight.y
    });

    // Calculate the width and height of the new bounding box
    float boundingWidth = maximumX - minimumX;
    float boundingHeight = maximumY - minimumY;

    // Return the new bounding rectangle
    return Rect({minimumX, minimumY}, boundingWidth, boundingHeight);
}


void DrawContext::DrawCenteredText(const std::string& text, int pixelSize, Point2D p, RGBColor c) {
    // transform the local point to screen coordinates
    Point2D screenPoint = Transform(p);
    // Draw the text centered at the given point
    // Create the text object
    sf::Text txt = sf::Text(*mFont, text, pixelSize);
    // Set the color of the text
    txt.setFillColor(sf::Color(c.r, c.g, c.b));
    // Get the bounds of the text
    sf::FloatRect bounds = txt.getLocalBounds();
    // center the text around its own origin
    txt.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f));
    // Set the position of the text
    txt.setPosition(sf::Vector2f(screenPoint.x, screenPoint.y));
    // apply the objects rotation
    txt.setRotation(sf::radians(mContextRotation));
    mWindow->draw(txt);
}

void DrawContext::DrawText(const std::string& text, int pixelSize, Point2D p, RGBColor c) {
    // transform the local point to screen coordinates
    Point2D screenPoint = Transform(p);
    // Draw the text at the given point
    // Create the text object
    sf::Text txt = sf::Text(*mFont, text, pixelSize);
    // Set the color of the text
    txt.setFillColor(sf::Color(c.r, c.g, c.b));
    // Set the position of the text
    txt.setPosition(sf::Vector2f(screenPoint.x, screenPoint.y));
    // rotate the text
    txt.setRotation(sf::radians(mContextRotation));

    mWindow->draw(txt);
}

void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    // convert the local point to screen coordinates
    Point2D screenCenter = Transform(p);
    // Draw a circle at the given point
    // Create the circle object
    sf::CircleShape circle(radius);
    // Set the position of the circle
    circle.setPosition(sf::Vector2f(screenCenter.x, screenCenter.y) - sf::Vector2f(radius, radius));
    // Set the color of the circle
    circle.setFillColor(sf::Color(c.r, c.g, c.b));
    // Draw the circle
    mWindow->draw(circle);
}

void DrawContext::DrawRect(Rect r, RGBColor c) {
    // transform the rectangle's top left corner
    Point2D screenPoint = Transform(r.topLeft);

    // Draw a rectangle at the given point
    // Create the rectangle object
    sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
    // Set the position of the rectangle
    rect.setPosition(sf::Vector2f(screenPoint.x, screenPoint.y));
    // apply the current rotation
    rect.setRotation(sf::radians(mContextRotation));
    // Set the color of the rectangle
    rect.setFillColor(sf::Color(c.r, c.g, c.b));
    // Draw the rectangle
    mWindow->draw(rect);
}

void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    // transform the rectangle's top left corner
    Point2D screenPoint = Transform(r.topLeft);

    // Draw the outline of a rectangle
    // Create the rectangle object
    sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
    // Set the position of the rectangle
    rect.setPosition(sf::Vector2f(screenPoint.x, screenPoint.y));
    // apply the current rotation
    rect.setRotation(sf::radians(mContextRotation));
    // Set the color of the rectangle
    rect.setFillColor(sf::Color::Transparent);
    // Set the outline color of the rectangle
    rect.setOutlineColor(sf::Color(c.r, c.g, c.b));
    // Set the outline thickness of the rectangle
    rect.setOutlineThickness(width);
    // Draw the rectangle
    mWindow->draw(rect);
}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line (Point2D).
 * @param to The ending point of the line (Point2D).
 * @param width The width of the line in pixels.
 * @param c The color of the line, specified as an RGBColor object.
 *
 * This function calculates the distance and angle between the two points
 * and uses a polygone shape to represent the line. The line is drawn
 * relative to the world offset and rendered onto the associated window.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    // convert the local points to screen coordinates
    Point2D screenFrom = Transform(from);
    Point2D screenTo = Transform(to);

    // Calculate the distance between the two points
    float length = screenFrom.Distance(screenTo);

    // If the distance is 0, return
    if (length == 0) return;
    // Calculate the angle between the two points
    sf::Angle angle = sf::radians(std::atan2(screenTo.y - screenFrom.y, screenTo.x - screenFrom.x));
    // Create the line object
    sf::RectangleShape line(sf::Vector2f(length, width));
    // Set the origin of the line
    line.setOrigin(sf::Vector2f(0, width / 2.0f));
    // Set the position of the line
    line.setPosition(sf::Vector2f(screenFrom.x, screenFrom.y));
    // Set the rotation of the line
    line.setRotation(angle);
    // Set the color of the line
    line.setFillColor(sf::Color(c.r, c.g, c.b));
    // Draw the line
    mWindow->draw(line);
}

int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350