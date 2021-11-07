#include "camera.h"

#include <glm/gtx/transform.hpp>

const float Camera::MOVEMENT_SPEED = 0.1f;

Camera::Camera() :
    UP(0.0f, 1.0f, 0.0f),
    position(1.41558f , 1.50537f , -3.47837f),
    viewDirection(-0.677488f, -0.708331f, 0.198111f),
    strafeDirection(glm::cross(viewDirection, UP)),
    oldMousePosition(0.0f, 0.0f)
{
    // empty
}

glm::mat4 Camera::getWorldToViewMatrix() const
{
    return glm::lookAt(position, position + viewDirection, UP);
}

const glm::vec3 &Camera::getPosition() const
{
    return position;
}

const glm::vec3 &Camera::getViewDirection() const
{
    return viewDirection;
}

void Camera::mouseUpdate(const glm::vec2 &newMousePosition)
{
    glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
    if (glm::length(mouseDelta) < 80) {
        // Update view direction
        const float ROTATIONAL_SPEED = 0.1f * glm::pi<float>() / 180.0f;
        glm::mat4 rotator = glm::rotate(-mouseDelta.x * ROTATIONAL_SPEED, UP) *
                            glm::rotate(-mouseDelta.y * ROTATIONAL_SPEED, strafeDirection);
        viewDirection = glm::mat3(rotator) * viewDirection;
        strafeDirection = glm::cross(viewDirection, UP);
    }
    oldMousePosition = newMousePosition;
}

void Camera::moveForward()
{
    position += MOVEMENT_SPEED * viewDirection;
}

void Camera::moveBackwards()
{
    position -= MOVEMENT_SPEED * viewDirection;
}

void Camera::strafeLeft()
{
    position -= MOVEMENT_SPEED * strafeDirection;
}

void Camera::strafeRight()
{
    position += MOVEMENT_SPEED * strafeDirection;
}

void Camera::moveUp()
{
    const glm::vec3 up = glm::mat3(glm::rotate(glm::pi<float>()/2.0f, strafeDirection)) * viewDirection;
    position += MOVEMENT_SPEED * up;
}

void Camera::moveDown()
{
    const glm::vec3 up = glm::mat3(glm::rotate(glm::pi<float>()/2.0f, strafeDirection)) * viewDirection;
    position -= MOVEMENT_SPEED * up;
}
