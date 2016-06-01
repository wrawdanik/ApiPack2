//
// Created by warmi on 5/8/16.
//

#include "LogicHandler.h"
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <glog/logging.h>
#include "IOHandler.h"
#include <folly/FBString.h>

using namespace ApiPack2;
using namespace folly;
using namespace moodycamel;
using namespace proxygen;


LogicHandler::LogicHandler(BlockingConcurrentQueue<LogicHandlerRequest> *queue) : mIsRunning(ATOMIC_FLAG_INIT),mRequestQueue(queue)
{
    mThread = nullptr;
    mMaxRequest=128;
    mWaitDurationBusy=50;
    mWaitDurationIdle=1000;
    mRequestMap.reserve(mMaxRequest);

    mResponseQueue=new BlockingConcurrentQueue<LogicHandlerResponse>(mMaxRequest);

}

LogicHandler::~LogicHandler()
{
    if(mThread && mThread->joinable())
    {
        LOG(INFO).flush()  << "Request to terminate Thread " << mThread->get_id()<< ".";
        if(mThread->joinable())
        {
            stop();
            mThread->join();
            delete mThread;
            mThread = nullptr;
        }
    }

    delete mResponseQueue;
}


void    LogicHandler::start()
{
    if (mThread == nullptr)
    {

        mIsRunning.test_and_set();

        mThread = new std::thread(
                [this]
                {
                    LogicHandler::executor_func(this);
                });
    }
}

void    LogicHandler::stopAndWait()
{
    mIsRunning.clear();
    mThread->join();
}

void    LogicHandler::stop()
{
    mIsRunning.clear();
}

void LogicHandler::signalBusyMessage(LogicHandlerRequest &request)
{
    request.ioHandler->eventBase()->runInEventBaseThread(
            [&, downstream=request.ioHandler->responseHandler()]() mutable
            {

                ResponseBuilder(downstream)
                        .status(503 , "Server at capacity.")
                        .sendWithEOM();
            });
}

void LogicHandler::executor_func(void * arg)
{

    LogicHandler *executor = (LogicHandler *) arg;


    if(executor->mRequestQueue!= nullptr)
    {
        LOG(INFO).flush()  << "Worker Thread " << std::this_thread::get_id()<< " is now running.";
        while (executor->mIsRunning.test_and_set())
        {
            if(executor->mRequestMap.size()<executor->mMaxRequest)
            {
                LogicHandlerRequest request;
                std::int64_t wait = executor->mWaitDurationIdle;

                if (!executor->mRequestMap.empty())
                {
                    wait = executor->mWaitDurationBusy;
                }

                if(executor->mRequestQueue->wait_dequeue_timed(request, wait * 1000))
                {
                    executor->mRequestMap.insert({request.ioHandler, LogicHandlerRequestStage(request, 100000,100)});
                }

                // process IOResponses and update all requests status values

                LogicHandlerResponse response;
                while(executor->mResponseQueue->try_dequeue(response))
                {
                    if(response.code==ResponseCode::Error )
                    {
                        LOG(INFO) << "Recorded request error ";
                        executor->mRequestMap.erase(response.ioHandler);
                    }
                    if(response.code==ResponseCode::Terminated )
                    {
                        LOG(INFO) << "Request finished ";
                        executor->mRequestMap.erase(response.ioHandler);
                    }
                    else if(response.code==ResponseCode::Paused)
                    {
                        auto found=executor->mRequestMap.find(response.ioHandler);
                        if(found!=executor->mRequestMap.end())
                        {
                            found->second.ioCode=response.code;
                        }
                    }
                    else if(response.code==ResponseCode::Ready)
                    {
                        auto found=executor->mRequestMap.find(response.ioHandler);
                        if(found!=executor->mRequestMap.end())
                        {
                            found->second.ioCode=response.code;
                        }
                    }
                }


               // process requests - everything that is not Paused will get their data pushed  to the IOHandler

                for( RequestMap::value_type p : executor->mRequestMap)
                {
                    if(p.second.ioCode!=ResponseCode ::Paused)
                    {
                        executor->handleRequestLogic(p.second);
                    }
                }
/*
                if (ready)
                {

                    request.ioHandler->eventBase()->runInEventBaseThread(
                            [&, downstream=request.ioHandler->responseHandler(), id=request.ioHandler->requestData()->getQueryParamPtr("id")]() mutable
                            {

                                ResponseBuilder(downstream)
                                        .status(200, "OK")
                                        .send();

                                int totalLines = 100000;

                                int insidePass = 1000;
                                int outsidePass = 120;

                                fbstring out;
                                out.reserve(4096 * 10);

                                for (int j = 0; j < outsidePass; j++)
                                {
                                    out.clear();
                                    for (int i = 0; i < insidePass; i++)
                                    {
                                        toAppend("0x7f7e28004608 ", i, "\n", &out);
                                    }
                                    ResponseBuilder(downstream)
                                            .body(std::move(out))
                                            .send();
                                }

                                ResponseBuilder(downstream)
                                        .sendWithEOM();

                            });
                }
                */
            }

            //mCommonBuffer.wait_dequeue(task);
        }
        LOG(INFO).flush()  << "Worker Thread " << std::this_thread::get_id()<< " terminating due to stop request.";
    }
    else
    {
        LOG(ERROR).flush()  << "Worker Thread " << std::this_thread::get_id()<< " cannot start.IO queue unavailable.";
    }

}

void LogicHandler::handleRequestLogic(LogicHandlerRequestStage &request)
{

}

void LogicHandler::logRequestStart(LogicHandlerRequest &request)
{

}
void LogicHandler::logRequestEnd(LogicHandlerRequest &request)
{

}