#pragma once

#include "DXF.h"

class SpinBlurShader : public BaseShader
{
private:
	struct spinBlurAmountBufferType
	{
		float blur;
		XMFLOAT3 padding;
	};
public:
	SpinBlurShader(ID3D11Device* device, HWND hwnd);
	~SpinBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float spinBlurValue);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* spinBlurAmountBuffer;
};

