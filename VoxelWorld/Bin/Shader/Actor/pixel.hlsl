cbuffer Brightness
{
    float3 brightness;
};

Texture2D<float3> tex;
SamplerState sam;

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
    return float4(pow(brightness * tex.Sample(sam, input.uv), 0.75f), 1.0f);
}
