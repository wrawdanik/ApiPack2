//
// Created by warmi on 5/23/16.
//

#ifndef APIPACK2_SCHEDULEDAY_H
#define APIPACK2_SCHEDULEDAY_H

#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace ApiPack2
{

    class ScheduleDay
    {
    public:



        explicit ScheduleDay(uint32_t stationId, const std::string &date);
        explicit ScheduleDay(uint32_t stationId, uint32_t day,uint32_t month, uint32_t year);
        explicit ScheduleDay(uint32_t stationId, uint32_t day, uint32_t year);
        explicit ScheduleDay(uint64_t compoundId):mId(compoundId){}

        inline uint32_t stationId() const
        {
            return (mId&mPackedMask);
        }
        inline uint32_t day() const
        {
            return (mId>>(mPackWidth*2))&(mPackedMask);
        }
        inline uint32_t year() const
        {
            return (mId>>mPackWidth)&(mPackedMask);
        }
        inline uint64_t id() const { return mId;}

        inline bool isValid() const { return mId!=0;}

        static uint64_t create(uint32_t stationId, const std::string &date);
        static uint64_t create(uint32_t stationId, uint32_t day,uint32_t month, uint32_t year);
        static uint64_t create(uint32_t stationId, uint32_t day, uint32_t year);

        static void formatDay(const boost::gregorian::date &day,std::string &buffer);

    private:

        static const uint64_t mPackWidth=21;
        static const uint64_t mPackedMask=0xFFFFF;

        uint64_t  mId;

    };

}
#endif //APIPACK2_SCHEDULEDAY_H
