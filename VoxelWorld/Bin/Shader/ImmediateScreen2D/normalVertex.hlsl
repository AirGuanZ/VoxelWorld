cbuffer Trans
{
    float2 PosLB;
    float2 PosWH;
};

struct VSInput
{
    float2 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    output.pos = float4(LB + pos * WH, 0.0f, 1.0f);
    output.uv = float2(pos.x, 1.0f - pos.y);
    return output;
}
