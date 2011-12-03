#ifndef ASCOMMANDPARAMETER_H
#define ASCOMMANDPARAMETER_H

#include "MUID.h"
#include "MTypes.h"
#include "mempool.h"

class MCommandParamCondition;
class MCommandParamConditionMinMax;

/// Ŀ�ǵ� �Ķ���� Ÿ��
enum MCommandParameterType{
	MPT_INT		= 0,
	MPT_UINT	= 1,
	MPT_FLOAT	= 2,
	MPT_BOOL	= 3,
	MPT_STR		= 4,
	MPT_VECTOR	= 5,
	MPT_POS		= 6,
	MPT_DIR		= 7,
	MPT_COLOR	= 8,
	MPT_UID		= 9,
	MPT_BLOB	= 10,

	MPT_CHAR	= 11,
	MPT_UCHAR	= 12,
	MPT_SHORT	= 13,
	MPT_USHORT	= 14,
	MPT_INT64	= 15,
	MPT_UINT64	= 16,
	// ���ڴ� ������ ���̴�. (Ȯ���� �����ϵ� ������ �Ұ�, ���� ���÷��� ����)

	MPT_SVECTOR	= 17,
	MPT_END		= 18,		///< �Ķ���� Ÿ�� �� ����
};

#define MAX_BLOB_SIZE		(0x100000)			// 1�ް�����Ʈ




/// Command Parameter Description
class MCommandParameterDesc{
protected:
	MCommandParameterType				m_nType;
	char								m_szDescription[64];
	vector<MCommandParamCondition*>		m_Conditions;
	void								InitializeConditions();
public:
	MCommandParameterDesc(MCommandParameterType nType, char* szDescription);
	virtual ~MCommandParameterDesc(void);

	MCommandParameterType GetType(void){ return m_nType; }
	const char* GetDescription(void){ return m_szDescription; }

	void AddCondition(MCommandParamCondition* pCondition);
	bool HasConditions() { return (!m_Conditions.empty()); }
	int GetConditionCount() { return (int)m_Conditions.size(); }
	MCommandParamCondition* GetCondition(int n) { return m_Conditions[n]; }

};


/// Command Parameter Abstract Class
class MCommandParameter{
protected:
	MCommandParameterType	m_nType;
public:
	MCommandParameter(MCommandParameterType nType){ m_nType = nType; }
	virtual ~MCommandParameter(void){}

	MCommandParameterType GetType(void){ return m_nType; }

	/// ���� �Ķ��Ƽ Ÿ������ ����
	virtual MCommandParameter* Clone(void) = 0;
	/// �� ����
	virtual void GetValue(void* p) = 0;
	/// �޸� ������ ����
	virtual int GetData(char* pData, int nSize) = 0;
	/// �޸� ������ ����
	virtual int SetData(char* pData) = 0;
	/// ù ������ ���
	virtual void *GetPointer(void) =0; 

	/// Ÿ�� �̸� ���
	virtual const char* GetClassName(void) = 0;
	/// ��Ʈ������ �� ���
	virtual void GetString(char* szValue) = 0;
	/// ������ ���
	virtual int GetSize() = 0;
};

/// ���� �Ķ����
class MCommandParameterInt : public MCommandParameter, public CMemPool<MCommandParameterInt> {
public:
	int		m_Value;
public:
	MCommandParameterInt(void);
	MCommandParameterInt(int Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "Int"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%d", m_Value); }
	virtual int GetSize() { return sizeof(int); }
};

/// ���
class MCommandParameterUInt : public MCommandParameter, public CMemPool<MCommandParameterUInt> {
public:
	unsigned int		m_Value;
public:
	MCommandParameterUInt(void);
	MCommandParameterUInt(unsigned int Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "UInt"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%u", m_Value); }
	virtual int GetSize() { return sizeof(unsigned int); }
};

/// �Ҽ� �Ķ����
class MCommandParameterFloat : public MCommandParameter, public CMemPool<MCommandParameterFloat> {
public:
	float	m_Value;
public:
	MCommandParameterFloat(void);
	MCommandParameterFloat(float Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "Float"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%f", m_Value); }
	virtual int GetSize() { return sizeof(float); }
};

/// ��Ʈ�� �Ķ����(65533������ ����)
class MCommandParameterString : public MCommandParameter{
public:
	char*	m_Value;
public:
	MCommandParameterString(void);
	MCommandParameterString(const char* Value);
	virtual ~MCommandParameterString(void);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "String"; }
	virtual void GetString(char* szValue)
	{ 
		// ���Լ��� �ݵ�� szValue������ ���̰� m_Value�� ���̺��� ���� ��.
		// �߿��� �κп� ���ɽô� ����˻簡 ���� �̷������ ��. - by �߱���.
		if( 0 != szValue )
		{
			if( 0 != m_Value )
				strcpy(szValue, m_Value); 
			else 
				strcpy(szValue, "\0" );
		}
	}
	virtual int GetSize();
};

/// 3D ���� �Ķ����
class MCommandParameterVector : public MCommandParameter {
public:
	float	m_fX;
	float	m_fY;
	float	m_fZ;
public:
	MCommandParameterVector(void);
	MCommandParameterVector(float x ,float y, float z);
	virtual ~MCommandParameterVector(void);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_fX; }
	virtual const char* GetClassName(void){ return "Vector"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%.2f,%.2f,%.2f", m_fX, m_fY, m_fZ); }
	virtual int GetSize() { return (sizeof(float)*3); }
};

/// 3D ������ �Ķ����
class MCommandParameterPos : public MCommandParameterVector, public CMemPool<MCommandParameterPos> {
public:
	MCommandParameterPos(void) : MCommandParameterVector() { m_nType=MPT_POS; }
	MCommandParameterPos(float x, float y, float z) : MCommandParameterVector(x, y, z){ m_nType=MPT_POS; }
	virtual ~MCommandParameterPos() { }

	virtual MCommandParameter* Clone(void){ return new MCommandParameterPos(m_fX, m_fY, m_fZ); }
	virtual const char* GetClassName(void){ return "Pos"; }
};

/// 3D �𷺼� �Ķ����
class MCommandParameterDir : public MCommandParameterVector, public CMemPool<MCommandParameterDir> {
public:
	MCommandParameterDir(void) : MCommandParameterVector() { m_nType=MPT_DIR; }
	MCommandParameterDir(float x, float y, float z) : MCommandParameterVector(x, y, z){ m_nType=MPT_DIR; }
	virtual ~MCommandParameterDir() { }

	virtual MCommandParameter* Clone(void){ return new MCommandParameterDir(m_fX, m_fY, m_fZ); }
	virtual const char* GetClassName(void){ return "Dir"; }
};

/// RGB �÷� �Ķ����(���߿� Alpha�� �߰��� ����)
class MCommandParameterColor : public MCommandParameterVector, public CMemPool<MCommandParameterColor> {
public:
	MCommandParameterColor(void) : MCommandParameterVector() { m_nType=MPT_COLOR; }
	MCommandParameterColor(float r, float g, float b) : MCommandParameterVector(r, g, b){ m_nType=MPT_COLOR; }
	virtual ~MCommandParameterColor() { }

	virtual MCommandParameter* Clone(void){ return new MCommandParameterColor(m_fX, m_fY, m_fZ); }
	virtual const char* GetClassName(void){ return "Color"; }
};

/// Bool �Ķ����
class MCommandParameterBool : public MCommandParameter, public CMemPool<MCommandParameterBool> {
	bool	m_Value;
public:
	MCommandParameterBool(void) : MCommandParameter(MPT_BOOL) { }
	MCommandParameterBool(bool bValue) : MCommandParameter(MPT_BOOL) {
		m_Value = bValue;
	}

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer(void); 
	virtual const char* GetClassName(void){ return "Bool"; }
	virtual void GetString(char* szValue){ if(m_Value==true) strcpy(szValue, "true"); else strcpy(szValue, "false"); }
	virtual int GetSize() { return sizeof(bool); }
};

/// MUID �Ķ����
class MCommandParameterUID : public MCommandParameter, public CMemPool<MCommandParameterUID> {
public:
	MUID	m_Value;
public:
	MCommandParameterUID(void);
	MCommandParameterUID(const MUID& uid);
	virtual ~MCommandParameterUID(void);

	virtual MCommandParameterUID* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "UID"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%u:%u", m_Value.High, m_Value.Low); }
	virtual int GetSize() { return sizeof(MUID); }
};

class MCommandParameterBlob : public MCommandParameter{
public:
	void*	m_Value;
	unsigned int	m_nSize;
public:
	MCommandParameterBlob(void);
	MCommandParameterBlob(const void* Value, int nSize);
	virtual ~MCommandParameterBlob(void);

	virtual MCommandParameterBlob* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return m_Value; }
	virtual const char* GetClassName(void){ return "Blob"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%02X%02X..", *((unsigned char*)(m_Value)), *((unsigned char*)(m_Value)+1)); }
	virtual int GetSize();
};


/// char�� �Ķ����
class MCommandParameterChar : public MCommandParameter, public CMemPool<MCommandParameterChar>
{
public:
	char	m_Value;
public:
	MCommandParameterChar(void);
	MCommandParameterChar(char Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "Char"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%d", m_Value); }
	virtual int GetSize() { return sizeof(char); }
};


/// unsigned char�� �Ķ����
class MCommandParameterUChar : public MCommandParameter, public CMemPool<MCommandParameterUChar>
{
public:
	unsigned char	m_Value;
public:
	MCommandParameterUChar(void);
	MCommandParameterUChar(unsigned char Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "UChar"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%u", m_Value); }
	virtual int GetSize() { return sizeof(unsigned char); }
};


/// short�� �Ķ����
class MCommandParameterShort : public MCommandParameter, public CMemPool<MCommandParameterShort>
{
public:
	short	m_Value;
public:
	MCommandParameterShort(void);
	MCommandParameterShort(short Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "Short"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%d", m_Value); }
	virtual int GetSize() { return sizeof(short); }
};

/// unsigned short�� �Ķ����
class MCommandParameterUShort : public MCommandParameter, public CMemPool<MCommandParameterUShort>
{
public:
	unsigned short	m_Value;
public:
	MCommandParameterUShort(void);
	MCommandParameterUShort(unsigned short Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "UShort"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%u", m_Value); }
	virtual int GetSize() { return sizeof(unsigned short); }
};


/// int64�� �Ķ����
class MCommandParameterInt64 : public MCommandParameter, public CMemPool<MCommandParameterInt64>
{
public:
	int64	m_Value;
public:
	MCommandParameterInt64(void);
	MCommandParameterInt64(int64 Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "Int64"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%lld", m_Value); }
	virtual int GetSize() { return sizeof(int64); }
};

/// unsigned int64�� �Ķ����
class MCommandParameterUInt64 : public MCommandParameter, public CMemPool<MCommandParameterUInt64>
{
public:
	uint64	m_Value;
public:
	MCommandParameterUInt64(void);
	MCommandParameterUInt64(uint64 Value);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(void){ return "UInt64"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%llu", m_Value); }
	virtual int GetSize() { return sizeof(uint64); }
};


/// short�� 3D ���� �Ķ����
class MCommandParameterShortVector : public MCommandParameter, public CMemPool<MCommandParameterShortVector> {
public:
	short	m_nX;
	short	m_nY;
	short	m_nZ;
public:
	MCommandParameterShortVector(void);
	MCommandParameterShortVector(short x ,short y, short z);
	MCommandParameterShortVector(float x ,float y, float z);	///< ���ο��� short�� ��ȯ���ش�.
	virtual ~MCommandParameterShortVector(void);

	virtual MCommandParameter* Clone(void);
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_nX; }
	virtual const char* GetClassName(void){ return "ShortVector"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%d,%d,%d", m_nX, m_nY, m_nZ); }
	virtual int GetSize() { return (sizeof(short)*3); }
};


/// Command Parameter Condition Abstract Class
class MCommandParamCondition
{
public:
	MCommandParamCondition(void) {}
	virtual ~MCommandParamCondition(void) {}
	virtual bool Check(MCommandParameter* pCP) = 0;
};

class MCommandParamConditionMinMax : public MCommandParamCondition
{
private:
	int m_nMin;
	int m_nMax;
public:
	MCommandParamConditionMinMax(int nMin, int nMax) : m_nMin(nMin), m_nMax(nMax) {}
	virtual ~MCommandParamConditionMinMax(void) {}
	virtual bool Check(MCommandParameter* pCP);
};



// Short Name
typedef MCommandParameterBlob			MCmdParamBlob;
typedef	MCommandParameterUID			MCmdParamUID;
typedef MCommandParameter				MCmdParam;
typedef MCommandParameterDesc			MCmdParamDesc;
typedef MCommandParameterInt			MCmdParamInt;
typedef MCommandParameterUInt			MCmdParamUInt;
typedef MCommandParameterFloat			MCmdParamFloat;
typedef MCommandParameterString			MCmdParamStr;
typedef MCommandParameterVector			MCmdParamVector;
typedef MCommandParameterPos			MCmdParamPos;
typedef MCommandParameterDir			MCmdParamDir;
typedef MCommandParameterColor			MCmdParamColor;
typedef MCommandParameterBool			MCmdParamBool;
typedef MCommandParameterChar			MCmdParamChar;
typedef MCommandParameterUChar			MCmdParamUChar;
typedef MCommandParameterShort			MCmdParamShort;
typedef MCommandParameterUShort			MCmdParamUShort;
typedef MCommandParameterInt64			MCmdParamInt64;
typedef MCommandParameterUInt64			MCmdParamUInt64;
typedef MCommandParameterShortVector	MCmdParamShortVector;





#endif
