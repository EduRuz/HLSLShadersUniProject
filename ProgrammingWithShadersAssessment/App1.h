// Main.cpp
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	
#include "ManipulationShader.h"
#include "SimpleShader.h"
#include "TextureShader.h"
#include "SpinBlurShader.h"
class App1 : public BaseApplication
{
public:
	App1();
	~App1();

	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);
	bool frame();

protected:

	bool render();
	void firstPass();
	void spinBlur();
	void finalPass();
	void gui();
	void manipulationShaderCode(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	void simpleShaderCode(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	
private:

	void initLights();
	void initPlanes();

	PlaneMesh* lowerLeftPlane;
	PlaneMesh* lowerRightPlane;
	PlaneMesh* topLeftPlane;
	PlaneMesh* topRightPlane;
	PlaneMesh* floorPlane;
	OrthoMesh* orthoMesh;

	SphereMesh* moon;
	ManipulationShader* manipulationShader;
	SimpleShader* simpleShader;
	TextureShader* textureShader;

	RenderTexture* renderTexture;
	RenderTexture* spinBlurTexture;
	SpinBlurShader* spinBlurShader;

	Light* pointLight1;
	Light* pointLight2;
	Light* pointLight3;
	Light* pointLight4;

	AttenuationValues AV;

	float spinBlurValue;
	float waveValues[3];
	float deltaTime;
};

#endif