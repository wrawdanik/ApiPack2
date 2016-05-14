//
// Created by warmi on 5/7/16.
//

#include "ApiPack2HandlerFactory.h"
#include "ApiPack2Handler.h"

using namespace ApiPack2;
using namespace proxygen;

void ApiPack2HandlerFactory::onServerStart(folly::EventBase *evb) noexcept
{
    LOG(INFO) << "Creating ApiPack2HandlerFactory with thread id :"<<std::this_thread::get_id();
}

void ApiPack2HandlerFactory::onServerStop() noexcept
{

}

RequestHandler * ApiPack2HandlerFactory::onRequest(RequestHandler *next, HTTPMessage *msg) noexcept
{
    //LOG(INFO) << "Got request  :" << msg->getURL();

    return new ApiPack2Handler();
}