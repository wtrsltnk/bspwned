#include "camera.h"

#include "opengl.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>
#include <iostream>

Camera::Camera() = default;

Camera::Camera(
    const glm::vec3 &position)
    : _position(position)
{}

Camera::~Camera() = default;

void Camera::Update()
{
    glLoadIdentity();
    glMultMatrixf(glm::value_ptr(_rotationMatrix));
    glTranslatef(_position[0], _position[1], _position[2]);
}

void Camera::SetPosition(
    const glm::vec3 &position)
{
    for (int i = 0; i < 3; i++)
    {
        _position[i] = position[i];
    }
}

glm::vec3 Camera::getPosition() const
{
    return _position;
}

void Camera::SetRotation(
    const glm::vec3 &rotation)
{
    for (int i = 0; i < 3; i++)
    {
        _rotation[i] = rotation[i];
    }

    glm::mat4 r;

    r = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    r = glm::rotate(r, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _rotationMatrix = glm::rotate(r, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::vec3 Camera::getRotation() const
{
    return _rotation;
}

void Camera::MoveGlobal(
    float forward,
    float left,
    float up)
{
    _position[0] += forward;
    _position[1] += left;
    _position[2] += up;
}

void Camera::MoveLocal(
    float forward,
    float left,
    float up)
{
    _position[0] += glm::value_ptr(_rotationMatrix)[2] * forward;
    _position[1] += glm::value_ptr(_rotationMatrix)[6] * forward;
    _position[2] += glm::value_ptr(_rotationMatrix)[10] * forward;

    _position[0] += glm::value_ptr(_rotationMatrix)[0] * left;
    _position[1] += glm::value_ptr(_rotationMatrix)[4] * left;
    _position[2] += glm::value_ptr(_rotationMatrix)[8] * left;

    _position[0] += glm::value_ptr(_rotationMatrix)[1] * up;
    _position[1] += glm::value_ptr(_rotationMatrix)[5] * up;
    _position[2] += glm::value_ptr(_rotationMatrix)[9] * up;
}
