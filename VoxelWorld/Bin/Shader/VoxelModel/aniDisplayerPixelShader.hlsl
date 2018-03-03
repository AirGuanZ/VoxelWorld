struct PSInput
{
    float4 pos   : SV_POSITION;
    float3 color : COLOR;
    float3 nor   : NORMAL;
};

float4 main(PSInput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}
