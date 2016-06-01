//
// Created by warmi on 5/23/16.
//

#include "ScheduleDay.h"
#include <folly/Format.h>
using namespace folly;

using namespace ApiPack2;

ScheduleDay::ScheduleDay(uint32_t stationId, const std::string &date)
{
    mId=create(stationId,date);
}

ScheduleDay::ScheduleDay(uint32_t stationId, uint32_t day,uint32_t month, uint32_t year)
{
    mId=create(stationId,day,month,year);
}
ScheduleDay::ScheduleDay(uint32_t stationId, uint32_t day, uint32_t year)
{
    mId=create(stationId,day,year);
}

uint64_t ScheduleDay::create(uint32_t stationId, const std::string &date)
{
    uint64_t val=0;

    try
    {
        boost::gregorian::date d(boost::gregorian::from_simple_string(date));
        val = ((uint64_t) stationId & (mPackedMask));
        val = (val & (~(mPackedMask << (mPackWidth * 2)))) | ((uint64_t) d.day_of_year() << (mPackWidth * 2));
        val = (val & (~(mPackedMask << (mPackWidth)))) | ((uint64_t) d.year() << (mPackWidth));
    }
    catch(...)
    {
        LOG(INFO) << "Invalid date " << date ;
    }
    return val;
}
uint64_t ScheduleDay::create(uint32_t stationId, uint32_t day,uint32_t month, uint32_t year)
{
    uint64_t val=0;
    try {
        boost::gregorian::date d(year, month, day);
        uint32_t dayNumber = d.day_of_year();
        val=((uint64_t)stationId&(mPackedMask));
        val=(val&(~(mPackedMask<<(mPackWidth*2))))|((uint64_t)d.day_of_year()<<(mPackWidth*2));
        val=(val&(~(mPackedMask<<(mPackWidth))))|((uint64_t)year<<(mPackWidth));

    }
    catch (std::out_of_range& e) {
        LOG(INFO) << "Invalid month " << month << " or year "<<year ;

    }
    return val;
}
uint64_t ScheduleDay::create(uint32_t stationId, uint32_t day, uint32_t year)
{
    uint64_t val=((uint64_t)stationId&(mPackedMask));
    val=(val&(~(mPackedMask<<(mPackWidth*2))))|((uint64_t)day<<(mPackWidth*2));
    val=(val&(~(mPackedMask<<(mPackWidth))))|((uint64_t)year<<(mPackWidth));
    return val;
}

void ScheduleDay::formatDay(const boost::gregorian::date &day,std::string &buffer)
{
    toAppend((unsigned short)day.year(),"-",(unsigned short)day.month(),"-",(unsigned short)day.day(),&buffer);
}