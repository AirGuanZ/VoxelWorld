Texture2D<float4> tex;
SamplerState sam;

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 c = tex.Sample(sam, input.uv);
    clip(c.a - 0.5f);
    return c;
}
