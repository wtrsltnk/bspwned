#include "camera.h"

#include "opengl.h"
#include <math.h>

Camera::Camera() = default;

Camera::Camera(
    const glm::vec3 &position)
    : _position(position)
{}

Camera::~Camera() = default;

void Camera::Update()
{
    // Reset the view matrix to identity(no transformations)
    glLoadIdentity();

    // First rotate the sceen around the origin. This makes this camera rotate around it's position
    glMultMatrixf(rotationMatrix);

    // Then move the camera to the correct position in space
    glTranslatef(_position[0], _position[1], _position[2]);
}

void Camera::SetPosition(
    const glm::vec3 &position)
{
    for (int i = 0; i < 3; i++)
        _position[i] = position[i];
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

    float rx[16], ry[16], rz[16], tmp[16];

    rotateMatrixX(rx, rotation[0]);
    rotateMatrixY(ry, rotation[1]);
    rotateMatrixZ(rz, rotation[2]);

    multiplyMatrix(rx, ry, tmp);
    multiplyMatrix(tmp, rz, rotationMatrix);
}

glm::vec3 Camera::getRotation() const
{
    return _rotation;
}

glm::vec3 Camera::getForwardVector() const
{
    glm::vec3 forward;

    forward[0] = rotationMatrix[2];
    forward[1] = rotationMatrix[6];
    forward[2] = rotationMatrix[10];

    return forward;
}

void Camera::MoveGlobal(float forward, float left, float up)
{
    _position[0] += forward;
    _position[1] += left;
    _position[2] += up;
}

void Camera::MoveLocal(float forward, float left, float up)
{
    _position[0] += rotationMatrix[2] * forward;
    _position[1] += rotationMatrix[6] * forward;
    _position[2] += rotationMatrix[10] * forward;

    _position[0] += rotationMatrix[0] * left;
    _position[1] += rotationMatrix[4] * left;
    _position[2] += rotationMatrix[8] * left;

    _position[0] += rotationMatrix[1] * up;
    _position[1] += rotationMatrix[5] * up;
    _position[2] += rotationMatrix[9] * up;
}

void Camera::multiplyMatrix(float first[16], float second[16], float out[16])
{
    out[0] = first[0] * second[0] + first[4] * second[1] + first[8] * second[2] + first[12] * second[3];
    out[1] = first[1] * second[0] + first[5] * second[1] + first[9] * second[2] + first[13] * second[3];
    out[2] = first[2] * second[0] + first[6] * second[1] + first[10] * second[2] + first[14] * second[3];
    out[3] = first[3] * second[0] + first[7] * second[1] + first[11] * second[2] + first[15] * second[3];

    out[4] = first[0] * second[4] + first[4] * second[5] + first[8] * second[6] + first[12] * second[7];
    out[5] = first[1] * second[4] + first[5] * second[5] + first[9] * second[6] + first[13] * second[7];
    out[6] = first[2] * second[4] + first[6] * second[5] + first[10] * second[6] + first[14] * second[7];
    out[7] = first[3] * second[4] + first[7] * second[5] + first[11] * second[6] + first[15] * second[7];

    out[8] = first[0] * second[8] + first[4] * second[9] + first[8] * second[10] + first[12] * second[11];
    out[9] = first[1] * second[8] + first[5] * second[9] + first[9] * second[10] + first[13] * second[11];
    out[10] = first[2] * second[8] + first[6] * second[9] + first[10] * second[10] + first[14] * second[11];
    out[11] = first[3] * second[8] + first[7] * second[9] + first[11] * second[10] + first[15] * second[11];

    out[12] = first[0] * second[12] + first[4] * second[13] + first[8] * second[14] + first[12] * second[15];
    out[13] = first[1] * second[12] + first[5] * second[13] + first[9] * second[14] + first[13] * second[15];
    out[14] = first[2] * second[12] + first[6] * second[13] + first[10] * second[14] + first[14] * second[15];
    out[15] = first[3] * second[12] + first[7] * second[13] + first[11] * second[14] + first[15] * second[15];
}

void Camera::rotateMatrixX(float matrix[16], float angle)
{
    for (int c = 0; c < 4; c++)
    {
        for (int r = 0; r < 4; r++)
        {
            matrix[c * 4 + r] = 0.0f;
        }
        matrix[c * 4 + c] = 1.0f;
    }

    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    matrix[5] = cosAngle;
    matrix[9] = -sinAngle;
    matrix[6] = sinAngle;
    matrix[10] = cosAngle;
}

void Camera::rotateMatrixY(float matrix[16], float angle)
{
    for (int c = 0; c < 4; c++)
    {
        for (int r = 0; r < 4; r++)
        {
            matrix[c * 4 + r] = 0.0f;
        }
        matrix[c * 4 + c] = 1.0f;
    }

    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    matrix[0] = cosAngle;
    matrix[8] = sinAngle;
    matrix[2] = -sinAngle;
    matrix[10] = cosAngle;
}

void Camera::rotateMatrixZ(float matrix[16], float angle)
{
    for (int c = 0; c < 4; c++)
    {
        for (int r = 0; r < 4; r++)
        {
            matrix[c * 4 + r] = 0.0f;
        }
        matrix[c * 4 + c] = 1.0f;
    }

    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    matrix[0] = cosAngle;
    matrix[4] = -sinAngle;
    matrix[1] = sinAngle;
    matrix[5] = cosAngle;
}
