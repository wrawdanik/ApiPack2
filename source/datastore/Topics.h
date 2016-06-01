//
// Created by warmi on 5/5/16.
//

#ifndef APIPACK2_TOPICS_H
#define APIPACK2_TOPICS_H

#include <cstddef>
#include <ostream>

#include <folly/Format.h>
using namespace folly;

namespace ApiPack2
{
    enum class Topic : size_t
    {
        Metamorphosis_Station ,
        Metamorphosis_Linuep,
        Metamorphosis_Image,
        Metamorphosis_Person,
        Metamorphosis_Program_TmsId,
        Metamorphosis_Affiliation,
        Metamorphosis_Market,
        Metamorphosis_Schedule_Day,
        Metamorphosis_Provider,
        Metamorphosis_Headend,
        Metamorphosis_Rating_Body,
        Metamorphosis_Rating_Value,
        Metamorphosis_Animation,
        Metamorphosis_Award_Type,
        Metamorphosis_Award_Body,
        Metamorphosis_Genre,
        Metamorphosis_Production_Company,
        Metamorphosis_Advisory,
        Metamorphosis_Holiday,
        Metamorphosis_Playoff,
        Metamorphosis_Team,
        Metamorphosis_Venue,
        Metamorphosis_Warning,
        Metamorphosis_Conference,
        Metamorphosis_University
    };

    std::ostream& operator<< (std::ostream & os, ApiPack2::Topic topic);

    namespace TopicConversion
    {

        const char *toChar(ApiPack2::Topic topic);
    }
}

namespace folly
{

template <>
class FormatValue<ApiPack2::Topic>
{
public:

    explicit FormatValue(ApiPack2::Topic topic) : mTopic(topic) { }

    template <class FormatCallback>
    void format(FormatArg& arg, FormatCallback& cb) const
    {

        FormatValue<std::string>(ApiPack2::TopicConversion::toChar(mTopic)).format(arg, cb);
    }
private:

    ApiPack2::Topic mTopic;
};
}
#endif //APIPACK2_TOPICS_H
