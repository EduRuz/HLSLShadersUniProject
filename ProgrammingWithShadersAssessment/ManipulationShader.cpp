#include "ManipulationShader.h"


ManipulationShader::ManipulationShader(ID3D11Device * device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"ManipulationVS.cso", L"4LightPS.cso");//With new shaders instead
}

ManipulationShader::~ManipulationShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	if (vertexSampleState)
	{
		vertexSampleState->Release();
		vertexSampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	// Release the camera constant buffer.
	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	// Release the time constant buffer.
	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void ManipulationShader::initShader(WCHAR * vsFilename, WCHAR * psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_SAMPLER_DESC vertexSamplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;
	D3D11_BUFFER_DESC attenuationBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);

	//set up camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	vertexSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	vertexSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	vertexSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	vertexSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	vertexSamplerDesc.MipLODBias = 0.0f;
	vertexSamplerDesc.MaxAnisotropy = 1;
	vertexSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	vertexSamplerDesc.MinLOD = 0;
	vertexSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&vertexSamplerDesc, &vertexSampleState);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	//Setup Attenuation Buffer
	attenuationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	attenuationBufferDesc.ByteWidth = sizeof(AttenuationBufferType);
	attenuationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	attenuationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	attenuationBufferDesc.MiscFlags = 0;
	attenuationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&attenuationBufferDesc, NULL, &attenuationBuffer);

}



void ManipulationShader::setShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * texture2, Light * light, Camera * cam, float deltaT, float waveValues[3])
{
	HRESULT result, timeResult, CamResult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	//send time data to the vertex shader
	TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = deltaT;
	timePtr->height = waveValues[0];
	timePtr->frequency = waveValues[1];
	timePtr->speed = waveValues[2];
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &timeBuffer);

	//send camera data to vertex shader
	CameraBufferType* camPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->camPosition = cam->getPosition();
	camPtr->padding = deltaT;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &cameraBuffer);

	////Additional
	//// Send light data to pixel shader
	//LightBufferType* lightPtr;
	//deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//lightPtr = (LightBufferType*)mappedResource.pData;
	//lightPtr->diffuse = light->getDiffuseColour();
	//lightPtr->ambient = light->getAmbientColour();
	//lightPtr->specular = light->getSpecularColour();
	//lightPtr->specularPower = light->getSpecularPower();
	//lightPtr->direction = light->getDirection();
	//deviceContext->Unmap(lightBuffer, 0);
	//deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	deviceContext->VSSetShaderResources(0, 1, &texture2);
	deviceContext->VSSetSamplers(0, 1, &vertexSampleState);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);



}

void ManipulationShader::setShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * texture2, Camera * cam, float deltaT, float waveValues[3], Light * pointLight1, Light * pointLight2, Light * pointLight3, Light * pointLight4, AttenuationValues AV)
{

	HRESULT result, timeResult, CamResult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	//send time data to the vertex shader
	TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = deltaT;
	timePtr->height = waveValues[0];
	timePtr->frequency = waveValues[1];
	timePtr->speed = waveValues[2];
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &timeBuffer);

	//send camera data to vertex shader
	CameraBufferType* camPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->camPosition = cam->getPosition();
	camPtr->padding = deltaT;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &cameraBuffer);

	//Additional
	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->diffuse[0] = pointLight1->getDiffuseColour();
	lightPtr->ambient[0] = pointLight1->getAmbientColour();
	lightPtr->position[0].x = pointLight1->getPosition().x;
	lightPtr->position[0].y = pointLight1->getPosition().y;
	lightPtr->position[0].z = pointLight1->getPosition().z;
	lightPtr->position[0].w = 1.0f;


	lightPtr->diffuse[1] = pointLight2->getDiffuseColour();
	lightPtr->ambient[1] = pointLight2->getAmbientColour();
	lightPtr->position[1].x = pointLight2->getPosition().x;
	lightPtr->position[1].y = pointLight2->getPosition().y;
	lightPtr->position[1].z = pointLight2->getPosition().z;
	lightPtr->position[1].w = 1.0f;

	lightPtr->diffuse[2] = pointLight3->getDiffuseColour();
	lightPtr->ambient[2] = pointLight3->getAmbientColour();
	lightPtr->position[2].x = pointLight3->getPosition().x;
	lightPtr->position[2].y = pointLight3->getPosition().y;
	lightPtr->position[2].z = pointLight3->getPosition().z;
	lightPtr->position[2].w = 1.0f;

	lightPtr->diffuse[3] = pointLight4->getDiffuseColour();
	lightPtr->ambient[3] = pointLight4->getAmbientColour();
	lightPtr->position[3].x = pointLight4->getPosition().x;
	lightPtr->position[3].y = pointLight4->getPosition().y;
	lightPtr->position[3].z = pointLight4->getPosition().z;
	lightPtr->position[3].w = 1.0f;

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	AttenuationBufferType* attenPtr;
	deviceContext->Map(attenuationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	attenPtr = (AttenuationBufferType*)mappedResource.pData;
	attenPtr->constantFactor = AV.constantFactor;
	attenPtr->linearFactor = AV.linearFactor;
	attenPtr->quadraticFactor = AV.quadraticFactor;
	attenPtr->padding = 0.0f;
	deviceContext->Unmap(attenuationBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &attenuationBuffer);


	deviceContext->VSSetShaderResources(0, 1, &texture2);
	deviceContext->VSSetSamplers(0, 1, &vertexSampleState);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

}
