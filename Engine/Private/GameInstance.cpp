#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Input_Device.h"
#include "PipeLine.h"
#include "Renderer.h"
#include "Light_Manager.h"
#include "font_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 내 게임에 필요한 필수 기능들에 대한 초기화과정을 수행한다. */
	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 타이머매니져를 준비한다. */
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	/* 입력 장치를 초기화한다. */
	/* 오브젝트, 컴포넌트 매니져를 사용할 준비를 한다. */
	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	
	m_pComponent_Manager = CComponent_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;


	/* 등등등등 */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pPicking_Manager = CPicking_Manager::Create();
	if (nullptr == m_pPicking_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pRound_Manager = CRound_Manager::Create();
	if (nullptr == m_pRound_Manager)
		return E_FAIL;
	
	m_pUI_Manager = CUIManager::Create();
	if (nullptr == m_pUI_Manager)
		return E_FAIL;
	return S_OK;
}

void CGameInstance::Update(_float fTimeDelta)
{
	/* 엔진에있는 객체들 중 반복적인 갱신이 필요한 녀석이라면 여기서 다 호출. */
	m_pInput_Device->Update_InputDev();

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pObject_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pPipeLine->Update();

	m_pLevel_Manager->Update(fTimeDelta);
}

void CGameInstance::Draw()
{
	/* 게임내에 필요한 대다수의 객체들을 모두 그려낸다. */
	
	m_pRenderer->Draw();

	/* 할일이 없어. 디버그모드에서만 디버그내용만 출력하는 용도 .*/
	m_pLevel_Manager->Render();
}

void CGameInstance::Clear(_uint iClearLevelID)
{
	m_pObject_Manager->Clear(iClearLevelID);
	m_pComponent_Manager->Clear(iClearLevelID);
	m_pRenderer->RenderList_Clear();
	/*iClearLevelID에 해당하는 자원들을 정리한다.*/
}

_float CGameInstance::Compute_Random_Normal()
{
	return rand() / (_float)RAND_MAX;
}

_float CGameInstance::Compute_Random(_float fMin, _float fMax)
{
	return (fMax - fMin) * Compute_Random_Normal() + fMin;
}

void CGameInstance::Reset_Object(_uint iClearLevelID)
{
	m_pObject_Manager->Clear(iClearLevelID);
}

HRESULT CGameInstance::Render_Begin(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 백버퍼를 내가 지정한 색으로 클리어(초기화)한다. */
	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);

	/* 깊이버퍼와 스텐실버퍼를 내가 지정한 값으로 클리어(초기화)한다.*/
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Render_End()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Present();

	return S_OK;
}
//
//HRESULT CGameInstance::Set_BlendState(const CGraphic_Device::BLEND_STATE& BS)
//{
//	if (nullptr == m_pGraphic_Device) return E_FAIL;
//
//	return m_pGraphic_Device->Set_BlendState(BS);
//}
//
//ID3D11Device* CGameInstance::Get_Device() { return m_pGraphic_Device->Get_Device(); }
//
//ID3D11DeviceContext* CGameInstance::Get_Context() { return m_pGraphic_Device->Get_Context(); }

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Update_TimeDelta(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Update_TimeDelta(strTimerTag);
}

_ubyte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_ubyte CGameInstance::Get_DIKeyState_Pressing(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState_Pressing(byKeyID);
}

_ubyte CGameInstance::Get_DIKeyState_Up(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState_Up(byKeyID);
}

_ubyte CGameInstance::Get_DIKeyState_Down(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState_Down(byKeyID);
}

_ubyte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_ubyte CGameInstance::Get_DIMouseState_Down(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState_Down(eMouse);
}

_ubyte CGameInstance::Get_DIMouseState_Pressing(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState_Pressing(eMouse);
}

_ubyte CGameInstance::Get_DIMouseState_Up(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState_Up(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

HRESULT CGameInstance::Open_Level(_uint iCurrentLevelID, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iCurrentLevelID, pNewLevel);
}

HRESULT CGameInstance::Close_Level(_uint iLevelID)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Close_Level(iLevelID);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex, _uint iPartObjID)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex, iPartObjID);
}


HRESULT CGameInstance::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);

}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	
	return m_pObject_Manager->Add_GameObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Add_GameObject_ToLayer_ReturnObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject_ToLayer_ReturnObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Get_Prototype(_uint iLevelIndex, const _tchar* pLayerTag, const _wstring& strPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Get_Prototype(iLevelIndex, pLayerTag, strPrototypeTag);
}

//CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex)
//{
//	if (nullptr == m_pObject_Manager)
//		return nullptr;
//	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
//}

CGameObject* CGameInstance::Find_Prototype(const _wstring& strPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Clone_Prototype(const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_Prototype(strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Prototype_Component(_uint iLevelIndex, const _wstring& strPrototypeTag)
{

	return m_pComponent_Manager->Find_Prototype(iLevelIndex, strPrototypeTag);
}

HRESULT CGameInstance::Add_RenderGameObject(CRenderer::RENDERGROUP eRenderGroup, CGameObject* pRenderGameObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGameObject(eRenderGroup, pRenderGameObject);
}

void CGameInstance::RenderList_Clear()
{
	m_pRenderer->RenderList_Clear();
}

const _float4x4* CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_TransformMatrix(eState);
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_TransformMatrixInverse(eState);
}

const _float4* CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_TransformMatrix(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_TransformMatrix(eState, TransformMatrix);

}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

_float3 CGameInstance::Get_MousePos_NDC(HWND hWnd, const unsigned int g_iWinSizeX, const unsigned int g_iWinSizeY)
{
	return m_pPicking_Manager->Get_MousePos_NDC(hWnd, g_iWinSizeX, g_iWinSizeY);
}

_float4 CGameInstance::Object_NDC_Cal(_float2 fPos, _float fSizeX, _float fSizeY, const unsigned int g_iWinSizeX, const unsigned int g_iWinSizeY)
{
	return m_pPicking_Manager->Object_NDC_Cal(fPos, fSizeX, fSizeY, g_iWinSizeX, g_iWinSizeY);
}

void CGameInstance::Get_MouseRayDirection(_float3 fPosition, XMMATRIX invProj, XMMATRIX invView, XMVECTOR* RayPos_Output, XMVECTOR* RayDir_Output)
{
	return m_pPicking_Manager->Get_MouseRayDirection(fPosition, invProj, invView, RayPos_Output, RayDir_Output);
}

_float3 CGameInstance::Picking_Terrain(XMVECTOR RayPos, XMVECTOR RayDir, const _float3* VtxPos, _uint VtxCountX, _uint VtxCountZ)
{
	return m_pPicking_Manager->Picking_Terrain(RayPos, RayDir, VtxPos, VtxCountX, VtxCountZ);
}

_float3 CGameInstance::Picking_Box_FAILED(_vector RayPos, _vector RayDir, const _float3* VtxPos)
{
	return m_pPicking_Manager->Picking_Box_FAILED(RayPos, RayDir, VtxPos);
}

void CGameInstance::CreateBoundingBox(const _float3& center, const _float3& size, _float3& fMinPoint, _float3& fMaxPoint)
{
	return m_pPicking_Manager->CreateBoundingBox(center, size, fMinPoint, fMaxPoint);

}

bool CGameInstance::Picking_Box(const _vector& rayOrigin, const _vector& rayDirection, const _float3& fMinPoint, const _float3& fMaxPoint, float& distance, DirectX::BoundingBox box)
{
	return m_pPicking_Manager->Picking_Box(rayOrigin, rayDirection, fMinPoint, fMaxPoint, distance, box);
}

void CGameInstance::Collision_Layer(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iPartObjID, _uint iDstPartObjID)
{
	
	m_pCollision_Manager->Collision_Layer(pSrcLayer, pDstLayer, strSrcComponentTag, strDstComponentTag, iPartObjID, iDstPartObjID);
}

void CGameInstance::Collision_Layer_Coin(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID, _uint iDstPartObjID)
{
	
	m_pCollision_Manager->Collision_Layer_Coin(pSrcLayer, pDstLayer, strSrcComponentTag, strDstComponentTag, iSrcPartObjID, iDstPartObjID);
}

_bool CGameInstance::Collision_Bullet(CLayer* Target, const _wstring& strTargetComponentTag, _vector vRayDior, _vector vRayPos, _bool* bShot, _float fDamage, _uint iTargetPartObjID)
{
	
	return m_pCollision_Manager->Collision_Bullet(Target, strTargetComponentTag, vRayDior, vRayPos, bShot, fDamage, iTargetPartObjID);
}

void CGameInstance::Collision_Trap(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID, _uint iDstPartObjID)
{
	m_pCollision_Manager->Collision_Trap(pSrcLayer, pDstLayer, strSrcComponentTag, strDstComponentTag, iSrcPartObjID, iDstPartObjID);
}

void CGameInstance::Collision_Explosion(CLayer* pExplosionLayer, CLayer* pAttackedLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iCount, _uint iSrcPartObjID, _uint iDstPartObjID)
{
	m_pCollision_Manager->Collision_Explosion(pExplosionLayer, pAttackedLayer, strSrcComponentTag, strDstComponentTag, iCount,iSrcPartObjID, iDstPartObjID);
}

void CGameInstance::Anti_OverLapping(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID, _uint iDstPartObjID)
{
	m_pCollision_Manager->Anti_OverLapping(pSrcLayer, pDstLayer, strSrcComponentTag, strDstComponentTag,  iSrcPartObjID, iDstPartObjID);
}

void CGameInstance::Anti_OverLapping_SameLayer(CLayer* pSrcLayer, const _wstring& strSrcComponentTag, _uint iSrcPartObjID)
{
	m_pCollision_Manager->Anti_OverLapping_SameLayer(pSrcLayer, strSrcComponentTag, iSrcPartObjID);
}

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, FXMVECTOR vColor, _float fScale, _float fRotation, const _float2& vPivot)
{
	return m_pFont_Manager->Render_Text(strFontTag, pText, vPosition, vColor, fScale, fRotation, vPivot);
}

void CGameInstance::Update_Round(_float fTimeDelta, _uint& iCurrentRound, _bool& bBuildMode, CLayer* Monster_Near, CLayer* Monster_Far, _bool& bRoundStart,_float& SkipTimer)
{
	
	return m_pRound_Manager->Update(fTimeDelta, iCurrentRound, bBuildMode, Monster_Near, Monster_Far, bRoundStart, SkipTimer);
}

void CGameInstance::CircleGauge_Interaction(CLayer* Item, CLayer* UI)
{
	m_pUI_Manager->CircleGauge_Interaction(Item, UI);
}


void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Free();

	CGameInstance::DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();

	Safe_Release(m_pGraphic_Device				   );
	Safe_Release(m_pInput_Device				   );
	Safe_Release(m_pTimer_Manager				   );
	Safe_Release(m_pLevel_Manager				   );
	Safe_Release(m_pObject_Manager				   );
	Safe_Release(m_pComponent_Manager			   );
	Safe_Release(m_pRenderer					   );
	Safe_Release(m_pPipeLine					   );
	Safe_Release(m_pLight_Manager				   );
	Safe_Release(m_pPicking_Manager				   );
	Safe_Release(m_pCollision_Manager			   );
	Safe_Release(m_pFont_Manager				   );
	Safe_Release(m_pRound_Manager				   );
	Safe_Release(m_pUI_Manager					   );
}
