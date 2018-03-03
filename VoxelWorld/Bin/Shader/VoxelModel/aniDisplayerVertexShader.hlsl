cbuffer Trans
{
    float4x4 WVP;
};

struct VSInput
{
    float3 pos   : POSITION;
    float3 color : COLOR;
    float3 nor   : NORMAL;
};

struct VSOutput
{
    float4 pos   : SV_POSITION;
    float3 color : COLOR;
    float3 nor   : NORMAL;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    output.pos   = mul(float4(input.pos, 1.0f), WVP);
    output.color = input.color;
    output.nor   = input.nor;
    return output;
}
