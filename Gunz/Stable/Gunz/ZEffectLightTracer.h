#ifndef ZEFFECTLIGHTTRACER_H
#define ZEFFECTLIGHTTRACER_H

#include "ZEffectBillboard.h"

#include "mempool.h"

// �����ϴ°͵��� �����..

class ZEffectLightTracer : public ZEffectBillboard , public CMemPoolSm<ZEffectLightTracer>
{
protected:
	unsigned long int m_nStartTime;

	D3DXVECTOR3	m_LightTracerDir;
	D3DXVECTOR3	m_Start, m_End;
	float		m_fLength;

public:
	ZEffectLightTracer(ZEffectBillboardSource* pEffectBillboardSource, D3DXVECTOR3& Start, D3DXVECTOR3& End);
	virtual ~ZEffectLightTracer(void);

	virtual bool Draw(unsigned long int nTime);
};

#endif