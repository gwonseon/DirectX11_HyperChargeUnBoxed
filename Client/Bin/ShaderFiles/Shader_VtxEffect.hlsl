#include "Engine_Shader_Defines.hlsli"
//      전역변수들 : 컨스턴트 테이블


vector              g_vCamPosition;
matrix              g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D           g_Texture;
texture2D           g_MaskTexture;
texture2D           g_DepthTexture;

float               g_fFar;
float2              g_Index;
float2              g_ImageEA;
float               g_fTex_Move;


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


PS_OUT PS_MAIN_SOFT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrl = g_Texture.Sample(PointSampler, In.vTexcoord);

    float2 fSize = float2(1.0 / g_ImageEA.x, 1.0 / g_ImageEA.y); // 각 프레임의 UV 크기
    float2 fStart = float2(g_Index.x * fSize.x, g_Index.y * fSize.y);
    float2 UV = fStart + fSize * In.vTexcoord;

    vMtrl = g_Texture.Sample(PointSampler, UV);
    if (vMtrl.a == vMtrl.b == vMtrl.g == vMtrl.r) 
        vMtrl.a = vMtrl.r;
    
    Out.vColor = g_Texture.Sample(LinearSampler, UV);
    float2 vTexcoord;
    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepth = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    float fOldZ = vDepth.y * g_fFar;
    float fViewZ = In.vProjPos.w;
    if (fOldZ < fViewZ)
        return Out;
    
    float newAlpha = Out.vColor.a * (fOldZ - fViewZ);
    if (Out.vColor.a > newAlpha)
        Out.vColor.a = newAlpha;
    
    if (Out.vColor.a == 0.f )
        discard;
    if (Out.vColor.r <= 0.1f && Out.vColor.g <= 0.1f && Out.vColor.b <= 0.1f)
        discard;
    
    return Out;
}

PS_OUT    PS_MAIN_SOFT2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrl = g_Texture.Sample(PointSampler, In.vTexcoord);

    float2 fSize = float2(1.0 / g_ImageEA.x, 1.0 / g_ImageEA.y); // 각 프레임의 UV 크기
    float2 fStart = float2(g_Index.x * fSize.x, g_Index.y * fSize.y);
    float2 UV = fStart + fSize * In.vTexcoord;

    vMtrl = g_Texture.Sample(PointSampler, UV);
    if (vMtrl.a == vMtrl.b == vMtrl.g == vMtrl.r) 
        vMtrl.a = vMtrl.r;
    
    Out.vColor = g_Texture.Sample(LinearSampler, UV);
    float2 vTexcoord;
    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepth = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    float fOldZ = vDepth.y * g_fFar;
    float fViewZ = In.vProjPos.w;
    if (fOldZ < fViewZ)
        return Out;
    float newAlpha = Out.vColor.a * (fOldZ - fViewZ);
    if (Out.vColor.a > newAlpha)
        Out.vColor.a = newAlpha;
    
    return Out;
}


PS_OUT PS_MAIN3(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrl = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float2 fSize = float2(1.0 / g_ImageEA.x, 1.0 / g_ImageEA.y); // 각 프레임의 UV 크기
    float2 fStart = float2(g_Index.x * fSize.x, g_Index.y * fSize.y);
    float2 UV = fStart + fSize * In.vTexcoord;

    vMtrl = g_Texture.Sample(LinearSampler, UV);

    if (vMtrl.a == 0.f) 
        discard;
    if (vMtrl.r == 0.f)
        discard;
    Out.vColor = vMtrl;

    return Out;
}


PS_OUT PS_MAIN_SOFT4(PS_IN In)
{
   PS_OUT         Out = (PS_OUT)0;
   Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
   float2      vTexcoord;
   vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
   vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

   float4      vDepth = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    float fOldZ = vDepth.y * g_fFar;
   float      fViewZ = In.vProjPos.w;
    if (fOldZ < fViewZ)
        return Out;
    float newAlpha = Out.vColor.a * (fOldZ - fViewZ);
    if (Out.vColor.a > newAlpha)
        Out.vColor.a = newAlpha;
   return Out;
}

PS_OUT PS_LIGHTNING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 center = float2(0.5, 0.5);
    float2 distance;
    distance = In.vTexcoord - center;

    if (abs(distance.x) > g_fTex_Move && abs(distance.y) > g_fTex_Move)
        discard;
    distance += center;
    
    vector vMtrlDiffuse = g_Texture.Sample(PointSampler, distance);
    if (vMtrlDiffuse.r == 0.f) 
        discard;

    Out.vColor.rgb = vMtrlDiffuse.rgb * float3(0.f, 0.8f, 1.f);
    Out.vColor.a = vMtrlDiffuse.r;

    return Out;

}


PS_OUT PS_MISSILE_FLAME(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
    return Out;
}

technique11 DefaultTechnique 
{
    pass AlphaBlend // 0
    {
        SetRasterizerState(RS_CULLNONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOFT();
    }

    pass AlphaBlend1 // 1
    {
        SetRasterizerState(RS_CULLNONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOFT2();
    }

    pass AlphaBlend2 // 2
    {
        SetRasterizerState(RS_CULLNONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN3();
    }

    pass AlphaBlend3 // 3
    {
        SetRasterizerState(RS_CULLNONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOFT4();
    }
    pass LightningPass // 4
    {
        SetRasterizerState(RS_CULLNONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LIGHTNING();
    }

    pass MISSILE_FLAME // 5
    {
        SetRasterizerState(RS_CULLNONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MISSILE_FLAME();
    }

    
}