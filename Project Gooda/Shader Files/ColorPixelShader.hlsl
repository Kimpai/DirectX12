struct PixelShaderInput
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

cbuffer LightBuffer : register(b1)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    //Set the default output color to the ambient light value for all pixels.
    float4 color = ambientColor;

    //Invert the light direction for calculations
    float3 lightDir = -lightDirection;

    //Calculate the amount of light on this pixel.
    float lightIntensity = saturate(dot(input.normal, lightDir));

    if (lightIntensity > 0.0f)
    {
        //Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        color += (diffuseColor * lightIntensity);
    }

     //Saturate the final light color.
    color = saturate(color);

    //Multiply the final color with the material color
    color *= input.color;

    //Return the interpolated color
    return input.color;
}