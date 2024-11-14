#pragma once

#include "Renderer.h"
#include "PipeLine.h"
#include "Graphic_Device.h"

#include "Component_Manager.h"
#include "Picking_Manager.h"
#include "CollisionMgr.h"
#include "Round_Manager.h"
#include "UIManager.h"


/* CGameInstance : */
/* 내 Engine에 유일하게 존재하는 싱글톤클래스다. */
/* Client사용자가 엔진의 기능을 이용하고자한다면 CGameInstance를 통해서 기능을 수행할 수 있도록 하겠다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void Update(_float fTimeDelta);
	void Draw();
	void Clear(_uint iClearLevelID);
	void Reset_Object(_uint iClearLevelID);
	void RenderGroup_Clear(_uint iClearLevelID);

	_float Compute_Random_Normal();
	_float Compute_Random(_float fMin, _float fMax);

public: /* For.Graphic_Device */
	HRESULT Render_Begin(_float4 vClearColor);
	HRESULT Render_End();
	// HRESULT Set_BlendState(const CGraphic_Device::BLEND_STATE& BS);


public: /* for.Timer_Manager */
	_float Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void Update_TimeDelta(const _wstring& strTimerTag);

public: // Input_Device 
	_ubyte Get_DIKeyState(_ubyte byKeyID);            // Pressing
	_ubyte Get_DIKeyState_Pressing(_ubyte byKeyID);   // Pressing
	_ubyte Get_DIKeyState_Up(_ubyte byKeyID);
	_ubyte Get_DIKeyState_Down(_ubyte byKeyID);

	_ubyte Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_ubyte Get_DIMouseState_Down(MOUSEKEYSTATE eMouse);
	_ubyte Get_DIMouseState_Pressing(MOUSEKEYSTATE eMouse);
	_ubyte Get_DIMouseState_Up(MOUSEKEYSTATE eMouse);
	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseState);



public: /* for.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelID, class CLevel* pNewLevel);
	HRESULT Close_Level(_uint iLevelID);

public: /* For.Object_Manager*/
	class CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0, _uint iPartObjID = 0);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Add_GameObject_ToLayer_ReturnObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg);
	class CGameObject* Get_Prototype(_uint iLevelIndex, const _tchar* pLayerTag, const _wstring& strPrototypeTag);
	//class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject* Clone_Prototype(const _wstring& strPrototypeTag, void* pArg = nullptr);


public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Prototype_Component(_uint iLevelIndex, const _wstring& strPrototypeTag);

public: /* For.Renderer	*/
	HRESULT Add_RenderGameObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderGameObject);
	void	RenderList_Clear();

public:// For PipeLine
	const _float4x4* Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eState);

	const _float4* Get_CamPosition();
	void Set_TransformMatrix(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);

public: // Light 매니저
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);


public:  // 피킹 매니저
	_float3 Get_MousePos_NDC(HWND hWnd, const unsigned int g_iWinSizeX, const unsigned int	g_iWinSizeY);
	_float4 Object_NDC_Cal(_float2 fPos, _float fSizeX, _float fSizeY, const unsigned int g_iWinSizeX, const unsigned int g_iWinSizeY);
	void Get_MouseRayDirection(_float3 fPosition, XMMATRIX invProj, XMMATRIX invView, XMVECTOR* RayPos_Output, XMVECTOR* RayDir_Output);
	_float3 Picking_Terrain(XMVECTOR RayPos, XMVECTOR RayDir, const _float3* VtxPos, _uint VtxCountX, _uint VtxCountZ);
	_float3 Picking_Box_FAILED(_vector  RayPos, _vector  RayDir, const _float3* VtxPos);
	void CreateBoundingBox(const _float3& center, const _float3& size, _float3& fMinPoint, _float3& fMaxPoint);
	bool Picking_Box(const _vector& rayOrigin, const _vector& rayDirection, const _float3& fMinPoint, const _float3& fMaxPoint, float& distance, DirectX::BoundingBox box);

public: // 콜리전 매니저
	void Collision_Layer(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iPartObjID = 0, _uint iDstPartObjID = 0);
	void Collision_Layer_Coin(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	_bool Collision_Bullet(CLayer* Target, const _wstring& strTargetComponentTag, _vector vRayDior, _vector vRayPos,  _bool* bShot, _float fDamage,_uint iTargetPartObjID = 0);
	void Collision_Trap(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Collision_Explosion(CLayer* pExplosionLayer, CLayer* pAttackedLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iCount, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Anti_OverLapping(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Anti_OverLapping_SameLayer(CLayer* pSrcLayer, const _wstring& strSrcComponentTag, _uint iSrcPartObjID = 0);

public: /* For.Font_Manager */
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePaht);
	HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, FXMVECTOR vColor, _float fScale = 1.f, _float fRotation = 0.f, const _float2& vPivot = _float2(0.f, 0.f));

public: // Round Manager
	void Update_Round(_float fTimeDelta, _uint& iCurrentRound, _bool& bBuildMode, CLayer* Monster_Near, CLayer* Monster_Far,_bool& bRoundStart,_float& SkipTimer);

public: // UI_Manager
	void CircleGauge_Interaction(CLayer* Item, CLayer* UI);



private:
	class CGraphic_Device*		m_pGraphic_Device	 = { nullptr };
	class CInput_Device*		m_pInput_Device			= { nullptr };
	class CTimer_Manager*		m_pTimer_Manager		= { nullptr };
	class CLevel_Manager*		m_pLevel_Manager	 = { nullptr };
	class CObject_Manager*		m_pObject_Manager		= { nullptr };
	class CComponent_Manager*	m_pComponent_Manager	 = { nullptr };
	class CRenderer*			m_pRenderer				 = { nullptr };
	class CPipeLine*			m_pPipeLine				 = { nullptr };
	class CLight_Manager*		m_pLight_Manager				= { nullptr };
	class CPicking_Manager*		m_pPicking_Manager			= { nullptr };
	class CCollisionMgr*		m_pCollision_Manager			 = { nullptr };
	class CFont_Manager*		m_pFont_Manager			= { nullptr };
	class CRound_Manager*		m_pRound_Manager		= { nullptr };
	class CUIManager*			m_pUI_Manager			= { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;

};

END