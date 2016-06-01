//
// Created by warmi on 5/28/16.
//

#ifndef APIPACK2_SLEEPTIMER_H
#define APIPACK2_SLEEPTIMER_H

#include <thread>
#include <folly/small_vector.h>

namespace ApiPack2
{
    class SleepTimer
    {
    public:

        SleepTimer(const std::initializer_list<size_t> &intervals);

        void sleep();
        inline void reset() { mOffset=0;}

    private:

        folly::small_vector<size_t, 16> mIntervals;
        size_t mOffset;
    };

}
#endif //APIPACK2_SLEEPTIMER_H
