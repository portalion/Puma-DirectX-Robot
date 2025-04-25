#pragma once

#include <DirectXMath.h>

class Camera
{
private:
	void ClampDistance();

	float m_angleX, m_angleY;
	float m_distance, m_minDistance, m_maxDistance;
	DirectX::XMFLOAT4 m_target;

public:
	explicit Camera(DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(0, 0, 0),
		float minDistance = 0.0f, float maxDistance = FLT_MAX, float distance = 0.0f);
	explicit Camera(float minDistance, float maxDistance = FLT_MAX, float distance = 0.0f);

	DirectX::XMMATRIX getViewMatrix() const;
	DirectX::XMFLOAT4 getCameraPosition() const;

	inline void MoveTarget(DirectX::XMFLOAT3 v) { MoveTarget(XMLoadFloat3(&v)); }
	void MoveTarget(DirectX::FXMVECTOR v);
	void Rotate(float dx, float dy);
	void Zoom(float dd);
	void SetDistanceRange(float minDistance, float maxDistance);

	inline float getXAngle() const { return m_angleX; }
	inline float getYAngle() const { return m_angleY; }
	inline float getDistance() const { return m_distance; }
	inline DirectX::XMFLOAT4 getTarget() const { return m_target; }

	DirectX::XMVECTOR getForwardDir() const;
	DirectX::XMVECTOR getRightDir() const;
};
