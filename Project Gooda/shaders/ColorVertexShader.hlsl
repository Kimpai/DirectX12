struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct VertexShaderOutput
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;
    float4x4 wvp = mul(worldMatrix, viewMatrix);
    wvp = mul(wvp, projectionMatrix);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    
    output.color = input.color;

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}