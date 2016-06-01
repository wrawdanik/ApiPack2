//
// Created by warmi on 5/7/16.
//

#include "IOHandler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <folly/io/async/EventBaseManager.h>

#include <glog/logging.h>

using namespace proxygen;

using folly::format;

namespace ApiPack2
{

    IOHandler::IOHandler(moodycamel::BlockingConcurrentQueue<LogicHandlerRequest> *queue)
            :mIsRunning(ATOMIC_FLAG_INIT),mRequestQueue(queue)
    {
        mStatus=ResponseCode::Ready;
        mResponseQueue=nullptr;

        mEventBase=folly::EventBaseManager::get()->getExistingEventBase();

        //LOG(INFO) << "Thread:" << std::this_thread::get_id()<< ". IOHandler created.";
        //LOG(INFO).flush();
    }

    void IOHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept
    {
        mHeaders=std::move(headers);



    }

    void IOHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept
    {
        if (mRequestBody)
        {
            mRequestBody->prependChain(std::move(body));
        } else
        {
            mRequestBody = std::move(body);
        }
    }

    void IOHandler::onEOM() noexcept
    {

        if(mRequestQueue)
        {
            mRequestQueue->enqueue(LogicHandlerRequest(StopTimer(),this));
        }

    }

    void IOHandler::onUpgrade(UpgradeProtocol protocol) noexcept
    {
        // no upgrades supported
    }

    void IOHandler::requestComplete() noexcept
    {
        mStatus=ResponseCode::Terminated;
        handleResponseNotification(mStatus);
    }

    void IOHandler::onError(ProxygenError err) noexcept
    {
        mStatus=ResponseCode::Error;
        handleResponseNotification(mStatus);
    }

    void IOHandler::onEgressPaused() noexcept
    {
        mStatus=ResponseCode::Paused;
        handleResponseNotification(mStatus);
    }

    void IOHandler::onEgressResumed() noexcept
    {
        mStatus=ResponseCode::Ready;
        handleResponseNotification(mStatus);
    }

    void IOHandler::setResponseQueue(BlockingConcurrentQueue<LogicHandlerResponse>  *queue)
    {
        mResponseQueue=queue;
        if(mStatus==ResponseCode::Paused || mStatus==ResponseCode::Error || mStatus==ResponseCode::Terminated)
            handleResponseNotification(mStatus);
    }

    void IOHandler::handleResponseNotification(ResponseCode status)
    {
        if(mResponseQueue!=nullptr)
        {
            mResponseQueue->enqueue(LogicHandlerResponse(status,this));
        }
    }

}