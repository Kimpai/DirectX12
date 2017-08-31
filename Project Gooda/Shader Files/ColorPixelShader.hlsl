struct PixelShaderInput
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    //Return the interpolated color
	return input.color;
}