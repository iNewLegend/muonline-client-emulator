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
	//	//GetTextRender().DrawUBB( L"[color=244,122,128]������[color=125,255,0,128][B]�˻��[/B][/color]��[color=64,128,255][I]��[/color]��[/I]��[color=255,64,64]�й�[/color]200[color=255,255,255]8[/color]�����ͣ� ��ӤΤ��������\�Ϥ���[color=255,255,255]�� ��[/color]������[/color]", &rc );
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
[size=16][color=255,64,0][size=20] [align=center]�S�y�R[/align][/size][/color][br]\
[color=255,64,0][size=12][align=RIGHT]�g�N:ʅ�R����[/align][/size][/color][br]\
������[color=255,0,0][size=20]��[/size][/color][color=255,128,128]Ҋ�@�٣���Ҋ���Y[/color]����\
[FLIPH]��[FLIPV]ݭ[/FLIPV]����[/FLIPH]���m�望�ٷN�o�h�O�V�R����Ҋ���@�١���Y�՜r���򽛪��ˡ���ž�o�ȓQ�H������[br] \
��[I][color=255,0,0][size=20]��[/size][/color][color=128,255,128]���S��䲳�Ҋ���Y�����Mʄ���ޣ����׃�sڬ�������s�|ڬ�����[׃�� \
���ӰՃ�������Һ���ثl�W�]�A��\
�Y䲱�������룬�������D�����V��[/I][/color]���{�m�O���������������O�~����������\
��榴��̛��ܣ�������ކO����������[br] \
����[color=255,0,0][size=20]��[/size][/color]�O�������S��䲳�Ҋ�Ċ��ߐh���Q�ق������������m�U௣�\
�ķN�o�h�O���X�ٴ��֝����̳����O�����ķN��ܰ���ķN䩇Y\
���ķN���P���ķN���䣬䦛��֢ٴ��֋���H�m�O�����𡣝�ʲ��\
�灓�O���������`䫡��ս⡢���������ދ����䢐����O�\
�a�O�����]�֊Ɖ�H�ߣ��֝��ʛi���i�O͇�����Q���͇����K����\
�Z�U�������~������䢱˛��O�Æ����]�����ټ����o�O�����ߣ�\
ݭ��[�դ�����䲜a���֜��m���|��r��[br]\
����[color=255,0,0][size=20]��[/size][/color]��ϯļ���O��[color=255,128,128]���U�[/color]����\
���h��[color=255,255,128]�o�K�O���o�K�OԊ�٘ӣ��\
����௛��]�����O����������e�����K�O�Ҍ����Z��[/color][br]\
����[color=255,128,128][size=20]ݭ[/size][color=255,128,128]�֚g�ǘӆ���������Y�����ж���i�؇��_ʼ�����ж���i����\
��ጣ����Y����߀�ܸ��X�|����䢱����M��\
�P�۾�Ȼ�����؇��P������c�Ї����ؑѧ�M������嘣�����þͲ�ݭ��ǰ����ݭ΢Ц����䲮�����[/color][/size]", rc );
			}
			else if( int naocan = 0 )
			{
				GetTextRender().BuildUBB(
					&g_TestUBB,
					L"  [size=10][color=255,64,0][size=15] [align=center]���cˮԡ���ο���[/align][/size][/color][br]\
 [color=255,64,0][size=8][align=RIGHT]rpgsky.com[/align][/size][/color][br]\
 --------------------------------------------------------------------------------\
 ���Ȱ㡢��΢����λ��A֪�R����һ������ޤ������cˮԡ�����v���ơ����T�Ĥ�Փ�����줿���פ�Ѳ��Ϥ��ޤ������������¤����ä���ȤȤ�ˡ������ߵ��̨�Ȥ��ơ�����򤹤���Ƥ�������˼���ޤ���\n\
  ���cˮԡ�򤴴�֪�ǤϤʤ����Ϥ���ä����ʤ��Ȥ�˼���ޤ��������g�˶��x�򤷤Ƥ����ޤ��礦�����~�β��ݤ��H���Ǝڤ��Фʤ��Ƥ����ί������ǡ��󤭤��֤��ơ����Ĥη���������ޤ����ҤȤĤϡ�1�����ϤΝ⤤�cˮ���~��̕r�g����cˮԡ�ȡ��⤦�ҤȤĤϡ�0.5��0.6���Ήcˮ���L�r�g�����ί����뷽���Ǥ����̕r�gԡ�Έ��ϡ��~��ˤ��ʤ��ؓ���򏊤���Σꓤ��ί������ǡ������~�����˸��Ť��Ƥ��뼚���γ�ȥ��ʹ�ä��ޤ�������L�r�gԡ�Έ��Ϥϡ�0.5��0.6���Ήcˮ�Ǥ��~�Β��Ӥˉ仯�Ϥʤ������䤫���~��λ؏ͤ�٤����ä������һ��˿������Ƥ��ޤ���\n\
   ���Ϥˤ�����ޤ����褦�ˡ������ί������ώڤ��Фʤ��Ƥ��ꡢ���~�ۺüҤȤ�ʤ�ȡ��ؤ��䲡���ί���һ���η����Ȥ��ƻ��ä���Ƥ��ޤ������η�����ޥ����`�����ˤϽ��~����ϳ����ʤ��ȤޤǤ����Ƥ���ˤ��v��餺���ιʉcˮԡ�����~�β����ί��˄��������뤫�Ȥ������Ӥ��Ɇ��ϡ��ۺüҤˤ���������h������뤳�Ȥ�δ������ޤ���Ǥ�����\n\
 ������[color=255,0,0][size=15]��[/size][/color][color=255,128,128]�����ޣ��ټ���Ȼ[/color]����[FLIPH]��[FLIPV]��[/FLIPV]����[/FLIPH]����ֻ��һ�����õ�Ը�������������ޡ�\n\
   ��Ȼ���ף������׺���ֻ�����ѻ����˼䡣[br] \
   ������[I][color=255,0,0][size=15]��[/size][/color][color=128,255,128]����ֻ�������������籯���ȣ����б�ȴ�����ģ�ȴ���������ױ䡣 \
 ��ɽ��������룬ҹ�������ղ�Թ��\
 ���籡�ҽ��¶���������֦����Ը��[/I][/color]�����������������飬���Ĵ�������Լ��\
 ����ֱ�����ܣ����˺������������[br] \
 ����[size=9][color=255,0,0][size=16]��[/size][/color]�ģ�������ֻ������Ǹö�ã�ÿһ���˵��������������\
   �������õĸо�һֱ��������ŵĻ���������ܰ��������Ȼ\
   ��������ϡ����ֻ��䣬��˾�һֱ����������������С�Ϊʲô\
   ���˵Ľ����л�����ᡢ�ѽ⡢�²�ͷ����أ�ֻ�е�������\
   ˮ���黳�����㹻���𣿾����δı������ѣ�ÿ������������ʱ��\
   �������к������д��б˴˵�ǣ�ң���Ҳ��һ��������������\
   �������֮������ˮҲ�����������ɣ�[/size][br]\
   ����[color=255,0,0][size=15]��[/size][/color]��ϯĻ�ݵġ�[color=255,128,128]������[/color]������˵��[color=255,255,128]�������κ�������ʫһ������\
 �ǿ�����������ģ���������û���ϵ���ʱ������֡�[/color][br]\
 ����[color=255,128,128][size=15]��[/size][color=255,128,128]ϲ���������Σ������һ�ж��������¿�ʼ��һ�ж���������\
   ���ͣ������������ܸо��������б��˷ѵ�\
   ʱ�⾹Ȼ�����ػ�ʱ�Ŀ�ϲ��м����ػ����������Ҹ���ֻ�����������ǰ������΢Ц��һ�統�ꡣ[/color][/size]", rc );
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