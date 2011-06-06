#include "DlgFPS.h"
#include "LumpFile.h"
#include "FileSystem.h"
/*#include "World.h"*/
#include "Camera.h"
#include "TextRender.h"

CDlgFPS::CDlgFPS()
{
}

CDlgFPS::~CDlgFPS()
{	
}

bool CDlgFPS::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return CUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDlgFPS::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	RegisterControl("IDC_STATIC_FPS", m_StaticFps);
	RegisterControl("IDC_STATIC_INFO", m_StaticInfo);
	//RegisterControl("IDC_DISPLAY_COORD", m_CoordDisplay);

	RegisterControlEvent("IDC_BTN_CLOSE", (PEVENT)&CDlgFPS::OnBtnClose, EVENT_BUTTON_CLICKED);
}

static bool s_bShowUBB=false;
void CDlgFPS::OnBtnClose()
{
	s_bShowUBB = !s_bShowUBB;
	//SetVisible(false);
}

void CDlgFPS::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	static float fFps = 0.0f;
	if (fElapsedTime!=0.0f)
	{
		fFps = fFps*0.99f+0.01f/fElapsedTime;
	}
	m_StaticFps.SetFloat(fFps,0,2);
	//m_StaticInfo.SetText(DXUTGetDeviceStats());
	// Show FPS
	//if( GetConfig().IsShowFPS() )
	//{
	//	RECT rc;
	//	SetRect( &rc, 70, pd3dsdBackBuffer->Height-60, 0, 0 ); 
	//	//GetTextRender().DrawUBB( L"[color=244,122,128]！《》[color=125,255,0,128][B]潘会会[/B][/color]是[color=64,128,255][I]蠢[/color]材[/I]。[color=255,64,64]中国[/color]200[color=255,255,255]8[/color]，加油！ 尊びのこの世の誠はそこ[color=255,255,255]に ○[/color]○♀♂☆☆[/color]", &rc );
	//	GetTextRender().SetInsertionPos( 70, pd3dsdBackBuffer->Height-40 );
	//	GetTextRender().SetShadowColor( D3DXCOLOR( 0.6f, 0.0f, 0.0f, 1.0f ) );
	//	GetTextRender().SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
	//	GetTextRender().drawText( DXUTGetFrameStats(true) );
	//	GetTextRender().SetForegroundColor( D3DXCOLOR( 1.0f, 0.6f, 1.0f, 0.6f ) );
	//	GetTextRender().drawText( DXUTGetDeviceStats());
	//}
	CUIDialog::OnFrameRender(mTransform,fTime,fElapsedTime);
	{
		/*static CUBB g_TestUBB;
		if (  1 && GetTextRender().m_bHasUpdata )
		{
			GetTextRender().m_bHasUpdata = false;
			RECT rc;
			SetRect( &rc, 7, 60, 600, 0 ); 

			if ( int naocan = 1 )
			{
				GetTextRender().BuildUBB( &g_TestUBB, L" \
[size=16][color=255,64,0][size=20] [align=center]係脃朢[/align][/size][/color][br]\
[color=255,64,0][size=12][align=RIGHT]莋鍺:蕝朢姺泩[/align][/size][/color][br]\
　　“[color=255,0,0][size=20]初[/size][/color][color=255,128,128]見驚滟，侢見畩嘫[/color]”，\
[FLIPH]茬[FLIPV]莪[/FLIPV]看唻[/FLIPH]，適呮湜①種羙恏哋蒝朢。初見，驚滟。蓦嘫冋渞，嶒經獊嗨。呮啪皁巳換孒亾簡。[br] \
“[I][color=255,0,0][size=20]亾[/size][/color][color=128,255,128]泩婼呮洳初見，哬倳偢颩蕜畵傓？等娴變卻诂亾杺，卻檤诂亾杺昜變。 \
骊屾娪罢凊宵柈，液雨霖玪蔠芣葾。\
哬洳薄圉婂扆ル，仳翼涟汥當ㄖ蒝。[/I][/color]”妠蘭萇纡凊堔纡凊，彵哋詞凊噺啘箹，\
妸姒矗杼洶臆，给亾佷堔哋亾泩憾圄。[br] \
　　[color=255,0,0][size=20]湜[/size][/color]哋，亾泩婼呮洳初見哪姟哆恏，烸①個亾當朂初啝沵楿喁，\
哪種羙恏哋憾覺①矗僦潒舂兲初倣哋埖，哪種溫馨、哪種洎嘫\
、哪種嫃諴、哪種冋忆，洇泚僦①矗弥嫚茬孒沵哋泩掵狆。潙什庅\
茬亾哋茭暀狆浍洧誤浍、曊解、猜恻啝悱议迡？呮洧惔惔哋洳\
渁哋凊懷芣僦娖夠孒嬤？僦潒苁沬湈媔哋蛧伖，烸佽茬蛧仩喁菿溡糇\
沍楿咑個妱苸，杺狆洊洧彼泚哋撁啩，芣竾湜①件佷羙哋倳凊嬤？\
莪想焄ふと茭惔洳渁竾僦湜適個檤理妑？[br]\
　　[color=255,0,0][size=20]侢[/size][/color]渎席募嫆哋《[color=255,128,128]初楿喁[/color]》，\
咜説：[color=255,255,128]羙婯哋夢啝羙婯哋詩①樣，嘟\
湜妸喁洏芣妸浗哋，瑺瑺茬朂莈能料菿哋溡尅里炪哯。[/color][br]\
　　[color=255,128,128][size=20]莪[/size][color=255,128,128]囍歡那樣啲夢，茬夢裏，┅切都鈳鉯重噺開始，┅切都鈳鉯慢慢\
解釋，惢裏甚至還能感覺箌，所洧被浪費啲\
塒咣竟然都能重囙塒啲狂囍與感噭。胸懷ф滿溢著圉鍢，呮因伱就茬莪眼前，對莪微笑，┅洳當姩。[/color][/size]", rc );
			}
			else if( int naocan = 0 )
			{
				GetTextRender().BuildUBB(
					&g_TestUBB,
					L"  [size=10][color=255,64,0][size=15] [align=center]「塩水浴」の考察[/align][/size][/color][br]\
 [color=255,64,0][size=8][align=RIGHT]rpgsky.com[/align][/size][/color][br]\
 --------------------------------------------------------------------------------\
 　先般、「微生物の基礎知識」で一部触れました「塩水浴」に関して、専門的に論述された文献に巡り合いました。その文章を引用するとともに、それを叩き台として、考察をすすめていこうと思います。\n\
  　塩水浴をご存知ではない方はいらっしゃらないとは思いますが、簡単に定義をしておきましょう。金魚の病気に際して広く行なわれている治療方法で、大きく分けて、二つの方法があります。ひとつは、1％以上の濃い塩水に魚を短時間入れる塩水浴と、もうひとつは、0.5～0.6％の塩水に長時間入れて治療する方法です。短時間浴の場合、魚体にかなりの負担を強いる危険な治療方法で、主に魚体表面に付着している細菌の除去に使用します。逆に長時間浴の場合は、0.5～0.6％の塩水では魚の挙動に変化はなく、緩やかに魚体の回復を促す作用があると一般に考えられています。\n\
   　上にも書きましたように、この治療方法は広く行なわれており、金魚愛好家ともなると、必ずや病気治療の一番の方法として活用されています。この方法をマスターせずには金魚飼育は出来ないとまでいわれているにも関わらず、何故塩水浴が金魚の病気治療に効果があるかという素朴な疑問は、愛好家にして明解に説明されることは未だありませんでした。\n\
 　　“[color=255,0,0][size=15]初[/size][/color][color=255,128,128]见惊艳，再见依然[/color]”，[FLIPH]在[FLIPV]我[/FLIPV]看来[/FLIPH]，这只是一种美好的愿望。初见，惊艳。\n\
   蓦然回首，曾经沧海。只怕早已换了人间。[br] \
   　　“[I][color=255,0,0][size=15]人[/size][/color][color=128,255,128]生若只如初见，何事秋风悲画扇？等闲变却故人心，却道故人心易变。 \
 骊山语罢清宵半，夜雨霖铃终不怨。\
 何如薄幸锦衣儿，比翼连枝当日愿。[/I][/color]”纳兰长于情深于情，他的词清新婉约，\
 可以直抒胸臆，给人很深的人生感悟。[br] \
 　　[size=9][color=255,0,0][size=16]是[/size][/color]的，人生若只如初见那该多好，每一个人当最初和你相遇，\
   那种美好的感觉一直就象春天初放的花，那种温馨、那种自然\
   、那种真诚、那种回忆，因此就一直弥漫在了你的生命中。为什么\
   在人的交往中会有误会、费解、猜测和非议呢？只有淡淡的如\
   水的情怀不就足够了吗？就象从未谋面的网友，每次在网上遇到时候\
   互相打个招呼，心中存有彼此的牵挂，不也是一件很美的事情吗？\
   我想君子之交淡如水也就是这个道理吧？[/size][br]\
   　　[color=255,0,0][size=15]再[/size][/color]读席幕容的《[color=255,128,128]初相遇[/color]》，她说：[color=255,255,128]美丽的梦和美丽的诗一样，都\
 是可遇而不可求的，常常在最没能料到的时刻里出现。[/color][br]\
 　　[color=255,128,128][size=15]我[/size][color=255,128,128]喜欢那样的梦，在梦里，一切都可以重新开始，一切都可以慢慢\
   解释，心里甚至还能感觉到，所有被浪费的\
   时光竟然都能重回时的狂喜与感激。胸怀中满溢着幸福，只因你就在我眼前，对我微笑，一如当年。[/color][/size]", rc );
			}
		}
		if (s_bShowUBB)
		{
			//UIGraph::getInstance().getTextRender
			//GetTextRender().DrawUBB( &g_TestUBB );
		}*/
	}
}

void CDlgFPS::SetViewDir(Vec3D& vViewDir)
{
	m_CoordDisplay.SetViewDir(vViewDir);
}