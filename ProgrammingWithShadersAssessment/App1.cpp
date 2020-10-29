#include "App1.h"


/**
Constructor.
*/
App1::App1()
{
}

/**
Destructor.
*/
App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (orthoMesh)
	{
		delete orthoMesh;
		orthoMesh = 0;
	}

	//Lights
	if (pointLight1)
	{
		delete pointLight1;
		pointLight1 = 0;
	}
	if (pointLight2)
	{
		delete pointLight2;
		pointLight2 = 0;
	}
	if (pointLight3)
	{
		delete pointLight3;
		pointLight3 = 0;
	}
	if (pointLight4)
	{
		delete pointLight4;
		pointLight4 = 0;
	}

	//Geometry
	if (lowerLeftPlane)
	{
		delete lowerLeftPlane;
		lowerLeftPlane = 0;
	}

	if (lowerRightPlane)
	{
		delete lowerRightPlane;
		lowerRightPlane = 0;
	}

	if (topLeftPlane)
	{
		delete topLeftPlane;
		topLeftPlane = 0;
	}

	if (topRightPlane)
	{
		delete topRightPlane;
		topRightPlane = 0;
	}

	if (floorPlane)
	{
		delete floorPlane;
		floorPlane = 0;
	}

	if (moon)
	{
		delete moon;
		moon = 0;
	}

	//Shaders
	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

	if (manipulationShader)
	{
		delete manipulationShader;
		manipulationShader = 0;
	}

	if (simpleShader)
	{
		delete simpleShader;
		simpleShader = 0;
	}

	if (spinBlurShader)
	{
		delete spinBlurShader;
		spinBlurShader = 0;
	}

	//Render Textures
	if (renderTexture)
	{
		delete renderTexture;
		renderTexture = 0;
	}

	if (spinBlurTexture)
	{
		delete spinBlurTexture;
		spinBlurTexture = 0;
	}


}

/**
Initialize Variables for the application.
@Param HINSTANCE.
@Param HWND.
@param int The screens Width.
@Param int The screens Height.
@Param Input Input class to input controls.
@Param bool VSYNC.
@Param bool FULL_SCREEN.
*/
void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input * in, bool VSYNC, bool FULL_SCREEN)
{

	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	textureMgr->loadTexture("grass", L"../res/grass.jpg");
	textureMgr->loadTexture("height", L"../res/height.png");
	textureMgr->loadTexture("height2", L"../res/HeightMap2.png");
	textureMgr->loadTexture("height3", L"../res/HeightMap3.jpg");
	textureMgr->loadTexture("height4", L"../res/HeightMap4.jpg");
	textureMgr->loadTexture("blank", L"../res/BlankHeightMap.jpg");
	textureMgr->loadTexture("moon", L"../res/Moon.jpg");
	textureMgr->loadTexture("brick", L"../res/brick1.dds");

	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	lowerLeftPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	lowerRightPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	topLeftPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	topRightPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	floorPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	moon = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	manipulationShader = new ManipulationShader(renderer->getDevice(), hwnd);
	simpleShader = new SimpleShader(renderer->getDevice(), hwnd);
	spinBlurShader = new SpinBlurShader(renderer->getDevice(), hwnd);

	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	spinBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);


	camera->setPosition(35.0, 5.0, -10.0);

	initLights();


	waveValues[0] = 0.2f;
	waveValues[1] = 0.09f;
	waveValues[2] = 5.0f;
}

/**
Frame
*/
bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{

	firstPass();

	spinBlur();

	finalPass();
	
	return true;
}

void App1::firstPass()
{
	// Set the render target to be the render to texture and clear it
	renderTexture->setRenderTarget(renderer->getDeviceContext());
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	
	camera->update();

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translationMatrix, tempWorldMatrix;
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	manipulationShaderCode(worldMatrix, viewMatrix, projectionMatrix);

	simpleShaderCode(worldMatrix, viewMatrix, projectionMatrix);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::spinBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	spinBlurTexture->setRenderTarget(renderer->getDeviceContext());
	spinBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = spinBlurTexture->getOrthoMatrix();

	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	spinBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, renderTexture->getShaderResourceView(), spinBlurValue);
	spinBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();

}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	//renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	//Black
	renderer->beginScene(0.0f, 0.0f, 0.0f, 1.0f);


	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, spinBlurTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	gui();

	//Swap the Buffers
	renderer->endScene();
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::SliderFloat("Spin Slider", &spinBlurValue, 0.0f, 3.0f);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void App1::manipulationShaderCode(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	XMMATRIX translationMatrix, bLWorldMx, bRWorldMx, tLWorldMx, tRWorldMx, floorWorldMx, moonWorldMx, scalingMatrix;
	deltaTime += timer->getTime();

	//Floor Plane
	translationMatrix = XMMatrixTranslation(-25.0, -5.0, 0.0);
	scalingMatrix = XMMatrixScaling(1.4f, 1.0f, 1.25f);
	floorWorldMx = XMMatrixMultiply(translationMatrix, worldMatrix);
	floorWorldMx = XMMatrixMultiply(floorWorldMx, scalingMatrix);
	//Draw Floor Plane
	floorPlane->sendData(renderer->getDeviceContext());
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), floorWorldMx, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), textureMgr->getTexture("blank"), camera, deltaTime, waveValues, pointLight1, pointLight2, pointLight3, pointLight4, AV);
	manipulationShader->render(renderer->getDeviceContext(), floorPlane->getIndexCount());
	//Lower left Plane

	translationMatrix = XMMatrixTranslation(-70.0, -10.0, 0.0);
	scalingMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	bLWorldMx = XMMatrixMultiply(translationMatrix, worldMatrix);
	bLWorldMx = XMMatrixMultiply(bLWorldMx, scalingMatrix);
	//Draw Lower Left Plane
	lowerLeftPlane->sendData(renderer->getDeviceContext());
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), bLWorldMx, viewMatrix, projectionMatrix, textureMgr->getTexture("grass"), textureMgr->getTexture("height"), camera, deltaTime, waveValues, pointLight1, pointLight2, pointLight3, pointLight4, AV);
	manipulationShader->render(renderer->getDeviceContext(), lowerLeftPlane->getIndexCount());
	
	//Lower Right Plane
	translationMatrix = XMMatrixTranslation(110.0, -9.0, -1.0);
	bRWorldMx = XMMatrixMultiply(translationMatrix, worldMatrix);
	bRWorldMx = XMMatrixMultiply(bRWorldMx, scalingMatrix);
	//Draw Lower Right Plane
	lowerRightPlane->sendData(renderer->getDeviceContext());
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), bRWorldMx, viewMatrix, projectionMatrix, textureMgr->getTexture("grass"), textureMgr->getTexture("height2"), camera, deltaTime, waveValues, pointLight1, pointLight2, pointLight3, pointLight4, AV);
	manipulationShader->render(renderer->getDeviceContext(), lowerRightPlane->getIndexCount());
	
	//Top Left Plane
	translationMatrix = XMMatrixTranslation(-70.0, -9.0, 147.5);
	tLWorldMx = XMMatrixMultiply(translationMatrix, worldMatrix);
	tLWorldMx = XMMatrixMultiply(tLWorldMx, scalingMatrix);
	//Draw Top Left Plane
	topLeftPlane->sendData(renderer->getDeviceContext());
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), tLWorldMx, viewMatrix, projectionMatrix, textureMgr->getTexture("grass"), textureMgr->getTexture("height3"), camera, deltaTime, waveValues, pointLight1, pointLight2, pointLight3, pointLight4, AV);
	manipulationShader->render(renderer->getDeviceContext(), topLeftPlane->getIndexCount());

	//Top Right Plane
	translationMatrix = XMMatrixTranslation(110.0, -9.0, 147.5);
	tRWorldMx = XMMatrixMultiply(translationMatrix, worldMatrix);
	tRWorldMx = XMMatrixMultiply(tRWorldMx, scalingMatrix);
	//Draw Top Right Plane
	topRightPlane->sendData(renderer->getDeviceContext());
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), tRWorldMx, viewMatrix, projectionMatrix, textureMgr->getTexture("grass"), textureMgr->getTexture("height4"), camera, deltaTime, waveValues, pointLight1, pointLight2, pointLight3, pointLight4, AV);
	manipulationShader->render(renderer->getDeviceContext(), topRightPlane->getIndexCount());

}

void App1::simpleShaderCode(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	XMMATRIX translationMatrix, moonWorldMx, scalingMatrix;
	deltaTime += timer->getTime();

	//Moon
	translationMatrix = XMMatrixTranslation(35.0, 5.0, 50.0);
	moonWorldMx = XMMatrixMultiply(translationMatrix, worldMatrix);
	//moonWorldMx = XMMatrixMultiply(moonWorldMx, scalingMatrix);
	//Draw Moon
	moon->sendData(renderer->getDeviceContext());
	simpleShader->setShaderParameters(renderer->getDeviceContext(), moonWorldMx, viewMatrix, projectionMatrix, textureMgr->getTexture("moon"), camera, pointLight1, pointLight2, pointLight3, pointLight4, AV);
	simpleShader->render(renderer->getDeviceContext(), moon->getIndexCount());

}

void App1::initLights()
{
	//WARNING Multiple Ambient Lights: Send in multiple ambient all being black but one, letting the user decide.
	pointLight1 = new Light;
	pointLight1->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	pointLight1->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f); //Only Ambient that is actually used!
	pointLight1->setPosition(80.0f,20.0f, 25.0f);

	pointLight2 = new Light;
	pointLight2->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	pointLight2->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	pointLight2->setPosition(0.0f, 20.0f, 25.0f);

	pointLight3 = new Light;
	pointLight3->setDiffuseColour(1.0f, 0.0f, 1.0f, 1.0f);
	pointLight3->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	pointLight3->setPosition(80.0f, 20.0f, 95.0f);

	pointLight4 = new Light;
	pointLight4->setDiffuseColour(0.0f, 1.0f, 0.0f, 1.0f);
	pointLight4->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	pointLight4->setPosition(0.0f, 20.0f, 95.0f);

	AV.constantFactor = 1.0f;
	AV.linearFactor = 0.025f;
	AV.quadraticFactor = 0.0f;
}

void App1::initPlanes()
{

}

