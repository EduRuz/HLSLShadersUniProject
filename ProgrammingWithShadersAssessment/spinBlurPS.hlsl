Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float blurValue;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 colour;

    const int SAMPLES = 15;
    float radialBlurLength = blurValue;
    float offset = 0.5f;

    float2 offsetCoord = input.tex - float2(offset, offset);
    float r = length(offsetCoord);
    float theta = atan2(offsetCoord.y, offsetCoord.x);

    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float2 tapCoords = (0.0f, 0.0f);

    for (float i = 0; i < SAMPLES; i++)
    {
        float tapTheta = theta + i * (radialBlurLength / SAMPLES);
        float tapR = r; 

        tapCoords.x = tapR * cos(tapTheta) + offset;
        tapCoords.y = tapR * sin(tapTheta) + offset;

        //sum += tex2D(shaderTexture, tapCoords);
        colour += shaderTexture.Sample(SampleType, tapCoords);

    }
    colour /= SAMPLES;

    return colour;
}