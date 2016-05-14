//
// Created by warmi on 5/14/16.
//

#ifndef APIPACK2_SERIALIZEDDATAOBJECT_H
#define APIPACK2_SERIALIZEDDATAOBJECT_H
#include <string>
#include <vector>
#include "Topics.h"
#include "DatastoreIterator.h"

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

        struct Header
        {
            std::string fileName;
            Topic topic;
            long offset;
            std::string &date;
        };

        SerializedDataObject(const std::string &filePath);

        void beginSerialize(Header header);

        void serialize(DataObjectArray &array);

        void endSerialize();

        Header beginDeserialize();

        SerializedDataObjectIterator deserialize();

    };

}
#endif //APIPACK2_SERIALIZEDDATAOBJECT_H
