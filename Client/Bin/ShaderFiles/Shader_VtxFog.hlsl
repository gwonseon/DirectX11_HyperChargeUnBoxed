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

VS_OUT VS_MAIN(VS_IN In) // ������ �Լ� ( ���� ���ϴ� �̸����� ���� �� �ִ�)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix); 
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = vPosition; // �� ���� �ȼ� ���̴��� �� ���� ������ ���� ��ǥ(z������ �ȵ� ����) �� �� ���̴�.
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

technique11 DefaultTechnique 
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