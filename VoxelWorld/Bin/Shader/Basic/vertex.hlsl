cbuffer Trans
{
    float4x4 WVP;
};

struct VSInput
{
    float3 pos        : POSITION;
    float2 texCoord   : TEXCOORD;
    float3 lightColor : LIGHTCOLOR;
};

struct VSOutput
{
    float4 pos        : SV_POSITION;
    float2 texCoord   : TEXCOORD;
    float3 lightColor : LIGHTCOLOR;
};

VSOutput main(VSInput input)
{
    VSOutput rt = (VSOutput)0;
    rt.pos        = mul(float4(input.pos, 1.0f), WVP);
    rt.texCoord   = input.texCoord;
    rt.lightColor = input.lightColor;
    return rt;
}
