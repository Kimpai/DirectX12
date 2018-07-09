Texture2D shaderTexture;
SamplerState sampleType;

struct PixelShaderInput
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 textureColor;

    //Sample the pixel color from the texture using the sampler at this texture coordinate location
    textureColor = shaderTexture.Sample(sampleType, input.tex);

	return textureColor;
}