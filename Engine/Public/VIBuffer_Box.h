#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class  ENGINE_DLL CVIBuffer_Box final : public CVIBuffer
{
private:
	CVIBuffer_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Box(const CVIBuffer_Box& Prototype);
	virtual ~CVIBuffer_Box() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBuffer_Box* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;





};

END