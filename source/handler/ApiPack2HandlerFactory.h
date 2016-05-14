//
// Created by warmi on 5/7/16.
//

#ifndef APIPACK2_APIPACK2HANDLERFACTORY_H
#define APIPACK2_APIPACK2HANDLERFACTORY_H

#include <proxygen/httpserver/RequestHandlerFactory.h>

using namespace proxygen;


namespace ApiPack2
{

    class ApiPack2HandlerFactory : public RequestHandlerFactory
    {
    public:

        void onServerStart(folly::EventBase *evb) noexcept override;

        void onServerStop() noexcept override;

        RequestHandler *onRequest(RequestHandler *, HTTPMessage *) noexcept override;

    };

}

#endif //APIPACK2_APIPACK2HANDLERFACTORY_H
