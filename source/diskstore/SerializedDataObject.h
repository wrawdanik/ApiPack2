//
// Created by warmi on 5/14/16.
//

#ifndef APIPACK2_SERIALIZEDDATAOBJECT_H
#define APIPACK2_SERIALIZEDDATAOBJECT_H
#include <string>
#include <vector>
#include "../datastore/Topics.h"
#include "../datastore/DatastoreIterator.h"

namespace ApiPack2
{
    class DataObject;

    using DataObjectArray=std::vector<DataObject*>;


    class SerializedDataObjectIterator
    {
    public:

        SerializedDataObjectIterator();
        ~SerializedDataObjectIterator();

        DataObject* next();

    };



    class SerializedDataObject
    {
    public:


        SerializedDataObject(const std::string &filePath,Topic topic);

        void beginSerialize();

        void serialize(DataObjectArray &array);

        void endSerialize();

        SerializedDataObjectIterator deserialize();

    };

}
#endif //APIPACK2_SERIALIZEDDATAOBJECT_H
