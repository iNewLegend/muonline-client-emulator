/*
	MHyperText.h
	Programming by Joongpil Cho

	���� ���� ������ ����ϰ� ����.	�Ϲ����� �ؽ�Ʈ�� �����ϳ� [[...]]���� ��ɾ ���ؼ� �ؽ�Ʈ�� ������ 
	��Ÿ���ų� �׸��� ������ �� �ִ�.
*/
#include <crtdbg.h>
#include <stdio.h>
#include "CMList.h"

#ifndef __HYPERTEXT_HEADER__
#define __HYPERTEXT_HEADER__

typedef enum {
	MHTE_PLAINTEXT = 0,	//TAG���� �Ϲ� �ؽ�Ʈ. (�±� �ƴ�)
	MHTE_STD,			//MAIET Hyper Text���� ǥ���ϴ� �±�
	MHTE_IMAGE,			//�̹��� �±�
	MHTE_STYLE,			//��Ÿ�� �±�
	MHTE_LINK,			//������ ��ũ ����
	MHTE_LINKEND,		//������ ��ũ�� ��
	MHTE_BR,			//���� �ٷ�...
	MHTE_DEFAULT,		//�ʱ⼳�����
} MHT_ELEMENT;

typedef enum {
	MHTA_TEXT = 0,		//PLAINTEXT�� ���� ����, char*�� ��� �ִ�.
	MHTA_BACKGROUND,	//STD�� ���� ���� �÷� �� Ȥ�� �̹��� ������ �ü� �ִ�.
	MHTA_COLOR,			//COLOR��, #���� �����ϴ� 16���� 6�ڸ�, �� 2�ڸ��� �ϳ��� ���������� ǥ���Ѵ�. (#RGB)
	MHTA_SIZE,			//SIZE��, ������()
	MHTA_ALIGN,			//ALIGN��, ������()
	MHTA_TYPE,			//TYPE��, ������()
	MHTA_SRC,			//SRC��, ���ڿ�
	MHTA_BOLD,			//BOLD��, ������()
	MHTA_HIGHLIGHT,
	MHTA_HREF,			//��ũ ���۷���, ���ڿ�
	MHTA_XMARGIN,		//�׸��� X�� ����
	MHTA_YMARGIN,		//�׸��� Y�� ����
} MHT_ARGUMENT;

// ������ Text Element�� ���� ������ġ���� ��
class MHyperTextArg {
public:
	MHT_ARGUMENT		uId;			// ������Ʈ �ƱԸ�Ʈ
	
	MHyperTextArg(MHT_ARGUMENT id){
		uId = id;
	}
};

template<MHT_ARGUMENT Arg>
class MHTA_IntegerArg : public MHyperTextArg {
public:
	int					nVal;

	MHTA_IntegerArg(int value) : MHyperTextArg(Arg){
		nVal = value;
	}
};

template<MHT_ARGUMENT Arg>
class MHTA_StringArg : public MHyperTextArg {
public:
	char*				val;

	MHTA_StringArg(char *szText):MHyperTextArg(Arg){
		val = strdup(szText);
	}
	~MHTA_StringArg(){
		if(val) free(val);
	}
};

template<MHT_ARGUMENT Arg>
class MHTA_ColorArg : public MHyperTextArg {
public:
	MCOLOR				sColor;

	MHTA_ColorArg(MCOLOR color) : MHyperTextArg(Arg){
		sColor = color;
	}
};

class MHTA_Background : public MHyperTextArg {
	MHTA_Background() : MHyperTextArg(MHTA_BACKGROUND){
		sColor = MCOLOR(0,0,0);
		szPath = NULL;
	}
public:
	char*				szPath;		//�̹��� �н�
	MCOLOR				sColor;

	MHTA_Background(MCOLOR color) : MHyperTextArg(MHTA_BACKGROUND){
		szPath = NULL;
		sColor = color;
	}

	MHTA_Background(char *path) : MHyperTextArg(MHTA_BACKGROUND){
		sColor = MCOLOR(0,0,0);
		szPath = strdup(path);
	}

	~MHTA_Background(){
		if(szPath) free(szPath);	
	}
};

typedef class MHTA_IntegerArg<MHTA_SIZE>	MHTA_Size;
typedef class MHTA_IntegerArg<MHTA_ALIGN>	MHTA_Align;
typedef class MHTA_IntegerArg<MHTA_TYPE>	MHTA_Type;
typedef class MHTA_IntegerArg<MHTA_BOLD>	MHTA_Bold;
typedef class MHTA_IntegerArg<MHTA_XMARGIN>	MHTA_XMargin;
typedef class MHTA_IntegerArg<MHTA_YMARGIN>	MHTA_YMargin;

typedef class MHTA_ColorArg<MHTA_COLOR>		MHTA_Color;
typedef class MHTA_ColorArg<MHTA_HIGHLIGHT>	MHTA_Highlight;

typedef class MHTA_StringArg<MHTA_TEXT>		MHTA_Text;
typedef class MHTA_StringArg<MHTA_SRC>		MHTA_Src;
typedef class MHTA_StringArg<MHTA_HREF>		MHTA_HRef;


class MHyperTextElement
{
public:
	MHT_ELEMENT					nType;	// ������Ʈ�� Ÿ��
	CMLinkedList<MHyperTextArg>	Args;	// ������Ʈ�� ���� ����Ʈ

	//������, �ı���
	MHyperTextElement(MHT_ELEMENT type){
		nType		= type;
	}

	virtual ~MHyperTextElement(){
		Args.DeleteAll();
	}

	void Add(MHyperTextArg* pNew){
		Args.Add(pNew);
	}
};

class MHyperText
{
private:
	char*			m_pBuffer;			// Text Buffer, MHyperText�� �޸𸮿� �ִ� ���븸�� �Ľ��Ѵ�.
	int				m_nLen;				// ������ ũ��
	int				bp;					// Buffer�� ������
	int				m_nOffset;			// m_szScan�� ����������
	bool			m_bTagReady;
	char			m_szScan[20480];	// Scan�� ���� ����Ǵ� ���ڿ� ������, yytext�� ������ ������ �ϴ� ���̴�.

	void			Gather(char b){ m_szScan[m_nOffset++] = b; }
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS

	int				Scan();
	bool			Parse();
	char			Input(){ return m_pBuffer[bp++]; }
	void			Unput(){ bp--; }
	void			GetDigit(char c);

public:
	CMLinkedList<MHyperTextElement>	Elements;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	
	MHyperText();
	virtual ~MHyperText(){ Close(); }

	/*	
		�� Ŭ������ ���� ����Ʈ�� ���� �ʴ´�.
		������ �ؽ�Ʈ�� ���ӳ��� ����Ǿ��� ������ ������ ��ӵǾ�� �ϹǷ�...

		szTextBuffer : �ؽ�Ʈ ����
	*/
	bool Open(char *szTextBuffer);
	void Close();

	bool IsValid(){ return (m_pBuffer==NULL)?false:true; }
};

inline void MHyperText::GetDigit(char c)
{
	for(;isdigit(c) ;c=Input()) Gather(c);
	Unput();
}

class MHyperTextFile
{
private:
	char*	m_pBuffer;
public:
	MHyperTextFile()
	{
		m_pBuffer = NULL;
	}

	virtual ~MHyperTextFile()
	{
		Close();
	}

	bool Open(char *szPath);

	char* GetBuffer(){ return m_pBuffer; }

	void Close()
	{
		if(m_pBuffer == NULL)
		{
			delete m_pBuffer;
			m_pBuffer = NULL;
		}
	}
};

#endif