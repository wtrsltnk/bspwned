/* 
 * File:   vector3.cpp
 * Author: wouter
 * 
 * Created on April 13, 2009, 5:24 PM
 */

#include "vector3.h"

#include <math.h>

Vector3::Vector3()
{
	this->mVector[0] = 0.0f;
	this->mVector[1] = 0.0f;
	this->mVector[2] = 0.0f;
}

Vector3::Vector3(float x, float y, float z)
{
	this->mVector[0] = x;
	this->mVector[1] = y;
	this->mVector[2] = z;
}

Vector3::Vector3(const Vector3& v)
{
	(*this) = v;
}

Vector3::Vector3(const float v[3])
{
	(*this) = v;
}

Vector3::~Vector3()
{
}

float Vector3::length() const
{
	return sqrtf(this->mVector[0] * this->mVector[0] + this->mVector[1] * this->mVector[1] + this->mVector[2] * this->mVector[2]);
}

const Vector3 Vector3::unit() const
{
	float length = this->length();

	return Vector3(this->mVector[0] / length, this->mVector[1] / length, this->mVector[2] / length);
}

const Vector3 Vector3::crossProduct(const Vector3& v) const
{
	float r[3];

	r[0] = this->mVector[1] * v.mVector[2] + this->mVector[2] * v.mVector[1];
	r[1] = this->mVector[2] * v.mVector[0] + this->mVector[0] * v.mVector[2];
	r[2] = this->mVector[0] * v.mVector[1] + this->mVector[1] * v.mVector[0];

	return Vector3(r);
}

float Vector3::dotProduct(const Vector3& v) const
{
	return (this->mVector[0] * v.mVector[0]) + (this->mVector[1] * v.mVector[1]) + (this->mVector[2] * v.mVector[2]);
}

Vector3::operator float* ()
{
	return this->mVector;
}

Vector3::operator const float* () const
{
	return this->mVector;
}

const Vector3& Vector3::operator = (const Vector3& v)
{
    for (int i = 0; i < 3; i++)
    {
        this->mVector[i] = v.mVector[i];
    }

    return *this;
}

const Vector3& Vector3::operator = (const float v[3])
{
    for (int i = 0; i < 3; i++)
    {
        this->mVector[i] = v[i];
    }

    return *this;
}

bool Vector3::operator == (const Vector3& v)
{
	return (this->mVector[0] == v.mVector[0] && this->mVector[1] == v.mVector[1] && this->mVector[2] == v.mVector[2]);
}

bool Vector3::operator != (const Vector3& v)
{
	return (this->mVector[0] != v.mVector[0] || this->mVector[1] != v.mVector[1] || this->mVector[2] != v.mVector[2]);
}

bool Vector3::operator == (const float v[3])
{
	return (this->mVector[0] == v[0] && this->mVector[1] == v[1] && this->mVector[2] == v[2]);
}

bool Vector3::operator != (const float v[3])
{
	return (this->mVector[0] != v[0] || this->mVector[1] != v[1] || this->mVector[2] != v[2]);
}

const Vector3 Vector3::operator * (const Vector3& v)
{
	return Vector3(this->mVector[0] * v.mVector[0], this->mVector[1] * v.mVector[1], this->mVector[2] * v.mVector[2]);
}

const Vector3& Vector3::operator *= (const Vector3& v)
{
	this->mVector[0] *= v.mVector[0];
	this->mVector[1] *= v.mVector[1];
	this->mVector[2] *= v.mVector[2];

	return (*this);
}

const Vector3 Vector3::operator + (const Vector3& v)
{
	return Vector3(this->mVector[0] + v.mVector[0], this->mVector[1] + v.mVector[1], this->mVector[2] + v.mVector[2]);
}

const Vector3& Vector3::operator += (const Vector3& v)
{
	this->mVector[0] += v.mVector[0];
	this->mVector[1] += v.mVector[1];
	this->mVector[2] += v.mVector[2];

	return (*this);
}

const Vector3 Vector3::operator * (float s)
{
	return Vector3(this->mVector[0] * s, this->mVector[1] * s, this->mVector[2] * s);
}

const Vector3& Vector3::operator *= (float s)
{
	this->mVector[0] *= s;
	this->mVector[1] *= s;
	this->mVector[2] *= s;

	return (*this);
}

const Vector3 Vector3::operator + (float s)
{
	return Vector3(this->mVector[0] + s, this->mVector[1] + s, this->mVector[2] + s);
}

const Vector3& Vector3::operator += (float s)
{
	this->mVector[0] += s;
	this->mVector[1] += s;
	this->mVector[2] += s;

	return (*this);
}

float* Vector3::copyTo(float f[3])
{
    f[0] = this->mVector[0];
    f[1] = this->mVector[1];
    f[2] = this->mVector[2];

    return f;
}

const Vector3 operator * (float s, const Vector3& v)
{
    return Vector3(v.mVector[0] * s, v.mVector[1] * s, v.mVector[2] * s);
}

const Vector3 operator + (float s, const Vector3& v)
{
    return Vector3(v.mVector[0] + s, v.mVector[1] + s, v.mVector[2] + s);
}

bool operator == (const float f[3], const Vector3& v)
{
    return (f[0] == v.mVector[0] && f[1] == v.mVector[1] && f[2] == v.mVector[2]);
}

bool operator != (const float f[3], const Vector3& v)
{
    return (f[0] != v.mVector[0] || f[1] != v.mVector[1] || f[2] != v.mVector[2]);
}
