// 
// File:   Camera.h
// Author: Wouter
//
// Created on April 20, 2008, 10:13 PM
//

#ifndef _CAMERA_H
#define	_CAMERA_H

class Camera
{
private:
    float rotationMatrix[16];
	
    float position[3];
    float rotation[3];

    void rotateMatrixX(float matrix[16], float angle);
    void rotateMatrixY(float matrix[16], float angle);
    void rotateMatrixZ(float matrix[16], float angle);

    void multiplyMatrix(float first[16], float second[16], float out[16]);
public:
    Camera();
    Camera(float position[3]);
    virtual ~Camera();
    
    void Update();
    
    void SetPosition(float position[3]);
    const float* getPosition() const;
    void SetRotation(float rotation[3]);
    const float* getRotation() const;

    const float* getForwardVector() const;
    
    void MoveGlobal(float forward, float left, float up);
    void MoveLocal(float forward, float left, float up);
    
};

#endif	/* _CAMERA_H */

