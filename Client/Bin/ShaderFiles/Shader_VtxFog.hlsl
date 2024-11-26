#include "Engine_Shader_Defines.hlsli"

float2 g_Index, g_Winsize;
float g_FogStart;
float g_FogEnd;
float  g_Time;
matrix              g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D           g_Texture;
texture2D g_DepthTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{ 
    float4 vPosition : SV_POSITION;   
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In) // 진입점 함수 ( 내가 원하는 이름으로 만들 수 있다)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix); 
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = vPosition; // 이 값이 픽셀 셰이더에 들어갈 때는 보간된 투영 좌표(z나누기 안된 상태) 가 들어갈 것이다.
    return Out;
    
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};


struct PS_OUT
{
	/* 변수에 대한 시멘틱을 정의한다. */
    vector vColor : SV_TARGET0;

};

PS_OUT PS_FOG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 pixelColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    float pixelDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord).r;

    float fogFactor = saturate((g_FogEnd - pixelDepth) / (g_FogEnd - g_FogStart));
    float3 FogColor = { 1.f, 1.f, 0.f };
    float3 finalColor = lerp(FogColor, pixelColor.rgb, fogFactor);

    Out.vColor.rgb = finalColor;
    Out.vColor.a =  pixelColor.a;
    
    return Out;
}

technique11 DefaultTechnique // Technique : 어떤 버전으로 적혔는지 구분한다.
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FOG();
    }
}