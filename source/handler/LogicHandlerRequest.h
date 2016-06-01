//
// Created by warmi on 5/30/16.
//

#ifndef APIPACK2_LOGICHANDLERREQUEST_H
#define APIPACK2_LOGICHANDLERREQUEST_H

#include "StopTimer.h"
#include "LogicHandlerResponse.h"

namespace ApiPack2
{
    class IOHandler;

    enum class RequestStage : size_t
    {
        Ready,
        Begin ,
        Continue,
        End
    };


    class LogicHandlerRequest
    {
    public:

        LogicHandlerRequest():ioHandler(nullptr)
        {

        }


        LogicHandlerRequest(StopTimer timer,IOHandler *handler):requestTimer(timer),ioHandler(handler)
        {

        }

        StopTimer requestTimer;
        IOHandler *ioHandler;
    };

    class LogicHandlerRequestStage
    {
    public:

        LogicHandlerRequestStage()
        {
        }
        LogicHandlerRequestStage(const LogicHandlerRequest &r,size_t l,size_t b):request(r),numLines(l),batch(b),stage(RequestStage::Ready),ioCode(ResponseCode::Ready)
        {
            currLine=0;
        }

        RequestStage stage;
        ResponseCode ioCode;
        size_t      numLines;
        size_t      currLine;
        size_t      batch;
        LogicHandlerRequest request;
    };

}


#endif //APIPACK2_LOGICHANDLERREQUEST_H
