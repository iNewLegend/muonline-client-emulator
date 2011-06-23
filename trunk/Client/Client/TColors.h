//-------------------------------------------------------------------------------------------------------------------------------------------
#ifndef COLORS_H
#define COLORS_H
//-------------------------------------------------------------------------------------------------------------------------------------------

#include <windows.h>
//-------------------------------------------------------------------------------------------------------------------------------------------

class CColors
{
public:
	CColors() {};
	virtual ~CColors() {};
	// ----
	static COLORREF	AliceBlue()				{ return 0xFFF0F8FF; };
	static COLORREF	AntiqueWhite()			{ return 0xFFFAEBD7; };
	static COLORREF	Aqua()					{ return 0xFF00FFFF; };
	static COLORREF	Aquamarine()			{ return 0xFF7FFFD4; };
	static COLORREF	Azure()					{ return 0xFFF0FFFF; };
	static COLORREF	Beige()					{ return 0xFFF5F5DC; };
	static COLORREF	Bisque()				{ return 0xFFFFE4C4; };
	static COLORREF	Black()					{ return 0xFF000000; };
	static COLORREF	BlanchedAlmond()		{ return 0xFFFFEBCD; };
	static COLORREF	Blue()					{ return 0xFF0000FF; };
	static COLORREF	BlueViolet()			{ return 0xFF8A2BE2; };
	static COLORREF	Brown()					{ return 0xFFA52A2A; };
	static COLORREF	BurlyWood()				{ return 0xFFDEB887; };
	static COLORREF	CadetBlue()				{ return 0xFF5F9EA0; };
	static COLORREF	Chartreuse()			{ return 0xFF7FFF00; };
	static COLORREF	Chocolate()				{ return 0xFFD2691E; };
	static COLORREF	Coral()					{ return 0xFFFF7F50; };
	static COLORREF	CornflowerBlue()		{ return 0xFF6495ED; };
	static COLORREF	Cornsilk()				{ return 0xFFFFF8DC; };
	static COLORREF	Crimson()				{ return 0xFFDC143C; };
	static COLORREF	Cyan()					{ return 0xFF00FFFF; };
	static COLORREF	DarkBlue()				{ return 0xFF00008B; };
	static COLORREF	DarkCyan()				{ return 0xFF008B8B; };
	static COLORREF	DarkGoldenrod()			{ return 0xFFB8860B; };
	static COLORREF	DarkGray()				{ return 0xFFA9A9A9; };
	static COLORREF	DarkGreen()				{ return 0xFF006400; };
	static COLORREF	DarkKhaki()				{ return 0xFFBDB76B; };
	static COLORREF	DarkMagenta()			{ return 0xFF8B008B; };
	static COLORREF	DarkOliveGreen()		{ return 0xFF556B2F; };
	static COLORREF	DarkOrange()			{ return 0xFFFF8C00; };
	static COLORREF	DarkOrchid()			{ return 0xFF9932CC; };
	static COLORREF	DarkRed()				{ return 0xFF8B0000; };
	static COLORREF	DarkSalmon()			{ return 0xFFE9967A; };
	static COLORREF	DarkSeaGreen()			{ return 0xFF8FBC8B; };
	static COLORREF	DarkSlateBlue()			{ return 0xFF483D8B; };
	static COLORREF	DarkSlateGray()			{ return 0xFF2F4F4F; };
	static COLORREF	DarkTurquoise()			{ return 0xFF00CED1; };
	static COLORREF	DarkViolet()			{ return 0xFF9400D3; };
	static COLORREF	DeepPink()				{ return 0xFFFF1493; };
	static COLORREF	DeepSkyBlue()			{ return 0xFF00BFFF; };
	static COLORREF	DimGray()				{ return 0xFF696969; };
	static COLORREF	DodgerBlue()			{ return 0xFF1E90FF; };
	static COLORREF	Firebrick()				{ return 0xFFB22222; };
	static COLORREF	FloralWhite()			{ return 0xFFFFFAF0; };
	static COLORREF	ForestGreen()			{ return 0xFF228B22; };
	static COLORREF	Fuchsia()				{ return 0xFFFF00FF; };
	static COLORREF	Gainsboro()				{ return 0xFFDCDCDC; };
	static COLORREF	GhostWhite()			{ return 0xFFF8F8FF; };
	static COLORREF	Gold()					{ return 0xFFFFD700; };
	static COLORREF	Goldenrod()				{ return 0xFFDAA520; };
	static COLORREF	Gray()					{ return 0xFF808080; };
	static COLORREF	Green()					{ return 0xFF008000; };
	static COLORREF	GreenYellow()			{ return 0xFFADFF2F; };
	static COLORREF	Honeydew()				{ return 0xFFF0FFF0; };
	static COLORREF	HotPink()				{ return 0xFFFF69B4; };
	static COLORREF	IndianRed()				{ return 0xFFCD5C5C; };
	static COLORREF	Indigo()				{ return 0xFF4B0082; };
	static COLORREF	Ivory()					{ return 0xFFFFFFF0; };
	static COLORREF	Khaki()					{ return 0xFFF0E68C; };
	static COLORREF	Lavender()				{ return 0xFFE6E6FA; };
	static COLORREF	LavenderBlush()			{ return 0xFFFFF0F5; };
	static COLORREF	LawnGreen()				{ return 0xFF7CFC00; };
	static COLORREF	LemonChiffon()			{ return 0xFFFFFACD; };
	static COLORREF	LightBlue()				{ return 0xFFADD8E6; };
	static COLORREF	LightCoral()			{ return 0xFFF08080; };
	static COLORREF	LightCyan()				{ return 0xFFE0FFFF; };
	static COLORREF	LightGoldenrodYellow()	{ return 0xFFFAFAD2; };
	static COLORREF	LightGray()				{ return 0xFFD3D3D3; };
	static COLORREF	LightGreen()			{ return 0xFF90EE90; };
	static COLORREF	LightPink()				{ return 0xFFFFB6C1; };
	static COLORREF	LightSalmon()			{ return 0xFFFFA07A; };
	static COLORREF	LightSeaGreen()			{ return 0xFF20B2AA; };
	static COLORREF	LightSkyBlue()			{ return 0xFF87CEFA; };
	static COLORREF	LightSlateGray()		{ return 0xFF778899; };
	static COLORREF	LightSteelBlue()		{ return 0xFFB0C4DE; };
	static COLORREF	LightYellow()			{ return 0xFFFFFFE0; };
	static COLORREF	Lime()					{ return 0xFF00FF00; };
	static COLORREF	LimeGreen()				{ return 0xFF32CD32; };
	static COLORREF	Linen()					{ return 0xFFFAF0E6; };
	static COLORREF	Magenta()				{ return 0xFFFF00FF; };
	static COLORREF	Maroon()				{ return 0xFF800000; };
	static COLORREF	MediumAquamarine()		{ return 0xFF66CDAA; };
	static COLORREF	MediumBlue()			{ return 0xFF0000CD; };
	static COLORREF	MediumOrchid()			{ return 0xFFBA55D3; };
	static COLORREF	MediumPurple()			{ return 0xFF9370DB; };
	static COLORREF	MediumSeaGreen()		{ return 0xFF3CB371; };
	static COLORREF	MediumSlateBlue()		{ return 0xFF7B68EE; };
	static COLORREF	MediumSpringGreen()		{ return 0xFF00FA9A; };
	static COLORREF	MediumTurquoise()		{ return 0xFF48D1CC; };
	static COLORREF	MediumVioletRed()		{ return 0xFFC71585; };
	static COLORREF	MidnightBlue()			{ return 0xFF191970; };
	static COLORREF	MintCream()				{ return 0xFFF5FFFA; };
	static COLORREF	MistyRose()				{ return 0xFFFFE4E1; };
	static COLORREF	Moccasin()				{ return 0xFFFFE4B5; };
	static COLORREF	NavajoWhite()			{ return 0xFFFFDEAD; };
	static COLORREF	Navy()					{ return 0xFF000080; };
	static COLORREF	OldLace()				{ return 0xFFFDF5E6; };
	static COLORREF	Olive()					{ return 0xFF808000; };
	static COLORREF	OliveDrab()				{ return 0xFF6B8E23; };
	static COLORREF	Orange()				{ return 0xFFFFA500; };
	static COLORREF	OrangeRed()				{ return 0xFFFF4500; };
	static COLORREF	Orchid()				{ return 0xFFDA70D6; };
	static COLORREF	PaleGoldenrod()			{ return 0xFFEEE8AA; };
	static COLORREF	PaleGreen()				{ return 0xFF98FB98; };
	static COLORREF	PaleTurquoise()			{ return 0xFFAFEEEE; };
	static COLORREF	PaleVioletRed()			{ return 0xFFDB7093; };
	static COLORREF	PapayaWhip()			{ return 0xFFFFEFD5; };
	static COLORREF	PeachPuff()				{ return 0xFFFFDAB9; };
	static COLORREF	Peru()					{ return 0xFFCD853F; };
	static COLORREF	Pink()					{ return 0xFFFFC0CB; };
	static COLORREF	Plum()					{ return 0xFFDDA0DD; };
	static COLORREF	PowderBlue()			{ return 0xFFB0E0E6; };
	static COLORREF	Purple()				{ return 0xFF800080; };
	static COLORREF	Red()					{ return 0xFFFF0000; };
	static COLORREF	RosyBrown()				{ return 0xFFBC8F8F; };
	static COLORREF	RoyalBlue()				{ return 0xFF4169E1; };
	static COLORREF	SaddleBrown()			{ return 0xFF8B4513; };
	static COLORREF	Salmon()				{ return 0xFFFA8072; };
	static COLORREF	SandyBrown()			{ return 0xFFF4A460; };
	static COLORREF	SeaGreen()				{ return 0xFF2E8B57; };
	static COLORREF	SeaShell()				{ return 0xFFFFF5EE; };
	static COLORREF	Sienna()				{ return 0xFFA0522D; };
	static COLORREF	Silver()				{ return 0xFFC0C0C0; };
	static COLORREF	SkyBlue()				{ return 0xFF87CEEB; };
	static COLORREF	SlateBlue()				{ return 0xFF6A5ACD; };
	static COLORREF	SlateGray()				{ return 0xFF708090; };
	static COLORREF	Snow()					{ return 0xFFFFFAFA; };
	static COLORREF	SpringGreen()			{ return 0xFF00FF7F; };
	static COLORREF	SteelBlue()				{ return 0xFF4682B4; };
	static COLORREF	Tan()					{ return 0xFFD2B48C; };
	static COLORREF	Teal()					{ return 0xFF008080; };
	static COLORREF	Thistle()				{ return 0xFFD8BFD8; };
	static COLORREF	Tomato()				{ return 0xFFFF6347; };
	static COLORREF	Transparent()			{ return 0xFFFFFFFF; };
	static COLORREF	Turquoise()				{ return 0xFF40E0D0; };
	static COLORREF	Violet()				{ return 0xFFEE82EE; };
	static COLORREF	Wheat()					{ return 0xFFF5DEB3; };
	static COLORREF	White()					{ return 0xFFFFFFFF; };
	static COLORREF	WhiteSmoke()			{ return 0xFFF5F5F5; };
	static COLORREF	Yellow()				{ return 0xFFFFFF00; };
};
//-------------------------------------------------------------------------------------------------------------------------------------------
#endif /* COLORS_H */
//-------------------------------------------------------------------------------------------------------------------------------------------


