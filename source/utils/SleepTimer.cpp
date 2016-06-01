//
// Created by warmi on 5/28/16.
//

#include "SleepTimer.h"

using namespace ApiPack2;

SleepTimer::SleepTimer(const std::initializer_list<size_t> &intervals):mIntervals(intervals),mOffset(0)
{

}

void SleepTimer::sleep()
{
    std::this_thread::sleep_for(std::chrono::seconds(mIntervals[mOffset]));
    if(mOffset<(mIntervals.size()-1))
        mOffset++;
}