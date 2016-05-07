//
// Created by warmi on 5/6/16.
//

#ifndef APIPACK2_CACHESTATISTICS_H
#define APIPACK2_CACHESTATISTICS_H

#include <cstddef>
#include <cstdint>

namespace ApiPack2
{
    class CacheStatistics
    {
    public:

        size_t sizeInBytes;
        size_t sizeEntries;
        size_t populationRatio;
        uint64_t    lastUpdated;
        uint64_t    numRequests;

    };

};
#endif //APIPACK2_CACHESTATISTICS_H
