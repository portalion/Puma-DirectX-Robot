#include "environmentMapper.h"
#include "d3dx/dxstructures.h"
#include "d3dx/dxDevice.h"

using namespace DirectX;
using namespace std;

const int EnvironmentMapper::TEXTURE_SIZE = 256;

EnvironmentMapper::EnvironmentMapper(const DxDevice& device, float nearPlane, float farPlane, XMFLOAT3 position)
	: m_nearPlane(nearPlane), m_farPlane(farPlane), m_position(position.x, position.y, position.z, 1.0f)
{
	Texture2DDescription texDesc;
	texDesc.Width = TEXTURE_SIZE;
	texDesc.Height = TEXTURE_SIZE;
	texDesc.MipLevels = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

	m_faceTexture = device.CreateTexture(texDesc);
	m_renderTarget = device.CreateRenderTargetView(m_faceTexture);

	SIZE s;
	s.cx = s.cy = TEXTURE_SIZE;
	m_depthBuffer = device.CreateDepthStencilView(s);

	Texture2DDescription envCubeMapDesc;
	envCubeMapDesc.ArraySize = 6;
	envCubeMapDesc.Width = TEXTURE_SIZE;
	envCubeMapDesc.Height = TEXTURE_SIZE;
	envCubeMapDesc.MipLevels = 1;
	envCubeMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	envCubeMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	m_envTexture = device.CreateTexture(envCubeMapDesc);
	m_envView = device.CreateShaderResourceView(m_envTexture);

	//Shaders
	auto vsCode = device.LoadByteCode(L"out/envMapVS.cso");
	m_envVS = device.CreateVertexShader(vsCode);
	auto psCode = device.LoadByteCode(L"out/envMapPS.cso");
	m_envPS = device.CreatePixelShader(psCode);
}

void EnvironmentMapper::Begin(const dx_ptr<ID3D11DeviceContext>& context) const
{
	context->VSSetShader(m_envVS.get(), nullptr, 0);
	auto tx_ptr = m_envView.get();
	context->PSSetShaderResources(0, 1, &tx_ptr);
	context->PSSetShader(m_envPS.get(), nullptr, 0);
}

DirectX::XMMATRIX EnvironmentMapper::FaceViewMtx(D3D11_TEXTURECUBE_FACE face) const
{
	switch (face)
	{
	case D3D11_TEXTURECUBE_FACE_POSITIVE_X:
		return XMMatrixLookAtLH(XMLoadFloat4(&m_position), XMLoadFloat4(&m_position) + XMVectorSet(1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_X:
		return XMMatrixLookAtLH(XMLoadFloat4(&m_position), XMLoadFloat4(&m_position) + XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));

	case D3D11_TEXTURECUBE_FACE_POSITIVE_Z:
		return XMMatrixLookAtLH(XMLoadFloat4(&m_position), XMLoadFloat4(&m_position) + XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_Z:
		return XMMatrixLookAtLH(XMLoadFloat4(&m_position), XMLoadFloat4(&m_position) + XMVectorSet(0.f, 0.f, -1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));

	case D3D11_TEXTURECUBE_FACE_POSITIVE_Y:
		return XMMatrixLookAtLH(XMLoadFloat4(&m_position), XMLoadFloat4(&m_position) + XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, -1.f, 0.f));
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_Y:
		return XMMatrixLookAtLH(XMLoadFloat4(&m_position), XMLoadFloat4(&m_position) + XMVectorSet(0.f, -1.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 1.f, 0.f));
	}

	return XMMatrixIdentity();
}

DirectX::XMFLOAT4X4 EnvironmentMapper::FaceProjMtx() const
{
	XMFLOAT4X4 proj;

	XMStoreFloat4x4(&proj, XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.f, m_nearPlane, m_farPlane));

	return proj;
}

void EnvironmentMapper::SetTarget(const dx_ptr<ID3D11DeviceContext>& context)
{
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = TEXTURE_SIZE;
	viewport.Height = TEXTURE_SIZE;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;


	context->RSSetViewports(1, &viewport);
	ID3D11RenderTargetView* targets[1] = { m_renderTarget.get() };
	context->OMSetRenderTargets(1, targets, m_depthBuffer.get());
}

void EnvironmentMapper::ClearTarget(const dx_ptr<ID3D11DeviceContext>& context)
{
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(m_renderTarget.get(), clearColor);
	context->ClearDepthStencilView(m_depthBuffer.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void EnvironmentMapper::SaveFace(const dx_ptr<ID3D11DeviceContext>& context, D3D11_TEXTURECUBE_FACE face)
{
	context->CopySubresourceRegion(
		m_envTexture.get(),
		static_cast<UINT>(face),
		0, 0, 0,
		m_faceTexture.get(),
		0,
		nullptr
	);

}