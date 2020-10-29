// Specular light pixel shader
// Calculate specular lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[4];
    float4 diffuse[4];
    float4 direction[2];
    float4 position[2];
   // float4 specular;
 //   float specularPower;
    //float3 padding;
};

cbuffer AttenuationBuffer : register(b1)
{
    float constantFactor;
    float linearFactor;
    float quadraticFactor;
    float padding2;
}
struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{

	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float4 specularPower) 
{
	//blinn-phong specular calculation
	float3 halfway = normalize(lightDirection + viewVector);
	float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
	return saturate(specularColour * specularIntensity);


}

float4 main(InputType input) : SV_TARGET
{
    float3 position1, position2;
    position1.x = position[0].x;
    position1.y = position[0].y;
    position1.z = position[0].z;

    position2.x = position[1].x;
    position2.y = position[1].y;
    position2.z = position[1].z;

    float3 distVect1 = position1 - input.worldPosition;
    float3 distVect2 = position2 - input.worldPosition;
    float dist1 = length(distVect1);
    float dist2 = length(distVect2);

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    float3 lightVector1 = normalize(position1 - input.worldPosition);
    float3 lightVector2 = normalize(position2 - input.worldPosition);
    float attenuation = 1 / (constantFactor + (linearFactor * dist1) + (quadraticFactor * pow(dist1, 2)));
    float attenuation2 = 1 / (constantFactor + (linearFactor * dist2) + (quadraticFactor * pow(dist2, 2)));


    float3 lightDirection1;
    lightDirection1.x = direction[0].x;
    lightDirection1.y = direction[0].y;
    lightDirection1.z = direction[0].z;

    float3 lightDirection2;
    lightDirection2.x = direction[1].x;
    lightDirection2.y = direction[1].y;
    lightDirection2.z = direction[1].z;

    float4 totalAmbient = ambient[0] + ambient[1];
    float4 totalAmbient2 = ambient[0] + ambient[1] + ambient[2] + ambient[3];
    float4 point1, point2, dir1, dir2;
    point1 = calculateLighting(lightVector1, input.normal, diffuse[0] * attenuation);
    point2 = calculateLighting(lightVector2, input.normal, diffuse[1] * attenuation2);
    dir1 = calculateLighting(-lightDirection1, input.normal, diffuse[2]);
    dir2 = calculateLighting(-lightDirection2, input.normal, diffuse[3]);
	//float4 lightColour = ambient + calculateLighting(lightVector, input.normal, diffuse);
    float4 lightColour = saturate(calculateLighting(-lightDirection1, input.normal, diffuse[2]) + calculateLighting(-lightDirection2, input.normal, diffuse[3]));
	//float4 lightColour = ambient + calculateLighting(lightVector, input.normal, (diffuse * attenuation));
	//float4 lightColour = totalAmbient + saturate(calculateLighting(lightVector1, input.normal, diffuse[0]) + calculateLighting(lightVector2, input.normal, diffuse[1]));
	//float4 lightColour2 = totalAmbient2 + saturate(point1 + point2);
    float4 lightColour2 = ambient[0] + saturate(point1 + point2 + dir1 + dir2);
    float lightC = ambient[0] + dir1;
    return lightColour2 * textureColour;
}