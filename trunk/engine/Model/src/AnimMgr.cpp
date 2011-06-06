#include "AnimMgr.h"

AnimNode::AnimNode():CurLoop(0),LoopCount(-1),uFrame(0),uTotalFrames(0),fSpeed(1.0f){}

int AnimNode::next()
{
	uFrame -= uTotalFrames;
	CurLoop++;
	if (LoopCount!=-1&&CurLoop>=LoopCount)
	{
		return 1;
	}
	return 0;
}

int AnimNode::prev()
{
	uFrame += uTotalFrames;
	CurLoop--;
	if (CurLoop<0)
	{
		return -1;
	}
	return 0;
}

int SingleAnimNode::Tick(unsigned long uElapsedTime)
{
	uFrame += unsigned long(uElapsedTime*fSpeed);
	if (uFrame >= uTotalFrames)
	{
		return next();
	}
	else if (uFrame < 0)
	{
		return prev();
	}
	return 0;
}

int ListAnimNode::Tick(unsigned long uElapsedTime)
{
	uTotalFrames = setNodes.size();
	if (setNodes.size()>uFrame)
	{
		uFrame+=setNodes[uFrame]->Tick(int(uElapsedTime*fSpeed));
		if (uFrame >= uTotalFrames)
		{
			return next();
		}
		else if (uFrame < 0)
		{
			return prev();
		}
	}
	return 0;
}

int ParallelAnimNode::Tick(unsigned long uElapsedTime)
{
	for (size_t i=0;i<setNodes.size();++i)
	{
		setNodes[i]->Tick(int(uElapsedTime*fSpeed));
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
AnimManager::AnimManager(ModelAnimation *anim) {
	AnimIDSecondary = -1;
	AnimIDMouth = -1;
	anims = anim;
	AnimParticles = false;

	Count = 1;
	PlayIndex = 0;
	CurLoop = 0;
	LoopCount = -1;
	Speed = 1.0f;
	mouthSpeed = 1.0f;
	Paused = false;

	Frame = 0;
	TotalFrames = 0;

	if (anims != NULL)
	{ 
	//	Set(0, 0, 0);
	}
}

AnimManager::~AnimManager() {
	anims = NULL;
}


void AnimManager::AddAnim(unsigned long id, short loops)
{
	if (Count > 3)
		return;

	animList[Count].AnimID = id;
	animList[Count].Loops = loops;
	Count++;
}

// void AnimManager::Set(short index, const std::string& strAnimName, short loops)
// {
// 	// error check, we currently only support 4 animations.
// 	if (index > 3)
// 		return;
// 
// 	animList[index].AnimID = strAnimName;
// 	animList[index].Loops = loops;
// 
// 
// 	// Just an error check for our "auto animate"
// 	if (index == 0)
// 	{
// 		if (Count == 0)
// 		{
// 			Count = 1;
// 		}
// 		PlayIndex = index;
// 		Frame = anims[id].timeStart;
// 		TotalFrames = anims[id].timeEnd - anims[id].timeStart;
// 	}
// }

void AnimManager::Play() {
	PlayIndex = 0;
	//if (Frame == 0 && PlayID == 0) {
		LoopCount = animList[PlayIndex].Loops;
		Frame = anims[animList[PlayIndex].AnimID].timeStart;
		TotalFrames = anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart;
	//}

	Paused = false;
	AnimParticles = false;
}

void AnimManager::Stop()
{
	Paused = true;
	PlayIndex = 0;
	Frame = anims[animList[0].AnimID].timeStart;
	LoopCount = animList[0].Loops;
}

void AnimManager::Pause(bool force) {
	if (Paused && force == false) {
		Paused = false;
		AnimParticles = !Paused;
	} else {
		Paused = true;
		AnimParticles = !Paused;
	}
}

void AnimManager::Next()
{
	if(CurLoop == LoopCount) // 进入下一动画
	{
		PlayIndex++;
		if (PlayIndex >= Count)
		{
			Stop();
			return;
		}
		CurLoop = 0;
		LoopCount = animList[PlayIndex].Loops;
	}
	else if(CurLoop < LoopCount) // 进入当前动画的下一循环
	{
		CurLoop++;
	}
	// 帧移动到动画最开始处
	Frame = anims[animList[PlayIndex].AnimID].timeStart;
}

void AnimManager::Prev()
{
	if(CurLoop == 0)
	{
		PlayIndex--;

		if (PlayIndex < 0)
		{
			Stop();
			return;
		}

		CurLoop = animList[PlayIndex].Loops;
	}
	else if(CurLoop > 0)
	{
		CurLoop--;
	}

	Frame = anims[animList[PlayIndex].AnimID].timeEnd;
}

int AnimManager::Tick(int time)
{
	if (Paused)
	{
		return 1;
	}
	if((Count < PlayIndex))
		return -1;

	for (int i=0; i<ANIM_TYPE_MAX; i++)
	{
		m_AnimsInfo[i].Tick(time);
	}

	// 
	Frame += unsigned long(time*Speed);
	if (Frame >= anims[animList[PlayIndex].AnimID].timeEnd)
	{
		Next();
		return 1;
	}
	else if (Frame < anims[animList[PlayIndex].AnimID].timeStart)
	{
		Prev();
		return 1;
	}

	return 0;
}

const ModelAnimation& AnimManager::getCurrentAnim()
{
	return anims[animList[PlayIndex].AnimID];
}

unsigned long AnimManager::GetFrameCount()
{
	return (anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart);
}


void AnimManager::NextFrame()
{
	//AnimateParticles();
	int id = animList[PlayIndex].AnimID;
	Frame += int((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = int((anims[id].timeEnd - anims[id].timeStart) / 60);

	Pause(true);
}

void AnimManager::PrevFrame()
{
	//AnimateParticles();
	int id = animList[PlayIndex].AnimID;
	Frame -= int((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = int((anims[id].timeEnd - anims[id].timeStart) / 60) * -1;

	Pause(true);
}

void AnimManager::SetFrame(unsigned long f)
{
	//TimeDiff = f - Frame;
	Frame = f;
}

int AnimManager::GetTimeDiff()
{
	int t = TimeDiff;
	TimeDiff = 0;
	return t;
}

void AnimManager::SetTimeDiff(int i)
{
	TimeDiff = i;
}

void AnimManager::Clear() {
	Stop();
	Paused = true;
	PlayIndex = 0;
	Count = 0;
	CurLoop = 0;
	LoopCount = 0;
	Frame = 0;
}