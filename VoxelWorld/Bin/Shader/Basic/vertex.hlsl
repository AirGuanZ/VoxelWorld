cbuffer Trans
{
    float4x4 VP;
};

struct VSInput
{
    float3 pos        : POSITION;
    float2 texCoord   : TEXCOORD;
    float3 lightColor : LIGHTCOLOR;
    float  sunlight   : SUNLIGHT;
};

struct VSOutput
{
    float4 pos        : SV_POSITION;
    float2 texCoord   : TEXCOORD;
    float3 lightColor : LIGHTCOLOR;
    float  sunlight   : SUNLIGHT;
    float3 posW       : WORLD_POS;
};

VSOutput main(VSInput input)
{
    VSOutput rt = (VSOutput)0;
    rt.pos        = mul(float4(input.pos, 1.0f), VP);
    rt.texCoord   = input.texCoord;
    rt.lightColor = input.lightColor;
    rt.sunlight   = input.sunlight;
    rt.posW       = input.pos;
    return rt;
}
