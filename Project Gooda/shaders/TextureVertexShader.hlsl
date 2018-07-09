struct VertexShaderInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct VertexShaderOutput
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD;
};

cbuffer constantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 rotationMatrix;
}


VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;
    float4x4 wvp = mul(worldMatrix, viewMatrix);
    wvp = mul(wvp, projectionMatrix);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.tex = input.tex;

    return output;
}