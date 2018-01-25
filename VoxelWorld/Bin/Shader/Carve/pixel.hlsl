cbuffer Sunlight
{
    float3 sunlightColor;
};

Texture2D<float4> tex;
SamplerState sam;

struct PSInput
{
    float4 pos        : SV_POSITION;
    float2 texCoord   : TEXCOORD;
    float3 lightColor : LIGHTCOLOR;
    float  sunlight   : SUNLIGHT;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 c = tex.Sample(sam, input.texCoord);
    clip(c.a - 0.5f);
    return float4(saturate(pow(c * (input.lightColor + input.sunlight * sunlightColor), 1.8f)), 1.0f);
}