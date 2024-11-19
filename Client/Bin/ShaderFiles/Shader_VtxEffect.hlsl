#include "Engine_Shader_Defines.hlsli"
//      전역변수들 : 컨스턴트 테이블


vector              g_vCamPosition;
matrix              g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D           g_Texture;
texture2D           g_DepthTexture;
float               g_fFar;

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


    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);


    float2 vTexcoord;

    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepth = g_DepthTexture.Sample(LinearSampler, vTexcoord);

    //if (vDepth.y == 0.0f)
    //{
    //    Out.vColor.a = Out.vColor.a; 
    //}
    //else
    //{
    //    float fOldZ = vDepth.y * g_fFar;
    //    float fViewZ = In.vProjPos.w;
    //    Out.vColor.a = Out.vColor.a * (fOldZ - fViewZ);
    //}
    return Out;
}



technique11 DefaultTechnique // Technique : 어떤 버전으로 적혔는지 구분한다.
{
    pass AlphaBlend
    {
        SetRasterizerState(RS_CULLNONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOFT();
    }

}