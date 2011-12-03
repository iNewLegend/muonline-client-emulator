#include "stdafx.h"
/*
	MHyperText.cpp
	--------------
	
	MHyperText�� ���� �ܼ��� ������ ������ �ֱ� ������ ����� �� �ļ�/��ĳ���� ���¸� ������ ������ �ʴ�.

	Programming by Joongpil Cho
	All copyright (c) 1999, MAIET entertainment Inc.
*/
#include "MWidget.h"
#include "MScrollBar.h"
#include "MHyperText.h"

#include <stdio.h>
#include <ctype.h>

#define TINVALID		-1		// ����
#define TINTEGER		0		// ������
#define TCOLORVAL		1		// �÷���, #�ڿ� 6���� 16������ ���´�.
#define TSTRING			2		// ���ڿ� "" �ȿ� ���ִ� ��Ʈ��

#define TTAGSTART		5		// <, �±� ���� ǥ��, ��ĳ���� ������ �� �� ��ū�� �������� m_nOffset���� 0�� �ƴ϶��
								// ����� �ؽ�Ʈ�� ����Ʈ�� �������� ���ϴ� ���̴�.
#define TTAGEND			6		// >, �±� �� ǥ��
#define TEQUAL			7		// EQUAL

// TT�� �����ϴ� ���� TAG�� ���� ��ū ����̴�.
#define TTSTD			50		// STD TAG, ������ MAIET STD�Ծ��� ��Ű�� ������ ��Ÿ����.
#define TTIMAGE			51		// IMAGE TAG, �ܺ� �̹��� ���ϰ� ��ũ�Ǿ� ������ ��Ÿ����.
#define TTSTYLE			52		// STYLE TAG, �̹����� �ؽ�Ʈ ������ ��Ÿ���� ��Ÿ����.
#define TTLINK			53		// ��ũ�� ���� TAG
#define TTBR			54		// �����ٷ�...
#define TTDEFAULT		55		// �ʱ� ��������
#define TTLINKEND		56		// LINK�� ���� �˸��� �±�

// TR�� �����ϴ� ���� TAG�� �������� ���� ���̴�. ARGUMENTs
#define TRBACKGROUND	100		// Background ���� (���� : STD)
#define TRCOLOR			101		// Color ���� (���� : STD, STYLE)
#define TRSIZE			102		// ũ�� (���� : STYLE)
#define TRALIGN			103		// ���� (���� : STYLE)
#define TRTYPE			104		// ��ũ�� ����
#define TRSRC			105		// �ҽ� ������ ��ġ
#define TRBOLD			106		// ��Ʈ�� ���� ��. (BOLD?)
#define TRHIGHLIGHT		107		// ���̶���Ʈ �÷� (���� : STD, STYLE)
#define TRHREF			108
#define TRXMARGIN		109
#define TRYMARGIN		110
//#define TRFACE			106		// ��Ʈ �̸�, ���� �������� ����.

// ������ͼ� ��Ÿ�� ���Ǹ� ���� �������
#define TRLEFT			150
#define TRCENTER		151
#define TRRIGHT			152

#define TROPEN			153		// �ٸ� STD������ ����.
#define TRWWW			154		// ������� ���� ��
#define TREXEC			155		// ������ ���� ��
#define TRYES			156
#define TRNO			157
#define TRTRUE			158
#define TRFALSE			159

#define TENDDOC			254		// EOS(END-OF-STRING) �Ǵ� EOF(END-OF-FILE) ���϶� �̰����� ����ȴ�.
								// �� ��ū ���� m_nOffset���� 0�� �ƴҶ� ����� �ؽ�Ʈ�� �����Ѵ�.

#define MAKERGB(r,g,b)			((DWORD)(((BYTE)(b)|((WORD) (g) << 8))|(((DWORD)(BYTE)(r)) << 16)))

static struct _reserved_word {
	int			nToken;
	char*		szText;
} Reserved[] = {
		/* TAG */
	{ TTSTD,		"std"		},
	{ TTIMAGE,		"image"		},
	{ TTSTYLE,		"style"		},
	{ TTLINK,		"link"		},
	{ TTLINKEND,	"linkend"	},
	{ TTBR,			"br"		},
	{ TTDEFAULT,	"default"	},

		/* TAG variable */
	{ TRBACKGROUND,	"background"},			// BACKGROUND, STD�� style
	{ TRCOLOR,		"color"		},			//
	{ TRSIZE,		"size"		},			//
	{ TRALIGN,		"align"		},			//
	{ TRTYPE,		"type"		},			//
	{ TRSRC,		"src"		},			// Source Data
	{ TRBOLD,		"bold"		},			//
	{ TRHIGHLIGHT,	"highlight"	},			//
	{ TRHREF,		"href"		},			// Reference
	{ TRXMARGIN,	"xmargin"	},			//
	{ TRYMARGIN,	"ymargin"	},			//

		/* Misc. */
	{ TINVALID,		NULL,		},
};

static struct _reserved_word_constant {
	int			nToken;
	char*		szText;
} Constant[] = {
		/* TAG variable constants */
	{ TRLEFT,		"left"		},			// left align.
	{ TRCENTER,		"center"	},			// center align.
	{ TRRIGHT,		"right"		},			// right align.

	{ TROPEN,		"open"		},			// open another STD file
	{ TRWWW,		"www"		},			// web browsing option
	{ TREXEC,		"exec"		},			// execution

	{ TRYES,		"yes"		},
	{ TRNO,			"no"		},

	{ TRTRUE,		"true"		},
	{ TRFALSE,		"false"		},

		/* Misc. */
	{ TINVALID,		NULL		},
};


// 32��Ʈ�� �÷� ���̴�.
static DWORD		dwScanColor = 0;	// �̷� ���� Ŭ���� �ۼ����� ���� ���� �ƴ�����...
										// ���ʿ��ϰ� Ŭ���� ����� �ϳ� ���� �ִ� �͵� ��ȿ�� ���̶� �Ұ����ϰ� �� ���� �ϰ� �Ǿ���.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MHyperText

static char *fold( char *s )
{
	int i;
	for(i=0; s[i]!='\0'; i++)
		if(isupper(s[i])) s[i] = s[i] + ('a'-'A');
    
	return(s);
}


//#define _HT_LOG_ENABLE_

#ifdef _HT_LOG_ENABLE_
//TODO:Delete...
#include "MDebug.h"
#endif


int	MHyperText::Scan()
{
	int ret = TINVALID, i;	//
	char c;					//Temporary Data
	bool bFirstSpace = false;

	m_nOffset = 0;

	if(m_bTagReady == false)
	{
read_plain_text_loop:
		c = Input();

		//�Ϲ� �ؽ�Ʈ�� �����ϰ� ������ �о� ���δ�.
		if((c=='\t'||c==' '||c=='\n') && bFirstSpace == true)
		{
			//���� ��ġ��
			for(c = Input(); (c=='\t'||c==' '||c=='\n'); );
			Unput();
			Gather(' ');
			goto read_plain_text_loop;
		}
		else if(c=='<')
		{
			//�±װ� ���۵Ǿ���.
			m_bTagReady = true;		//�±� ��ŸƮ
			Gather('\0');			//�ؽ�Ʈ���� ����
			return TTAGSTART;
		}
		else if(c==EOF || c=='\0')	//���� m_bTagReady�� true�� ���¿��� �̰��� �޾Ҵٸ� �װ��� ������.
		{	//��
			Gather('\0');
			return TENDDOC;
		}
		else
		{
			if(bFirstSpace == false){
				bFirstSpace = true;
			}
			Gather(c);
			goto read_plain_text_loop;
		}
	}
	else 
	{
		//�±װ� ���۵Ǿ����Ƿ� �±׿� ���õ� ��ĳ���� �Ѵ�.
		//�±׿� ���õ� ���̹Ƿ� �˾� ���� ���ϴ� ���� ������ ������ ������.
read_tag_loop:
		c = Input();

		if(c=='\t' || c==' ' || c=='\n'){		//���� �����ϱ�
			goto read_tag_loop;
		}
		else if(c=='>'){
			m_nOffset = 0; Gather('\0');
			m_bTagReady = false;
			return TTAGEND;
		}
		else if(isdigit(c)){					//���ڰ� ���Դ�.
			m_nOffset = 0;
			GetDigit(c);
			Gather('\0');
			return TINTEGER;
		}
		else if(c==EOF || c=='\0'){				//�±׵� ������ �ʾҴµ� �̷� ���� ����ٸ� �װ� ���� ������.
			m_nOffset = 0;
			Gather('\0');
			return TINVALID;
		}
		else if(c=='#'){						//�÷����� ���Դ�.
			BYTE nHigh, nLow;
			BYTE rgb[3];

			for(i=0;i<3;i++){				
				c = Input();

				if(isdigit(c) == 0){
					if((c < 'a' || c > 'z') && (c < 'A' || c > 'Z')){
						Gather('\0');
						return TINVALID;
					}else{
						if(c >= 'a' && c <= 'z') nHigh = 10 + (c - 'a'); else nHigh = 10 + (c - 'A');
					}
				}else{
					nHigh = c - 48;	/* 48�� 0�� �ƽ�Ű�ڵ� */
				}

				c = Input();

				if(isdigit(c) == 0){
					if((c < 'a' || c > 'z') && (c < 'A' || c > 'Z')){
						Gather('\0');
						return TINVALID;
					}else{
						if(islower(c)) nLow = 10 + (c - 'a'); else nLow = 10 + (c - 'A');
					}
				}else{
					nLow = c - 48;
				}
				rgb[i] = nHigh*16 + nLow;
			}
			dwScanColor = MINT_RGB(rgb[0],rgb[1],rgb[2]);
			
			c = Input();
			if(isdigit(c) == 0){
				Unput();
				Gather('\0');
#ifdef _HT_LOG_ENABLE_
				rslog("COLORVAL : %d (%d,%d,%d)\n",dwScanColor, rgb[0], rgb[1], rgb[2]);
#endif
				return TCOLORVAL;
			} else {
				return false;
			}
		}	// if( # )
		else if(isalpha(c))		// CHECK RESERVED WORD
		{
			m_nOffset = 0;
			Gather(c);
			for(c=Input(); isalpha(c) || isdigit(c); c=Input()) Gather(c);
			Unput();
			Gather('\0');

			fold(m_szScan);
			for(i=0; Reserved[i].szText != NULL; i++){
				if(strcmp(m_szScan, Reserved[i].szText) == 0){
					break;
				}
			}

			if(Reserved[i].szText != NULL)
			{
				return Reserved[i].nToken;
			}
			else
			{	//��� üũ
				for(i=0; Constant[i].szText != NULL; i++){
					if(strcmp(m_szScan, Constant[i].szText) == 0){
						break;
					}
				}
				if(Constant[i].szText != NULL) {
					return Constant[i].nToken;
				} else {
					//Undefined Symbol : ERROR!
					m_nOffset = 0;
					Gather('\0');
					return TINVALID;
				}
			}
		}
		else if(c == '\"')
		{	// �̰� ���ڿ��̴�.
			m_nOffset = 0;
			for(c = Input(); ; c = Input()){
				if(c == '\"'){
					if((c=Input()) == '\"'){
						Gather('\\');
						Gather('\"');
					} else {
						break;
					}
				} else {
					Gather(c);
				}
			}
			Unput();
			Gather('\0');
			return TSTRING;
		}
		else if(c == '=')
		{
			return TEQUAL;
		}
	}
	return ret;
}

MHyperText::MHyperText()
{
	m_pBuffer = NULL;
	m_szScan[0] = '\0';
}

bool MHyperText::Parse()
{
	//�±׿� Plain-Text�� �о�� �κ��� STD������ Element��� �θ���.
	MHyperTextElement*	pElement = NULL;
	MHyperTextArg*		pArg = NULL;
	//���� �Ľ��ϴ� �����͸� ���� ��ū
	int nToken, nVal;

	//��! ���� �Ľ��� ���Ͽ� ���̴� 2���� ������
	int nLastArg = TINVALID;
	bool bEqual = false;

	m_bTagReady	= false;			//��ĵ�ϱ� ���� �ϴ� ��� ���� ����
	bp			= 0;
	m_nOffset	= 0;

	do {
		m_nOffset = 0;

		if((nToken = Scan()) == TINVALID) return false;

		if(nToken == TTAGSTART || nToken == TENDDOC){

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// MAKE PLAIN TEXT LIST

			if(strcmp(m_szScan, "") != 0 && strcmp(m_szScan, " ") != 0 && strcmp(m_szScan, "\n") != 0){
				pElement = new MHyperTextElement(MHTE_PLAINTEXT);
				pArg = new MHTA_Text(m_szScan);

				if(pArg){
					pElement->Add(pArg);
					Elements.Add(pElement);
				}
				pElement = NULL;
				pArg = NULL;
			}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		} else {

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// MAKE TAG LIST

			switch(nToken)
			{
			case TTAGEND :		//�±װ� ������.
				if(pElement != NULL) Elements.Add(pElement);
				pElement	= NULL;
				pArg		= NULL;
				break;


			case TTSTD :		//TAG
				if(pElement != NULL) goto parse_failure;	//�±װ� �����Ѵ�.
				pElement = new MHyperTextElement(MHTE_STD);
				break;
			case TTIMAGE :		//TAG
				if(pElement != NULL) goto parse_failure;	//�±װ� �����Ѵ�.
				pElement = new MHyperTextElement(MHTE_IMAGE);
				break;
			case TTSTYLE :		//TAG
				if(pElement != NULL) goto parse_failure;	//�±װ� �����Ѵ�.
				pElement = new MHyperTextElement(MHTE_STYLE);
				break;
			case TTLINK :		//TAG
				if(pElement != NULL) goto parse_failure;	//�±װ� �����Ѵ�.
				pElement = new MHyperTextElement(MHTE_LINK);
				break;
			case TTLINKEND :
				if(pElement != NULL) goto parse_failure;
				pElement = new MHyperTextElement(MHTE_LINKEND);
				break;
			case TTBR :			//TAG
				if(pElement != NULL) goto parse_failure;	//�±װ� �����Ѵ�.
				pElement = new MHyperTextElement(MHTE_BR);
				break;
			case TTDEFAULT :
				if(pElement != NULL) goto parse_failure;
				pElement = new MHyperTextElement(MHTE_DEFAULT);
				break;


			case TRBACKGROUND :	//ARG
			case TRCOLOR :		//ARG
			case TRSIZE :		//ARG
			case TRALIGN :		//ARG
			case TRTYPE :		//ARG
			case TRSRC :		//ARG
			case TRBOLD :		//ARG
			case TRHIGHLIGHT :	//ARG
			case TRHREF :		//ARG
			case TRXMARGIN :	//ARG
			case TRYMARGIN :	//ARG
				//�±װ� �����Ǿ��°��� '='�� �ڿ� ������ �ʵ��� üũ�Ѵ�.
				if(pElement == NULL || bEqual == true) goto parse_failure;
				nLastArg = nToken;
				break;


			case TEQUAL :		//EQUAL�� L-VALUE�� �䱸�ȴ�.
				if(bEqual==true || pElement==NULL) goto parse_failure;
				bEqual = true;
				break;


			case TINTEGER :		//R-VALUE
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;
				
				nVal = atoi(m_szScan);

				switch(nLastArg){
				case TRSIZE:	pArg = new MHTA_Size(nVal);		break;
				case TRALIGN:	pArg = new MHTA_Align(nVal);	break;
				case TRTYPE:	pArg = new MHTA_Type(nVal);		break;
				case TRBOLD:	pArg = new MHTA_Bold(nVal);		break;
				case TRXMARGIN:	pArg = new MHTA_XMargin(nVal);	break;
				case TRYMARGIN:	pArg = new MHTA_YMargin(nVal);	break;
				default:		goto parse_failure;
				}

				if(pArg != NULL) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;

			case TCOLORVAL:		//R-VALUE
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;

				switch(nLastArg){
				case TRBACKGROUND:
					pArg = new MHTA_Background(dwScanColor); break;
				case TRCOLOR:
					pArg = new MHTA_Color(dwScanColor); break;
				case TRHIGHLIGHT:
					pArg = new MHTA_Highlight(dwScanColor); break;
				default:
					goto parse_failure;
				}

				if(pArg != NULL) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;

			case TSTRING :		//R-VALUE
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;
				
				switch(nLastArg){
				case TRBACKGROUND:	pArg = new MHTA_Background(m_szScan); break;
				case TRSRC :		pArg = new MHTA_Src(m_szScan); break;
				case TRHREF :		pArg = new MHTA_HRef(m_szScan); break;
				default :			goto parse_failure;
				}

				if(pArg != NULL) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;

			case TRYES :
			case TRNO :
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;
				if(nLastArg != TRBOLD) goto parse_failure;

				if(nToken == TRYES){
					nVal = 1;
				} else if(nToken == TRNO){
					nVal = 0;
				}

				pArg = new MHTA_Bold(nVal);
				if(pArg) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;

			case TRTRUE :
			case TRFALSE :
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;
				if(nLastArg != TRBOLD) goto parse_failure;

				if(nToken == TRTRUE){
					nVal = 1;
				} else if(nToken == TRFALSE){
					nVal = 0;
				}

				pArg = new MHTA_Bold(nVal);
				if(pArg) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;

			case TRLEFT :		//R-VALUE
			case TRCENTER :		//R-VALUE
			case TRRIGHT :		//R-VALUE
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;
				if(nLastArg != TRALIGN) goto parse_failure;

				nVal = 0;
				
				if(nToken == TRLEFT){
					nVal = 0;
				}else if(nToken == TRCENTER){
					nVal = 1;
				}else if(nToken == TRRIGHT){
					nVal = 2;
				}

				pArg = new MHTA_Align(nVal);
				if(pArg) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;

			case TROPEN :
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;
				if(nLastArg != TRTYPE) goto parse_failure;
				pArg = new MHTA_Type(0);
				if(pArg) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;

			case TRWWW :		//R-VALUE
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;
				if(nLastArg != TRTYPE) goto parse_failure;
				pArg = new MHTA_Type(1);
				if(pArg) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;

			case TREXEC :		//R-VALUE
				if(pElement == NULL || bEqual==false || nLastArg == TINVALID) goto parse_failure;
				if(nLastArg != TRTYPE) goto parse_failure;
				pArg = new MHTA_Type(2);
				if(pArg) pElement->Add(pArg);
				pArg = NULL;
				bEqual = false;
				break;
			default : goto parse_failure;
			}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		}
	} while(nToken != TENDDOC);

	return true;

parse_failure:
	if(bEqual == true && pArg != NULL) delete pArg;
	if(pElement) delete pElement;

	return false;
}

bool MHyperText::Open(char *szTextBuffer)
{
	bool ret;

	if(szTextBuffer == NULL){
		Close();
		return false;
	}

	Close();

	m_nLen = strlen(szTextBuffer);
	m_pBuffer = strdup(szTextBuffer);

	ret = Parse();
	if(ret == false){
		Close();
		return false;
	}

#ifdef _HT_LOG_ENABLE_
	for(int i=0; i<Elements.GetCount(); i++)
	{
		MHyperTextElement *pEl = Elements.Get(i);
		rslog("[%d] TYPE:%d [Argument(count = %d) ", i, pEl->nType, pEl->Args.GetCount());

		for(int j=0; j<pEl->Args.GetCount(); j++)
		{
			MHyperTextArg* pArg = pEl->Args.Get(j);
			if(pArg){
				rslog(" [%d]", pArg->uId);
			}
		}
		rslog("]\n");
	}
#endif

	//�Ľ��� STD�� ���ο� ��ġ�� �� �ֵ��� ����Ʈ�� �����Ѵ�.
	while(Elements.GetCount() > 0){
		MHyperTextElement *pEl = Elements.Get(0);
		if(pEl->nType != MHTE_STD) Elements.Delete(0); else break;
	}
	if(Elements.GetCount() == 0){
		Close();
		return false;
	}
	
	return ret;
}

void MHyperText::Close()
{
	m_nLen = 0;
	bp = 0;

	Elements.DeleteAll();

	if(m_pBuffer){ free(m_pBuffer); m_pBuffer = NULL; }
}

bool MHyperTextFile::Open(char *szPath)
{
	FILE *fp;
	long lFileSize;
	int i, ch;

	fp = fopen(szPath, "rt");
	if(fp){
		fseek(fp, 0, SEEK_END);
		lFileSize = ftell(fp);

		if(lFileSize == 0){
			fclose(fp);
			return false;
		}

		fseek(fp, 0, SEEK_SET);

		m_pBuffer = new char[lFileSize+1];	
		i = 0;

		do {
			ch = fgetc(fp);
			if(ch != EOF) m_pBuffer[i] = (char)ch; else m_pBuffer[i] = '\0';
			i++;
		} while(feof(fp) == 0);
		fclose(fp);
		
		return true;
	}
	return false;
}








