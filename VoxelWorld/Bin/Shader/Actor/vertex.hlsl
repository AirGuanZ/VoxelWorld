cbuffer Trans
{
    float4x4 WVP;
};

struct VSInput
{
    float3 pos : POSITION;
    float3 nor : NORMAL;
    float2 uv  : TEXCOORD;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD;
};

VSOutput main(VSInput input)
{
    VSOutput rt = (VSOutput)0;
    rt.pos = mul(float4(input.pos, 1.0f), WVP);
    rt.uv = input.uv;
    return rt;
}
