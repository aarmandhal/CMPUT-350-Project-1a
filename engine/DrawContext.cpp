#include "DrawContext.h"

namespace CMPUT350 {

DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

void DrawContext::DrawCenteredText(const std::string& text, int pixelSize, Point2D p, RGBColor c) {
    // Draw the text centered at the given point
    // Create the text object
    sf::Text txt = sf::Text(*mFont, text, pixelSize);
    // Set the color of the text
    txt.setFillColor(sf::Color(c.r, c.g, c.b));
    // Get the bounds of the text
    sf::FloatRect bounds = txt.getLocalBounds();
    // Set the position of the text
    txt.setPosition(sf::Vector2f(p.x, p.y) - sf::Vector2f(bounds.size.x, bounds.size.y) / 2.0f);
    mWindow->draw(txt);
}

void DrawContext::DrawText(const std::string& text, int pixelSize, Point2D p, RGBColor c) {
    // Draw the text at the given point
    // Create the text object
    sf::Text txt = sf::Text(*mFont, text, pixelSize);
    // Set the color of the text
    txt.setFillColor(sf::Color(c.r, c.g, c.b));
    // Set the position of the text
    txt.setPosition(sf::Vector2f(p.x, p.y));
    mWindow->draw(txt);
}

void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    // Draw a circle at the given point
    // Create the circle object
    sf::CircleShape circle(radius);
    // Set the position of the circle
    circle.setPosition(sf::Vector2f(p.x, p.y) - sf::Vector2f(radius, radius));
    // Set the color of the circle
    circle.setFillColor(sf::Color(c.r, c.g, c.b));
    // Draw the circle
    mWindow->draw(circle);
}

void DrawContext::DrawRect(Rect r, RGBColor c) {
    // Draw a rectangle at the given point
    // Create the rectangle object
    sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
    // Set the position of the rectangle
    rect.setPosition(sf::Vector2f(r.topLeft.x, r.topLeft.y));
    // Set the color of the rectangle
    rect.setFillColor(sf::Color(c.r, c.g, c.b));
    // Draw the rectangle
    mWindow->draw(rect);
}

void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    // Draw the outline of a rectangle
    // Create the rectangle object
    sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
    // Set the position of the rectangle
    rect.setPosition(sf::Vector2f(r.topLeft.x, r.topLeft.y));
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
    // Calculate the distance between the two points
    float length = from.Distance(to);
    // If the distance is 0, return
    if (length == 0) return;
    // Calculate the angle between the two points
    sf::Angle angle = sf::radians(std::atan2(to.y - from.y, to.x - from.x));
    // Create the line object
    sf::RectangleShape line(sf::Vector2f(length, width));
    // Set the origin of the line
    line.setOrigin(sf::Vector2f(0, width / 2.0f));
    // Set the position of the line
    line.setPosition(sf::Vector2f(from.x, from.y));
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