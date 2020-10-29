#pragma once
#include "DXF.h"

struct AttenuationValues
{
	float constantFactor;
	float linearFactor;
	float quadraticFactor;
};

class ManipulationShader : public BaseShader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 camPosition;
		float padding;

	};

	struct TimeBufferType
	{
		float time;
		float height;
		float frequency;
		float speed;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[4];
		XMFLOAT4 diffuse[4];
		XMFLOAT4 position[4];
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

	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* attenuationBuffer;
	
public:
	ManipulationShader(ID3D11Device* device, HWND hwnd);
	~ManipulationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, Light* light, Camera* cam, float deltaT, float waveValues[3]);
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, Camera* cam, float deltaT, float waveValues[3], Light* pointLight1, Light* pointLight2, Light* pointLight3, Light* pointLight4, AttenuationValues AV);

};

