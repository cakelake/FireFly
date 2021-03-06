#include "Tween.h"

Tween::Tween()
	: mTweens()
	, mDurations()
	, mTotalTime(0)
	, mTimer()
{
	mTimer.restart();
}

void Tween::addTween(CDBTweener::CEquation *pEquation, CDBTweener::EEasing eEasing, float fDuration, float *fpValue, float fTarget)
{
	CDBTweener::CTween* oTween = new CDBTweener::CTween(pEquation, eEasing, fDuration, fpValue, fTarget);
	mTweens.push_back(oTween);
	mDurations.push_back(fDuration);
	mTotalTime += fDuration;
}

void Tween::clearTweens()
{
	for (auto tween : mTweens)
		delete tween;

	mTweens = vector<CDBTweener::CTween*>();
	mDurations = vector<float>();
	mTimer.restart();
	mTotalTime = 0;
}

bool Tween::update(sf::Time dt)
{

	if (mTimer.getElapsedTime().asSeconds() > mTotalTime)
		return true;

	
	float duration = 0;
	for (unsigned i = 0; i < mTweens.size(); i++)
	{
		duration += mDurations[i];
		if (mTimer.getElapsedTime().asSeconds() > duration-mDurations[i] &&  mTimer.getElapsedTime().asSeconds() < duration)
		{
			mTweens[i]->step(dt.asSeconds());
		}
	}
	

	//mTweens[0].step(dt.asSeconds());

	return false;
}

void Tween::restart()
{
	mTimer.restart();
	for (CDBTweener::CTween* tween : mTweens)
		tween->setElapsedSec(0);
}