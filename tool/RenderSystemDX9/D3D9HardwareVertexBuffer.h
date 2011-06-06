#pragma once
#include "HardwareVertexBuffer.h"
#include "D3D9RenderSystemCommon.h"

class CD3D9HardwareVertexBuffer : public CHardwareVertexBuffer 
{
protected:
	LPDIRECT3DVERTEXBUFFER9 mlpD3DBuffer;
	D3DPOOL mD3DPool;
	/** See HardwareBuffer. */
	void* lockImpl(size_t offset, size_t length, LockOptions options);
	/** See HardwareBuffer. */
	void unlockImpl(void);
public:
	CD3D9HardwareVertexBuffer();
	~CD3D9HardwareVertexBuffer();
	bool create(size_t vertexSize, size_t numVertices, CHardwareBuffer::Usage usage, bool useSystemMem);
	/** See HardwareBuffer. */
	void readData(size_t offset, size_t length, void* pDest);
	/** See HardwareBuffer. */
	void writeData(size_t offset, size_t length, const void* pSource,
		bool discardWholeBuffer = false);
	//
	void releaseBuffer(void);
	/// For dealing with lost devices - release the resource if in the default pool
	bool releaseIfDefaultPool(void);
	/// For dealing with lost devices - recreate the resource if in the default pool
	bool recreateIfDefaultPool(void);

	/// Get D3D9-specific vertex buffer
	LPDIRECT3DVERTEXBUFFER9 getD3D9VertexBuffer(void) const { return mlpD3DBuffer; }
};