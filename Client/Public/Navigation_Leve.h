#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "VIBuffer_Terrain.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "Environment.h"
#include "Camera_Free.h"
#include "CollisionBox.h"
#include "Terrain.h"

BEGIN(Client)

class CNavigation_Leve final: public CLevel
{
public:
	enum IMGUI_TYPE {
		IMGUI_OBJECT_NONANIM,IMGUI_OBJECT_ANIM,IMGUI_BUILD,IMGUI_MAPTOOL,NAVIGATION,IMGUI_END
	};
	enum NAVIGATION_MODE {
		CREATE_NAVIPOINT,SELECT_NAVIPOINT,NAVIMODE_END
	};
private:
	CNavigation_Leve(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual ~CNavigation_Leve() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Lights();


private:
	void Load_Map();


private:
	void Add_Point(_float fTimeDelta,_float3 fPointPos);
	HRESULT Save_Navigation(_float fTimeDelta);

private:
	CVIBuffer_Terrain* pVIBuffer_Terrain = {nullptr}; // 터레인 피킹
	CTerrain* m_pTerrain;
	_bool bAble_Select = true;
	_float3 m_fPickingPos{};
	LEVELID m_eLevel = LEVEL_YARD;
private:
	XMFLOAT3		vPoints[3];
	float			fPoints[3];
	int			m_iCellType = 0;
	vector<CCollisionBox*> m_vecCollision{};  // 콜리전 박스 담아두기

	NAVIGATION_MODE		eNaviMode = CREATE_NAVIPOINT;


	vector<class CCell*>	m_Cells;



	_uint		m_iSelected_index = -1;  // 선택한 인덱스 번호
	_uint		m_iIndex{}; // 전체 인덱스 
	_int		m_iCount{};		// 이건 배열에 들어가는 인덱스 번호


	_bool m_bAfter_AddPoints = false;
	_bool m_bDelete = false;
	_bool m_bClick = false;
	_bool Save = false;
private:
	CTexture* m_pLoad = nullptr;
	CTexture* m_pSave = nullptr;
	ID3D11ShaderResourceView* my_Savetexture = nullptr;
	ID3D11ShaderResourceView* my_Loadtexture = nullptr;

public:
	static CNavigation_Leve* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END