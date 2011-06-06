#include "color.h"

// Compute RGB from HLS. The light and saturation are between [0,1]
// and hue is between [0,360]. The returned r,g,b triplet is between [0,1].

// a local auxiliary function
struct CLocal
{
	static float HLSToRGB(float fRn1, float fRn2, float fHuei)
	{
		float hue = fHuei;
		if(hue>360) hue = hue - 360;
		if(hue<0)   hue = hue + 360;
		if(hue<60) return fRn1 + (fRn2-fRn1)*hue/60;
		if(hue<180) return fRn2;
		if(hue<240) return fRn1 + (fRn2-fRn1)*(240-hue)/60;
		return fRn1;
	}
};

//Color32(float fA, float fR, float fG, float fB): a(min(fA*255,255)), r(min(fR*255,255)), g(min(fG*255,255)), b(min(fB*255,255)) {}
// HLS
Color32::Color32(int nHue, int nLight, int nSaturation)
{
	Color32((360.0f/255.0f)*float(nHue), (1.0f/255.0f)*float(nLight), (1.0f/255.0f)*float(nSaturation));
}

Color32::Color32(float fHue, float fLight, float fSaturation)
{
	float rh, rl, rs, rm1, rm2;
	rh = rl = rs = 0;
	if(fHue>0)		  rh = fHue;		if(rh>360) rh = 360;
	if(fLight>0)	  rl = fLight;		if(rl>1)   rl = 1;
	if(fSaturation>0) rs = fSaturation; if(rs>1)   rs = 1;

	if(rl<=0.5f)
		rm2 = rl*(1.0f + rs);
	else
		rm2 = rl + rs - rl*rs;
	rm1 = 2.0f*rl - rm2;

	if(!rs) 
	{ 
		r = (unsigned char)(rl*255); 
		g = (unsigned char)(rl*255); 
		b = (unsigned char)(rl*255); 
	}
	else
	{
		r = (unsigned char)(CLocal::HLSToRGB(rm1, rm2, rh+120)*255);
		g = (unsigned char)(CLocal::HLSToRGB(rm1, rm2, rh)*255);
		b = (unsigned char)(CLocal::HLSToRGB(rm1, rm2, rh-120)*255);
	}
}

const Color32 Color32::modulate(const Color32 &v1, const Color32 &v2)
{
	Color32 c;
	c.r = (v1.r*v2.r)>>8;
	c.g = (v1.g*v2.g)>>8;
	c.b = (v1.b*v2.b)>>8;
	c.a = (v1.a*v2.a)>>8;
	return c;
}

const Color32 Color32::modulateX2(const Color32 &v1, const Color32 &v2)
{
	Color32 c;
	c.r = (v1.r*v2.r)>>9;
	c.g = (v1.g*v2.g)>>9;
	c.b = (v1.b*v2.b)>>9;
	c.a = (v1.a*v2.a)>>9;
	return c;
}

const Color32 Color32::modulateX4(const Color32 &v1, const Color32 &v2)
{
	Color32 c;
	c.r = (v1.r*v2.r)>>10;
	c.g = (v1.g*v2.g)>>10;
	c.b = (v1.b*v2.b)>>10;
	c.a = (v1.a*v2.a)>>10;
	return c;
}

Color32& Color32::operator= (const unsigned long color)
{
	c = color;
	return *this;
}

bool Color32::operator== (const Color32& clr) const
{
	return c==clr.c;
}

Color32& Color32::operator= (const std::string& str)
{
	if (str.length()==0)
	{
		return* this;
	}
	const char* pChar = str.c_str();
	if (*pChar == '#')
	{
		pChar++;
	}
	if (str.length()>=8)
	{
		for (int i = 0; i < 8; i++)
		{
			int n = 0;
			if ('\0' == *pChar)
			{
				break;
			}
			else if ('0' <= *pChar && '9' >= *pChar)
			{
				n = *pChar - '0';
			}
			else if ('A' <= *pChar && 'F' >= *pChar)
			{
				n = 10 + *pChar - 'A';
			}
			else if ('a' <= *pChar && 'f' >= *pChar)
			{
				n = 10 + *pChar - 'a';
			}
			c<<=4;
			c|=n;
			pChar++;
		}
	}
	else if (str.length()>=4)
	{
		for (int i = 0; i < 4; i++)
		{
			int n = 0;
			if ('\0' == *pChar)
			{
				break;
			}
			else if ('0' <= *pChar && '9' >= *pChar)
			{
				n = *pChar - '0';
			}
			else if ('A' <= *pChar && 'F' >= *pChar)
			{
				n = 10 + *pChar - 'A';
			}
			else if ('a' <= *pChar && 'f' >= *pChar)
			{
				n = 10 + *pChar - 'a';
			}
			c<<=4;
			c|=n;
			c<<=4;
			c|=n;
			pChar++;
		}
	}
	return *this;
}