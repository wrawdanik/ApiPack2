//
// Created by warmi on 5/7/16.
//

#ifndef APIPACK2_APIPACK2HANDLER_H
#define APIPACK2_APIPACK2HANDLER_H

#include <folly/Memory.h>
#include <proxygen/httpserver/RequestHandler.h>


#include "LogicHandlerRequest.h"
#include "blockingconcurrentqueue.h"
#include "LogicHandlerResponse.h"

using namespace moodycamel;

namespace proxygen
{
    class ResponseHandler;
}

namespace folly
{
    class EventBase;
}

namespace ApiPack2
{

    class IOHandler : public proxygen::RequestHandler
    {


    public:




        explicit IOHandler(moodycamel::BlockingConcurrentQueue<LogicHandlerRequest> *queue);

        void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
                noexcept override;

        void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

        void onEOM() noexcept override;

        void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

        void requestComplete() noexcept override;

        void onError(proxygen::ProxygenError err) noexcept override;

        void onEgressPaused() noexcept override;

        void onEgressResumed() noexcept override;

        inline proxygen::ResponseHandler*  responseHandler() const
        {
            return downstream_;
        }
        inline folly::EventBase*  eventBase() const
        {
            return mEventBase;
        }

        const proxygen::HTTPMessage* requestData() const
        {
            return mHeaders.get();
        }

        const folly::IOBuf* requestBody() const
        {
            return mRequestBody.get();
        }

        ResponseCode status() const
        {
            return mStatus;
        }

        void setResponseQueue(BlockingConcurrentQueue<LogicHandlerResponse>  *queue);
    private:



        void handleResponseNotification(ResponseCode status);

        std::unique_ptr<folly::IOBuf> mRequestBody;
        folly::EventBase *mEventBase;
        mutable std::atomic_flag mIsRunning;
        BlockingConcurrentQueue<LogicHandlerRequest> *mRequestQueue;
        std::unique_ptr<proxygen::HTTPMessage> mHeaders;
        BlockingConcurrentQueue<LogicHandlerResponse>  *mResponseQueue;

        ResponseCode  mStatus;
    };
}

#endif //APIPACK2_APIPACK2HANDLER_H


