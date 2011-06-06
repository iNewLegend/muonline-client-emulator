#pragma once
#define NULL    0

class CHardwareBufferMgr;
class CHardwareBuffer 
{
public:
	enum Usage 
	{
		HBU_STATIC = 1,
		HBU_DYNAMIC = 2,
		HBU_WRITE_ONLY = 4,
		HBU_DISCARDABLE = 8,
		HBU_STATIC_WRITE_ONLY = 5, 
		HBU_DYNAMIC_WRITE_ONLY = 6,
		HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE = 14
	};
	enum LockOptions
	{
		HBL_NORMAL,
		HBL_DISCARD,
		HBL_READ_ONLY,
		HBL_NO_OVERWRITE
	};
protected:
	size_t	m_uBufferSize;
	Usage	m_Usage;
	bool	m_bIsLocked;
	size_t	m_uLockStart;
	size_t	m_uLockSize;
	bool	m_bSystemMemory;

	CHardwareBufferMgr* m_pHardwareBufferMgr;

	virtual void* lockImpl(size_t offset, size_t length, LockOptions options = HBL_NORMAL) = 0;
	virtual void unlockImpl(void) = 0;

public:
	CHardwareBuffer():m_bIsLocked(false),
		m_pHardwareBufferMgr(NULL)
	{
	}

	virtual ~CHardwareBuffer() {}

	void setHardwareBufferMgr(CHardwareBufferMgr* pHardwareBufferMgr)
	{
		m_pHardwareBufferMgr=pHardwareBufferMgr;
	}

	virtual void* lock(size_t offset, size_t length, LockOptions options)
	{
		//assert(!isLocked() && "Cannot lock this buffer, it is already locked!");
		void* ret;

		// Lock the real buffer if there is no shadow buffer 
		ret = lockImpl(offset, length, options);
		m_bIsLocked = true;

		m_uLockStart = offset;
		m_uLockSize = length;
		return ret;
	}

	void* lock(LockOptions options)
	{
		return this->lock(0, m_uBufferSize, options);
	}

	virtual void unlock(void)
	{
		//assert(isLocked() && "Cannot unlock this buffer, it is not locked!");

		// Otherwise, unlock the real one
		unlockImpl();
		m_bIsLocked = false;
	}

	virtual void readData(size_t offset, size_t length, void* pDest) = 0;
	virtual void writeData(size_t offset, size_t length, const void* pSource,
		bool discardWholeBuffer = false) = 0;
	virtual void copyData(CHardwareBuffer& srcBuffer, size_t srcOffset, 
		size_t dstOffset, size_t length, bool discardWholeBuffer = false)
	{
		const void *srcData = srcBuffer.lock(
			srcOffset, length, HBL_READ_ONLY);
		this->writeData(dstOffset, length, srcData, discardWholeBuffer);
		srcBuffer.unlock();
	}
	/// Returns the size of this buffer in bytes
	size_t getSizeInBytes(void) const { return m_uBufferSize; }
	/// Returns the Usage flags with which this buffer was created
	Usage getUsage(void) const { return m_Usage; }
	/// Returns whether this buffer is held in system memory
	bool isSystemMemory(void) const { return m_bSystemMemory; }
	/// Returns whether or not this buffer is currently locked.
	bool isLocked(void) const { 
		return m_bIsLocked; 
	}
};