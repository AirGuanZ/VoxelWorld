cbuffer Trans
{
    float4x4 WVP;
};

struct VSInput
{
    float3 pos   : POSITION;
    float3 nor   : NORMAL;
    float3 color : COLOR;
};

struct VSOutput
{
    float4 pos   : SV_POSITION;
    float3 nor   : POSITION;
    float3 color : COLOR;
};

VSOutput main(VSInput input)
{
    VSOutput rt = (VSOutput)0;
    rt.pos = mul(float4(input.pos, 1.0f), WVP);
    rt.nor = input.nor;
    rt.color = input.color;
    return rt;
}
