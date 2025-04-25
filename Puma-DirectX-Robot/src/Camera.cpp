#include "Camera.h"

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 target, float minDistance, float maxDistance, float distance)
    : m_angleX(0.0f), m_angleY(0.0f), m_target(target.x, target.y, target.z, 1.f), m_distance(distance)
{
    SetDistanceRange(minDistance, maxDistance);
}

Camera::Camera(float minDistance, float maxDistance, float distance)
    : Camera(DirectX::XMFLOAT3{ 0.f, 0.f, 0.f }, minDistance, maxDistance, distance)
{
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
    return XMMatrixTranslation(-m_target.x, -m_target.y, -m_target.z) * XMMatrixRotationY(-m_angleY) *
        XMMatrixRotationX(-m_angleX) * XMMatrixTranslation(0, 0, m_distance);
}

DirectX::XMFLOAT4 Camera::GetCameraPosition() const
{
    if (m_distance == 0.0f)
        return m_target;
    XMMATRIX viewMtx = GetViewMatrix();
    XMVECTOR det;
    viewMtx = XMMatrixInverse(&det, viewMtx);
    XMFLOAT3 result(0.0f, 0.0f, 0.0f);
    auto translation = XMVector3TransformCoord(XMLoadFloat3(&result), viewMtx);
    XMStoreFloat3(&result, translation);
    return XMFLOAT4(result.x, result.y, result.z, 1.0f);
}

void Camera::MoveTarget(DirectX::FXMVECTOR v)
{
    auto pos = XMLoadFloat4(&m_target);
    pos = pos + v;
    XMStoreFloat4(&m_target, pos);
}

void Camera::Rotate(float dx, float dy)
{
    m_angleX = XMScalarModAngle(m_angleX + dx);
    m_angleY = XMScalarModAngle(m_angleY + dy);
}

void Camera::Zoom(float dd)
{
    m_distance += dd;
    ClampDistance();
}

void Camera::SetDistanceRange(float minDistance, float maxDistance)
{
    if (maxDistance < minDistance)
        maxDistance = minDistance;
    m_minDistance = minDistance;
    m_maxDistance = maxDistance;
    ClampDistance();
}

DirectX::XMVECTOR Camera::GetForwardDir() const
{
    auto forward = XMVectorSet(0, 0, 1, 0);
    return XMVector3TransformNormal(forward, XMMatrixRotationY(GetYAngle()));
}

DirectX::XMVECTOR Camera::GetRightDir() const
{
    auto right = XMVectorSet(1, 0, 0, 0);
    return XMVector3TransformNormal(right, XMMatrixRotationY(GetYAngle()));
}

void Camera::ClampDistance()
{
    if (m_distance < m_minDistance)
        m_distance = m_minDistance;
    else if (m_distance > m_maxDistance)
        m_distance = m_maxDistance;
}
