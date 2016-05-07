//
// Created by warmi on 5/6/16.
//

#ifndef APIPACK2_DATAOBJECTCACHE_H
#define APIPACK2_DATAOBJECTCACHE_H

#include <cstddef>
#include "CacheEntry.h"
#include <unordered_map>

namespace ApiPack2
{
    class DataObject;

    class GridCache
    {
    public:

        GridCache(size_t numberOfDaysBack,size_t numberOfDaysForward, size_t numberOfStations);

        ~GridCache();

        CacheEntryArray* array(long station,bool createIfMissing=false);

        inline size_t numberOfDaysBack() const { return mDaysBack;}
        inline size_t numberOfDaysForward() const { return mDaysForward;}
        inline size_t numberOfStations() const { return mNumStations;}


    private:

        size_t mDaysBack;
        size_t mDaysForward;
        size_t mNumStations;

        using StationScheduleCache=std::unordered_map<long,CacheEntryArray*>;


        StationScheduleCache *mCache;
    };

}
#endif //APIPACK2_DATAOBJECTCACHE_H
