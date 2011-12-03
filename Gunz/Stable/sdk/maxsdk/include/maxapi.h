/*********************************************************************
 *<
	FILE: maxapi.h

	DESCRIPTION: These are functions that are exported from the 
	             3DS MAX executable.

	CREATED BY:	Rolf Berteig

	HISTORY: Created 28 November 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __JAGAPI__
#define __JAGAPI__

#include <ole2.h>
#include "cmdmode.h"
#include "sceneapi.h"
#include "rtclick.h"
#include "evuser.h"
#include "maxcom.h"
#include "plugin.h"
#include "log.h"
#include "ActionTable.h"
#include "iTreeVw.h"
#include "iRenderPresets.h"
#include "excllist.h"
#include "DefaultActions.h"

class ViewParams;
class ModContext;
class HitData;
class HitLog;
class CtrlHitLog;
class MtlBase;
class Mtl;
class Texmap;
class PickNodeCallback;
class Renderer;
class IScanRenderer;
class RendParams;
class IRenderElementMgr;
class RendProgressCallback;
class Bitmap;
class BitmapInfo;
class Texmap;
class SoundObj;
class GenCamera;
class GenLight;
class NameTab;
class ShadowType;
class MacroRecorder;
class CommandLineCallback;

#ifdef _OSNAP
class IOsnapManager;
class MouseManager;
#endif
class MtlBaseLib;
class Atmospheric;
class IRollupWindow;
class ITrackViewNode;
class DllDir;
class Effect;
class SpaceArrayCallback;
class IMenuManager;
class IColorManager;

// RB: I didn't want to include render.h here
#ifndef _REND_TYPE_DEFINED
#define _REND_TYPE_DEFINED
enum RendType { 
	RENDTYPE_NORMAL,
	RENDTYPE_REGION,
	RENDTYPE_BLOWUP,
	RENDTYPE_SELECT,
	RENDTYPE_REGIONCROP,
	// The following 2 types are to be passed into plugin renderers.  There purpos is for passing to
	//  the function Interface::OpenCurRenderer, which converts them into RENDTYPE_REGION and RENDTYPE_REGIONCROP, resp.
	RENDTYPE_REGION_SEL, // do a region render using the bounding rectangle of the selection
	RENDTYPE_CROP_SEL,	// do a crop render using the bounding rectangle of the selection
	RENDTYPE_BAKE_SEL,	// bake textures on selected objects
	RENDTYPE_BAKE_ALL,	// bake textures on all objects
	};
#endif

// mjm - begin - 05.26.00
// used to define distinct sets of rendering parameters
enum RenderSettingID
{
	RS_Production = 0,
	RS_Draft,
	RS_IReshade,
	RS_MEdit		// [dl | 15may2003]
};
// mjm - end

// This type describes the clone type when cloning nodes
enum CloneType { NODE_COPY, NODE_INSTANCE, NODE_REFERENCE };

// values for dupAction, passed into MergeFromFile
#define MERGE_DUPS_PROMPT 0    // prompt user when duplicate node names encountered
#define MERGE_DUPS_MERGE  1	   // merge new, keep old when duplicate node names encountered
#define MERGE_DUPS_SKIP   2    // skip duplicates
#define MERGE_DUPS_DELOLD 3    // delete old when duplicate node names encountered
#define MERGE_LIST_NAMES  4    // Causes a list of objects in the file to be 
							   // placed into mrgList. No actual merging will be done. mergeAll must be TRUE.
// added for r6.0 - 8/5/03
// values for dupMtlAction, passed into MergeFromFile
#define MERGE_DUP_MTL_PROMPT      0    // prompt user when duplicate materials encountered
#define MERGE_DUP_MTL_USE_MERGED  MERGE_DUPS_DELOLD	   // use merged material
#define MERGE_DUP_MTL_USE_SCENE   MERGE_DUPS_SKIP    // use scene material
#define MERGE_DUP_MTL_RENAME      1000    // auto-rename merged material

// values for reparentAction, passed into MergeFromFile
#define MERGE_REPARENT_PROMPT     0    // prompt user when reparenting
#define MERGE_REPARENT_ALWAYS     1    // always reparent
#define MERGE_REPARENT_NEVER      2    // never reparent

// CCJ: Property Set definitions
// For internal reasons these must be bitflags
#define PROPSET_SUMMARYINFO		0x01
#define PROPSET_DOCSUMMARYINFO	0x02
#define PROPSET_USERDEFINED		0x04

// Interface::Execute cmd values
#define  I_EXEC_ACTIVATE_TEXTURE    1  //	arg1:  Texmap *tx;	arg2:  Mtl *mtl; arg3:  int subnum;
#define  I_EXEC_DEACTIVATE_TEXTURE  2  //	arg1:  Texmap *tx;	arg2:  Mtl *mtl; arg3:  int subnum;
#define  I_EXEC_RENDER_MTL_SAMPLE 3    //   arg1:  MtlBase *mb; arg2: int size; arg3: UBYTE *image;  returns 0:fail, 1: success
#define  I_EXEC_GET_VPDISPLAY_DIB 4    // 	arg1: Texmap *tx; arg2: TimeValue t; arg3: TexHandleMaker *thm; arg4: BOOL mono; 
										//   arg5: int forceW; arg6: int forceH --	returns *BITMAPINFO

// added as complement to Interface::GetDir(), 020919  --prs.
#define  I_EXEC_SET_DIR 10             //   arg1:  int which; arg2: TCHAR *dir;

#ifdef ACAD_MAXSCRIPT_EXTENSIONS
#define  I_EXEC_OFFSET_SPLINE   80     //   arg1:  INode *spline; arg2: float amount;
#define  I_EXEC_OFFSET_MEASURE  81     //   arg1:  INode *spline; arg2: Point3 *point; arg3: float *result;
#define  I_EXEC_TRIM_EXTEND     82     //   arg1:  INodeTab *all; arg2: INodeTab *act;
//#define  I_EXEC_REG_VIEWWINDOW2 85     //   arg1:  ViewWindow2* (deprecated)
#endif

#ifdef DESIGN_VER
#define  I_EXEC_SET_DISABLED_COLOR 84  //   arg1:  COLORREF 
#endif

//SS 3/11/2002: Added to max to support Substitute modifier
#define  I_EXEC_NEW_OBJ_XREF_DLG 83    //   arg1:  INodeTab *nodes; arg2: BOOL forceSingle;
#define	 I_EXEC_MODIFYTASK_INVALIDATEPANEL 86 //No Args

// russom - 08/27/03 - ECO 1146
#define I_EXEC_INVALIDATE_VIEWEXP 87	// arg1: ViewExp *vp -- set invalidate flag on ViewExp*

#define I_EXEC_COUNT_MTL_SCENEREFS  0x2001 // arg1 Mtl *mt: returns number of refs from scene 

// Interface::Execute return values
#ifdef ACAD_MAXSCRIPT_EXTENSIONS
#define I_EXEC_RET_NULL_NODE    1
#define I_EXEC_RET_NULL_OBJECT  2
#define I_EXEC_RET_NOT_SPLINE   3
#endif
#define I_EXEC_RET_OFFSET_FAIL  4

class NameMaker {
	public:
		virtual void MakeUniqueName(TSTR &name)=0;
		virtual void AddName(TSTR &name) = 0;
        virtual BOOL NameExists(TSTR& name) = 0;
        virtual ~NameMaker() {}
	};


//JH 05/06/98 
// VIEW_OTHER must be last, since "other" types are then numbered consecutively!!!
// And the order can't be changed, or old MAX files won't load properly DB 11/98
enum ViewType { VIEW_LEFT,VIEW_RIGHT,VIEW_TOP,VIEW_BOTTOM,VIEW_FRONT,VIEW_BACK, 
	VIEW_ISO_USER, VIEW_PERSP_USER, VIEW_CAMERA, VIEW_GRID, VIEW_NONE, VIEW_TRACK, 
	VIEW_SPOT, VIEW_SHAPE, VIEW_SCHEMATIC, VIEW_RENDER, VIEW_OTHER};


// class for registering a window that can appear in a MAX viewport DB 10/6/98
class ViewWindow : public InterfaceServer {
public:
	virtual TCHAR *GetName()=0;
	virtual HWND CreateViewWindow(HWND hParent, int x, int y, int w, int h)=0;
	virtual void DestroyViewWindow(HWND hWnd)=0;
	// CanCreate can be overridden to return FALSE if a ViewWindow can only have
	// a single instance, and that instance is already present.  If CanCreate
	// returns FALSE, then the menu item for this ViewWindow will be grayed out.
	virtual BOOL CanCreate() { return TRUE; }
	// mep 7/12/99
	// NumberCanCreate() returns the number of instances of a given window that
	// can be created. This allows, for example, the UI to know without physically
	// creating a window that only a limited number of windows of that type can
	// be created. A -1 implies that the count is not known.
	virtual int NumberCanCreate() { return -1; }
};

// class for accessing the TrackBar (the Mini TrackView)
#define TRACKBAR_FILTER_ALL			1
#define TRACKBAR_FILTER_TMONLY		2
#define TRACKBAR_FILTER_CURRENTTM	3
#define TRACKBAR_FILTER_OBJECT		4
#define TRACKBAR_FILTER_MATERIAL	5

#define TRACKBAR_INTERFACE Interface_ID(0x2aff3557, 0x16aa714b)

class ITrackBar : public FPMixinInterface {
public:
	virtual void		SetVisible(BOOL bVisible) = 0;
	virtual BOOL		IsVisible() = 0;
	virtual void		SetFilter(UINT nFilter) = 0;
	virtual UINT		GetFilter() = 0;
	virtual TimeValue	GetNextKey(TimeValue tStart, BOOL bForward) = 0;
	virtual void		RedrawTrackBar(bool bForce = false) = 0;
	virtual void		SetShowFrames(bool b) = 0;
	virtual bool		GetShowFrames() = 0;
	virtual void		SetShowAudio(bool b) = 0;
	virtual bool		GetShowAudio() = 0;
	virtual void		SetShowSelectionRange(bool b) = 0;
	virtual bool		GetShowSelectionRange() = 0;
	virtual void		SetSnapToFrames(bool b) = 0;
	virtual bool		GetSnapToFrames() = 0;
	virtual void		SetKeyTransparency(int xp) = 0;
	virtual int			GetKeyTransparency() = 0;
	virtual void		SetSelKeyTransparency(int xp) = 0;
	virtual int			GetSelKeyTransparency() = 0;
	virtual void		SetCursorTransparency(int xp) = 0;
	virtual int			GetCursorTransparency() = 0;
	
	//Function ID's for Function Publishing -- Added by AF (09/27/00)
	enum { fIdgetVisible, fIdsetVisible, fIdgetFilter, fIdsetFilter, fIdgetNextKeyTime, fIdgetPreviousKeyTime, fIdredrawTrackBar,
		fIdgetShowFrames, fIdsetShowFrames, fIdgetShowAudio, fIdsetShowAudio, fIdgetShowSelectionRange, fIdsetShowSelectionRange,
		fIdgetSnapToFrames, fIdsetSnapToFrames, fIdgetKeyTransparency, fIdsetKeyTransparency, 
		fIdgetSelKeyTransparency, fIdsetSelKeyTransparency, fIdgetCursorTransparency, fIdsetCursorTransparency,
	};
	//symbolic enum ID for Function Publishing
	enum {enumIDfilterType,
	};
	//Function Publishing methods added by AF (9/27/00)
	BaseInterface* GetInterface(Interface_ID id) { return (id == TRACKBAR_INTERFACE) ? this : FPMixinInterface::GetInterface(id); }
	FPInterfaceDesc* GetDesc() { return (FPInterfaceDesc*)GetCOREInterface(TRACKBAR_INTERFACE); }

	// dispatch map for FP-published functions
	BEGIN_FUNCTION_MAP
	VFN_1(fIdredrawTrackBar, RedrawTrackBar, TYPE_bool);
	FNT_0(fIdgetNextKeyTime, TYPE_TIMEVALUE, fpGetNextKey);
	FNT_0(fIdgetPreviousKeyTime, TYPE_TIMEVALUE, fpGetPreviousKey);
	PROP_FNS(fIdgetVisible, IsVisible, fIdsetVisible, SetVisible, TYPE_BOOL);
	PROP_FNS(fIdgetFilter, GetFilter, fIdsetFilter, SetFilter, TYPE_ENUM);
	PROP_FNS(fIdgetShowFrames, GetShowFrames, fIdsetShowFrames, SetShowFrames, TYPE_bool);
	PROP_FNS(fIdgetShowAudio, GetShowAudio, fIdsetShowAudio, SetShowAudio, TYPE_bool);
	PROP_FNS(fIdgetShowSelectionRange, GetShowSelectionRange, fIdsetShowSelectionRange, SetShowSelectionRange, TYPE_bool);
	PROP_FNS(fIdgetSnapToFrames, GetSnapToFrames, fIdsetSnapToFrames, SetSnapToFrames, TYPE_bool);
	PROP_FNS(fIdgetKeyTransparency, GetKeyTransparency, fIdsetKeyTransparency, SetKeyTransparency, TYPE_INT);
	PROP_FNS(fIdgetSelKeyTransparency, GetSelKeyTransparency, fIdsetSelKeyTransparency, SetSelKeyTransparency, TYPE_INT);
	PROP_FNS(fIdgetCursorTransparency, GetCursorTransparency, fIdsetCursorTransparency, SetCursorTransparency, TYPE_INT);
	END_FUNCTION_MAP 

private:
	//Private methods for Maxscript exposure -- Added by AF (09/27/00)
	TimeValue	fpGetNextKey(TimeValue t) {return GetNextKey(t, TRUE);}
	TimeValue	fpGetPreviousKey(TimeValue t) { return GetNextKey(t, FALSE);}

	};

// This class provides functions that expose the portions of View3D
// that are exported for use by plug-ins.
class ViewExp : public InterfaceServer {
	public:
		virtual Point3 GetPointOnCP(const IPoint2 &ps)=0;
		virtual Point3 SnapPoint(const IPoint2 &in, IPoint2 &out, Matrix3 *plane2d = NULL, DWORD flags = 0)=0;
#ifdef _OSNAP
		virtual void SnapPreview(const IPoint2 &in, IPoint2 &out, Matrix3 *plane2d = NULL, DWORD flags = 0)=0;
		virtual void GetGridDims(float *MinX, float *MaxX, float *MinY, float *MaxY) = 0;
#endif							  
		virtual float SnapLength(float in)=0;
		virtual float GetCPDisp(const Point3 base, const Point3& dir, 
                        const IPoint2& sp1, const IPoint2& sp2, BOOL snap = FALSE )=0;
		virtual GraphicsWindow *getGW()=0;
		virtual int IsWire()=0;
		virtual Rect GetDammageRect()=0;		

		virtual Point3 MapScreenToView( IPoint2& sp, float depth )=0;
		virtual void MapScreenToWorldRay(float sx, float sy, Ray& ray)=0;

		// set the affine tm for the view and ret TRUE if the view is ISO_USER or PERSP_USER
		// else do nothing and return FALSE
		virtual BOOL SetAffineTM(const Matrix3& m)=0;
		virtual void GetAffineTM( Matrix3& tm )=0;
		virtual int GetViewType() = 0;
		virtual BOOL IsPerspView()=0;
		virtual float GetFOV()=0;
		virtual float GetFocalDist()=0;
		virtual float GetScreenScaleFactor(const Point3 worldPoint)=0;

		// return the viewPort screen width factor in world space at 
		// a point in world space
		virtual float GetVPWorldWidth(const Point3 wPoint)=0;
		virtual Point3 MapCPToWorld(const Point3 cpPoint)=0;
		virtual void GetConstructionTM( Matrix3 &tm )=0;
		virtual void SetGridSize( float size )=0;
		virtual float GetGridSize()=0;
		virtual BOOL IsGridVisible()=0;
		virtual int GetGridType()=0;

		// Get the camera if there is one.
		virtual INode *GetViewCamera()=0;

		// Set this viewport to a camera view
		virtual void SetViewCamera(INode *camNode)=0;

		// Set this viewport to a user view 
		virtual void SetViewUser(BOOL persp)=0;

		// Get the spot if there is one
		virtual INode *GetViewSpot()=0;

		// Set this viewport to a spotlight view
		virtual void SetViewSpot(INode *spotNode)=0;

		// node level hit-testing
		virtual void ClearHitList()=0;
		virtual INode *GetClosestHit()=0;
		virtual INode *GetHit(int i)=0;
		virtual int HitCount()=0;
		// subobject level hit-testing
		virtual	void LogHit(INode *nr, ModContext *mc, DWORD dist, ulong info, HitData *hitdat = NULL)=0;		
		virtual HitLog&	GetSubObjHitList()=0;
		virtual void ClearSubObjHitList()=0;
		virtual int NumSubObjHits()=0;

		// For controller apparatus hit testing
		virtual void CtrlLogHit(INode *nr,DWORD dist,ulong info,DWORD infoExtra)=0;
		virtual CtrlHitLog&	GetCtrlHitList()=0;
		virtual void ClearCtrlHitList()=0;
		
		virtual float NonScalingObjectSize()=0;  // 1.0 is "default"

		// Turn on and off image background display
		virtual BOOL setBkgImageDsp(BOOL onOff)=0;
		virtual int	getBkgImageDsp(void)=0;		

		// Turn on and off safe frame display
		virtual void setSFDisplay(int onOff)=0;
		virtual int getSFDisplay(void)=0;

		// This is the window handle of the viewport. This is the
		// same window handle past to GetViewport() to get a ViewExp*
		virtual HWND GetHWnd()=0;

		// Test if the viewport is active
		virtual	BOOL IsActive() = 0;
		// Test if the viewport is enabled
		virtual	BOOL IsEnabled() = 0;

		//methods for floating grids
		virtual void TrackImplicitGrid(IPoint2 m, Matrix3* mat = NULL, ULONG hitTestFlags = 0) = 0;
		virtual void CommitImplicitGrid(IPoint2 m, int mouseflags, Matrix3* mat = NULL) = 0;
		virtual void ReleaseImplicitGrid() = 0;

		// Generic expansion function
		virtual INT_PTR_MSVC70 Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; } 
	};


// return values for CreateMouseCallBack
#define CREATE_CONTINUE 1
#define CREATE_STOP	0	    // creation terminated normally
#define CREATE_ABORT -1		// delete the created object and node

//JH 5/15/01
//return values for MouseCallback
//Historically this method just returned 0 or 1 to indicate if processing should continue
//Now we'll add a return value to indicate that the Mouse proc requires window capture
#define MOUSE_STOP	0
#define MOUSE_CONTINUE	1
#define MOUSE_CAPTURE_CONTINUE 2

// This is a specific call-back proc for handling the creation process
// of a plug-in object.
// The vpt passed in will have had setTransform called with the 
// transform of the current construction plane.
class CreateMouseCallBack {
	public:
		virtual int proc( 
			ViewExp *vpt,
			int msg, 
			int point, 
			int flags, 
			IPoint2 m,
			Matrix3& mat
			)=0;
		virtual int override(int mode) { return mode; }	// Defaults to mode supplied

        // Tells the system that we aren't starting a new creation
        virtual BOOL StartNewCreation() { return TRUE; }

		//Tells the system if ortho mode makes sense for this creation
		//In general this won't be true but splines and such...
		virtual BOOL TolerateOrthoMode() {	return FALSE; }
	};


class Object;
class ConstObject;
class MouseCallBack;
class IObjCreate;
class IObjParam;
class ModContext;
class ModContextList;
class INodeTab;

// Passed to getBkgFrameRange()
#define VIEWPORT_BKG_START			0
#define VIEWPORT_BKG_END			1

// Passed to setBkgORType().
#define VIEWPORT_BKG_BLANK			0
#define VIEWPORT_BKG_HOLD			1
#define VIEWPORT_BKG_LOOP			2

// Passed to setBkgImageAspect()
#define VIEWPORT_BKG_ASPECT_VIEW	0
#define VIEWPORT_BKG_ASPECT_BITMAP	1
#define VIEWPORT_BKG_ASPECT_OUTPUT	2


// Identify the transform tool buttons
#define MOVE_BUTTON		1
#define ROTATE_BUTTON	2
#define NUSCALE_BUTTON	3
#define USCALE_BUTTON	4
#define SQUASH_BUTTON	5
#define SELECT_BUTTON	6

// Axis constraints.
#define AXIS_XY		2
#define AXIS_ZX		1
#define AXIS_YZ		0
#define AXIS_X		3
#define AXIS_Y		4
#define AXIS_Z		5

// Origin modes		
#define ORIGIN_LOCAL		0	// Object's pivot
#define ORIGIN_SELECTION	1	// Center of selection set (or center of individual object for local or parent space)
#define ORIGIN_SYSTEM		2	// Center of the reference coord. system

// Reference coordinate system
#define COORDS_HYBRID	0
#define COORDS_SCREEN	1
#define COORDS_WORLD	2
#define COORDS_PARENT	3
#define COORDS_LOCAL	4
#define COORDS_OBJECT	5
#define COORDS_GIMBAL	6

// Task Modes
#define TASK_MODE_CREATE		1
#define TASK_MODE_MODIFY		2
#define TASK_MODE_HIERARCHY		3
#define TASK_MODE_MOTION		4
#define TASK_MODE_DISPLAY		5
#define TASK_MODE_UTILITY		6

// Max cursors
#define SYSCUR_MOVE			1
#define SYSCUR_ROTATE		2
#define SYSCUR_USCALE		3
#define SYSCUR_NUSCALE		4
#define SYSCUR_SQUASH		5
#define SYSCUR_SELECT		6
#define SYSCUR_DEFARROW		7
#define SYSCUR_MOVE_SNAP	8

// flags to pass to RedrawViews
#define REDRAW_BEGIN		(1<<0)
#define REDRAW_INTERACTIVE	(1<<1)
#define REDRAW_END			(1<<2)
#define REDRAW_NORMAL		(1<<3)

// Return values for GetNumAxis()
#define NUMAXIS_ZERO		0 	// Nothing to transform
#define NUMAXIS_ALL			1	// Use only one axis.
#define NUMAXIS_INDIVIDUAL	2	// Do all, one at a time

// MAX Directories
#define APP_FONT_DIR	 	         0
#define APP_SCENE_DIR		      1
#define APP_IMPORT_DIR		      2
#define APP_EXPORT_DIR		      3
#define APP_HELP_DIR		         4
#define APP_EXPRESSION_DIR	      5
#define APP_PREVIEW_DIR		      6
#define APP_IMAGE_DIR		      7
#define APP_SOUND_DIR		      8
#define APP_PLUGCFG_DIR		      9
#define APP_MAXSTART_DIR	      10
#define APP_VPOST_DIR		      11
#define APP_DRIVERS_DIR		      12
#define APP_AUTOBACK_DIR	      13
#define APP_MATLIB_DIR		      14
#define APP_SCRIPTS_DIR		      15
#define APP_STARTUPSCRIPTS_DIR   16
#define APP_MARKETDEFAULTS_DIR    17
#define APP_RENDER_PRESETS_DIR   18
#ifndef RENDER_VER_CONFIG_PATHS // xavier robitaille | 03.01.24 | modify kahn's config. paths
#define APP_UI_DIR			      19	// this must be next to last!!
#define APP_MAXROOT_DIR		      20	// this must be last!!
#else
#define APP_PLUGCFG_CATALOGS_DIR 19
#define APP_UI_DIR			      20	// this must be next to last!!
#define APP_MAXROOT_DIR		      21	// this must be last!!
#endif // RENDER_VER_CONFIG_PATHS



// Types for status numbers
#define STATUS_UNIVERSE					1
#define STATUS_SCALE					2
#define STATUS_ANGLE					3
#define STATUS_OTHER					4
#define STATUS_UNIVERSE_RELATIVE		5
#define STATUS_POLAR					6
#define STATUS_POLAR_RELATIVE			7

// Extended display modes
#define EXT_DISP_NONE				0
#define EXT_DISP_SELECTED			(1<<0)		// object is selected
#define EXT_DISP_TARGET_SELECTED	(1<<1)		// object's target is selected
#define EXT_DISP_LOOKAT_SELECTED	(1<<2)		// object's lookat node is selected
#define EXT_DISP_ONLY_SELECTED		(1<<3)		// object is only thing selected
#define EXT_DISP_DRAGGING			(1<<4)		// object is being "dragged"
#define EXT_DISP_ZOOM_EXT			(1<<5)		// object is being tested for zoom ext
#define EXT_DISP_GROUP_EXT			(1<<6)		// object is being tested for extents as member of group

// Render time types passed to SetRendTimeType()
#define REND_TIMESINGLE		0
#define REND_TIMESEGMENT	1
#define REND_TIMERANGE		2
#define REND_TIMEPICKUP		3

// Flag bits for hide by category.
#define HIDE_OBJECTS	0x0001
#define HIDE_SHAPES		0x0002
#define HIDE_LIGHTS		0x0004
#define HIDE_CAMERAS	0x0008
#define HIDE_HELPERS	0x0010
#define HIDE_WSMS		0x0020
#define HIDE_SYSTEMS	0x0040
#define HIDE_PARTICLES	0x0080
#define HIDE_BONEOBJECTS 0x0100
#define HIDE_ALL		0xffff
#define HIDE_NONE		0



// viewport layout configuration
//   VP_LAYOUT_ LEGEND
//		# is number of viewports (total) in view panel
//		V = vertical split
//		H = horizontal split
//		L/R	= left/right placement
//		T/B = top/bottom placement
//   CONSTANT LEGEND
//		bottom nibble is total number of views
#define VP_LAYOUT_1			0x0001
#define VP_LAYOUT_2V		0x0012
#define VP_LAYOUT_2H		0x0022
#define VP_LAYOUT_2HT		0x0032
#define VP_LAYOUT_2HB		0x0042
#define VP_LAYOUT_3VL		0x0033
#define VP_LAYOUT_3VR		0x0043
#define VP_LAYOUT_3HT		0x0053
#define VP_LAYOUT_3HB		0x0063
#define VP_LAYOUT_4			0x0074
#define VP_LAYOUT_4VL		0x0084
#define VP_LAYOUT_4VR		0x0094
#define VP_LAYOUT_4HT		0x00a4
#define VP_LAYOUT_4HB		0x00b4
#define VP_LAYOUT_1C		0x00c1
#define VP_NUM_VIEWS_MASK	0x000f


class DWORDTab : public Tab<DWORD> {};


// A callback object passed to Execute
#define  I_EXEC_REGISTER_POSTSAVE_CB  1001
#define  I_EXEC_UNREGISTER_POSTSAVE_CB  1002
#define  I_EXEC_REGISTER_PRESAVE_CB  1003
#define  I_EXEC_UNREGISTER_PRESAVE_CB  1004

class GenericCallback {
 public:
  virtual void Callme()=0;
 };

// A callback object passed to RegisterTimeChangeCallback()
class TimeChangeCallback {
	public:
		virtual void TimeChanged(TimeValue t)=0;
	};


// A callback object passed to RegisterCommandModeChangeCallback()
class CommandModeChangedCallback {
	public:
		virtual void ModeChanged(CommandMode *oldM, CommandMode *newM)=0;
	};

// A callback to allow plug-ins that aren't actually objects (such as utilities)
// to draw something in the viewports.
class ViewportDisplayCallback {
	public:
		virtual void Display(TimeValue t, ViewExp *vpt, int flags)=0;		
		virtual void GetViewportRect( TimeValue t, ViewExp *vpt, Rect *rect )=0;
		virtual BOOL Foreground()=0; // return TRUE if the object changes a lot or FALSE if it doesn't change much		
	};

// A callback object that will get called before the program exits.
class ExitMAXCallback {
	public:
		// MAX main window handle is passed in. Return FALSE to abort
		// the exit, TRUE otherwise.
		virtual BOOL Exit(HWND hWnd)=0;
	};

class MAXFileOpenDialog {
	public:
	virtual BOOL BrowseMAXFileOpen(TSTR& fileName, TSTR* defDir, TSTR* defFile) = 0;
	};

class MAXFileSaveDialog {
	public:
	virtual BOOL BrowseMAXFileSave(TSTR& fileName) = 0;
	};

// A callback object to filter selection in the track view.
class TrackViewFilter : public InterfaceServer{
	public:
		// Return TRUE to accept the anim as selectable.
		virtual BOOL proc(Animatable *anim, Animatable *client, int subNum)=0;
		virtual BOOL TextColor(Animatable *anim, Animatable *client, int subNum, COLORREF& color) { return FALSE; }
	};

// Stores the result of a track view pick
class TrackViewPick {
	public:
		ReferenceTarget *anim;
		ReferenceTarget *client;
		int subNum;

		TrackViewPick() {anim=NULL;client=NULL;subNum=0;}
	};

// A callback object passed to SetPickMode()
class PickModeCallback : public InterfaceServer {
	public:
		// Called when ever the pick mode needs to hit test. Return TRUE if something was hit
		virtual BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)=0;
		
		// Called when the user picks something. The vpt should have the result of the hit test in it.
		// return TRUE to end the pick mode.
		virtual BOOL Pick(IObjParam *ip,ViewExp *vpt)=0;

		//Called when a node or controller is picked from Trackview or Schematic view
		//return TRUE if the Pick Mode callback accepts the animatable object.
		//the track will not be selected in trackview if the this function returns FALSE
		virtual BOOL PickAnimatable(Animatable* anim) { return TRUE; }

		// Called when the user right-clicks or presses ESC
		// return TRUE to end the pick mode, FALSE to continue picking
		virtual BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{ return FALSE; }

		// Called when the mode is entered and exited.
		virtual void EnterMode(IObjParam *ip) {}
		virtual void ExitMode(IObjParam *ip) {}

		virtual HCURSOR GetDefCursor(IObjParam *ip) {return NULL;}
		virtual HCURSOR GetHitCursor(IObjParam *ip) {return NULL;}

		// If the user hits the H key while in your pick mode, you
		// can provide a filter to filter the name list.
		virtual PickNodeCallback *GetFilter() {return NULL;}

		// Return TRUE to allow the user to pick more than one thing.
		// In this case the Pick method may be called more than once.
		virtual BOOL AllowMultiSelect() {return FALSE;}
	};

// Not to be confused with a PickMODEcallback...
// Used to filter node's during a hit test (PickNode)
class PickNodeCallback {
	public:
		// Return TRUE if this is an acceptable hit, FALSE otherwise.
		virtual BOOL Filter(INode *node)=0;
	};

// Used with DoHitByNameDialog();
class HitByNameDlgCallback {
public:
	virtual TCHAR *dialogTitle()	{ return _T(""); }
	virtual TCHAR *buttonText() 	{ return _T(""); }
	virtual BOOL singleSelect()		{ return FALSE; }
	virtual BOOL useFilter()		{ return TRUE; }
	virtual int filter(INode *node)	{ return TRUE; }
	virtual BOOL useProc()			{ return TRUE; }
	virtual void proc(INodeTab &nodeTab) {}
	virtual BOOL doCustomHilite()	{ return FALSE; }
	virtual BOOL doHilite(INode *node)	{ return FALSE; }
	virtual BOOL showHiddenAndFrozen()	{ return FALSE; }
};


//watje new selection filters
//this is a call back for the selection filter drop down in the tab panel
//it allows plugins and maxscripts to add additional filters to this list
class SelectFilterCallback : public InterfaceServer
{
public:
	virtual TCHAR* GetName() = 0;  // the name that will appear in the drop down list when it is registered
	//the actual proc that does the filtering
	// the node, class id, and super class id are passed as parameter to check the filter again
	//return FALSE if the node is not selectable, true if it is selectable
	virtual BOOL IsFiltered(SClass_ID sid, Class_ID cid, INode *node) = 0;
};


//watje new display filters
//this is a call back for the display filter list in the display command panel
//it allows plugins and maxscripts to add additional filters to this list
class DisplayFilterCallback : public InterfaceServer
{
public:
	BOOL on;
	virtual TCHAR* GetName() = 0;  // the name that will appear in the drop down list when it is registered
	//the actual proc that does the filtering
	// the node, class id, and super class id are passed as parameter to check the filter again
	//return FALSE if the node is not selectable, true if it is selectable
	virtual BOOL IsVisible(SClass_ID sid, Class_ID cid, INode *node) = 0;
};


class Interface;

// A callback used with RegisterRedrawViewsCallback()
class RedrawViewsCallback {
	public:
		// this will be called after all the viewport have completed drawing.
		virtual void proc(Interface *ip)=0;
	};

// A callback used with RegisterAxisChangeCallback()
class AxisChangeCallback {
	public:
		// this will be called when the axis system is changed
		virtual void proc(Interface *ip)=0;
	};

// parameters for creation of a preview
class PreviewParams {
public:
	BOOL	outputType;	// 0=default AVI codec, 1=user picks file, 2=user picks device
	int		pctSize;	// percent (0-100) of current rendering output res
	// frame limits
	int		start;
	int		end;
	int		skip;
	// frame rate
	int		fps;
	// display control
	BOOL	dspGeometry;
	BOOL	dspShapes;
	BOOL	dspLights;
	BOOL	dspCameras;
	BOOL	dspHelpers;
	BOOL	dspSpaceWarps;
	BOOL	dspGrid;
	BOOL	dspSafeFrame;
	BOOL	dspFrameNums;
	// rendering level
	int		rndLevel;	// 0=smooth/hi, 1=smooth, 2=facet/hi, 3=facet
						// 4=lit wire, 6=wire, 7=box
	int		dspBkg;
};

// viewport transparency rendering modes
#define VPT_TRANS_NONE			0
#define VPT_TRANS_STIPPLE		1
#define VPT_TRANS_BLEND			2
#define VPT_TRANS_SORT_BLEND	3		// not supported (yet?)

#define MAIN_MAX_INTERFACE Interface_ID(0x64854123, 0x7b9e551c)

// Generic interface into Jaguar
class Interface : public FPStaticInterface {
	public:
		virtual HFONT GetAppHFont()=0;
		virtual void RedrawViews(TimeValue t,DWORD vpFlags=REDRAW_NORMAL, ReferenceTarget *change=NULL)=0;		
		virtual BOOL SetActiveViewport(HWND hwnd)=0;
		virtual ViewExp *GetActiveViewport()=0; // remember to release ViewExp* with ReleaseViewport()
		virtual IObjCreate *GetIObjCreate()=0;
		virtual IObjParam *GetIObjParam()=0;
		virtual HWND GetMAXHWnd()=0;

		// This will cause all viewports to be completely redrawn.
		// This is extremely heavy handed and should only be used when
		// necessary.
		virtual void ForceCompleteRedraw(BOOL doDisabled=TRUE)=0;

		// This will draw the active view (if it is a camera view with a multi-pass effect
		// enabled) with that effect active.  Returns TRUE if the active view is a camera view 
		// with a multi-pass effect enabled, FALSE otherwise
		virtual BOOL DisplayActiveCameraViewWithMultiPassEffect() = 0;

		// Determines the way transparency will be displayed in the active viewport, based on transType setting
		//		VPT_TRANS_NONE => all objects are always opaque
		//		VPT_TRANS_STIPPLE => use screendoor transparency
		//		VPT_TRANS_BLEND => use hardware blending, based on drawing transparent objects last
		virtual BOOL SetActiveViewportTransparencyDisplay(int transType) = 0;

		// Turn scene redraw on/off.  Each call to disable redraw should be
		// matched by a call to enable it, since this is implemented with an
		// internal counter.  This can be used to stop scene redraws from 
		// happening during rendering, for example.
		virtual void DisableSceneRedraw()=0;
		virtual void EnableSceneRedraw()=0;
		virtual int	 IsSceneRedrawDisabled()=0;	// returns non-zero if disabled

		// Register a call back object that gets called evrytime
		// the viewports are redrawn.
		virtual void RegisterRedrawViewsCallback(RedrawViewsCallback *cb)=0;
		virtual void UnRegisterRedrawViewsCallback(RedrawViewsCallback *cb)=0;
//watje new selection filters
//these work just like all the other register callback function
//except they are used to register filter selection call backs
		virtual void RegisterSelectFilterCallback(SelectFilterCallback *cb)=0;
		virtual void UnRegisterSelectFilterCallback(SelectFilterCallback *cb)=0;

//watje new display filters
//these work just like all the other register callback function
//except they are used to register display selection call backs
		virtual void RegisterDisplayFilterCallback(DisplayFilterCallback *cb)=0;
		virtual void UnRegisterDisplayFilterCallback(DisplayFilterCallback *cb)=0;

		// set the selection type to automatic window/crossing (based on dir, as set by 2nd arg), 
		// or to manual window/crossing (based on second arg)
		//
		// if autoWinCross is TRUE, then Window/Crossing is chosen by direction
		//		if winCrossOrAutoDir is set to AC_DIR_RL_CROSS, then right-to-left implies crossing
		//		if winCrossOrAutoDir is set to AC_DIR_LR_CROSS, then left-to-right implies crossing
		// if autoWinCross is FALSE, then Window/Crossing is set explicitly, as follows:
		//		if winCrossOrAutoDir is TRUE, then you get crossing selection
		//		if winCrossOrAutoDir is FALSE, then you get window selection
		virtual BOOL SetSelectionType(BOOL autoWinCross, int winCrossOrAutoDir) = 0;

		// For use with extended views: 
		// - make the extended viewport active (set on mouse click, for example)
		// - put up the view type popup menu (put up on right-click, for example)
		virtual void MakeExtendedViewportActive(HWND hWnd)=0;
		virtual void PutUpViewMenu(HWND hWnd, POINT pt)=0;

		// Execute a track view pick dialog.
		virtual BOOL TrackViewPickDlg(HWND hParent, TrackViewPick *res, TrackViewFilter *filter=NULL, DWORD flags=0)=0;
//watje
		virtual BOOL TrackViewPickMultiDlg(HWND hParent, Tab<TrackViewPick> *res, TrackViewFilter *filter=NULL,DWORD flags=0)=0;

		// Command mode methods
		virtual void PushCommandMode( CommandMode *m )=0;
		virtual void SetCommandMode( CommandMode *m )=0;
		virtual void PopCommandMode()=0;		
		virtual CommandMode* GetCommandMode()=0;
		virtual void SetStdCommandMode( int cid )=0;
		virtual void PushStdCommandMode( int cid )=0;		
		virtual void RemoveMode( CommandMode *m )=0;
		virtual void DeleteMode( CommandMode *m )=0;

		// This will set the command mode to a standard pick mode.
		// The callback implements hit testing and a method that is
		// called when the user actually pick an item.
		virtual void SetPickMode(PickModeCallback *pc)=0;
		
		// makes sure no pick modes are in the command stack.
		virtual void ClearPickMode()=0;

		//added by AF (08/18/2000)
		//if a Pick Mode is on top of the command stack this function will return it 
		//otherwise it returns NULL
		virtual PickModeCallback* GetCurPickMode()=0;						

		// Puts up a hit by name dialog. If the callback is NULL it 
		// just does a standard select by name.
		// returns TRUE if the user OKs the dialog, FALSE otherwise.
		virtual BOOL DoHitByNameDialog(HitByNameDlgCallback *hbncb=NULL)=0;

		// status panel prompt stuff
		virtual void PushPrompt( TCHAR *s )=0;
		virtual void PopPrompt()=0;
		virtual void ReplacePrompt( TCHAR *s )=0;
		virtual void DisplayTempPrompt( TCHAR *s, int msec=1000)=0;
		virtual void RemoveTempPrompt()=0;

		// put up a directory choose dialog
		// hWnd = parent
		// title is dialog box title
		// dir is return value for chosen dir (empty on cancel)
		// desc, if non-null, puts up a description field and returns new desc.
		virtual void ChooseDirectory(HWND hWnd, TCHAR *title, TCHAR *dir, TCHAR *desc=NULL)=0;

		// auto-backup control -- times are in minutes
		virtual float GetAutoBackupTime()=0;
		virtual void SetAutoBackupTime(float minutes)=0;
		virtual BOOL AutoBackupEnabled()=0;
		virtual void EnableAutoBackup(BOOL onOff)=0;

		// status panel progress bar
		virtual BOOL ProgressStart(TCHAR *title, BOOL dispBar, LPTHREAD_START_ROUTINE fn, LPVOID arg)=0;
		virtual void ProgressUpdate(int pct, BOOL showPct = TRUE, TCHAR *title = NULL)=0;
		virtual void ProgressEnd()=0;
		virtual BOOL GetCancel()=0;
		virtual void SetCancel(BOOL sw)=0;

		// create preview from active view.
		// If pvp is NULL, this uses the parameters from the preview rendering dialog box.
		virtual void CreatePreview(PreviewParams *pvp=NULL)=0;
		
		// Some info about the current grid settings
		virtual float GetGridSpacing()=0;
		virtual int GetGridMajorLines()=0;

		// Write values to x,y,z status boxes. Before doing this, mouse
		// tracking must be disabled. Typically a plug-in would disable
		// mouse tracking on mouse down and enable it on mouse up.		
		virtual void DisableStatusXYZ()=0;
		virtual void EnableStatusXYZ()=0;
		virtual void SetStatusXYZ(Point3 xyz,int type)=0;
		virtual void SetStatusXYZ(AngAxis aa)=0; // this will convert the aa for status display

		// Extended display modes (such as camera cones that only appear when dragging a camera)
		virtual void SetExtendedDisplayMode(int flags)=0;
		virtual int GetExtendedDisplayMode()=0;

		// UI flyoff timing
		virtual void SetFlyOffTime(int msecs)=0;
		virtual int  GetFlyOffTime()=0;

		// Get standard Jaguar cursors.
		virtual HCURSOR GetSysCursor( int id )=0;

		// Turn on or off a cross hair cursor which draws horizontal and vertical
		// lines the size of the viewport's width and height and intersect at
		// the mouse position.
		virtual void SetCrossHairCur(BOOL onOff)=0;
		virtual BOOL GetCrossHairCur()=0;

		// This pops all modes above the create or modify mode.
		// NOTE: This is obsolete with the new modifiy panel design.
		virtual void RealizeParamPanel()=0;

		// Snap an angle value (in radians)
		virtual float SnapAngle(float angleIn, BOOL fastSnap=TRUE, BOOL forceSnap=FALSE)=0;

		// Snap a percentage value (1.0 = 100%)
		virtual float SnapPercent(float percentIn)=0;

		// Get the snap switch state
		virtual BOOL GetSnapState()=0;

		// Get the snap type -- Absolute or Relative (grid.h)
		virtual int GetSnapMode()=0;

		// Set the snap mode -- Set to absolute will fail if not in screen space
		// Returns TRUE if succeeded
		virtual BOOL SetSnapMode(int mode)=0;

		// Hit tests the screen position for nodes and returns a 
		// INode pointer if one is hit, NULL otherwise.
		virtual INode *PickNode(HWND hWnd,IPoint2 pt,PickNodeCallback *filt=NULL)=0;

		// Region hit testing. To access the result, use the ViewExp funtions
		// GetClosestHit() or GetHit().		
		virtual void BoxPickNode(ViewExp *vpt,IPoint2 *pt,BOOL crossing,PickNodeCallback *filt=NULL)=0;
		virtual void CirclePickNode(ViewExp *vpt,IPoint2 *pt,BOOL crossing,PickNodeCallback *filt=NULL)=0;
		virtual void FencePickNode(ViewExp *vpt,IPoint2 *pt,BOOL crossing,PickNodeCallback *filt=NULL)=0;

		//----- Modify-related Methods--------------------------

		// Registers the sub-object types for a given plug-in object type.

		// This method is obsolete for the modifiy panel from 3DS MAX 4 on. 
		// All subobject types have to be registered through the new methods 
		// BaseObject::NumSubObjTypes() and BaseObject::GetSubObjType(int i). 
		// This method is still working for motion panel subobject types.
		// Please check the SDK help file for more information.

		virtual void RegisterSubObjectTypes( const TCHAR **types, int count,
                                             int startIndex = 0)=0;

		// Add sub-object named selection sets the named selection set drop down.
		// This should be done whenever the selection level changes.
		virtual void AppendSubObjectNamedSelSet(const TCHAR *set)=0;

		// Clear the named selections from the drop down.
		virtual void ClearSubObjectNamedSelSets()=0;

		// Clears the edit field of the named selection set drop down
		virtual void ClearCurNamedSelSet()=0;

		// Sets the edit field of the named selection set drop down
		virtual void SetCurNamedSelSet(TCHAR *setName)=0;

		// new for V2... tell the system that the named sets have changed at
		// that the drop down needs to be rebuilt.
		virtual void NamedSelSetListChanged()=0;

		// Returns the state of the sub object drop-down. 0 is object level
		// and >= 1 refer to the levels registered by the object.
		virtual int GetSubObjectLevel()=0;
		
		// Sets the sub-object drop down. This will cause the object being edited
		// to receive a notification that the current subobject level has changed.
        // if force == TRUE, the it will set the level even if the current
        // level is the same as the level requested.  This is to support
        // objects that change sub-object levels on the fly, like NURBS
		virtual void SetSubObjectLevel(int level, BOOL force = FALSE)=0;

		// Returns the number of entries in the sub-object drop down list.
		virtual int GetNumSubObjectLevels()=0;

		// Enables or disables sub object selection. Note that it
		// will already be disabled if there are no subobject levels
		// registered. In this case, it can not be enabled.
		virtual void EnableSubObjectSelection(BOOL enable)=0;
		virtual BOOL IsSubObjectSelectionEnabled()=0;

		// Notifies the system that the selection level in the pipeline has chaned.
		virtual void PipeSelLevelChanged()=0;

		// Returns the sub-object selection level at the point in the
		// pipeline  just before the current place in the history.
		virtual void GetPipelineSubObjLevel(DWORDTab &levels)=0;

		// Get's all instance contexts for the modifier at the current
		// place in the history.
		virtual void GetModContexts(ModContextList& list, INodeTab& nodes)=0;

		// Get the object (or modifier) that is currently being edited in the
		// modifier panel
		virtual BaseObject* GetCurEditObject()=0;

		// Hit tests the object currently being edited at the sub object level.
		virtual int SubObHitTest(TimeValue t, int type, int crossing, 
			int flags, IPoint2 *p, ViewExp *vpt)=0;

		// Is the selection set frozen?
		virtual BOOL SelectionFrozen()=0;
		virtual void FreezeSelection()=0;
		virtual void ThawSelection()=0;

		// Nodes in the current selection set.
		virtual INode *GetSelNode(int i)=0;
		virtual int GetSelNodeCount()=0;

		// Enable/disable, get/set show end result. 
		virtual void EnableShowEndResult(BOOL enabled)=0;
		virtual BOOL GetShowEndResult ()=0;
		virtual void SetShowEndResult (BOOL show)=0;

		// Returns the state of the 'crossing' preference for hit testing.
		virtual BOOL GetCrossing()=0;

		// Sets the state of one of the transform tool buttons.
		// TRUE indecates pressed, FALSE is not pressed.
		virtual void SetToolButtonState(int button, BOOL state )=0;
		virtual BOOL GetToolButtonState(int button)=0;
		virtual void EnableToolButton(int button, BOOL enable=TRUE )=0;

        // Enable and disable Undo/Redo.
        virtual void EnableUndo(BOOL enable)=0;

		// Get and set the command panel task mode
		virtual int GetCommandPanelTaskMode()=0;
		virtual void SetCommandPanelTaskMode(int mode)=0;

		// Finds the vpt given the HWND
		virtual ViewExp *GetViewport( HWND hwnd )=0;		
		virtual void ReleaseViewport( ViewExp *vpt )=0;		

		// Disables/Enables animate button
		virtual void EnableAnimateButton(BOOL enable)=0;
		virtual BOOL IsAnimateEnabled()=0;

		// Turns the animate button on or off
		virtual void SetAnimateButtonState(BOOL onOff)=0;

		// In some simpulation task, a time sequence of matrices are computed
		// and to be set to node via
		// INode::SetNodeTM(TimeValue t, Matrix3& tm). If the node
		// employs the standard PRS controller and the rotation is the
		// standard Euler XYZ controller, it would achieve better animation
		// result if we decides euler angles at this frame based on those at
		// the previous frame. The Progressive Mode tells the Euler
		// controller to derive angles based on the previous frame.
		// It is assumed that in this mode, SetNodeTM() are called
		// in strictly forward time order.
		// Synoposis:
		//   GetCOREInterface()->BeginProgressiveMode();
		//   for (t = start_time, t < end_time; t += time_per_frame) {
		//      node.SetNodeTM(t, tm_at_t);
		//   }
		//   GetCOREInterface()->EndProgressiveMode();
		//
		virtual bool InProgressiveMode() =0;
		virtual void BeginProgressiveMode() =0;
		virtual void EndProgressiveMode() =0;

		// Registers a callback that gets called whenever the axis
		// system is changed.
		virtual void RegisterAxisChangeCallback(AxisChangeCallback *cb)=0;
		virtual void UnRegisterAxisChangeCallback(AxisChangeCallback *cb)=0;
		 
		// Gets/Sets the state of the axis constraints.
		virtual int GetAxisConstraints()=0;
		virtual void SetAxisConstraints(int c)=0;
		virtual void EnableAxisConstraints(int c,BOOL enabled)=0;
		// An axis constraint stack
		virtual void PushAxisConstraints(int c) = 0;
		virtual void PopAxisConstraints() = 0;

		// Gets/Sets the state of the coordinate system center
		virtual int GetCoordCenter()=0;
		virtual void SetCoordCenter(int c)=0;
		virtual void EnableCoordCenter(BOOL enabled)=0;

		// Gets/Sets the reference coordinate systems
		virtual int GetRefCoordSys()=0;
		virtual void SetRefCoordSys(int c)=0;
		virtual void EnableRefCoordSys(BOOL enabled)=0;

	// watje Gets/Sets the selection filter
		virtual int GetSelectFilter()=0;
		virtual void SetSelectFilter(int c)=0;
		virtual int GetNumberSelectFilters()=0;
		virtual TCHAR* GetSelectFilterName(int index)=0;

		virtual BOOL GetDisplayFilter(int index) = 0;
		virtual void SetDisplayFilter(int index, BOOL on) = 0;
		virtual int GetNumberDisplayFilters() = 0;
		virtual BOOL DisplayFilterIsNodeVisible(int index, int sid, Class_ID cid, INode *node) = 0;
		virtual TCHAR* GetDisplayFilterName(int index)=0;



		// Gets the axis which define the space in which transforms should
		// take place. 
		// The node and subIndex refer to the object and sub object which the axis
		// system should be based on (this should be the thing the user clicked on)
		// If 'local' is not NULL, it will be set to TRUE if the center of the axis
		// is the pivot point of the node, FALSE otherwise.
		virtual Matrix3 GetTransformAxis(INode *node,int subIndex,BOOL* local = NULL)=0;

		// This returns the number of axis tripods in the scene. When transforming
		// multiple sub-objects, in some cases each sub-object is transformed in
		// a different space.
		// Return Values:
		// NUMAXIS_ZERO			- Nothing to transform
		// NUMAXIS_ALL			- Use only one axis.
		// NUMAXIS_INDIVIDUAL	- Do all, one at a time
		virtual int GetNumAxis()=0;

		// Locks axis tripods so that they will not be updated.
		virtual void LockAxisTripods(BOOL onOff)=0;
		virtual BOOL AxisTripodLocked()=0;

		// Registers a dialog window so IsDlgMesage() gets called for it.
		virtual void RegisterDlgWnd( HWND hDlg )=0;
		virtual int UnRegisterDlgWnd( HWND hDlg )=0;

		// Registers a keyboard accelerator table
        // These functions are obsolete.  Use the AcceleratorTable
        // funciton below to get tables that use the keyboard prefs dialog
		virtual void RegisterAccelTable( HWND hWnd, HACCEL hAccel )=0;
		virtual int UnRegisterAccelTable( HWND hWnd, HACCEL hAccel )=0;

        // The action manager is used to manage ActionTables which
        // plug-ins can use to export operations that can be tied to UI
        // elements like keyboard shortcuts, menus and toolbars.
        virtual IActionManager* GetActionManager() = 0;

        // Returns a pointer to the manager for cusomizable menus
        // see iMenuMan.h for a description of the menu manager
        virtual IMenuManager* GetMenuManager() = 0;
    
		// Adds rollup pages to the command panel. Returns the window
		// handle of the dialog that makes up the page.
		virtual HWND AddRollupPage( HINSTANCE hInst, TCHAR *dlgTemplate, 
				DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0, int category = ROLLUP_CAT_STANDARD )=0;

		virtual HWND AddRollupPage( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, 
				DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0, int category = ROLLUP_CAT_STANDARD )=0;

		// Removes a rollup page and destroys it.
		virtual void DeleteRollupPage( HWND hRollup )=0;

		// Replaces existing rollup with another. (and deletes the old one)
		virtual HWND ReplaceRollupPage( HWND hOldRollup, HINSTANCE hInst, TCHAR *dlgTemplate, 
						DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0, int category = ROLLUP_CAT_STANDARD )=0;
		
		virtual HWND ReplaceRollupPage( HWND hOldRollup, HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, 
						DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0, int category = ROLLUP_CAT_STANDARD )=0;
		
		// Gets a rollup window interface to the command panel rollup
		virtual IRollupWindow *GetCommandPanelRollup()=0;

		// When the user mouses down in dead area, the plug-in should pass
		// mouse messages to this function which will pass them on to the rollup.
		virtual void RollupMouseMessage( HWND hDlg, UINT message, 
				WPARAM wParam, LPARAM lParam )=0;

		// get/set the current time.
		virtual TimeValue GetTime()=0;	
		virtual void SetTime(TimeValue t,BOOL redraw=TRUE)=0;

		// get/set the anim interval.
		virtual Interval GetAnimRange()=0;
		virtual void SetAnimRange(Interval range)=0;

		// Register a callback object that will get called every time the
		// user changes the frame slider.
		virtual void RegisterTimeChangeCallback(TimeChangeCallback *tc)=0;
		virtual void UnRegisterTimeChangeCallback(TimeChangeCallback *tc)=0;

		// Register a callback object that will get called when the user
		// causes the command mode to change
		virtual void RegisterCommandModeChangedCallback(CommandModeChangedCallback *cb)=0;
		virtual void UnRegisterCommandModeChangedCallback(CommandModeChangedCallback *cb)=0;

		// Register a ViewportDisplayCallback
		// If 'preScene' is TRUE then the callback will be called before object are rendered (typically, but not always).
		virtual void RegisterViewportDisplayCallback(BOOL preScene,ViewportDisplayCallback *cb)=0;
		virtual void UnRegisterViewportDisplayCallback(BOOL preScene,ViewportDisplayCallback *cb)=0;
		virtual void NotifyViewportDisplayCallbackChanged(BOOL preScene,ViewportDisplayCallback *cb)=0;

		// Register a ExitMAXCallback
		virtual void RegisterExitMAXCallback(ExitMAXCallback *cb)=0;
		virtual void UnRegisterExitMAXCallback(ExitMAXCallback *cb)=0;

		virtual RightClickMenuManager* GetRightClickMenuManager()=0;

		// Delete key notitfication
		virtual void RegisterDeleteUser(EventUser *user)=0;		// Register & Activate
		virtual void UnRegisterDeleteUser(EventUser *user)=0;	// Deactivate & UnRegister

		//----- Creation-related Methods--------------------------
		
		virtual void MakeNameUnique(TSTR &name)=0;
		virtual INode *CreateObjectNode( Object *obj)=0;		
		virtual GenCamera *CreateCameraObject(int type) = 0;
		virtual Object   *CreateTargetObject() = 0;
		virtual GenLight *CreateLightObject(int type) = 0;
		virtual void *CreateInstance(SClass_ID superID, Class_ID classID)=0;
		virtual int BindToTarget(INode *laNode, INode *targNode)=0;
		virtual int IsCPEdgeOnInView()=0;		
		virtual void DeleteNode(INode *node, BOOL redraw=TRUE, BOOL overrideSlaves=FALSE)=0;
		virtual INode *GetRootNode()=0;
		virtual void NodeInvalidateRect( INode *node )=0;
		virtual void SelectNode( INode *node, int clearSel = 1)=0;
		virtual void DeSelectNode(INode *node)=0;
		virtual void SelectNodeTab(INodeTab &nodes,BOOL sel,BOOL redraw=TRUE)=0;
		virtual void ClearNodeSelection(BOOL redraw=TRUE)=0;
		virtual void AddLightToScene(INode *node)=0; 
		virtual void AddGridToScene(INode *node) = 0;
		virtual void SetNodeTMRelConstPlane(INode *node, Matrix3& mat)=0;
		virtual void SetActiveGrid(INode *node)=0;
		virtual INode *GetActiveGrid()=0;

		// When a plug-in object implements it's own BeginCreate()/EndCreate()
		// it can cause EndCreate() to be called by calling this method.
		virtual void StopCreating()=0;

		// This creates a new object/node with out going throught the usual
		// create mouse proc sequence.
		// The matrix is relative to the contruction plane.
		virtual Object *NonMouseCreate(Matrix3 tm)=0;
		virtual void NonMouseCreateFinish(Matrix3 tm)=0;

		// directories
		virtual TCHAR *GetDir(int which)=0;		// which = APP_XXX_DIR
		virtual int	GetPlugInEntryCount()=0;	// # of entries in PLUGIN.INI
		virtual TCHAR *GetPlugInDesc(int i)=0;	// ith description
		virtual TCHAR *GetPlugInDir(int i)=0;	// ith directory

		// bitmap path
		virtual int GetMapDirCount(BOOL xref=FALSE)=0;			// number of dirs in path
		virtual TCHAR *GetMapDir(int i, BOOL xref=FALSE)=0;		// i'th dir of path
		virtual BOOL AddMapDir(TCHAR *dir, BOOL xref=FALSE)=0;	// add a path to the list

		virtual BOOL AddSessionMapDir(TCHAR *dir, int update=TRUE, BOOL xref=FALSE)=0;
		virtual int GetSessionMapDirCount(BOOL xref=FALSE)=0;
		virtual TCHAR *GetSessionMapDir(int i, BOOL xref=FALSE)=0;
		virtual BOOL DeleteSessionMapDir(int i, int update=TRUE, BOOL xref=FALSE)=0;

		//These methods provide access to the combined list of permanent and temporary (session) map dirs,
		//therefore the current total set of map directories
		virtual int GetCurMapDirCount(BOOL xref=FALSE)=0;
		virtual TCHAR *GetCurMapDir(int i, BOOL xref=FALSE)=0;

		virtual float GetLightConeConstraint()=0;

		// New Method for light exclusion/inclusion lists
		virtual int DoExclusionListDialog(ExclList *nl, BOOL doShadows=TRUE)=0;

		// Convert a NameTab to an ExclList ( the new format for Exclusion lists).
		virtual void ConvertNameTabToExclList(const NameTab *nt, ExclList *excList)=0;
		
		virtual MtlBase *DoMaterialBrowseDlg(HWND hParent,DWORD flags,BOOL &newMat,BOOL &cancel)=0;

		virtual void PutMtlToMtlEditor(MtlBase *mb, int slot=-1)=0;
		virtual MtlBase* GetMtlSlot(int slot)=0;
		virtual MtlBaseLib* GetSceneMtls()=0;

		// Before assigning material to scene, call this to avoid duplicate names.
		// returns 1:OK  0:Cancel
		virtual	BOOL OkMtlForScene(MtlBase *m)=0;

		// Access names of current files
		virtual TSTR &GetCurFileName()=0;
		virtual TSTR &GetCurFilePath()=0;
		virtual TCHAR *GetMatLibFileName()=0;

		// These may bring up file requesters
		virtual void FileOpen()=0;
		virtual BOOL FileSave()=0;
		virtual BOOL FileSaveAs()=0;
		virtual void FileSaveSelected()=0;
		virtual void FileReset(BOOL noPrompt=FALSE)=0;
		virtual void FileMerge()=0;
		virtual void FileHold()=0;
		virtual void FileFetch()=0;
		virtual void FileOpenMatLib(HWND hWnd)=0;  // Window handle is parent window
#ifndef NO_MATLIB_SAVING // orb 01-09-2002
		virtual void FileSaveMatLib(HWND hWnd)=0;
		virtual void FileSaveAsMatLib(HWND hWnd)=0;
#endif // NO_MATLIB_SAVING
		virtual BOOL FileImport()=0;
		virtual BOOL FileExport()=0;

		// This loads 3dsmax.mat (if it exists
		virtual void LoadDefaultMatLib()=0;

		// These do not bring up file requesters
		virtual int LoadFromFile(const TCHAR *name, BOOL refresh=TRUE)=0;
		// LAM - 8/8/03 - ECO 1125 - added clearNeedSaveFlag, useNewFile
		// if clearNeedSaveFlag is false, the scene 'dirty' flag is not cleared
		// if useNewFile is false, the file is not added to the MRU list and the current scene file is not set to the saved file
		virtual int SaveToFile(const TCHAR *fname, BOOL clearNeedSaveFlag = TRUE, BOOL useNewFile = TRUE)=0; 
		virtual void FileSaveSelected(TCHAR *fname)=0;
		virtual void FileSaveNodes(INodeTab* nodes, TCHAR *fname)=0;
		virtual int LoadMaterialLib(const TCHAR *name, MtlBaseLib *lib=NULL)=0;
#ifndef NO_MATLIB_SAVING // orb 01-09-2002
		virtual int SaveMaterialLib(const TCHAR *name, MtlBaseLib *lib=NULL)=0;
#endif // #ifndef NO_MATLIB_SAVING // orb 01-09-2002
		// dupMtlAction and reparentAction added for r6.0 - 8/5/03
		virtual int MergeFromFile(const TCHAR *name, 
				BOOL mergeAll=FALSE,    // when true, merge dialog is not put up
				BOOL selMerged=FALSE,   // select merged items?
				BOOL refresh=TRUE,      // refresh viewports ?
				int dupAction = MERGE_DUPS_PROMPT,  // what to do when duplicate node names are encountered
				NameTab* mrgList=NULL,  // names to be merged (mergeAll must be TRUE)
				int dupMtlAction = MERGE_DUP_MTL_PROMPT,  // what to do when duplicate material names are encountered
				int reparentAction = MERGE_REPARENT_PROMPT   // what to do when can reparent
				)=0;
		virtual BOOL ImportFromFile(const TCHAR *name, BOOL suppressPrompts=FALSE, Class_ID *importerID=NULL)=0;
		virtual BOOL ExportToFile(const TCHAR *name, BOOL suppressPrompts=FALSE, DWORD options=0, Class_ID *exporterID=NULL)=0;

		// Returns TRUE if this instance of MAX is in slave mode
		virtual BOOL InSlaveMode()=0;

		// Brings up the object color picker. Returns TRUE if the user
		// picks a color and FALSE if the user cancels the dialog.
		// If the user picks a color then 'col' will be set to the color.
		virtual BOOL NodeColorPicker(HWND hWnd,DWORD &col)=0;

		
		// The following gourping functions will operate on the table
		// of nodes passed in or the current selection set if the table is NULL
		
		// If name is NULL a dialog box will prompt the user to select a name. 
		// If sel group is TRUE, the group node will be selected after the operation completes.
		// returns a pointer to the group node created.
		virtual INode *GroupNodes(INodeTab *nodes=NULL,TSTR *name=NULL,BOOL selGroup=TRUE)=0;
		virtual void UngroupNodes(INodeTab *nodes=NULL)=0;
		virtual void ExplodeNodes(INodeTab *nodes=NULL)=0;
		virtual void OpenGroup(INodeTab *nodes=NULL,BOOL clearSel=TRUE)=0;
		virtual void CloseGroup(INodeTab *nodes=NULL,BOOL selGroup=TRUE)=0;

		// Flashes nodes (to be used to indicate the completion of a pick operation, for example)
		virtual void FlashNodes(INodeTab *nodes)=0;

		// If a plug-in needs to do a PeekMessage() and wants to actually remove the
		// message from the queue, it can use this method to have the message
		// translated and dispatched.
		virtual void TranslateAndDispatchMAXMessage(MSG &msg)=0;
		
		// This will go into a PeekMessage loop until there are no more
		// messages left. If this method returns FALSE then the user
		// is attempting to quit MAX and the caller should return.
		virtual BOOL CheckMAXMessages()=0;

		// Access viewport background image settings.
		virtual BOOL		setBkgImageName(TCHAR *name)=0;
		virtual TCHAR *		getBkgImageName(void)=0;
		virtual void		setBkgImageAspect(int t)=0;
		virtual int			getBkgImageAspect()=0;
		virtual void		setBkgImageAnimate(BOOL onOff)=0;
		virtual int			getBkgImageAnimate(void)=0;
		virtual void		setBkgFrameRange(int start, int end, int step=1)=0;
		virtual int			getBkgFrameRangeVal(int which)=0;
		virtual void		setBkgORType(int which, int type)=0; // which=0 => before start, which=1 =>	after end
		virtual int			getBkgORType(int which)=0;
		virtual void		setBkgStartTime(TimeValue t)=0;
		virtual TimeValue	getBkgStartTime()=0;
		virtual void		setBkgSyncFrame(int f)=0;
		virtual int			getBkgSyncFrame()=0;
		virtual int			getBkgFrameNum(TimeValue t)=0;

		// Gets the state of the real-time animation playback toggle.
		virtual BOOL GetRealTimePlayback()=0;
		virtual void SetRealTimePlayback(BOOL realTime)=0;
		virtual BOOL GetPlayActiveOnly()=0;
		virtual void SetPlayActiveOnly(BOOL playActive)=0;
		virtual void StartAnimPlayback(int selOnly=FALSE)=0;
		virtual void EndAnimPlayback()=0;
		virtual BOOL IsAnimPlaying()=0;
		virtual int GetPlaybackSpeed()=0;
		virtual void SetPlaybackSpeed(int s)=0;
		virtual BOOL GetPlaybackLoop()=0;
		virtual void SetPlaybackLoop(BOOL loop)=0;

		// mjm - 09.07.00
		// provide access to a global validity token, used essentially as a timestamp.
		// several areas of code may use it, one being interactive reshading.
		virtual void IncrementValidityToken()=0;
		virtual unsigned int CurrentValidityToken()=0;


		// The following APIs provide a simplistic method to call
		// the renderer and render frames. The renderer just uses the
		// current user specified parameters.
		// Note that the renderer uses the width, height, and aspect
		// of the specified bitmap so the caller can control the size
		// of the rendered image rendered.

		// Renderer must be opened before frames can be rendered.
		// Either camNode or view must be non-NULL but not both.
		// 
		// Returns the result of the open call on the current renderer.
		// 0 is fail and 1 is succeed.
		virtual int OpenCurRenderer(INode *camNode,ViewExp *view, RendType t = RENDTYPE_NORMAL, int w=0, int h=0)=0;

		// optional way to specify the view when Opening the renderer.
		virtual int OpenCurRenderer(ViewParams *vpar, RendType t = RENDTYPE_NORMAL, int w=0, int h=0)=0;

		// The renderer must be closed when you are done with it.
		virtual void CloseCurRenderer()=0;

		// Renders a frame to the given bitmap.
		// The RendProgressCallback is an optional callback (the base class is
		// defined in render.h).
		//
		// Returns the result of the render call on the current renderer.
		// 0 is fail and 1 is succeed.
		virtual int CurRendererRenderFrame(TimeValue t, Bitmap *bm, RendProgressCallback *prog=NULL, float frameDur = 1.0f, ViewParams *vp=NULL, RECT *regionRect = NULL)=0;

		// creates a default scanline renderer - must be deleted with IScanRenderer::DeleteThis()
		virtual IScanRenderer *CreateDefaultScanlineRenderer()=0;

		// [dl | 15may2003] Creates an instance of the default renderer for the given render setting. 
		// If the class ID of the default renderer (see SetDefaultRendererClassID()) does not 
		// exist, then an instance of the MAX scanline renderer is created.
		// The instance returned must be deleted with Renderer::DeleteThis().
		virtual Renderer* CreateDefaultRenderer(RenderSettingID renderSettingID) = 0;
		// [dl | 15may2003] Gets/sets the class ID of the default renderer for the given render setting. 
		// An instance of the default renderer can be created by using CreateDefaultRenderer()
		virtual Class_ID GetDefaultRendererClassID(RenderSettingID renderSettingID) = 0;
		virtual void SetDefaultRendererClassID(RenderSettingID renderSettingID, Class_ID classID) = 0;

		// a set of functions parallel to those above, to work with any Renderer instance
		virtual int OpenRenderer(Renderer *pRenderer, INode *camNode,ViewExp *view, RendType type = RENDTYPE_NORMAL, int w=0, int h=0)=0;
		virtual int OpenRenderer(Renderer *pRenderer, ViewParams *vpar, RendType type = RENDTYPE_NORMAL, int w=0, int h=0)=0;
		virtual int RendererRenderFrame(Renderer *pRenderer, TimeValue t, Bitmap *bm, RendProgressCallback *prog=NULL, float frameDur = 1.0f, ViewParams *vp=NULL, RECT *regionRect = NULL)=0;
		virtual void CloseRenderer(Renderer *pRenderer)=0;

		// To get more control over the renderer, the renderer can be called directly.
		// The following methods give access to the current renderer and the the user's
		// current rendering settings.

		// gets a pointer to the renderer being used with the currently active render settings
		virtual Renderer *GetCurrentRenderer()=0;
		// gets a pointer to the renderer being used for each of the render settings
		// GetRenderer(RenderSettingID) can be used to access all settings
		virtual Renderer *GetProductionRenderer()=0;
		virtual Renderer *GetDraftRenderer()=0;
		// this function can be used instead of the two above.
		// RS_MEdit: Returns the renderer in the MEdit slot. Does not consider the lock.
		virtual Renderer *GetRenderer(RenderSettingID renderSettingID)=0;				// mjm - 05.26.00

		// assigns a renderer to be used with the currently active render settings
		virtual void AssignCurRenderer(Renderer *rend)=0;
		// assigns a renderer to be used with each of the render settings
		// AssignRenderer(RenderSettingID, Renderer*) can be used to access all settings
		virtual void AssignProductionRenderer(Renderer *rend)=0;
		virtual void AssignDraftRenderer(Renderer *rend)=0;
		// this function can be used instead of the two above.
		// RS_MEdit: Assign the renderer to the MEdit slot. Does not consider the lock.
		virtual void AssignRenderer(RenderSettingID renderSettingID, Renderer *rend)=0;	// mjm - 05.26.00

		// in order to support more than just two render settings, the following two functions should no
		// longer be used. instead, use ChangeRenderSetting() below.

		// SetUseDraftRenderer() sets the CurrentRenderSetting to 'production'
		// if parameter (b == 0) and sets it to 'draft' if (b != 0)
		virtual void SetUseDraftRenderer(BOOL b)=0;

		// GetUseDraftRenderer() returns TRUE if the CurrentRenderSetting is 'draft'
		// it returns TRUE if the CurrentRenderSetting is not 'draft'
		// there is no way with this function to determine if the CurrentRenderSetting is anything but 'draft'
		virtual BOOL GetUseDraftRenderer()=0;

		// these functions can be used instead of the two above.
		// RS_MEdit cannot be the current render setting. Calling ChangeRenderSetting(RS_MEdit) has no effect.
		virtual void ChangeRenderSetting(RenderSettingID renderSettingID)=0;			// mjm - 05.26.00
		virtual RenderSettingID GetCurrentRenderSetting()=0;							// mjm - 05.26.00

		// [dl | 15may2003] Gets/sets the renderer assigned to the MEdit slot.
		// NOTE: This ignores the state of the MEdit renderer lock, and will not necesarily
		// get/set the renderer that is actually used for MEdit.
		virtual Renderer* GetMEditRenderer() = 0;
		virtual void AssignMEditRenderer(Renderer* renderer) = 0;
		// [dl | 15may2003] This returns the renderer to be used for MEdit.
		// This TAKES THE LOCK INTO ACCOUNT: this will return the Current renderer if
		// the MEdit lock is ON.
		virtual Renderer* GetActualMEditRenderer() = 0;
		// [dl | 15may2003] Gets/sets the status of the MEdit renderer lock.
		// When ON, the Current renderer should be used to render in MEdit.
		virtual bool GetMEditRendererLocked() = 0;
		virtual void SetMEditRendererLocked(bool locked) = 0;
		virtual bool GetMEditRendererLocked_DefaultValue() = 0;
		virtual void SetMEditRendererLocked_DefaultValue(bool locked) = 0;

// mjm - begin - 06.30.00
		// gets the current (production vs. draft) render element manager
		virtual IRenderElementMgr *GetCurRenderElementMgr()=0;
		// gets production or draft render element manager -- passing in renderSettingID = RS_IReshade will return NULL
		virtual IRenderElementMgr *GetRenderElementMgr(RenderSettingID renderSettingID)=0;
// mjm - end

		// Fills in a RendParams structure that can be passed to the
		// renderer with the user's current rendering settings.
		// A vpt pointer only needs to be passed in if the RendType
		// is RENDTYPE_REGION or RENDTYPE_BLOWUP. In these cases it will
		// set up the RendParams regxmin,regxmax,regymin,regymax from
		// values stored in the viewport.
		virtual void SetupRendParams(RendParams &rp, ViewExp *vpt, RendType t = RENDTYPE_NORMAL)=0;

		// fills in a ViewParams structure based on type of node passed (camera or light)
		// can be used when instantiating a renderer and calling Open(), Render(), and Close()
		// directly on it.
		virtual void GetViewParamsFromNode(INode* vnode, ViewParams& vp, TimeValue t)=0;

		// Call during render to check if user has cancelled render.  
		// Returns TRUE iff user has cancelled.
		virtual BOOL CheckForRenderAbort()=0;

        // Call during render to abort the render
        virtual void AbortRender()=0;

		// These give access to individual user specified render parameters
		// These are either parameters that the user specifies in the
		// render dialog or the renderer page of the preferences dialog.
		virtual int GetRendTimeType()=0;
		virtual void SetRendTimeType(int type)=0;
		virtual TimeValue GetRendStart()=0;
		virtual void SetRendStart(TimeValue start)=0;
		virtual TimeValue GetRendEnd()=0;
		virtual void SetRendEnd(TimeValue end)=0;
		virtual int GetRendNThFrame()=0;
		virtual void SetRendNThFrame(int n)=0;

#ifndef WEBVERSION //some properties are read-only method in Webversion, (have Get() but not Set())

		virtual BOOL GetRendShowVFB()=0;
		virtual void SetRendShowVFB(BOOL onOff)=0;
		virtual BOOL GetRendSaveFile()=0;
		virtual void SetRendSaveFile(BOOL onOff)=0;
		virtual BOOL GetRendUseDevice()=0;
		virtual void SetRendUseDevice(BOOL onOff)=0;
		virtual BOOL GetRendUseNet()=0;
		virtual void SetRendUseNet(BOOL onOff)=0;

		virtual BitmapInfo& GetRendFileBI()=0;
		virtual BitmapInfo& GetRendDeviceBI()=0;
		virtual int GetRendWidth()=0;
		virtual void SetRendWidth(int w)=0;
		virtual int GetRendHeight()=0;
		virtual void SetRendHeight(int h)=0;
		virtual float GetRendApect()=0;
		virtual void SetRendAspect(float a)=0;

		virtual float GetRendImageAspect()=0;

		virtual float GetRendApertureWidth()=0;	 // get aperture width in mm.
		virtual void SetRendApertureWidth(float aw)=0; // set aperture width in mm.
		virtual BOOL GetRendFieldRender()=0;
		virtual void SetRendFieldRender(BOOL onOff)=0;
		virtual BOOL GetRendColorCheck()=0;
		virtual void SetRendColorCheck(BOOL onOff)=0;
		virtual BOOL GetRendSuperBlack()=0;
		virtual void SetRendSuperBlack(BOOL onOff)=0;

		virtual BOOL GetRendHidden()=0;
		virtual void SetRendHidden(BOOL onOff)=0;
		virtual BOOL GetRendForce2Side()=0;
		virtual void SetRendForce2Side(BOOL onOff)=0;

		virtual BOOL GetRendAtmosphere()=0;
		virtual void SetRendAtmosphere(BOOL onOff)=0;
		virtual BOOL GetRendEffects()=0;
		virtual void SetRendEffects(BOOL onOff)=0;
		virtual BOOL GetRendDisplacement()=0;
		virtual void SetRendDisplacement(BOOL onOff)=0;

		virtual TSTR& GetRendPickFramesString()=0;
		virtual BOOL GetRendDitherTrue()=0;
		virtual void SetRendDitherTrue(BOOL onOff)=0;
		virtual BOOL GetRendDither256()=0;
		virtual void SetRendDither256(BOOL onOff)=0;
		virtual BOOL GetRendMultiThread()=0;
		virtual void SetRendMultiThread(BOOL onOff)=0;
		virtual BOOL GetRendNThSerial()=0;
		virtual void SetRendNThSerial(BOOL onOff)=0;
		virtual int GetRendVidCorrectMethod()=0; // 0->FLAG, 1->SCALE_LUMA 2->SCALE_SAT
		virtual void SetRendVidCorrectMethod(int m)=0;
		virtual int GetRendFieldOrder()=0; // 0->even, 1-> odd
		virtual void SetRendFieldOrder(int fo)=0;
		virtual int GetRendNTSC_PAL()=0; // 0 ->NTSC,  1 ->PAL
		virtual void SetRendNTSC_PAL(int np)=0;
		virtual int GetRendSuperBlackThresh()=0;
		virtual void SetRendSuperBlackThresh(int sb)=0;

//		virtual float GetRendMaxPixelSize()=0;
//		virtual void SetRendMaxPixelSize(float s)=0;
		virtual int GetRendFileNumberBase()=0;
		virtual void SetRendFileNumberBase(int n)=0;
		virtual BOOL GetSkipRenderedFrames()=0;
		virtual void SetSkipRenderedFrames(BOOL onOff)=0;

#else // WEBVERSION IS DEFINED

		virtual BOOL GetRendShowVFB()=0;
		virtual BOOL GetRendSaveFile()=0;
		virtual void SetRendSaveFile(BOOL onOff)=0;
		virtual BOOL GetRendUseDevice()=0;
		virtual BOOL GetRendUseNet()=0;

		virtual BitmapInfo& GetRendFileBI()=0;
		virtual BitmapInfo& GetRendDeviceBI()=0;
		virtual int GetRendWidth()=0;
		virtual void SetRendWidth(int w)=0;
		virtual int GetRendHeight()=0;
		virtual void SetRendHeight(int h)=0;
		virtual float GetRendApect()=0;

		virtual float GetRendImageAspect()=0;

		virtual float GetRendApertureWidth()=0;	 // get aperture width in mm.
		virtual BOOL GetRendFieldRender()=0;
		virtual BOOL GetRendColorCheck()=0;
		virtual BOOL GetRendSuperBlack()=0;

		virtual BOOL GetRendHidden()=0;
		virtual void SetRendHidden(BOOL onOff)=0;
		virtual BOOL GetRendForce2Side()=0;
		virtual void SetRendForce2Side(BOOL onOff)=0;

		virtual BOOL GetRendAtmosphere()=0;
		virtual BOOL GetRendEffects()=0;
		virtual BOOL GetRendDisplacement()=0;

		virtual TSTR& GetRendPickFramesString()=0;
		virtual BOOL GetRendDitherTrue()=0;
		virtual void SetRendDitherTrue(BOOL onOff)=0;
		virtual BOOL GetRendDither256()=0;
		virtual void SetRendDither256(BOOL onOff)=0;
		virtual BOOL GetRendMultiThread()=0;
		virtual void SetRendMultiThread(BOOL onOff)=0;
		virtual BOOL GetRendNThSerial()=0;
		virtual void SetRendNThSerial(BOOL onOff)=0;
		virtual int GetRendVidCorrectMethod()=0; // 0->FLAG, 1->SCALE_LUMA 2->SCALE_SAT
		virtual void SetRendVidCorrectMethod(int m)=0;
		virtual int GetRendFieldOrder()=0; // 0->even, 1-> odd
		virtual void SetRendFieldOrder(int fo)=0;
		virtual int GetRendNTSC_PAL()=0; // 0 ->NTSC,  1 ->PAL
		virtual void SetRendNTSC_PAL(int np)=0;
		virtual int GetRendSuperBlackThresh()=0;

//		virtual float GetRendMaxPixelSize()=0;
//		virtual void SetRendMaxPixelSize(float s)=0;
		virtual int GetRendFileNumberBase()=0;
		virtual void SetRendFileNumberBase(int n)=0;
		virtual BOOL GetSkipRenderedFrames()=0;

#endif // WEBVERSION

		//Max 6.0
		virtual BOOL GetRendSimplifyAreaLights() = 0;
		virtual void SetRendSimplifyAreaLights(BOOL onOff) = 0;

		virtual DWORD GetHideByCategoryFlags()=0;
		virtual void SetHideByCategoryFlags(DWORD f)=0;

		virtual int GetViewportLayout()=0;
		virtual void SetViewportLayout(int layout)=0;
		virtual BOOL IsViewportMaxed()=0;
		virtual void SetViewportMax(BOOL max)=0;

		// Zoom extents the active viewport, or all
		virtual void ViewportZoomExtents(BOOL doAll, BOOL skipPersp=FALSE)=0;

	//watje this zooms the active viewport or all to the specified bounding box which is in world space
		virtual void ZoomToBounds(BOOL doAll, Box3 box) =0;

		// Gets the world space bounding box of the selection.
		virtual void GetSelectionWorldBox(TimeValue t,Box3 &box)=0;
		
		// Find an INode with the given name
		virtual INode *GetINodeByName(const TCHAR *name)=0;

		// Find an INode with the given handle
		virtual INode *GetINodeByHandle(ULONG handle)=0;

		// For use with gbuffer BMM_CHAN_NODE_RENDER_ID channel during video post
		virtual INode *GetINodeFromRenderID(UWORD id)=0;

		// Executes a MAX command. See maxcom.h for available commands
		virtual void ExecuteMAXCommand(int id)=0;

		// Returns a class used for efficiently creating unique names
		virtual NameMaker* NewNameMaker(BOOL initFromScene = TRUE)=0;

		// Get set the viewport background color.
		virtual void SetViewportBGColor(const Point3 &color)=0;
		virtual Point3 GetViewportBGColor()=0;

		// Get/Set the environment texture map, ambient light and other effects
		virtual Texmap *GetEnvironmentMap()=0;
		virtual void SetEnvironmentMap(Texmap *map)=0;
		virtual BOOL GetUseEnvironmentMap()=0;
		virtual void SetUseEnvironmentMap(BOOL onOff)=0;

		virtual Point3 GetAmbient(TimeValue t,Interval &valid)=0;
		virtual void SetAmbient(TimeValue t, Point3 col)=0;
		virtual Control *GetAmbientController()=0;
		virtual void SetAmbientController(Control *c)=0;

		virtual Point3 GetLightTint(TimeValue t,Interval &valid)=0;
		virtual void SetLightTint(TimeValue t, Point3 col)=0;
		virtual Control *GetLightTintController()=0;
		virtual void SetLightTintController(Control *c)=0;

		virtual float GetLightLevel(TimeValue t,Interval &valid)=0;
		virtual void SetLightLevel(TimeValue t, float lev)=0;
		virtual Control *GetLightLevelController()=0;
		virtual void SetLightLevelController(Control *c)=0;

		virtual int NumAtmospheric()=0;
		virtual Atmospheric *GetAtmospheric(int i)=0;
		virtual void SetAtmospheric(int i,Atmospheric *a)=0;
		virtual void AddAtmosphere(Atmospheric *atmos)=0;
		virtual void DeleteAtmosphere(int i)=0;
		virtual void EditAtmosphere(Atmospheric *a, INode *gizmo=NULL)=0;

		virtual int NumEffects()=0;
		virtual Effect *GetEffect(int i)=0;
		virtual void SetEffect(int i,Effect *e)=0;
		virtual void AddEffect(Effect *eff)=0;
		virtual void DeleteEffect(int i)=0;
		virtual void EditEffect(Effect *e, INode *gizmo=NULL)=0;

		virtual Point3 GetBackGround(TimeValue t,Interval &valid)=0;
		virtual void SetBackGround(TimeValue t,Point3 col)=0;
		virtual Control *GetBackGroundController()=0;
		virtual void SetBackGroundController(Control *c)=0;

		// Get/Set the current sound object.
		virtual SoundObj *GetSoundObject()=0;
		virtual void SetSoundObject(SoundObj *snd)=0;

#ifdef _OSNAP
		virtual IOsnapManager *GetOsnapManager()=0;
		virtual MouseManager *GetMouseManager()=0;
		virtual void InvalidateOsnapdraw()=0;
#endif

		// Access the current mat lib loaded.
		virtual MtlBaseLib& GetMaterialLibrary()=0;

		virtual BOOL IsNetServer()=0; // returns 1 iff is network server
		//-- GUPSTART
		virtual void SetNetServer()=0;
		//-- GUPEND


		//-- Logging Facilities (Replaces the old NetLog() stuff)
		// 
		//   Check log.h for methods

		virtual LogSys *Log()=0;


		// get ref to the central DLL directory
		virtual DllDir& GetDllDir()=0; 

		// Generic expansion function
		virtual INT_PTR_MSVC70 Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0, ULONG_PTR arg4=0, ULONG_PTR arg5=0, ULONG_PTR arg6=0)=0; 
									// WIN64 Cleanup: Martell and Shuler
		virtual void *GetInterface(DWORD id)=0;

		// Get pointer to the scene.
		virtual ReferenceTarget *GetScenePointer()=0;

		// Get a pointer to the Track View root node.
		virtual ITrackViewNode *GetTrackViewRootNode()=0;

		// Free all bitmaps used by the scene
		virtual void FreeSceneBitmaps()=0;

		// Access the DllDir
		virtual DllDir *GetDllDirectory()=0;

		// Enumerate Bitmap Files
		virtual void EnumAuxFiles(NameEnumCallback& nameEnum, DWORD flags)=0;

		// Render a 2D bitmap from a procedural texture
		virtual void RenderTexmap(Texmap *tex, Bitmap *bm, float scale3d=1.0f, BOOL filter=FALSE, BOOL display=FALSE, float z=0.0f)=0;

		// Activate and deactivate a texture map in the viewports. 
		// mtl is the TOP LEVEL of the material containing the texture map. If it
		// is a Multi-material, then subNum specifies which sub branch of the
		// multi contains tx.
		CoreExport void DeActivateTexture(MtlBase *tx, Mtl *mtl, int subNum=-1);
		CoreExport void ActivateTexture(MtlBase *tx, Mtl *mtl, int subNum=-1);

		// Access to named selection sets at the object level
		virtual int GetNumNamedSelSets()=0;
		virtual TCHAR *GetNamedSelSetName(int setNum)=0;
		virtual int GetNamedSelSetItemCount(int setNum)=0;
		virtual INode *GetNamedSelSetItem(int setNum,int i)=0;
		virtual void AddNewNamedSelSet(INodeTab &nodes,TSTR &name)=0;
		virtual void RemoveNamedSelSet(TSTR &name)=0;
		virtual void ReplaceNamedSelSet(INodeTab &nodes,int setNum)=0;
		virtual void GetNamedSelSetList(INodeTab &nodes,int setNum)=0;

		// Get new material and map names to maintain name uniqueness
		virtual void AssignNewName(Mtl *m)=0;
		virtual void AssignNewName(Texmap *m)=0;

		// rescale world units of entire scene, or selection
		virtual void RescaleWorldUnits(float f, BOOL selected)=0;

		// Initialize snap info structure with current snap settings
		// (Returns zero if snap is OFF)
		virtual int InitSnapInfo(SnapInfo *info)=0;

		// Time configuration dialog key step options
		virtual BOOL GetKeyStepsSelOnly()=0;
		virtual void SetKeyStepsSelOnly(BOOL onOff)=0;
		virtual BOOL GetKeyStepsUseTrans()=0;
		virtual void SetKeyStepsUseTrans(BOOL onOff)=0;
		virtual BOOL GetKeyStepsPos()=0;
		virtual void SetKeyStepsPos(BOOL onOff)=0;
		virtual BOOL GetKeyStepsRot()=0;
		virtual void SetKeyStepsRot(BOOL onOff)=0;
		virtual BOOL GetKeyStepsScale()=0;
		virtual void SetKeyStepsScale(BOOL onOff)=0;
		virtual BOOL GetKeyStepsUseTrackBar()=0;
		virtual void SetKeyStepsUseTrackBar(BOOL onOff)=0;

		// Enables/disables the use of Transform Gizmos
		virtual BOOL GetUseTransformGizmo()=0;
		virtual void SetUseTransformGizmo(BOOL onOff)=0;

		// Get/Set if the TGiz restores the previous axis when released.
		virtual void SetTransformGizmoRestoreAxis(BOOL bOnOff)=0;
		virtual BOOL GetTransformGizmoRestoreAxis()=0;

		// Turn off axis follows transform mode AI.
		virtual BOOL GetConstantAxisRestriction()=0;
		virtual void SetConstantAxisRestriction(BOOL onOff)=0;

		// Used to hittest transform Gizmos for sub-objects
		virtual int HitTestTransformGizmo(IPoint2 *p, ViewExp *vpt, int axisFlags) = 0;

		// Used to deactiveate the Transform Gizmo when it is released.
		virtual void DeactivateTransformGizmo() = 0;

		// put up dialog to let user configure the bitmap loading paths.
		// returns 1: OK ,  0: Cancel.
		virtual int ConfigureBitmapPaths()=0;

		// Puts up the space array dialog. If the callback is NULL it 
		// just does the standard space array tool.
		// returns TRUE if the user OKs the dialog, FALSE otherwise.
		virtual BOOL DoSpaceArrayDialog(SpaceArrayCallback *sacb=NULL)=0;

		// dynamically add plugin-class.
		// returns -1 if superclass was unknown
		// returns 0 if class already exists
		// returns 1 if class added successfully
		virtual int AddClass(ClassDesc *cd)=0;

		// dynamically delete plugin-class.
		// returns -1 if superclass was unknown
		// returns 0 if class does not exist
		// returns 1 if class deleted successfully
		virtual int DeleteClass(ClassDesc *cd)=0;

		// Number of CommandModes in the command mode stack
		virtual int GetCommandStackSize()=0;
		// Get the CommandMode at this position in the command mode stack (0 = current)
		virtual CommandMode* GetCommandStackEntry(int entry)=0;

		
		// This method should be called in an light's BeginEditParams, after adding rollups 
		// to the modify panel: it puts up a rollup containing a list of all Atmospherics 
		// and Effects that use the current selected node as a "gizmo"
		virtual void AddSFXRollupPage(ULONG flags=0)=0; // flags are for future use

		// This is called in a light's EndEditParams when removing rollups
		virtual void DeleteSFXRollupPage()=0;

		// This is called by an Atmospheric or Effect when it adds or removes a "gizmo" reference.
		virtual void RefreshSFXRollupPage()=0;

		// PropertySet access
		// Legal values for "int PropertySet" are defined above:
		//	PROPSET_SUMMARYINFO
		//	PROPSET_DOCSUMMARYINFO
		//	PROPSET_USERDEFINED
		virtual int					GetNumProperties(int PropertySet)=0;
		virtual int					FindProperty(int PropertySet, const PROPSPEC* propspec)=0;
		virtual const PROPVARIANT*	GetPropertyVariant(int PropertySet, int idx)=0;
		virtual const PROPSPEC*		GetPropertySpec(int PropertySet, int idx)=0;
		virtual void				AddProperty(int PropertySet, const PROPSPEC* propspec, const PROPVARIANT* propvar)=0;
		virtual void				DeleteProperty(int PropertySet, const PROPSPEC* propspec)=0;

		// register a window that can appear in a MAX viewport
		virtual BOOL RegisterViewWindow(ViewWindow *vw)=0;
		virtual BOOL UnRegisterViewWindow(ViewWindow *vw)=0;
		
		// Get and set the global shadow generator ( used by light.cpp)
		virtual ShadowType *GetGlobalShadowGenerator()=0;
		virtual void SetGlobalShadowGenerator(ShadowType *st)=0;

		// Get the Import zoom-extents flag
		virtual BOOL GetImportZoomExtents()=0;
		virtual void SetImportZoomExtents(BOOL onOff)=0;

		virtual unsigned int HardwareLockID()=0;
		virtual BOOL CheckForSave()=0;

		virtual ITrackBar*	GetTrackBar()=0;

		// For scene XRefs. Most of the time the XRef trees (whose root node is a child of the
		// client scene's root node) are skipped when traversing the hierarchy. When this option
		// is turned on, all root nodes will include child XRef scene root nodes in any traversal
		// related functions such as NumberOfChildren() and GetChildNode(i).
		// 
		// This option is turned on automatically before rendering and turned off after so that
		// scene XRefs appear in the production renderer. This option should not be left on if
		// it is turned on since it would cause scene XRef objects to be accessible to the user in the client scene.
		virtual void SetIncludeXRefsInHierarchy(BOOL onOff)=0;
		virtual BOOL GetIncludeXRefsInHierarchy()=0;

		// Use these two suspend automatic reloading of XRefs. 
		virtual BOOL IsXRefAutoUpdateSuspended()=0;
		virtual void SetXRefAutoUpdateSuspended(BOOL onOff)=0;

		// get the macroRecorder interface pointer
		virtual MacroRecorder* GetMacroRecorder()=0;


		// DS 2/2/99: 
		virtual void UpdateMtlEditorBrackets()=0;

		// PRS 2/4/99
		virtual bool IsTrialLicense() = 0;
		virtual bool IsNetworkLicense() = 0;
		virtual bool IsEmergencyLicense() = 0;

		// CCJ - 2/12/99
		virtual void SetMAXFileOpenDlg(MAXFileOpenDialog* dlg) = 0;
		virtual void SetMAXFileSaveDlg(MAXFileSaveDialog* dlg) = 0;

		virtual void RAMPlayer(HWND hWndParent, TCHAR* szChanA=NULL, TCHAR* szChanB=NULL) = 0;

		//KAE - 3/4/99
		virtual void FlushUndoBuffer() = 0;

		// CCJ 3/16/99
		virtual bool DeferredPluginLoadingEnabled() = 0;
		virtual void EnableDeferredPluginLoading(bool onOff) = 0;

		// RB: 3/30/99
		virtual BOOL IsSceneXRefNode(INode *node)=0;
	
		// MEP 4/19/99
		virtual bool CanImportFile(const TCHAR* filename)=0;
		virtual bool IsMaxFile(const TCHAR* filename)=0;
		virtual bool IsInternetCachedFile(const TCHAR* filename)=0;

		// MEP 6/21/99
		virtual bool CanImportBitmap(const TCHAR* filename)=0;

		//JH 8/07/99: implementing scheme to capture subobject registration
		virtual bool CaptureSubObjectRegistration(bool OnOff, Class_ID cid)=0;

		// MEP 8/06/99: easy method to download a file from a URL
		// JH 5/4/99: adding argument to control dialog appearance
		#define DOWNLOADDLG_NOPLACE		1<<0
		virtual bool DownloadUrl(HWND hwnd, const TCHAR* url, const TCHAR* filename, DWORD flags = 0)=0;

		// MEP 3/10/2000: if drag-and-dropped onto a node, you can retrieve the node with this
		virtual INode* GetImportCtxNode(void)=0;

		// child treeview creation - JBW 5.25.00
		virtual ITreeView* CreateTreeViewChild(ReferenceTarget* root, HWND hParent, DWORD style=0, ULONG id=0, int open=OPENTV_SPECIAL)=0;

		// CCJ 6.7.00 - Create a viewport material from a renderer material
		virtual void ConvertMtl(TimeValue t, Material &gm, Mtl *mtl,  BOOL doTex, int subNum, float vis, BOOL &needDecal, INode *node, BitArray *needTex, GraphicsWindow *gw)=0;

		// 000817  --prs.
		virtual int GetProductVersion()=0;
		virtual int GetLicenseBehavior()=0;
		// 000821  --prs.
		virtual bool IsFeatureLicensed(int subNum)=0;
		// 000824  --prs.
		virtual int GetLicenseDaysLeft()=0;

		// CCJ 8.23.00 - Clone a node hierarchy
		virtual bool CloneNodes(INodeTab& nodes, Point3& offset, bool expandHierarchies = true, CloneType cloneType = NODE_COPY, INodeTab* resultSource = NULL, INodeTab* resultTarget = NULL) = 0;

		// NS: 9/9/00
		virtual void CollapseNode(INode *node, BOOL noWarning = FALSE)=0;
		virtual BOOL CollapseNodeTo(INode *node, int modIndex, BOOL noWarning = FALSE)=0;
		virtual BOOL ConvertNode(INode *node, Class_ID &cid)=0;

		// TB 5/22/03
		virtual IRenderPresetsManager* GetRenderPresetsManager()=0;

		// LAM - 6/24/03
		//-- Default Action Facilities 
		//   Check DefaultActions.h for methods
		virtual DefaultActionSys *DefaultActions()=0;

		// Sets internal flag, returns old value. 
		// Set to TRUE to indicate that no dialogs should be displayed to user.
		virtual BOOL SetQuietMode(BOOL onOff)=0;

		// The return value from this method should be called before displaying any dialogs.
		// Returns internal flag set by SetQuietMode. If checkNetSlave == TRUE, returns internal 
		// flag OR-ed with whether max is running in network rendering server mode. 
		virtual BOOL GetQuietMode(BOOL checkNetSlave = TRUE)=0;

}; // class Interface

#ifdef DESIGN_VER
class IAggregation;
class VizInterface : public Interface
{
public:
 	virtual IAggregation& GetAggregationMgr()= 0;

};
class IObjParam: public VizInterface{};
#else
class IObjParam: public Interface{};
#endif // DESIGN_VER

class IObjCreate: public IObjParam{};

#endif // __JAGAPI__

