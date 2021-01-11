/* 
 * File:   vector3.h
 * Author: wouter
 *
 * Created on April 13, 2009, 5:24 PM
 */

#ifndef _VECTOR3_H
#define	_VECTOR3_H

class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(const Vector3& v);
	Vector3(const float v[3]);
	virtual ~Vector3();

	/// The magnitude or length of this vector. It is calculated with (x^2, y^2, z^2)^.5
	float length() const;

	/// Returns the unit vector with the same direction as this vector. It is normalized by dividing the vector with it's length
	const Vector3 unit() const;

	/// Returns the cross product. Calculated with (this)x(V)
	const Vector3 crossProduct(const Vector3& v) const;

	/// Returns the dotproduct. Calculated with (this).(V), this.x*v.x + this.y*v.y + this.z*v.z
	float dotProduct(const Vector3& v) const;

	/// Returns the vector as a float array
	operator float* ();

	/// Returns the vector as a constant float array
	operator const float* () const;

	/// Sets this vector to the v vector and returns a constant reference
	const Vector3& operator = (const Vector3& v);

	/// Sets this vector to the v array with floats and returns a constant reference
	const Vector3& operator = (const float v[3]);

	/// The equal operator for two vectors
	bool operator == (const Vector3& v);

	/// The not equal operator for two vectors
	bool operator != (const Vector3& v);

	/// The equal operator for this vector and a float array
	bool operator == (const float v[3]);

	/// The not equal operator for this vector and a float array
	bool operator != (const float v[3]);

	/// The operator for getting the result of multiplying this vector with another vector
	const Vector3 operator * (const Vector3& v);

	/// The operator for multiplying this vector with another vector
	const Vector3& operator *= (const Vector3& v);

	/// The operator for getting the result of adding another vector to this vector
	const Vector3 operator + (const Vector3& v);

	/// The operator for adding another vector to this vector
	const Vector3& operator += (const Vector3& v);

	/// The operator for getting the result of multiplying this vector with a scalar
	const Vector3 operator * (float s);

	/// The operator for multiplying this vector with a scalar
	const Vector3& operator *= (float s);

	/// The operator for getting the result of adding a scalar to this vector
	const Vector3 operator + (float s);

	/// The operator for adding a scalar to this vector
	const Vector3& operator += (float s);

	/// Returns the x value of the vector
	float x() { return mVector[0]; }

	/// Sets the x value of the vector
	void x(float x) { mVector[0] = x; }

	/// Returns the y value of the vector
	float y() { return mVector[1]; }

	/// Sets the y value of the vector
	void y(float y) { mVector[1] = y; }

	/// Returns the z value of the vector
	float z() { return mVector[2]; }

	/// Sets the z value of the vector
	void z(float z) { mVector[2] = z; }

        /// Copies the vector to the given float array
        float* copyTo(float f[3]);
	
private:
	float mVector[3];

	friend const Vector3 operator * (float s, const Vector3& v);
	friend const Vector3 operator + (float s, const Vector3& v);
	friend bool operator == (const float f[3], const Vector3& v);
	friend bool operator != (const float f[3], const Vector3& v);
	
};

/// The public operator for getting the result of multiplying a scalar with a vector
const Vector3 operator * (float s, const Vector3& v);

/// The public operator for getting the result of adding a scalar to a vector
const Vector3 operator + (float s, const Vector3& v);

/// The public equal operator for this vector and a float array
bool operator == (const float f[3], const Vector3& v);

/// The public not equal operator for this vector and a float array
bool operator != (const float f[3], const Vector3& v);

#endif	/* _VECTOR3_H */

