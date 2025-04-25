#pragma once
#include "d3dx/dxApplication.h"
#include "mesh.h"
#include "environmentMapper.h"
#include "particleSystem.h"

class Puma : public DxApplication
{
public:
	using Base = DxApplication;

	explicit Puma(HINSTANCE appInstance);

protected:
	void Update(const Clock& dt) override;
	void Render() override;

private:
#pragma region CONSTANTS
	static const DirectX::XMFLOAT4 LIGHT_POS;
#pragma endregion
	dx_ptr<ID3D11Buffer> m_cbWorldMtx, //vertex shader constant buffer slot 0
		m_cbProjMtx;	//vertex shader constant buffer slot 2 & geometry shader constant buffer slot 0
	dx_ptr<ID3D11Buffer> m_cbViewMtx; //vertex shader constant buffer slot 1
	dx_ptr<ID3D11Buffer> m_cbSurfaceColor;	//pixel shader constant buffer slot 0
	dx_ptr<ID3D11Buffer> m_cbLightPos; //pixel shader constant buffer slot 1

	dx_ptr<ID3D11Buffer> m_vbParticleSystem;

	Mesh m_box;

	DirectX::XMFLOAT4X4 m_projMtx;
	DirectX::XMFLOAT4X4 m_mirrorMtx;

	ParticleSystem m_particleSystem;

	dx_ptr<ID3D11RasterizerState> m_rsCullFront;
	dx_ptr<ID3D11RasterizerState> m_rsCullBack;
	dx_ptr<ID3D11RasterizerState> m_rsCCW;
	dx_ptr<ID3D11BlendState> m_bsAlpha;
	dx_ptr<ID3D11BlendState> m_bsAdd;
	dx_ptr<ID3D11DepthStencilState> m_dssNoWrite;
	dx_ptr<ID3D11DepthStencilState> m_dssStencilWrite;
	dx_ptr<ID3D11DepthStencilState> m_dssStencilTest;
	dx_ptr<ID3D11SamplerState> m_samplerWrap;

	dx_ptr<ID3D11InputLayout> m_inputlayout, m_particleLayout;

	dx_ptr<ID3D11VertexShader> m_phongVS, m_textureVS, m_multiTexVS, m_particleVS;
	dx_ptr<ID3D11GeometryShader> m_particleGS;
	dx_ptr<ID3D11PixelShader> m_phongPS, m_texturePS, m_colorTexPS, m_multiTexPS, m_particlePS;

	void UpdateCameraCB(DirectX::XMMATRIX viewMtx);
	void UpdateCameraCB() { UpdateCameraCB(m_camera.getViewMatrix()); }

	void DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx);
	void DrawBox();

	void SetWorldMtx(DirectX::XMFLOAT4X4 mtx);
	void SetSurfaceColor(DirectX::XMFLOAT4 color);
	void SetShaders(const dx_ptr<ID3D11VertexShader>& vs, const dx_ptr<ID3D11PixelShader>& ps);
	void SetTextures(std::initializer_list<ID3D11ShaderResourceView*> resList, const dx_ptr<ID3D11SamplerState>& sampler);
	void SetTextures(std::initializer_list<ID3D11ShaderResourceView*> resList) { SetTextures(std::move(resList), m_samplerWrap); }

	void DrawScene();
};
