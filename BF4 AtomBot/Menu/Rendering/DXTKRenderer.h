#pragma once

#include "Menu/Rendering/DXTKFonts.h"
#include <d3d11.h>
#include <directxmath.h>
#include "DXTK/Inc/PrimitiveBatch.h"
#include "DXTK/Inc/VertexTypes.h"
#include <Effects.h>
#include <DirectXColors.h>
#include "DXTK/Inc/SpriteFont.h"
#include "DXTK/Inc/CommonStates.h"
#include "DXTK/Inc/GeometricPrimitive.h"
#include <codecvt>
#include <locale>
#include "DXTK/Inc/SimpleMath.h"
#include "DXTK/Inc/Effects.h"
#include <atlbase.h>
#include "Menu/Rendering/D3D11StateSaver.h"
#pragma comment(lib,"DirectXTK")

class DXTKRenderer :public RenderInterface
{
public:
	DXTKRenderer() :RenderInterface() {};
	virtual void DrawLineBox(const Vector2f& Position, const Vector2f& Size, const Color& color) override;
	virtual void DrawLineCircle(const Vector2f& Position,const float Radius, const Color& color) override;
	virtual void DrawLine(const Vector2f& Point1, const Vector2f& P2, const Color& color) override;
	virtual void DrawFilledBox(const Vector2f& Position, const Vector2f& Size, const Color& color) override;
	virtual void DrawFilledCircle(const Vector2f& Position, float Radius, const Color& color) override;
	virtual void DrawSphere(const SM::Vector3& Position, float Radius, const Color& color) override;
	virtual void RenderText(const Vector2f& Position, const Color& color, const char* format, ...) override;
	virtual Vector2f MeasureString(const char* format, ...) override;
	virtual HRESULT Init(IDXGISwapChain* pSwapChain, int Width, int Height);
	virtual ~DXTKRenderer();
	virtual void PreFrame() override;
	virtual void Present() override;

	virtual void BeginLine() override;
	virtual void EndLine() override;
	virtual void BeginText() override;
	virtual	void EndText() override;
private:
	float m_Width, m_Height;
	CComPtr<IDXGISwapChain> m_SwapChain;
	CComPtr<ID3D11Device> m_Device;
	CComPtr<ID3D11DeviceContext> m_DeviceContext;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>    m_Batch;
	std::unique_ptr<DirectX::BasicEffect> m_BatchEffect;
	std::unique_ptr<DirectX::SpriteBatch> m_SpriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_Font;
	std::unique_ptr<DirectX::CommonStates> m_CommonStates;

	CComPtr<ID3D11InputLayout> m_BatchInputLayout;

	StateSaver m_GameState;
	StateSaver m_SavedState;
};

void DXTKRenderer::DrawLineBox(const Vector2f& Position, const Vector2f& Size, const Color& color)
{
	DirectX::VertexPositionColor Verts[5];
	DirectX::SimpleMath::Vector4 DXColor = DirectX::SimpleMath::Vector4(color.R, color.G, color.B, color.A);
	DirectX::VertexPositionColor TopLeft(DirectX::XMFLOAT3(Position.x, Position.y, 0.0f), DXColor);
	DirectX::VertexPositionColor TopRight(DirectX::XMFLOAT3(Position.x + Size.x, Position.y, 0.0f), DXColor);
	DirectX::VertexPositionColor BottomLeft(DirectX::XMFLOAT3(Position.x, Position.y + Size.y, 0.0f), DXColor);
	DirectX::VertexPositionColor BottomRight(DirectX::XMFLOAT3(Position.x + Size.x, Position.y + Size.y, 0.0f), DXColor);

	Verts[0] = TopLeft;
	Verts[1] = TopRight;
	Verts[2] = BottomRight;
	Verts[3] = BottomLeft;

	TopLeft.position.y = TopLeft.position.y - 1;
	Verts[4] = TopLeft;
	m_Batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, Verts, 5);
}

void DXTKRenderer::DrawLineCircle(const Vector2f& Position,const float Radius, const Color& color)
{

}

void DXTKRenderer::DrawLine(const Vector2f& Point1, const Vector2f& Point2, const Color& color)
{
	DirectX::SimpleMath::Vector4 DXColor = DirectX::SimpleMath::Vector4(color.R, color.G, color.B, color.A);
	DirectX::VertexPositionColor v1(DirectX::XMFLOAT3(Point1.x,Point1.y,0.0f),DXColor);
	DirectX::VertexPositionColor v2(DirectX::XMFLOAT3(Point2.x, Point2.y, 0.0f), DXColor);
	m_Batch->DrawLine(v1, v2);
}

void DXTKRenderer::DrawFilledBox(const Vector2f& Position, const Vector2f& Size, const Color& color)
{
	DirectX::SimpleMath::Vector4 DXColor = DirectX::SimpleMath::Vector4(color.R, color.G, color.B, color.A);
	DirectX::VertexPositionColor TopLeft(DirectX::XMFLOAT3(Position.x, Position.y, 0.0f), DXColor);
	DirectX::VertexPositionColor TopRight(DirectX::XMFLOAT3(Position.x + Size.x, Position.y, 0.0f), DXColor);
	DirectX::VertexPositionColor BottomLeft(DirectX::XMFLOAT3(Position.x, Position.y + Size.y, 0.0f), DXColor);
	DirectX::VertexPositionColor BottomRight(DirectX::XMFLOAT3(Position.x + Size.x, Position.y + Size.y, 0.0f), DXColor);
	m_Batch->DrawQuad(TopLeft, TopRight, BottomRight, BottomLeft);
}

void DXTKRenderer::DrawFilledCircle(const Vector2f& Position, float Radius,const Color& FillColor)
{

}

void DXTKRenderer::DrawSphere(const SM::Vector3& Position, float Radius, const Color& color)
{
	SM::Matrix Trans = SM::Matrix::Identity * SM::Matrix::CreateTranslation(Position);

}

void DXTKRenderer::RenderText(const Vector2f& Position,const Color& color,const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char Buffer[1024];
	vsnprintf(Buffer, 1024, format, args);
	va_end(args);

	DirectX::SimpleMath::Vector4 DXColor{ color.R, color.G, color.B, color.A };
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring WStr = converter.from_bytes(Buffer);

	m_Font->DrawString(m_SpriteBatch.get(), WStr.c_str(), DirectX::XMFLOAT2(Position.x, Position.y), DXColor);
}

Vector2f DXTKRenderer::MeasureString(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char Buffer[1024];
	vsnprintf(Buffer, 1024, format, args);
	va_end(args);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring WStr = converter.from_bytes(Buffer);
	DirectX::SimpleMath::Vector2 Size = m_Font->MeasureString(WStr.c_str());
	return Vector2f(Size.x, Size.y);
}

HRESULT DXTKRenderer::Init(IDXGISwapChain* pSwapChain,int Width,int Height)
{
	HRESULT hr = S_OK;
	hr = pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&m_Device);
	m_Device->GetImmediateContext(&m_DeviceContext);
	m_SwapChain = pSwapChain;
	
	m_Width = Width;
	m_Height = Height;

	m_SpriteBatch.reset(new DirectX::SpriteBatch(m_DeviceContext));
	m_Font.reset(new DirectX::SpriteFont(m_Device, (uint8_t*)Consolas,sizeof(Consolas)));
	m_CommonStates.reset(new DirectX::CommonStates(m_Device));
	m_Batch.reset(new DirectX::PrimitiveBatch<DirectX::VertexPositionColor>(m_DeviceContext));
	m_BatchEffect.reset(new DirectX::BasicEffect(m_Device));
	m_BatchEffect->SetVertexColorEnabled(true);
	m_BatchEffect->SetWorld(DirectX::XMMatrixIdentity());
	m_BatchEffect->SetView(DirectX::XMMatrixIdentity());
	m_BatchEffect->SetProjection(DirectX::XMMatrixOrthographicOffCenterRH(0.0f, m_Width, m_Height,
		0.0f, 0.0f, 1.0f));

	void const* ShaderByteCode;
	size_t ByteCodeLength;
	m_BatchEffect->GetVertexShaderBytecode(&ShaderByteCode, &ByteCodeLength);
	hr = m_Device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		ShaderByteCode, ByteCodeLength, &m_BatchInputLayout);

	return hr;
}

DXTKRenderer::~DXTKRenderer()
{
	
}

void DXTKRenderer::BeginLine()
{
	m_BatchEffect->Apply(m_DeviceContext);
	m_DeviceContext->IASetInputLayout(m_BatchInputLayout);
	m_Batch->Begin();
}

void DXTKRenderer::EndLine()
{
	m_Batch->End();
}

void DXTKRenderer::BeginText()
{
	m_SavedState.StoreState(m_DeviceContext);
	m_SpriteBatch->Begin(DirectX::SpriteSortMode_Deferred);
}

void DXTKRenderer::EndText()
{
	m_SpriteBatch->End();
	m_SavedState.RestoreState(m_DeviceContext);
}

void DXTKRenderer::PreFrame()
{
	m_GameState.StoreState(m_DeviceContext);
	m_GameState.ZeroShaders(m_DeviceContext);
}

void DXTKRenderer::Present()
{
	m_GameState.RestoreState(m_DeviceContext);
}

