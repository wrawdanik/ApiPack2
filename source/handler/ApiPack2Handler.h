//
// Created by warmi on 5/7/16.
//

#ifndef APIPACK2_APIPACK2HANDLER_H
#define APIPACK2_APIPACK2HANDLER_H

#include <folly/Memory.h>
#include <proxygen/httpserver/RequestHandler.h>

namespace proxygen
{
    class ResponseHandler;
}

namespace ApiPack2
{

    class ApiPack2Handler : public proxygen::RequestHandler
    {


    public:
        explicit ApiPack2Handler();

        void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
                noexcept override;

        void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

        void onEOM() noexcept override;

        void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

        void requestComplete() noexcept override;

        void onError(proxygen::ProxygenError err) noexcept override;

        void onEgressPaused() noexcept override;

        void onEgressResumed() noexcept override;

    private:

        std::unique_ptr<folly::IOBuf> body_;

        size_t mRepCount;
        std::string mText;
    };
}

#endif //APIPACK2_APIPACK2HANDLER_H


