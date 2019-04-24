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
    float3 viewDirection : DIRECTION;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 projectionMatrix;
}

cbuffer ConstantBuffer : register(b2)
{
    float4x4 viewMatrix;
    float3 cameraPos;
    float padding;
}

VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;

    float4 worldPos = mul(float4(input.pos, 1.0f), worldMatrix);
    output.viewDirection = cameraPos - worldPos.xyz;
    output.viewDirection = normalize(output.viewDirection);

    float4x4 wvp = mul(worldMatrix, viewMatrix);
    wvp = mul(wvp, projectionMatrix);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    
    output.color = input.color;

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}