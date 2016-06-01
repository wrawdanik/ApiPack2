//
// Created by warmi on 5/15/16.
//

#ifndef APIPACK2_THRIFTCONVERTER_H
#define APIPACK2_THRIFTCONVERTER_H

#include "Topics.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TTransportUtils.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace ApiPack2
{

    class ThriftConverter
    {
    public:

        ThriftConverter(size_t initialBuffer=1024*256);
        ~ThriftConverter();

        void *createPopulated(Topic topic,const unsigned char *data=0, size_t sizeInBytes=0);

    private:

        boost::shared_ptr<TMemoryBuffer> mMemBuffer;
        boost::shared_ptr<TBinaryProtocol> mProtocol;
    };

    namespace ThriftConverterUtils
    {

        template<class T>
        T *createAndPopulate(const unsigned char *data, size_t sizeInBytes, boost::shared_ptr<TBinaryProtocol> protocol,
                             boost::shared_ptr<TMemoryBuffer> buffer, Topic topic)
        {
            T *thriftObject = new T;
            if (data != nullptr)
            {
                buffer->resetBuffer((uint8_t *) data, (uint32_t) sizeInBytes);
                //TBinaryProtocol thisProtocol(buffer);
                try
                {
                    thriftObject->read(protocol.get());


                }
                catch(TTransportException& ttx)
                {
                    LOG(ERROR) << "Exception serializing object of topic " << topic << " : "<<ttx.what();
                    delete thriftObject;
                    thriftObject= nullptr;
                }

            }
            return thriftObject;
        }
    }

}
#endif //APIPACK2_THRIFTCONVERTER_H
