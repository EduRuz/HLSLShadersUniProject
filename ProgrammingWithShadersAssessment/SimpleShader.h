#pragma once
#include "DXF.h"
#include "ManipulationShader.h"


class SimpleShader : public BaseShader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 camPosition;
		float padding;

	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[4];
		XMFLOAT4 diffuse[4];
		XMFLOAT4 position[4];
		//XMFLOAT4 position[2];
		//XMFLOAT4 specular;
		//float specularPower;
		//XMFLOAT3 padding;
	};

	struct AttenuationBufferType
	{
		float constantFactor;
		float linearFactor;
		float quadraticFactor;
		float padding;

	};

	void initShader(WCHAR*, WCHAR*);

	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* vertexSampleState;

	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* attenuationBuffer;

public:
	SimpleShader(ID3D11Device* device, HWND hwnd);
	~SimpleShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light, Camera* cam);
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Camera* cam, Light* spotLight1, Light* spotLight2, Light* dirLight1, Light* dirLight2, AttenuationValues AV);
};

