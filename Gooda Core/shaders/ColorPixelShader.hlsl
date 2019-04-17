struct PixelShaderInput
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 viewDirection : DIRECTION;
};

cbuffer LightBuffer : register(b1)
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float3 lightDirection;
    float specFactor;
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

        //Calculate the reflection vector based on the light intensity, normal vector, and light direction.
        float3 reflection = normalize(2 * lightIntensity * input.normal - lightDir);

        //Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
        float4 specular = pow(saturate(dot(reflection, input.viewDirection)), specFactor);

        color += specular;
    }

    //Multiply the final color with the material color
    color = color * input.color;

    //Saturate the final light color.
    color = saturate(color);

    //Return the interpolated color
    return color;
}