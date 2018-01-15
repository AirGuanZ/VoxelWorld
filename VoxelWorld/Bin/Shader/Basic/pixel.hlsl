Texture2D<float4> tex;
SamplerState sam;

struct PSInput
{
    float4 pos        : SV_POSITION;
    float2 texCoord   : TEXCOORD;
    float3 lightColor : LIGHTCOLOR;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 c = tex.Sample(sam, input.texCoord);
    return float4(c * input.lightColor, 1.0f);
}
