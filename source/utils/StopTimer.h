//
// Created by Walter Rawdanik on 9/25/15.
//

#ifndef APIPACK_STOPTIMER_H
#define APIPACK_STOPTIMER_H


#include <chrono>
#include <thread>

namespace ApiPack2
{
    using namespace std;

    class StopTimer
    {
        using HighResClock = std::chrono::high_resolution_clock;
    public:

        using ElapsedTime=std::pair<uint64_t,float> ;

        StopTimer()
        {
            restart();
        }

        inline uint64_t precision() const { return HighResClock::period::den; }

        static inline uint64_t systemTime()
        {
            return chrono::duration_cast<std::chrono::duration<std::uint64_t>>(HighResClock::now().time_since_epoch()).count();
        }

        inline void restart() { mPrevValue=HighResClock::now();}
        inline uint64_t elapsedTimeMilliseconds() const
        {
            return chrono::duration_cast<chrono::duration<uint64_t,std::milli>>(HighResClock::now() - mPrevValue).count();
        }
        inline ElapsedTime	elapsedTime() const
        {
            auto elapsed=HighResClock::now() - mPrevValue;
            return std::make_pair(chrono::duration_cast<chrono::duration<uint64_t,std::milli>>(elapsed).count(),(chrono::duration_cast<chrono::duration<float,std::milli>>(elapsed).count())/1000.0f);
        }

        static void sleep(size_t millis)
        {
            std::this_thread::sleep_for(chrono::milliseconds(millis));
        }

    protected:
        mutable HighResClock::time_point mPrevValue;


    };

}


#endif //APIPACK_STOPTIMER_H
