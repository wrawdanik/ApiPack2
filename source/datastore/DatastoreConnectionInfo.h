//
// Created by warmi on 5/28/16.
//

#ifndef APIPACK2_DATASTORECONNECTIONINFO_H
#define APIPACK2_DATASTORECONNECTIONINFO_H

#include <folly/dynamic.h>

using namespace folly;

namespace ApiPack2
{
    class DatastoreConnectionInfo
    {
    public:
        DatastoreConnectionInfo(dynamic &config);

        std::string& getNextUri();

        inline size_t getDefaultFetchSize() const
        {
            return mDefaultFetchSize;
        }

        inline size_t size() const
        {
            return mConnectionStrings.size();
        }
    private:

        size_t  mDefaultFetchSize;
        std::vector<std::string> mConnectionStrings;
        size_t mLastUri;
        std::string mEmpty;

    };
}

#endif //APIPACK2_DATASTORECONNECTIONINFO_H
