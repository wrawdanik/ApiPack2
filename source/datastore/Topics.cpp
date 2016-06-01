//
// Created by warmi on 5/14/16.
//

#include "Topics.h"

using namespace ApiPack2;

std::ostream& ApiPack2::operator<< (std::ostream & os, ApiPack2::Topic topic)
{
    return os << TopicConversion::toChar(topic);
}


const char *ApiPack2::TopicConversion::toChar( ApiPack2::Topic topic)
{
    switch (topic)
    {
        case Topic::Metamorphosis_Station : return "metamorphosis.station" ;
        case Topic::Metamorphosis_Linuep : return "metamorphosis.lineup";
        case Topic::Metamorphosis_Image : return "metamorphosis.image";
        case Topic::Metamorphosis_Person : return  "metamorphosis.person";
        case Topic::Metamorphosis_Program_TmsId : return "metamorphosis.program.tmsId";
        case Topic::Metamorphosis_Affiliation : return "metamorphosis.affiliation";
        case Topic::Metamorphosis_Market : return "metamorphosis.market";
        case Topic::Metamorphosis_Schedule_Day : return "metamorphosis.schedule.day";
        case Topic::Metamorphosis_Provider : return "metamorphosis.provider" ;
        case Topic::Metamorphosis_Headend : return "metamorphosis.headend";
        case Topic::Metamorphosis_Rating_Body : return  "metamorphosis.rating.body";
        case Topic::Metamorphosis_Rating_Value : return  "metamorphosis.rating.value";
        case Topic::Metamorphosis_Animation : return  "metamorphosis.animation";
        case Topic::Metamorphosis_Award_Type : return  "metamorphosis.award.type";
        case Topic::Metamorphosis_Award_Body : return  "metamorphosis.award.body";
        case Topic::Metamorphosis_Genre : return "metamorphosis.genre";
        case Topic::Metamorphosis_Production_Company : return  "metamorphosis.production.company";
        case Topic::Metamorphosis_Advisory : return "metamorphosis.advisory";
        case Topic::Metamorphosis_Holiday : return  "metamorphosis.holiday";
        case Topic::Metamorphosis_Playoff : return  "metamorphosis.playoff";
        case Topic::Metamorphosis_Team : return  "metamorphosis.team";
        case Topic::Metamorphosis_Venue : return  "metamorphosis.venue";
        case Topic::Metamorphosis_Warning : return  "metamorphosis.warning";
        case Topic::Metamorphosis_Conference : return  "metamorphosis.conference";
        case Topic::Metamorphosis_University : return  "metamorphosis.university";
    };
}
