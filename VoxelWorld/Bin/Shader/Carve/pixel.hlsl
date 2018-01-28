cbuffer Sunlight
{
    float3 sunlightColor;
};

cbuffer Fog
{
    float fogStart;
    float3 fogColor;
    float fogRange;
    float3 camPosW;
};

Texture2D<float4> tex;
SamplerState sam;

struct PSInput
{
    float4 pos        : SV_POSITION;
    float2 texCoord   : TEXCOORD;
    float3 lightColor : LIGHTCOLOR;
    float  sunlight   : SUNLIGHT;
    float3 posW       : WORLD_POS;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 c = tex.Sample(sam, input.texCoord);
    clip(c.a - 0.5f);
    float3 appColor = pow(c * max(input.lightColor, input.sunlight * sunlightColor), 1.65f);

    float fogFactor = saturate((distance(input.posW.xz, camPosW.xz) - fogStart) / fogRange);
    float dFogFactor = 1.0f - fogFactor;

    return float4(dFogFactor * appColor + fogFactor * fogColor, dFogFactor * c.a + fogFactor);
}
