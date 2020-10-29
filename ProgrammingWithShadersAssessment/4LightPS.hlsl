// Specular light pixel shader
// Calculate specular lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[4];
    float4 diffuse[4];
    float4 position[4];
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
    float3 position1, position2, position3, position4;
    position1.x = position[0].x;
    position1.y = position[0].y;
    position1.z = position[0].z;

    position2.x = position[1].x;
    position2.y = position[1].y;
    position2.z = position[1].z;

    position3.x = position[2].x;
    position3.y = position[2].y;
    position3.z = position[2].z;

    position4.x = position[3].x;
    position4.y = position[3].y;
    position4.z = position[3].z;



    float3 distVect1 = position1 - input.worldPosition;
    float3 distVect2 = position2 - input.worldPosition;
    float3 distVect3 = position3 - input.worldPosition;
    float3 distVect4 = position4 - input.worldPosition;
    float dist1 = length(distVect1);
    float dist2 = length(distVect2);
    float dist3 = length(distVect3);
    float dist4 = length(distVect4);

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    float3 lightVector1 = normalize(position1 - input.worldPosition);
    float3 lightVector2 = normalize(position2 - input.worldPosition);
    float3 lightVector3 = normalize(position3 - input.worldPosition);
    float3 lightVector4 = normalize(position4 - input.worldPosition);
    float attenuation = 1 / (constantFactor + (linearFactor * dist1) + (quadraticFactor * pow(dist1, 2)));
    float attenuation2 = 1 / (constantFactor + (linearFactor * dist2) + (quadraticFactor * pow(dist2, 2)));
    float attenuation3 = 1 / (constantFactor + (linearFactor * dist3) + (quadraticFactor * pow(dist3, 2)));
    float attenuation4 = 1 / (constantFactor + (linearFactor * dist4) + (quadraticFactor * pow(dist4, 2)));


    float4 point1, point2, point3, point4;
    point1 = calculateLighting(lightVector1, input.normal, diffuse[0] * attenuation);
    point2 = calculateLighting(lightVector2, input.normal, diffuse[1] * attenuation2);
    point3 = calculateLighting(lightVector3, input.normal, diffuse[2] * attenuation3);
    point4 = calculateLighting(lightVector4, input.normal, diffuse[3] * attenuation4);

	
    float4 lightColour2 = ambient[0] + saturate(point1 + point2 + point3 + point4);

    float4 lightPoints = ambient[0] + saturate(point1 + point2);
    float4 lightPoints2 = ambient[0] + saturate(point3 + point4);
    return lightColour2 * textureColour;
}