#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "VIBuffer_Terrain.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "Environment.h"
#include "Player.h"
#include "Coin.h"

BEGIN(Client)


class CLevel_ImGui final : public CLevel
{
public:
	enum IMGUI_TYPE{ IMGUI_OBJECT_NONANIM, IMGUI_OBJECT_ANIM, IMGUI_BUILD, IMGUI_ITEM, IMGUI_END};
	enum IMGUI_MODE{ IMGUI_CREATE, IMGUI_SELECT, MODE_END};
private:
	CLevel_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_ImGui() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CPlayer* m_pPlayer;

private:
	float Position[3] = { 0,0,0 };
	float Scale[3] = { 0,0,0 };

	float CollisionBox_Scale[3] = { 1,1,1 };
	float CollisionBox_Pos[3] = { 0,0,0 };

	_float3 m_fPickingPos{};


private:
	_bool Save = false;
	_bool bAble_Select = true;

public:
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);

	
public:
	void Object_NonAnim_Update(_float fTimeDelta);
	void Object_Anim_Update(_float fTimeDelta);
	void Build_Update(_float fTimeDelta);
	void Item_Update(_float fTimeDelta);


	void Object_NonAnim();
	void Object_Anim();
	void Object_Build();
	void Object_Item();

public:
	HRESULT Picking_Create();
	
	HRESULT Environment_Add();
	HRESULT Environment_DataChange(_float fTimeDelta);
	void	Environment_Save();
	void	Environment_Load();
	HRESULT	Environment_Select();

	HRESULT Build_Add();
	HRESULT Build_DataChange(_float fTimeDelta);
	void	Build_Save();
	void	Build_Load();
	HRESULT Build_Select();

	HRESULT Item_Add();
	HRESULT Item_DataChange(_float fTimeDelta);
	void	Item_Save();
	void	Item_Load();
	HRESULT Item_Select();


private:
	vector<CEnvironment*> m_vecEnvironment;
	vector<CEnvironment*> m_vecBuild;
	vector<CCoin*> m_vecCoin;

	CGameObject* pGameObj = { nullptr };
private:
	_uint		m_iEnvironment_Count = 0; // 생성한 개수
	_uint		m_iBuild_Count = 0;			// 생성한 개수
	_uint		m_iCoin_Count = 0;			// 생성한 코인개수



	vector<_int> m_vecModelIndex; // Environment Index 저장용
	vector<_int> m_vecBuildIndex; // Build Index 저장용


	LEVELID m_eID = LEVEL_YARD;   // 이거 바꿔서 어떤 레벨을 수정할지 설정

private:
	CVIBuffer_Terrain* pVIBuffer_Terrain = { nullptr }; // 터레인 피킹

	
private: // ImGui
	_bool m_bWindowsMove = false; 
	IMGUI_TYPE	m_eImGui_Type = {}; // ImGui창 타입 선택
	_uint m_iModeSelect = 0; // Create or Select 모드 선택


public:
	static CLevel_ImGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


#pragma region 이미지버튼
private:// 이미지 버튼
	void Create_ImageButton();
	void ButtonImage_List();
private: // 이미지버튼
	CTexture* m_pLoad = nullptr;
	CTexture* m_pSave = nullptr;
	CTexture* m_pEnviron = { nullptr };
	CTexture* m_pBuild = { nullptr };
private:// 이미지 버튼
	ID3D11ShaderResourceView* my_Savetexture = nullptr;
	ID3D11ShaderResourceView* my_Loadtexture = nullptr;
	ID3D11ShaderResourceView* pButton;
private:
	_int  m_iModelIndex = 0;
		
#pragma endregion 이미지버튼


};

END