#include "dxApplication.h"
#include "utils/exceptions.h"

using namespace DirectX;
using namespace std;

DxApplication::DxApplication(HINSTANCE hInstance, int wndWidth, int wndHeight, std::wstring wndTitle)
	: WindowApplication(hInstance, wndWidth, wndHeight, wndTitle),
	m_device(m_window), m_inputDevice(hInstance),
	m_mouse(m_inputDevice.CreateMouseDevice(m_window.getHandle())),
	m_keyboard(m_inputDevice.CreateKeyboardDevice(m_window.getHandle())),
	m_camera(XMFLOAT3(0.5f, 1.2f, 2.5f), 0.47f, -2.85f), m_viewport{ m_window.getClientSize() }
{
	ID3D11Texture2D *temp = nullptr;
	auto hr = m_device.swapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&temp));
	const dx_ptr<ID3D11Texture2D> backTexture{ temp };
	if (FAILED(hr))
		THROW_DX(hr);
	m_backBuffer = m_device.CreateRenderTargetView(backTexture);
	m_depthBuffer = m_device.CreateDepthStencilView(m_window.getClientSize());

	ResetRenderTarget();
}

int DxApplication::MainLoop()
{
	MSG msg = { nullptr };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_clock.Query();
			Update(m_clock);
			Render();
			m_device.swapChain()->Present(0, 0);
		}
	}
	return static_cast<int>(msg.wParam);
}

void DxApplication::Render()
{
	const float clearColor[] = { 0.5f, 0.5f, 1.0f, 1.0f };
	m_device.context()->ClearRenderTargetView(m_backBuffer.get(), clearColor);
	m_device.context()->ClearDepthStencilView(m_depthBuffer.get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DxApplication::UpdateBuffer(const dx_ptr<ID3D11Buffer>& buffer, const void* data, size_t count)
{
	D3D11_MAPPED_SUBRESOURCE res;
	auto hr = m_device.context()->Map(buffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	if (FAILED(hr))
		THROW_DX(hr);
	memcpy(res.pData, data, count);
	m_device.context()->Unmap(buffer.get(), 0);
}

bool DxApplication::HandleCameraInput(double dt)
{
	MouseState mstate;
	KeyboardState kbState;
	bool moved = false;
	if (!m_mouse.GetState(mstate))
		return false;

	if (!m_keyboard.GetState(kbState))
		return false;

	auto d = mstate.getMousePositionChange();
	if (mstate.isButtonDown(0))
	{
		m_camera.Rotate(d.y * ROTATION_SPEED, d.x * ROTATION_SPEED);
		moved = true;
	}
	else if (mstate.isButtonDown(1))
	{
		m_camera.Zoom(d.y * ZOOM_SPEED);
		moved = true;
	}

	XMVECTOR moveVec = XMVectorZero();
	float moveSpeed = MOVE_SPEED * dt; // Units per second

	if (kbState.isKeyDown(DIK_W))
		moveVec += m_camera.getForwardDir(); // world forward (+Z)

	if (kbState.isKeyDown(DIK_S))
		moveVec -= m_camera.getForwardDir(); // world backward (-Z)

	if (kbState.isKeyDown(DIK_A))
		moveVec -= m_camera.getRightDir(); // world left (-X)

	if (kbState.isKeyDown(DIK_D))
		moveVec += m_camera.getRightDir();  // world right (+X)

	if (kbState.isKeyDown(DIK_Q))
		moveVec += XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);  // world up (+Y)

	if (kbState.isKeyDown(DIK_E))
		moveVec += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // world down (-Y)

	if (!XMVector3Equal(moveVec, XMVectorZero()))
	{
		moved = true;
		moveVec = XMVector3Normalize(moveVec);
		moveVec = XMVectorScale(moveVec, moveSpeed);
		m_camera.MoveTarget(moveVec);
	}


	return moved;
}

void DxApplication::ResetRenderTarget()
{
	auto backBuffer = m_backBuffer.get();
	m_device.context()->OMSetRenderTargets(1, &backBuffer, m_depthBuffer.get());
	m_device.context()->RSSetViewports(1, &m_viewport);
}
