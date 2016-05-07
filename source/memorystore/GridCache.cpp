//
// Created by warmi on 5/6/16.
//

#include <cstring>
#include "GridCache.h"

using namespace ApiPack2;

GridCache::GridCache(size_t numberOfDaysBack,size_t numberOfDaysForward, size_t numberOfStations)
        :mDaysForward(numberOfDaysForward),mDaysBack(numberOfDaysBack),mNumStations(numberOfStations)
{
    //mArray=new CacheEntryArray(numberOfDaysBack*numberOfDaysForward*numberOfStations);

    mCache=new StationScheduleCache(numberOfStations);



}

GridCache::~GridCache()
{

}

CacheEntryArray* GridCache::array(long station, bool createIfMissing)
{
    auto found=mCache->find(station);

    if(found!=mCache->end())
    {
        return found->second;
    }
    if(createIfMissing)
    {
        CacheEntryArray *entry=new CacheEntryArray(mDaysBack*mDaysForward);
        memset(&entry[0],0,entry->size());
        mCache->insert(std::pair<long,CacheEntryArray*>(station,entry));
        return entry;
    }

    return nullptr;
}
