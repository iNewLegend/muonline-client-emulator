#include "D3D9HardwareVertexBuffer.h"
#include "D3D9RenderSystem.h"

CD3D9HardwareVertexBuffer::CD3D9HardwareVertexBuffer()
{
	mlpD3DBuffer=NULL;
}
//---------------------------------------------------------------------

CD3D9HardwareVertexBuffer::~CD3D9HardwareVertexBuffer()
{
	D3D9S_REL(mlpD3DBuffer);
}
//---------------------------------------------------------------------

bool CD3D9HardwareVertexBuffer::create(size_t vertexSize, size_t numVertices, CHardwareBuffer::Usage usage, bool useSystemMemory)
{
	CHardwareVertexBuffer::create(vertexSize, numVertices, usage, useSystemMemory);
	// Create the vertex buffer
//#if D3D_MANAGE_BUFFERS
	mD3DPool = useSystemMemory? D3DPOOL_SYSTEMMEM : 
		// If not system mem, use managed pool UNLESS buffer is discardable
		// if discardable, keeping the software backing is expensive
		(usage & CHardwareBuffer::HBU_DISCARDABLE)? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
//#else
//	mD3DPool = useSystemMemory? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;
//#endif
	IDirect3DDevice9* pD3D9Device = GetD3D9RenderSystem().GetD3D9Device();
	// ----
	D3D9HR( pD3D9Device->CreateVertexBuffer(
		static_cast<UINT>(m_uBufferSize), 
		UsageForD3D9(usage), 
		0, // No FVF here, thankyou
		mD3DPool,
		&mlpD3DBuffer,
		NULL) );
	return mlpD3DBuffer!=NULL;
}
//---------------------------------------------------------------------

void* CD3D9HardwareVertexBuffer::lockImpl(size_t offset, 
										 size_t length, LockOptions options)
{
	void* pBuf;
	D3D9HR( mlpD3DBuffer->Lock(
		static_cast<UINT>(offset), 
		static_cast<UINT>(length), 
		&pBuf,
		LockOptionsForD3D9(options, m_Usage)) );
	return pBuf;
}
//---------------------------------------------------------------------
void CD3D9HardwareVertexBuffer::unlockImpl(void)
{
	D3D9HR( mlpD3DBuffer->Unlock());
}
//---------------------------------------------------------------------
void CD3D9HardwareVertexBuffer::readData(size_t offset, size_t length, 
										void* pDest)
{
	// There is no functional interface in D3D, just do via manual 
	// lock, copy & unlock
	void* pSrc = this->lock(offset, length, CHardwareBuffer::HBL_READ_ONLY);
	memcpy(pDest, pSrc, length);
	this->unlock();

}
//---------------------------------------------------------------------
void CD3D9HardwareVertexBuffer::writeData(size_t offset, size_t length, 
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
void CD3D9HardwareVertexBuffer::releaseBuffer(void)
{
	D3D9S_REL(mlpD3DBuffer);
}

//---------------------------------------------------------------------
bool CD3D9HardwareVertexBuffer::releaseIfDefaultPool(void)
{
	if (mD3DPool == D3DPOOL_DEFAULT)
	{
		releaseBuffer();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------
bool CD3D9HardwareVertexBuffer::recreateIfDefaultPool(void)
{
	LPDIRECT3DDEVICE9 pDev = GetD3D9RenderSystem().GetD3D9Device();
	if (mD3DPool == D3DPOOL_DEFAULT)
	{
		return D3D9HR( pDev->CreateVertexBuffer(
			static_cast<UINT>(m_uBufferSize), 
			UsageForD3D9(m_Usage), 
			0, // No FVF here, thankyou
			mD3DPool,
			&mlpD3DBuffer,
			NULL) );
	}
	return false;
}