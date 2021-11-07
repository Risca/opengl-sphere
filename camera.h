#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    glm::mat4 getWorldToViewMatrix() const;
    const glm::vec3& getPosition() const;
    const glm::vec3& getViewDirection() const;

    void mouseUpdate(const glm::vec2& newMousePosition);
    void moveForward();
    void moveBackwards();
    void strafeLeft();
    void strafeRight();
    void moveUp();
    void moveDown();

private:
    const glm::vec3 UP;
    glm::vec3 position;
    glm::vec3 viewDirection;
    glm::vec3 strafeDirection;
    glm::vec2 oldMousePosition;
    static const float MOVEMENT_SPEED;
};

#endif // CAMERA_H
