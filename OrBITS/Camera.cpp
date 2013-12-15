#include "Camera.h"

// Conversion factor from degrees to radians
#define DEG_IN_RAD(x) (x * M_PI / 180.0)

Camera::Camera()
: nearClippingDistance(0.1f), farClippingDistance(1000.0f), fieldOfView(DEG_IN_RAD(67.0f)), aspectRatio(1.0f) {
	Setup();
	Reset();	// Initialize position and orientation.

	int width = 512;
	int height = 512;
	aspectRatio = (float)width / (float)height;

	CalculateProjectionMatrix();
}

Camera::Camera(float nearClip, float farClip, float fov)
: nearClippingDistance(nearClip), farClippingDistance(farClip), fieldOfView(fov), aspectRatio(1.0f) {
	Setup();
	Reset();	// Initialize position and orientation.
	CalculateProjectionMatrix();
}

// Set up speed fields
void Camera::Setup()
{
	zoom_speed = 0.5f;
	move_speed = 10.0f;
	rotation_speed = 10.0f;
}

// Put camera back at initial position and orientation
void Camera::Reset() {
	//position = Vector3(0.0f, 0.0f, 0.0f);		// Set position to origin.
	position = initialPos;	// Set position to initial position
	orientation = Vector4(0.f, 0.f, 0.f, 1.f);			// Identity quaternion
	rightVector = Vector3(1.0f, 0.0f, 0.0f);	// X-Axis
	upVector = Vector3(0.0f, 1.0f, 0.0f);		// Y-Axis
	forwardVector = Vector3(0.0f, 0.0f, 1.0f);	// Z-Axis
	//viewMatr ix = identity();
	viewMatrix = Matrix4();	// Essentially same as above line
}

void Camera::SetViewportSize(int width, int height)
{
	aspectRatio = (float)width / height;
}

void Camera::Translate(float x, float y, float z) {
	position.x += x;
	position.y += y;
	position.z += z;
	CalculateViewMatrix();
}

void Camera::Translate(Vector3 direction, float distance) {
	Translate(direction.x * distance, direction.y * distance, direction.z * distance);
}

void Camera::SetInitialPosition(GLfloat x, GLfloat y, GLfloat z)
{
	Translate(x,y,z);
	initialPos = position;
}

void Camera::SetPosition(float x, float y, float z) {
	position = Vector3(x, y, z);
	CalculateViewMatrix();
}

void Camera::SetPosition(Vector3 pos) {
	position = pos;
	CalculateViewMatrix();
}

void Camera::Pitch(float angle) {
	Rotate(Vector3(5.0f, 0.0f, 0.0f), angle, true);
}

void Camera::Yaw(float angle) {
	Rotate(Vector3(0.0f, 5.0f, 0.0f), angle, true);
}

void Camera::Roll(float angle) {
	Rotate(Vector3(0.0f, 0.0f, 5.0f), angle, true);
}

void Camera::Rotate(Vector3 axis, float angle, bool inLocalSpace) {
	float radians = DEG_IN_RAD(angle);

	// Convert the axis and angle to a quaternion.
	Vector4 rotation(
		axis.x * sin(radians / 2.0f),
		axis.y * sin(radians / 2.0f),
		axis.z * sin(radians / 2.0f),
		cos(radians / 2.0f)
	);

	// Apply the rotation to our orientation quaternion.
	// How do we change a rotation in local space to a rotation in world space?
	float qx, qy, qz, qw;
	if (inLocalSpace)
	{
		qx = rotation.w * orientation.x + rotation.x * orientation.w + rotation.y * orientation.z - rotation.z * orientation.y;
		qy = rotation.w * orientation.y - rotation.x * orientation.z + rotation.y * orientation.w + rotation.z * orientation.x;
		qz = rotation.w * orientation.z + rotation.x * orientation.y - rotation.y * orientation.x + rotation.z * orientation.w;
		qw = rotation.w * orientation.w - rotation.x * orientation.x - rotation.y * orientation.y - rotation.z * orientation.z;
	}
	else
	{
		qx = orientation.w * rotation.x + orientation.x * rotation.w + orientation.y * rotation.z - orientation.z * rotation.y;
		qy = orientation.w * rotation.y - orientation.x * rotation.z + orientation.y * rotation.w + orientation.z * rotation.x;
		qz = orientation.w * rotation.z + orientation.x * rotation.y - orientation.y * rotation.x + orientation.z * rotation.w;
		qw = orientation.w * rotation.w - orientation.x * rotation.x - orientation.y * rotation.y - orientation.z * rotation.z;
	}

	float n = 1.0f / sqrt(qx * qx + qy * qy + qz * qz + qw * qw);	// Normalization factor

	// Update the orientation.
	orientation = Vector4(qx * n, qy * n, qz * n, qw * n);

	CalculateLocalVectors();
	CalculateViewMatrix();
}

void Camera::Zoom(float angle) {
	fieldOfView += angle;
	if (fieldOfView < 0.0f)	{ fieldOfView = 0.0f; }
	if (fieldOfView > M_PI)	{ fieldOfView = M_PI; }

	CalculateProjectionMatrix();
}

Vector3 Camera::Right() {
	return rightVector;
}

Vector3 Camera::Up() {
	return upVector;
}

Vector3 Camera::Forward() {
	return forwardVector;
}

Matrix4 Camera::ViewMatrix() {
	return viewMatrix;
}

Matrix4 Camera::ProjectionMatrix() {
	return projectionMatrix;
}

void Camera::CalculateLocalVectors() {
	float qx = orientation.x;
	float qy = orientation.y;
	float qz = orientation.z;
	float qw = orientation.w;

	// Derive the local axis vectors from the orientation quaternion.
	rightVector = Vector3(
		1.0f - 2.0f * (qy * qy + qz * qz),
		2.0f * (qx * qy - qw * qz),
		2.0f * (qx * qz + qw * qy)
	);

	upVector = Vector3(
		2.0f * (qx * qy + qw * qz),
		1.0f - 2.0f * (qx * qx + qz * qz),
		2.0f * (qy * qz - qw * qx)
	);

	forwardVector = Vector3(
		2.0f * (qx * qz - qw * qy),
		2.0f * (qy * qz + qw * qx),
		1.0f - 2.0f * (qx * qx + qy * qy)
	);
}

void Camera::CalculateViewMatrix() {
	float tx = position.x;
	float ty = position.y;
	float tz = position.z;

	// Convert the camera position into a translation matrix.
	// Remember that to translate the camera, we actually translate the world in the opposite direction.
	Matrix4 translationMatrix(
		1.0f, 0.0f, 0.0f, -position.x,
		0.0f, 1.0f, 0.0f, -position.y,
		0.0f, 0.0f, 1.0f, -position.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	//translationMatrix = translationMatrix.transpose();

	float qx = orientation.x;
	float qy = orientation.y;
	float qz = orientation.z;
	float qw = orientation.w;

	// Convert the orientation quaternion into a rotation matrix.
	Matrix4 rotationMatrix = Matrix4(
		1.0f - 2.0f * qy * qy - 2.0f * qz * qz	, 2.0f * qx * qy - 2.0f * qz * qw		, 2.0f * qx * qz + 2.0f * qy * qw		, 0.0f,
		2.0f * qx * qy + 2.0f * qz * qw			, 1.0f - 2.0f * qx * qx - 2.0f * qz * qz, 2.0f * qy * qz - 2.0f * qx * qw			, 0.0f,
		2.0f * qx * qz - 2.0f * qy * qw			, 2.0f * qy * qz + 2.0f * qx * qw		, 1.0f - 2.0f * qx * qx - 2.0f * qy * qy	, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	//rotationMatrix = rotationMatrix.transpose();

	// Multiply the two matrices to get the camera view matrix.
	viewMatrix = translationMatrix * rotationMatrix;
	//viewMatrix = viewMatrix.transpose();
}

void Camera::CalculateProjectionMatrix() {
	float range = tan(fieldOfView * 0.5f) * nearClippingDistance;
	float Sx = nearClippingDistance / (range * aspectRatio);
	float Sy = nearClippingDistance / range;
	float Sz = (nearClippingDistance + farClippingDistance) / (nearClippingDistance - farClippingDistance);
	float Pz = (2.0f * farClippingDistance * nearClippingDistance) / (nearClippingDistance - farClippingDistance);

	projectionMatrix = Matrix4(
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, Pz,
		0.0f, 0.0f, -1.0f, 0.0f
	);
}