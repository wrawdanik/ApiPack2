//
// Created by warmi on 5/7/16.
//

#ifndef APIPACK2_APIPACK2HANDLERFACTORY_H
#define APIPACK2_APIPACK2HANDLERFACTORY_H

#include <proxygen/httpserver/RequestHandlerFactory.h>
#include "blockingconcurrentqueue.h"
#include "LogicHandlerRequest.h"

using namespace proxygen;


namespace ApiPack2
{

    class IOHandlerFactory : public RequestHandlerFactory
    {
    public:

        IOHandlerFactory(moodycamel::BlockingConcurrentQueue<LogicHandlerRequest> *queue);

        void onServerStart(folly::EventBase *evb) noexcept override;

        void onServerStop() noexcept override;

        RequestHandler *onRequest(RequestHandler *, HTTPMessage *) noexcept override;

    private:
        folly::EventBase *mEventBase;
        moodycamel::BlockingConcurrentQueue<LogicHandlerRequest> *mQueue;
    };

}

#endif //APIPACK2_APIPACK2HANDLERFACTORY_H
