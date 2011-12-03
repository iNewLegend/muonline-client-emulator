#ifndef MTYPES_H
#define MTYPES_H

/// ��ġ
struct MPOINT{
	int x, y;
public:
	MPOINT(void){}
	MPOINT(int x, int y){ MPOINT::x = x, MPOINT::y = y; }

	void Scale(float x, float y);
	void ScaleRes(void);				///< 648*480���ذ��� ���� �ػ󵵿� ���� �����ϸ�
	void TranslateRes(void);			///< 648*480 �߽������� �Ÿ���ŭ ���� �ػ󵵿� �°� �̵�
};

/// ����
struct MRECT{
	int	x, y;	///< ��ġ
	int w, h;	///< ũ��
public:
	MRECT(void){}
	MRECT(int x, int y, int w, int h){
		Set(x, y, w, h);
	}
	bool InPoint(MPOINT& p){
		if(p.x>=x && p.x<=x+w && p.y>=y && p.y<=y+h) return true;
		return false;
	}
	void Set(int x, int y, int w, int h){
		MRECT::x = x, MRECT::y = y;
		MRECT::w = w, MRECT::h = h;
	}

	void ScalePos(float x, float y);	///< �������� ���� �����ϸ�
	void ScaleArea(float x, float y);	///< �������� ���̿� ���� �����ϸ�
	void ScalePosRes(void);				///< 648*480���ذ��� ���� �ػ󵵿� ���� �����ϸ�
	void ScaleAreaRes(void);			///< 648*480���ذ��� ���� �ػ󵵿� ���� �����ϸ�
	void TranslateRes(void);			///< 648*480 �߽������� �Ÿ���ŭ ���� �ػ󵵿� �°� �̵�

	void EnLarge(int w);				///< �¿� ���ϸ� w��ŭ�� ũ�� Ű���
	void Offset(int x, int y);			///< ������ �̵�

	bool Intersect(MRECT* pIntersect, const MRECT& r);	///< �� �簢���� ���� ���� ����
};

/// ũ��
struct MSIZE{
	int w, h;
public:
	MSIZE(void){}
	MSIZE(int w, int h){
		MSIZE::w = w;
		MSIZE::h = h;
	}
};

/// a,r,g,b���� unsigned long int������ ��ȯ
#define MINT_ARGB(a,r,g,b)		( ((((unsigned long int)a)&0xFF)<<24) | ((((unsigned long int)r)&0xFF)<<16) | ((((unsigned long int)g)&0xFF)<<8) | (((unsigned long int)b)&0xFF) )
/// r,g,b���� unsigned long int������ ��ȯ
#define MINT_RGB(r,g,b)			( ((((unsigned long int)r)&0xFF)<<16) | ((((unsigned long int)g)&0xFF)<<8) | (((unsigned long int)b)&0xFF) )

/// r, g, b, a �÷�
struct MCOLOR{
public:
	unsigned char	r;		///< Red
	unsigned char	g;		///< Green
	unsigned char	b;		///< Blue
	unsigned char	a;		///< Alpha
public:
	MCOLOR(void){
		r = g = b = a = 0;
	}
	MCOLOR(unsigned char r, unsigned char g, unsigned char b, unsigned char a=255){
		MCOLOR::r = r, MCOLOR::g = g, MCOLOR::b = b, MCOLOR::a = a;
	}
	MCOLOR(unsigned long int argb){
		a = unsigned char( (argb & 0xFF000000) >> 24 );
		r = unsigned char( (argb & 0x00FF0000) >> 16 );
		g = unsigned char( (argb & 0x0000FF00) >> 8 );
		b = unsigned char( (argb & 0x000000FF) );
	}
	
	unsigned long int GetARGB(void){
		return MINT_ARGB(a, r, g, b);
	}
	unsigned long int GetRGB(void){
		return MINT_RGB(r, g, b);
	}
};


#endif
