//
// Created by warmi on 5/6/16.
//

#ifndef APIPACK2_CACHEENTRY_H
#define APIPACK2_CACHEENTRY_H

#include <cstddef>
#include <cstdint>
#include <vector>


namespace ApiPack2
{
    class DataObject;

    union CacheEntryMeta
    {
        struct
        {
            size_t ttl:             48;
            size_t lockCount:       16;
        };
        uint64_t value;
    };


    struct CacheEntry
    {
        DataObject *data;
        CacheEntryMeta meta;

        inline bool locked() const { return (meta.lockCount>0);}
        inline void lock() { meta.lockCount++;}
        inline void unlock() { meta.lockCount--;}
    };

    using CacheEntryArray=std::vector<CacheEntry>;

}

#endif //APIPACK2_CACHEENTRY_H
