//
// Created by warmi on 5/7/16.
//

#include <cstdio>

#include <glog/logging.h>

#include <gflags/gflags.h>
#include <folly/Memory.h>
#include <folly/Portability.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <unistd.h>

#include "IOHandlerFactory.h"


#include "blockingconcurrentqueue.h"
#include "LogicHandler.h"

using namespace ApiPack2;
using namespace proxygen;
using namespace moodycamel;

DEFINE_int32(http_port, 8080, "Port to listen on with HTTP protocol");
DEFINE_string(ip, "172.16.0.119", "IP/Hostname to bind to");
DEFINE_int32(threads, 0, "Number of threads to listen on. Numbers <= 0 "
             "will use the number of cores on this machine.");


using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;

int main(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();


    LOG(INFO) << "ApiPack2 started with 4 threads .... ";

    std::vector<HTTPServer::IPConfig> IPs = {
            {SocketAddress(FLAGS_ip, FLAGS_http_port, true), HTTPServer::Protocol::HTTP}
    };

    BlockingConcurrentQueue<LogicHandlerRequest> *queue=new BlockingConcurrentQueue<LogicHandlerRequest>(256);


    HTTPServerOptions options;
    options.threads = static_cast<size_t>(4);
    options.idleTimeout = std::chrono::milliseconds(60000);
    options.shutdownOn = {SIGINT, SIGTERM};
    options.enableContentCompression = false;
    options.handlerFactories = RequestHandlerChain()
            .addThen<IOHandlerFactory>(queue)
            .build();

    LogicHandler *mHandler1=new LogicHandler(queue);
    mHandler1->start();

    LogicHandler *mHandler2=new LogicHandler(queue);
    mHandler2->start();

    LogicHandler *mHandler3=new LogicHandler(queue);
    mHandler3->start();

    LogicHandler *mHandler4=new LogicHandler(queue);
    mHandler4->start();

    HTTPServer server(std::move(options));
    server.bind(IPs);

    // Start HTTPServer mainloop in a separate thread
    std::thread t([&] () {
        server.start();
    });

    t.join();

    delete mHandler1;
    delete mHandler2;
    delete mHandler3;
    delete mHandler4;

    LOG(INFO) << "ApiPack2 finished .... ";

    return 0;

}