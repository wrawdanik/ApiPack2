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

#include "ApiPack2HandlerFactory.h"

using namespace ApiPack2;
using namespace proxygen;

DEFINE_int32(http_port, 8080, "Port to listen on with HTTP protocol");
DEFINE_string(ip, "localhost", "IP/Hostname to bind to");
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


    HTTPServerOptions options;
    options.threads = static_cast<size_t>(4);
    options.idleTimeout = std::chrono::milliseconds(60000);
    options.shutdownOn = {SIGINT, SIGTERM};
    options.enableContentCompression = false;
    options.handlerFactories = RequestHandlerChain()
            .addThen<ApiPack2HandlerFactory>()
            .build();

    HTTPServer server(std::move(options));
    server.bind(IPs);

    // Start HTTPServer mainloop in a separate thread
    std::thread t([&] () {
        server.start();
    });

    t.join();
    return 0;

}