#pragma once
#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <math.h>
#include <chrono>
#include <ctime>

class Vector
{
public:
	Vector(float inX = 0.0f, float inY = 0.0f, float inZ = 0.0f)
		: x_(inX), y_(inY), z_(inZ)
	{
	}

	Vector(const Vector& rhs)
		: x_(rhs.x_), y_(rhs.y_), z_(rhs.z_)
	{
	}

	Vector(Vector&& rhs)
		: x_(std::move(rhs.x_)), y_(std::move(rhs.y_)), z_(std::move(rhs.z_))
	{
	}

	Vector& operator=(const Vector& rhs)
	{
		this->x_ = rhs.x_;
		this->y_ = rhs.y_;
		this->z_ = rhs.z_;

		return *this;
	}

	Vector& operator=(Vector&& rhs)
	{
		this->x_ = std::move(rhs.x_);
		this->y_ = std::move(rhs.y_);
		this->z_ = std::move(rhs.z_);

		return *this;
	}

	Vector& operator-=(const Vector& rhs)
	{
		this->x_ -= rhs.x_;
		this->y_ -= rhs.y_;
		this->z_ -= rhs.z_;

		return *this;
	}

	friend Vector operator-(Vector lhs, const Vector& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	Vector Normalize2D() const
	{
		float length = sqrtf(powf(x_, 2) + powf(y_, 2));

		return Vector(x_ / length, y_ / length);
	}

	static float Distance(const Vector& v1, const Vector& v2)
	{
		return sqrtf(powf(v1.x_ - v2.x_, 2) + powf(v1.y_ - v2.y_, 2) + powf(v1.z_ - v2.z_, 2));
	}

	static float Angle(const Vector& v1, const Vector& v2)
	{
		float dotProcuct = v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_;
		float angle = acosf(dotProcuct);
		angle *= (180.0 / M_PI);

		return angle;
	}

	float x_;
	float y_;
	float z_;
};


class Rotator
{
public:
	Rotator(float inRoll = 0.0f, float inPitch = 0.0f, float inYaw = 0.0f)
		: roll_(inRoll), pitch_(inPitch), yaw_(inYaw)
	{
	}

	Rotator(const Rotator& rhs)
		: roll_(rhs.roll_), pitch_(rhs.pitch_), yaw_(rhs.yaw_)
	{
	}

	Rotator(Rotator&& rhs)
		: roll_(std::move(rhs.roll_)), pitch_(std::move(rhs.pitch_)), yaw_(std::move(rhs.yaw_))
	{
	}

	Rotator& operator=(const Rotator& rhs)
	{
		this->roll_ = rhs.roll_;
		this->pitch_ = rhs.pitch_;
		this->yaw_ = rhs.yaw_;

		return *this;
	}

	Rotator& operator=(Rotator&& rhs)
	{
		this->roll_ = std::move(rhs.roll_);
		this->pitch_ = std::move(rhs.pitch_);
		this->yaw_ = std::move(rhs.yaw_);

		return *this;
	}

	Vector Vector2D() const
	{
		float radinas = yaw_ * (M_PI / 180.0);
		return Vector(cosf(radinas), sinf(radinas));
	}

	float roll_;
	float pitch_;
	float yaw_;
};

struct Point
{
	int x;
	int y;
};
