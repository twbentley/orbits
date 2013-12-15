#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Angel.h"

class Camera {
public:
	float zoom_speed;		// Camera zoom speed in unites per second
	float move_speed;		// Camera speed in units per second
	float rotation_speed;	// Camera rotation speed in degrees per second

	Camera();
	Camera(float nearClip, float farClip, float fov);

	void Setup();	// Set speed values

	void Reset();	// Revert back to the default state.

	void SetViewportSize(int width, int height);

	void Translate(float x, float y, float z);
	void Translate(Vector3 direction, float distance=1);

	void SetInitialPosition(GLfloat x, GLfloat y, GLfloat z);
	void SetPosition(float x, float y, float z);
	void SetPosition(Vector3 pos);

	void Pitch(float degrees);	// Rotate camera about the X-axis
	void Yaw(float degrees);	// Rotate camera about the Y-axis
	void Roll(float degrees);	// Rotate camera about the Z-axis
	void Zoom(float degrees);
	void Rotate(Vector3 axis, float degrees, bool inLocalSpace=false);

	Vector3 Right();
	Vector3 Up();
	Vector3 Forward();

	Matrix4 ViewMatrix();
	Matrix4 ProjectionMatrix();

//private:
	//Matrix4 CalculateRotationMatrix();
	//Matrix4 CalculateViewMatrix();
	//Matrix4 CalculateProjectionMatrix();

	// These fields determine the view matrix of the camera.
	Vector3 initialPos;
	Vector3 position;
	Vector4 orientation;	// Quaternion

	/*
	Matrix4 translationMatrix;
	Matrix4 rotationMatrix;
	bool rotationDirty;
	bool viewDirty;
	bool projDirty;
	*/
	// These fields determine the projection matrix of the camera.
	float nearClippingDistance;	// Shortest distance from camera that objects will be rendered.
	float farClippingDistance;	// Furthest distance from camera that objects will be rendered.
	float fieldOfView;
	float aspectRatio;

	// The following are all derived using the position and orientation.
	Vector3 rightVector;
	Vector3 upVector;
	Vector3 forwardVector;
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;

	void CalculateLocalVectors();
	void CalculateViewMatrix();
	void CalculateProjectionMatrix();
};

#endif