#pragma once

#include "Client_Defines.h"
#include "Effect.h"


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)


class CMissile_Flame final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		_bool*   bDraw = {nullptr};
		_uint	iTexNum{};
		_vector* vecPos = { nullptr };
		LEVELID eLevel{};
		const _float4x4* matWorld = { nullptr };
	}MISSILE_FLAME_DESC;
private:
	CMissile_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissile_Flame(const CMissile_Flame& Prototype);
	virtual ~CMissile_Flame() = default;

public:
	/* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_vector* m_vecPos = { nullptr };
	_bool* m_bDraw = { nullptr };
	const _float4x4* m_matWorld = { nullptr };


private:
	LEVELID m_eLevel{};
	_uint						m_iTextureNum{};

	_float2						m_fFrame{};
	_float						m_fMoveUV{};

	_float2						m_fMaxFrame{};
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMissile_Flame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END