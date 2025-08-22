#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPivot final: public CPartObject
{
public:
	typedef struct: CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = {nullptr};

	}PIVOT_DESC;
private:
	CPivot(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CPivot(const CPivot& Prototype);
	virtual ~CPivot() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;


public:
	_vector* Get_TPS_CameraPos() {
		return &m_vecTPS_CamPos;
	}


private:

	const _float4x4* m_pSocketMatrix = {nullptr};


private:
	_vector m_vecTPS_CamPos{};

	_float3 Position= {-0.439998f,-0.109998f,1.84475};


public:
	static CPivot* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END