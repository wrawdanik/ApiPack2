//
// Created by warmi on 5/7/16.
//

#include "ApiPack2Handler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include <glog/logging.h>

using namespace proxygen;

using folly::format;

namespace ApiPack2
{

    ApiPack2Handler::ApiPack2Handler()
    {
        mRepCount=0;
    }

    void ApiPack2Handler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept
    {
        const std::string *param1=headers->getQueryParamPtr("repCount");
        const std::string *param2=headers->getQueryParamPtr("text");

        mRepCount=1;

        if(param1!= nullptr)
        {
            mRepCount=atoi(param1->c_str());
        }

        if(param2!= nullptr)
        {
            mText=std::string(*param2);
        }
        else
        {
            mText=std::string("Line of text #");
        }

    }

    void ApiPack2Handler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept
    {
        if (body_)
        {
            body_->prependChain(std::move(body));
        } else
        {
            body_ = std::move(body);
        }
    }

    void ApiPack2Handler::onEOM() noexcept
    {

        ResponseBuilder(downstream_)
                .status(200, "OK")
                .send();

        for(int i=0;i<mRepCount;i++)
        {
            std::string out=folly::sformat("{} {}\n", mText, mRepCount);

            ResponseBuilder(downstream_)
                    .body(std::move(out))
                    .send();
        }

        ResponseBuilder(downstream_)
                .sendWithEOM();
    }

    void ApiPack2Handler::onUpgrade(UpgradeProtocol protocol) noexcept
    {
        // handler doesn't support upgrades
    }

    void ApiPack2Handler::requestComplete() noexcept
    {
        delete this;
    }

    void ApiPack2Handler::onError(ProxygenError err) noexcept
    {
        delete this;
    }

    void ApiPack2Handler::onEgressPaused() noexcept
    {
        LOG(INFO) << "Got onEgressPaused";
    }

    void ApiPack2Handler::onEgressResumed() noexcept
    {
        LOG(INFO) << "Got onEgressResumed";
    }

}