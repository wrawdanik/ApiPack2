//
// Created by warmi on 5/7/16.
//

#include "IOHandlerFactory.h"
#include "IOHandler.h"
#include <folly/io/async/EventBaseManager.h>

using namespace ApiPack2;
using namespace proxygen;


IOHandlerFactory::IOHandlerFactory(moodycamel::BlockingConcurrentQueue<LogicHandlerRequest> *queue):mQueue(queue)
{

}

void IOHandlerFactory::onServerStart(folly::EventBase *eBase) noexcept
{
    mEventBase=eBase;
    LOG(INFO).flush()  << "Creating IOHandlerFactory with thread id :"<<std::this_thread::get_id();
}

void IOHandlerFactory::onServerStop() noexcept
{

}

RequestHandler * IOHandlerFactory::onRequest(RequestHandler *next, HTTPMessage *msg) noexcept
{
    //LOG(INFO) << "Got request  :" << msg->getURL();

   // LOG(INFO).flush() << "Thread:" << std::this_thread::get_id()<< ". IOHandlerFactory responding to request " << msg->getURL() << "for event base "<<mEventBase;


    return new IOHandler(mQueue);
}