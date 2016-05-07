//
// Created by warmi on 5/5/16.
//

#ifndef APIPACK2_TOPICS_H
#define APIPACK2_TOPICS_H

#include <cstddef>

namespace ApiPack2
{
    enum class Topic : size_t
    {
        Program ,
        ScheduleDay,
        Lineup,
        Headend,
        Station,
        Image,
        Provider,
        Market
    };
}

#endif //APIPACK2_TOPICS_H
