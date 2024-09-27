#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "VIBuffer_Terrain.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "Environment.h"


BEGIN(Client)


class CLevel_ImGui final : public CLevel
{
public:
	enum IMGUI_TYPE{ IMGUI_OBJECT_NONANIM, IMGUI_OBJECT_ANIM, IMGUI_MAPTOOL, IMGUI_END};

private:
	CLevel_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_ImGui() = default;

public:
	virtual HRESULT Initialize() override;

	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	float Position[3] = { 0,0,0 };
	float Scale[3] = { 0,0,0 };
	float Rotation[3] = { 0,0,0 };
	_int  m_iModelIndex = 0;
	_float3 m_fPickingPos{};
	_bool Save = false;
	_bool bAble_Select = true;
public:
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);

	
public:
	void Object_NonAnim_Update(_float fTimeDelta);
	void Object_Anim_Update(_float fTimeDelta);
	void MapTool_Update(_float fTimeDelta);
	void Object_NonAnim();
	void Object_Anim();
	void MapTool();




public:
	void Environment_List();


public:
	HRESULT Picking_Create();
	HRESULT Picking_Select();
	
	HRESULT Environment_Add();
	HRESULT Environment_DataChange(_float fTimeDelta);
	void	Environment_Save();
	void	Environment_Load();






	
private:
	_bool bCheck = false;
	IMGUI_TYPE	m_eImGui_Type = {};
	
	CMesh* pMesh = { nullptr };
	CGameObject* pGameObj = { nullptr };
	CVIBuffer_Terrain* pVIBuffer_Terrain = { nullptr };
	vector<CEnvironment*> m_vecEnvironment;
	
	_uint		m_iEnvironment_Count = 0;

public:
	static CLevel_ImGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


// 이미지버튼
private: 
	CTexture* m_pLoad = nullptr;
	CTexture* m_pSave = nullptr;

	CTexture*				 m_pChair0									= nullptr;
	CTexture*				 m_pChair1									= nullptr;
	CTexture*				 m_pChair2									= nullptr;
	CTexture*				 m_pChair3									= nullptr;
	CTexture*				 m_pChair4									= nullptr;
	CTexture*				 m_pChair5									= nullptr;
	CTexture*				 AidKit									= nullptr;
	CTexture*				 Card0										= nullptr;
	CTexture*				 Card1										= nullptr;
	CTexture*				 Desk0										= nullptr;
	CTexture*				 Desk1										= nullptr;
	CTexture*				 Desk2										= nullptr;
	CTexture*				 Desk3										= nullptr;
	CTexture*				 KeyPad									= nullptr;
	CTexture*				 Sprinkler									= nullptr;
	CTexture*				 Trim0										= nullptr;
	CTexture*				 Vent0										= nullptr;
	CTexture*				 Vent1										= nullptr;
	CTexture*				 WasteBin									= nullptr;



private:
	ID3D11ShaderResourceView* my_Savetexture = nullptr;
	ID3D11ShaderResourceView* my_Loadtexture = nullptr;
	
	ID3D11ShaderResourceView*					 SRV_m_pChair0		= nullptr;
	ID3D11ShaderResourceView*					 SRV_m_pChair1		= nullptr;
	ID3D11ShaderResourceView*					 SRV_m_pChair2		= nullptr;
	ID3D11ShaderResourceView*					 SRV_m_pChair3		= nullptr;
	ID3D11ShaderResourceView*					 SRV_m_pChair4		= nullptr;
	ID3D11ShaderResourceView*					 SRV_m_pChair5		= nullptr;
	ID3D11ShaderResourceView*					 SRV_AidKit			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Card0			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Card1			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Desk0			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Desk1			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Desk2			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Desk3			= nullptr;
	ID3D11ShaderResourceView*					 SRV_KeyPad			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Sprinkler		= nullptr;
	ID3D11ShaderResourceView*					 SRV_Trim0			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Vent0			= nullptr;
	ID3D11ShaderResourceView*					 SRV_Vent1			= nullptr;
	ID3D11ShaderResourceView*					 SRV_WasteBin		= nullptr;






private:
	void Create_ImageButton();
	void Button_Free();
private:
	_uint m_iNumMeshes;





};

END