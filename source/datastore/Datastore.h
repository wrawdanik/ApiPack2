//
// Created by warmi on 5/5/16.
//

#ifndef APIPACK2_DATASTORE_H
#define APIPACK2_DATASTORE_H

#include "DataObject.h"

#include <vector>
#include "Topics.h"
#include "DatastoreIterator.h"

namespace ApiPack2
{

    using IdArray=std::vector<long>;

    class Datastore
    {
    public:

        Datastore(const std::string &configFilePath);

        const std::string configurationFilePath() const
        {
            return mConfigFile;
        }


        DatastoreIteratorPtr dataObject(IdArray &idArray, Topic topic);

        // enable deletion of a Derived* through a Base*
        virtual ~Datastore() = default;

    private:

        std::string mConfigFile;

    };
}

#endif //APIPACK2_DATASTORE_H
