//
// Created by warmi on 5/28/16.
//

#include "DatastoreConnectionInfo.h"

using namespace ApiPack2;

DatastoreConnectionInfo::DatastoreConnectionInfo(dynamic &config):mDefaultFetchSize(500)
{
    if(config["database"]!= nullptr)
    {
        if (config["database"]["connections"]!= nullptr)
        {
            for (auto &val : config["database"]["connections"])
            {
                mConnectionStrings.push_back(val["uri"].asString());
            }
        }
        if (config["database"]["fetchSize"]!= nullptr)
        {
            mDefaultFetchSize=(size_t)config["database"]["fetchSize"].asInt();
        }
    }
    mLastUri=0;
}


std::string& DatastoreConnectionInfo::getNextUri()
{
    if(!mConnectionStrings.empty())
    {
        mLastUri++;
        if(mLastUri==mConnectionStrings.size())
        {
            mLastUri=0;
        }
        return mConnectionStrings[mLastUri];
    }
    return mEmpty;
}
