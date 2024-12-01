
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler LinearSampler_Clamp = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

sampler PointSampler = sampler_state
{
    filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};



RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_CULLNONE
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
    FillMode = Solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Debug
{
    FillMode = WireFrame;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
	//BlendEnable[1] = true;

    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = Inv_Src_Alpha;
    BlendOp[0] = Add;

	//SrcBlend[1] = One;
	//DescBlend[1] = One;
	//BlendOp[1] = Add;
};
BlendState BS_OneByOne
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

 // 스포트 조명
float Calc_Spot_LightPower(float3 vLightDir, float3 vLightPos, float3 vNormal, float3 vPixelPos, float fAngle)
{
    float fNDotL = dot(vLightDir, vNormal);
    if (fNDotL > 0.f)
        return 0.f;

    float3 vLightToPixel = vPixelPos - vLightPos;
    float fSpotPower = dot(normalize(vLightDir), normalize(vLightToPixel));

        // 최소 값
    float fLimit = cos(radians(fAngle * 0.5));

        // 허용 범위
    float fGap = 1.f - fLimit;

    float ranges[4] =
    {
        fLimit + 0.5 * fGap,
                    fLimit + 0.3 * fGap,
                    fLimit + 0.1 * fGap,
                    fLimit
    };

    float values[4] = { 0.6f, 0.45f, 0.3f, 0.15f };

    for (int i = 0; i < 4; i++)
    {
        if (fSpotPower > ranges[i])
        {
            return values[i];
        }

    }

    return 0.f;
}