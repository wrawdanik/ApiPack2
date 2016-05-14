//
// Created by warmi on 5/5/16.
//

#ifndef APIPACK2_DATAOBJECT_H
#define APIPACK2_DATAOBJECT_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

namespace ApiPack2
{

    class DataObject
    {
    public:

        long version;
        //public TBase *data;
    };


}

#endif //APIPACK2_DATAOBJECT_H
