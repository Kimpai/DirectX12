struct PixelShaderInput
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

cbuffer LightBuffer : register(b1)
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    //Invert the light direction for calculations
    float3 lightDir = -lightDirection;

    //Calculate the amount of light on this pixel
    float lightIntensity = saturate(dot(input.normal, lightDir));

    //Determine the final amount of diffuse color based on the diffuse color combined with the light intensity
    input.color *= saturate(diffuseColor * lightIntensity);

    //Return the interpolated color
    return input.color;
}