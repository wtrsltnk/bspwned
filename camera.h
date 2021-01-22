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
    glm::mat4 _rotationMatrix = glm::mat4();
    glm::vec3 _position = glm::vec3(0.0f);
    glm::vec3 _rotation = glm::vec3(0.0f);

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

    void MoveGlobal(
        float forward,
        float left,
        float up);

    void MoveLocal(
        float forward,
        float left,
        float up);
};

#endif /* _CAMERA_H */
