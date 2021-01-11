#include "camera.h"

#include "opengl.h"
#include <math.h>

Camera::Camera()
{
    for (int i = 0; i < 3; i++)
    {
        position[i] = 0.0f;
        rotation[i] = 0.0f;
    }
}

Camera::Camera(float position[3])
{
    for (int i = 0; i < 3; i++)
    {
        this->position[i] = position[i];
        this->rotation[i] = 0.0f;
    }
}

Camera::~Camera()
{
}

void Camera::Update()
{
    // Reset the view matrix to identity(no transformations)
    glLoadIdentity();

    // First rotate the sceen around the origin. This makes this camera rotate around it's position
    glMultMatrixf(this->rotationMatrix);

    // Then move the camera to the correct position in space
    glTranslatef(this->position[0], this->position[1], this->position[2]);
}

void Camera::SetPosition(float position[3])
{
    for (int i = 0; i < 3; i++)
        this->position[i] = position[i];
}

const float* Camera::getPosition() const
{
    return this->position;
}

void Camera::SetRotation(float rotation[3])
{
    for (int i = 0; i < 3; i++)
        this->rotation[i] = rotation[i];

    float rx[16], ry[16], rz[16], tmp[16];

    rotateMatrixX(rx, rotation[0]);
    rotateMatrixY(ry, rotation[1]);
    rotateMatrixZ(rz, rotation[2]);

    multiplyMatrix(rx, ry, tmp);
    multiplyMatrix(tmp, rz, this->rotationMatrix);
}

const float* Camera::getRotation() const
{
    return this->rotation;
}

const float* Camera::getForwardVector() const
{
    static float forward[3] = { 0 };

    forward[0] = this->rotationMatrix[2];
    forward[1] = this->rotationMatrix[6];
    forward[2] = this->rotationMatrix[10];

    return forward;
}

void Camera::MoveGlobal(float forward, float left, float up)
{
    this->position[0] += forward;
    this->position[1] += left;
    this->position[2] += up;
}

void Camera::MoveLocal(float forward, float left, float up)
{
    this->position[0] += this->rotationMatrix[2] * forward;
    this->position[1] += this->rotationMatrix[6] * forward;
    this->position[2] += this->rotationMatrix[10] * forward;

    this->position[0] += this->rotationMatrix[0] * left;
    this->position[1] += this->rotationMatrix[4] * left;
    this->position[2] += this->rotationMatrix[8] * left;

    this->position[0] += this->rotationMatrix[1] * up;
    this->position[1] += this->rotationMatrix[5] * up;
    this->position[2] += this->rotationMatrix[9] * up;
}

void Camera::multiplyMatrix(float first[16], float second[16], float out[16])
{
    out[0] = first[0]*second[0] + first[4]*second[1] + first[8]*second[2] + first[12]*second[3];
    out[1] = first[1]*second[0] + first[5]*second[1] + first[9]*second[2] + first[13]*second[3];
    out[2] = first[2]*second[0] + first[6]*second[1] + first[10]*second[2] + first[14]*second[3];
    out[3] = first[3]*second[0] + first[7]*second[1] + first[11]*second[2] + first[15]*second[3];

    out[4] = first[0]*second[4] + first[4]*second[5] + first[8]*second[6] + first[12]*second[7];
    out[5] = first[1]*second[4] + first[5]*second[5] + first[9]*second[6] + first[13]*second[7];
    out[6] = first[2]*second[4] + first[6]*second[5] + first[10]*second[6] + first[14]*second[7];
    out[7] = first[3]*second[4] + first[7]*second[5] + first[11]*second[6] + first[15]*second[7];

    out[8] = first[0]*second[8] + first[4]*second[9] + first[8]*second[10] + first[12]*second[11];
    out[9] = first[1]*second[8] + first[5]*second[9] + first[9]*second[10] + first[13]*second[11];
    out[10] = first[2]*second[8] + first[6]*second[9] + first[10]*second[10] + first[14]*second[11];
    out[11] = first[3]*second[8] + first[7]*second[9] + first[11]*second[10] + first[15]*second[11];

    out[12] = first[0]*second[12] + first[4]*second[13] + first[8]*second[14] + first[12]*second[15];
    out[13] = first[1]*second[12] + first[5]*second[13] + first[9]*second[14] + first[13]*second[15];
    out[14] = first[2]*second[12] + first[6]*second[13] + first[10]*second[14] + first[14]*second[15];
    out[15] = first[3]*second[12] + first[7]*second[13] + first[11]*second[14] + first[15]*second[15];
}

void Camera::rotateMatrixX(float matrix[16], float angle)
{
    for (int c = 0; c < 4; c++)
    {
        for(int r = 0; r < 4; r++)
        {
            matrix[c*4+r] = 0.0f;
        }
        matrix[c*4+c] = 1.0f;
    }

    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    matrix[5] =  cosAngle;
    matrix[9] = -sinAngle;
    matrix[6] =  sinAngle;
    matrix[10] =  cosAngle;
}

void Camera::rotateMatrixY(float matrix[16], float angle)
{
    for (int c = 0; c < 4; c++)
    {
        for(int r = 0; r < 4; r++)
        {
            matrix[c*4+r] = 0.0f;
        }
        matrix[c*4+c] = 1.0f;
    }

    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    matrix[0] =  cosAngle;
    matrix[8] =  sinAngle;
    matrix[2] = -sinAngle;
    matrix[10] =  cosAngle;
}

void Camera::rotateMatrixZ(float matrix[16], float angle)
{
    for (int c = 0; c < 4; c++)
    {
        for(int r = 0; r < 4; r++)
        {
            matrix[c*4+r] = 0.0f;
        }
        matrix[c*4+c] = 1.0f;
    }

    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    matrix[0] =  cosAngle;
    matrix[4] = -sinAngle;
    matrix[1] =  sinAngle;
    matrix[5] =  cosAngle;
}
