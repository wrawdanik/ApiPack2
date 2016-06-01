//
// Created by warmi on 5/8/16.
//

#ifndef APIPACK2_LOGICHANDLER_H
#define APIPACK2_LOGICHANDLER_H

#include <thread>
#include <atomic>
#include <unordered_map>

#include "blockingconcurrentqueue.h"
#include "LogicHandlerRequest.h"
#include "LogicHandlerResponse.h"

using namespace moodycamel;

namespace ApiPack2
{

    class LogicHandler
    {
    public:
        LogicHandler(BlockingConcurrentQueue<LogicHandlerRequest> *queue);
        ~LogicHandler();


        void    start();
        void    stop();
        void    stopAndWait();

        static void executor_func(void * arg);

        using RequestMap=std::unordered_map<IOHandler*,LogicHandlerRequestStage>;

    private:

        void signalBusyMessage(LogicHandlerRequest &request);

        void handleRequestLogic(LogicHandlerRequestStage &request);
        void logRequestStart(LogicHandlerRequest &request);
        void logRequestEnd(LogicHandlerRequest &request);

        std::thread     *mThread;
        std::atomic_flag mIsRunning;

        BlockingConcurrentQueue<LogicHandlerRequest>  *mRequestQueue;
        BlockingConcurrentQueue<LogicHandlerResponse>  *mResponseQueue;
        RequestMap mRequestMap;
        size_t  mMaxRequest;
        std::int64_t    mWaitDurationBusy;
        std::int64_t    mWaitDurationIdle;
    };

}


#endif //APIPACK2_LOGICHANDLER_H
