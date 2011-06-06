#include "D3D9HardwareIndexBuffer.h"
#include "D3D9RenderSystem.h"


CD3D9HardwareIndexBuffer::CD3D9HardwareIndexBuffer()
{
	mlpD3DBuffer=NULL;
}
//---------------------------------------------------------------------

CD3D9HardwareIndexBuffer::~CD3D9HardwareIndexBuffer()
{
	D3D9S_REL(mlpD3DBuffer);
}

bool CD3D9HardwareIndexBuffer::create(CHardwareIndexBuffer::IndexType idxType, size_t numIndexes, CHardwareBuffer::Usage usage, bool useSystemMemory)
{
	CHardwareIndexBuffer::create(idxType, numIndexes, usage, useSystemMemory);
	//#if OGRE_D3D_MANAGE_BUFFERS
	mD3DPool = useSystemMemory? D3DPOOL_SYSTEMMEM : 
		// If not system mem, use managed pool UNLESS buffer is discardable
		// if discardable, keeping the software backing is expensive
		(usage & CHardwareBuffer::HBU_DISCARDABLE)? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	//#else
	//	mD3DPool = useSystemMemory? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;
	//#endif
	// Create the Index buffer
	IDirect3DDevice9* pD3D9Device = GetD3D9RenderSystem().GetD3D9Device();
	// ----
	D3D9HR( pD3D9Device->CreateIndexBuffer(
		static_cast<UINT>(m_uBufferSize),
		UsageForD3D9(m_Usage),
		(D3DFORMAT)IndexTypeForD3D9(m_IndexType),
		mD3DPool,
		&mlpD3DBuffer,
		NULL
		) );
	return mlpD3DBuffer!=NULL;
}
//---------------------------------------------------------------------

void* CD3D9HardwareIndexBuffer::lockImpl(size_t offset, 
										size_t length, LockOptions options)
{
	void* pBuf;
	D3D9HR( mlpD3DBuffer->Lock(
		static_cast<UINT>(offset), 
		static_cast<UINT>(length), 
		&pBuf,options) );
	return pBuf;
}
//---------------------------------------------------------------------
void CD3D9HardwareIndexBuffer::unlockImpl(void)
{
	D3D9HR( mlpD3DBuffer->Unlock() );
}
//---------------------------------------------------------------------
void CD3D9HardwareIndexBuffer::readData(size_t offset, size_t length, 
									   void* pDest)
{
	// There is no functional interface in D3D, just do via manual 
	// lock, copy & unlock
	void* pSrc = this->lock(offset, length, CHardwareBuffer::HBL_READ_ONLY);
	memcpy(pDest, pSrc, length);
	this->unlock();

}
//---------------------------------------------------------------------
void CD3D9HardwareIndexBuffer::writeData(size_t offset, size_t length, 
										const void* pSource,
										bool discardWholeBuffer)
{
	// There is no functional interface in D3D, just do via manual 
	// lock, copy & unlock
	void* pDst = this->lock(offset, length, 
		discardWholeBuffer ? CHardwareBuffer::HBL_DISCARD : CHardwareBuffer::HBL_NORMAL);
	memcpy(pDst, pSource, length);
	this->unlock();
}
//---------------------------------------------------------------------
void CD3D9HardwareIndexBuffer::releaseBuffer()
{
	D3D9S_REL(mlpD3DBuffer);
}
//---------------------------------------------------------------------
bool CD3D9HardwareIndexBuffer::releaseIfDefaultPool()
{
	if (mD3DPool == D3DPOOL_DEFAULT)
	{
		releaseBuffer();
		return true;
	}
	return false;

}
//---------------------------------------------------------------------
bool CD3D9HardwareIndexBuffer::recreateIfDefaultPool()
{
	LPDIRECT3DDEVICE9 pDev = GetD3D9RenderSystem().GetD3D9Device();
	if (mD3DPool == D3DPOOL_DEFAULT)
	{
		// Create the Index buffer
		return D3D9HR( pDev->CreateIndexBuffer(
			static_cast<UINT>(m_uBufferSize),
			UsageForD3D9(m_Usage),
			(D3DFORMAT)IndexTypeForD3D9(m_IndexType),
			mD3DPool,
			&mlpD3DBuffer,
			NULL) );
	}
	return false;
}