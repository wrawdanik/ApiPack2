//
// Created by warmi on 5/31/16.
//

#ifndef APIPACK2_LOGINHANDLERRESPONSE_H
#define APIPACK2_LOGINHANDLERRESPONSE_H

#include <cstddef>

namespace ApiPack2
{
    class IOHandler;

    enum class ResponseCode : size_t
    {
        Ready,
        Error,
        Terminated,
        Paused
    };


    class LogicHandlerResponse
    {
    public:
        LogicHandlerResponse():code(ResponseCode::Ready)
        {

        }

        LogicHandlerResponse(ResponseCode pCode, IOHandler *pHandler):code(pCode),ioHandler(pHandler)
        {

        }


        ResponseCode code;
        IOHandler *ioHandler;
    };
}

#endif //APIPACK2_LOGINHANDLERRESPONSE_H
