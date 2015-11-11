#pragma once
#define SAFE_RELEASE( x ) if( x ){ x->Release( ); x = nullptr; }
typedef struct
{
	CComPtr<ID3D11RasterizerState> pRasterizer;
	CComPtr<ID3D11DepthStencilState> pDepthStencilState;
	CComPtr<ID3D11BlendState> pBlendState;
	CComPtr<ID3D11SamplerState> pSamplerState;
	CComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	CComPtr<ID3D11PixelShader> pPixelShader;
	CComPtr<ID3D11VertexShader> pVertexShader;
	CComPtr<ID3D11InputLayout> pInputLayout;
	
	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology;

	FLOAT fBlendFactor[4];
	UINT uiBlendSampleMask, uiDepthStencilRef;
}ID3D11StateBlock;

class StateSaver
{
public:
	void StoreState(ID3D11DeviceContext* pContext);
	void RestoreState(ID3D11DeviceContext* pContext);
	void ZeroShaders(ID3D11DeviceContext* pContext);
	~StateSaver();
private:
	ID3D11StateBlock m_SavedState;
};

StateSaver::~StateSaver()
{
	
}

void StateSaver::StoreState(ID3D11DeviceContext* pContext)
{
	pContext->IAGetInputLayout(&m_SavedState.pInputLayout);
	pContext->IAGetPrimitiveTopology(&m_SavedState.PrimitiveTopology);

	pContext->PSGetSamplers(0, 1, &m_SavedState.pSamplerState);
	pContext->PSGetShaderResources(0, 1, &m_SavedState.pShaderResourceView);
	pContext->PSGetShader(&m_SavedState.pPixelShader, NULL, NULL);

	pContext->RSGetState(&m_SavedState.pRasterizer);
	
	pContext->OMGetBlendState(&m_SavedState.pBlendState, m_SavedState.fBlendFactor, &m_SavedState.uiBlendSampleMask);
	pContext->OMGetDepthStencilState(&m_SavedState.pDepthStencilState, &m_SavedState.uiDepthStencilRef);
	
	pContext->VSGetShader(&m_SavedState.pVertexShader, NULL, NULL);
}

void StateSaver::RestoreState(ID3D11DeviceContext* pContext)
{
	pContext->IASetInputLayout(m_SavedState.pInputLayout);
	pContext->IASetPrimitiveTopology(m_SavedState.PrimitiveTopology);

	pContext->PSSetSamplers(0, 1, &m_SavedState.pSamplerState);
	pContext->PSSetShaderResources(0, 1, &m_SavedState.pShaderResourceView);
	pContext->PSSetShader(m_SavedState.pPixelShader, NULL, NULL);

	pContext->RSSetState(m_SavedState.pRasterizer);

	pContext->OMSetBlendState(m_SavedState.pBlendState, m_SavedState.fBlendFactor, m_SavedState.uiBlendSampleMask);
	pContext->OMSetDepthStencilState(m_SavedState.pDepthStencilState, m_SavedState.uiDepthStencilRef);

	pContext->VSSetShader(m_SavedState.pVertexShader, NULL, NULL);
}

void StateSaver::ZeroShaders(ID3D11DeviceContext* pContext)
{
	pContext->VSSetShader(NULL, NULL, 0);
	pContext->PSSetShader(NULL, NULL, 0);
	pContext->HSSetShader(NULL, NULL, 0);
	pContext->DSSetShader(NULL, NULL, 0);
	pContext->GSSetShader(NULL, NULL, 0);
}