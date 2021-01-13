//
// File:   Camera.h
// Author: Wouter
//
// Created on April 20, 2008, 10:13 PM
//

#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/glm.hpp>

class Camera
{
private:
    float rotationMatrix[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    glm::vec3 _position = glm::vec3(0.0f);
    glm::vec3 _rotation = glm::vec3(0.0f);

    void rotateMatrixX(float matrix[16], float angle);
    void rotateMatrixY(float matrix[16], float angle);
    void rotateMatrixZ(float matrix[16], float angle);

    void multiplyMatrix(float first[16], float second[16], float out[16]);

public:
    Camera();

    Camera(
        const glm::vec3 &position);

    virtual ~Camera();

    void Update();

    void SetPosition(
        const glm::vec3 &position);

    glm::vec3 getPosition() const;

    void SetRotation(
        const glm::vec3 &rotation);

    glm::vec3 getRotation() const;

    glm::vec3 getForwardVector() const;

    void MoveGlobal(float forward, float left, float up);

    void MoveLocal(float forward, float left, float up);
};

#endif /* _CAMERA_H */
