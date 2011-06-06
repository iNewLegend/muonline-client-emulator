#pragma once
#include "animated.h"

// This will be our animation manager
// instead of using a STL vector or list or table, etc.  
// Decided to just limit it upto 4 animations to loop through - for experimental testing.
// The second id and loop count will later be used for being able to have a primary and secondary animation.

// Currently, this is more of a "Wrapper" over the existing code
// but hopefully over time I can remove and re-write it so this is the core.
struct AnimInfo {
	short Loops;
	unsigned long AnimID;
};

enum ANIMTYPE
{
	ANIM_TYPE_MOUTH,		// 计算口型动画
	ANIM_TYPE_SECONDARY,	// 上半身动画
	ANIM_TYPE_MAX,
};

struct AnimNode
{
	AnimNode();
	short	LoopCount;			// Current loop that we're upto.
	short	CurLoop;
	unsigned long	uFrame;
	unsigned long	uTotalFrames;
	float	fSpeed;
	virtual int Tick(unsigned long uElapsedTime)=0;
	virtual int next();
	virtual int prev();
};

struct SingleAnimNode:public AnimNode
{
	virtual int Tick(unsigned long uElapsedTime);
};

struct ListAnimNode:public AnimNode
{
	short nPlayIndex;		// Current animation index we're upto
	std::vector<AnimNode*>	setNodes;
	virtual int Tick(unsigned long uElapsedTime);
};

struct ParallelAnimNode:public AnimNode
{
	std::vector<AnimNode*>	setNodes;
	virtual int Tick(unsigned long uElapsedTime);
};

struct MyAnimInfo
{
	MyAnimInfo():nAnimID(-1),uFrame(0),uTotalFrames(0),fSpeed(0){}
	int		nAnimID;
	unsigned long	timeStart;
	unsigned long	timeEnd;
	unsigned long	uFrame;
	unsigned long	uTotalFrames;
	float	fSpeed;
	void Tick(unsigned long uElapsedTime)
	{
		if (-1 < nAnimID)
		{
			uFrame += unsigned long(uElapsedTime*fSpeed);
			if (uFrame >= timeEnd)
			{
				uTotalFrames = timeEnd - timeStart;
				uFrame -= uTotalFrames;
			}
			else if (uFrame < timeStart)
			{
				uTotalFrames = timeEnd - timeStart;
				uFrame += uTotalFrames;
			}
		}
	}
};

class AnimManager
{
	ModelAnimation *anims;

	bool Paused;
	bool AnimParticles;

	AnimInfo animList[4];

	// 当前帧
	unsigned long Frame;		// Frame number we're upto in the current animation
	unsigned long TotalFrames;

	// 口型帧
	int AnimIDSecondary;
	// 口型帧
	unsigned long FrameSecondary;

	// 口型动画ID
	int AnimIDMouth;
	// 口型帧
	unsigned long FrameMouth;

	short Count;			// Total index of animations
	short PlayIndex;		// Current animation index we're upto
	short CurLoop;			// Current loop that we're upto.
short LoopCount;
	int TimeDiff;			// Difference in time between each frame

	float Speed;			// The speed of which to multiply the time given for Tick();
	float mouthSpeed;

	MyAnimInfo	m_AnimsInfo[ANIM_TYPE_MAX];
public:
	AnimManager(ModelAnimation *anim);
	~AnimManager();

	void AddAnim(unsigned long id, short loop); // Adds an animation to our array.
	//void Set(short index, unsigned long id, short loop); // sets one of the 4 existing animations and changes it (not really used currently)

	void SetSecondary(int id) {
		AnimIDSecondary = id;
		FrameSecondary = anims[id].timeStart;
	}
	void ClearSecondary() { AnimIDSecondary = -1; }
	int GetSecondaryID() { return AnimIDSecondary; }
	unsigned long GetSecondaryFrame() { return FrameSecondary; }

	// For independent mouth movement.
	void SetMouth(int id) {
		AnimIDMouth = id;
		FrameMouth = anims[id].timeStart;
	}
	void ClearMouth() { AnimIDMouth = -1; }
	int GetMouthID() { return AnimIDMouth; }
	unsigned long GetMouthFrame() { return FrameMouth; }
	void SetMouthSpeed(float speed) {
		mouthSpeed = speed;
	}

	void Play(); // Players the animation, and reconfigures if nothing currently inputed
	void Stop(); // Stops and resets the animation
	void Pause(bool force = false); // Toggles 'Pause' of the animation, use force to pause the animation no matter what.

	void Next(); // Plays the 'next' animation or loop
	void Prev(); // Plays the 'previous' animation or loop

	int Tick(int time);

	const ModelAnimation& getCurrentAnim();
	unsigned long GetFrameCount();
	unsigned long GetFrame() {return Frame;}
	void SetFrame(unsigned long f);
	void SetSpeed(float speed) {Speed = speed;}
	float GetSpeed() {return Speed;}

	void PrevFrame();
	void NextFrame();

	void Clear();
	void Reset() { Count = 0; }

	bool IsPaused() { return Paused; }
	bool IsParticlePaused() { return !AnimParticles; }
	void AnimateParticles() { AnimParticles = true; }

	unsigned long GetAnim() { return animList[PlayIndex].AnimID; }

	int GetTimeDiff();
	void SetTimeDiff(int i);
};