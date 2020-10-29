// Specular light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

cbuffer TimeBuffer : register(b1)
{
    float time;
    float height;
    float frequency;
    float speed;
}

cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition;
    float padding;
}

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewVector : TEXCOORD1;
};

OutputType main(InputType input)
{
    OutputType output;

	//offset Position based on sine wave
	//could multiply by height or move up by speed or increase by frequency
	//input.position.y += (height * sin( frequency * (input.position.x) + (time * speed)));
	

	//modify normals
	//input.normal.x = 1 - cos(input.position.x + time);
	//input.normal.y = abs(cos(input.position.x + time));

    float textureMap;
    textureMap = texture0.SampleLevel(sampler0, input.tex, 0);
    input.position.y = textureMap * 45.0f;
	//input.normal.y = textureMap * 15.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
	//output.tex += time * 0.1;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(normalize(input.normal), (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    output.viewVector = mul(input.position, worldMatrix).xyz;
	
	
    return output;
}