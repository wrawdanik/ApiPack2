//
// Created by warmi on 5/15/16.
//

#include "ThriftConverter.h"

#include <core/common_types.h>
#include <core/common_constants.h>
#include <core/station_types.h>
#include <core/lineup_types.h>
#include <core/image_types.h>
#include <core/person_types.h>
#include <TMSId-Programs/programTMS_types.h>
#include <core/affiliation_types.h>
#include <core/market_types.h>
#include <core/scheduleDay_types.h>
#include <core/provider_types.h>
#include <core/headend_types.h>
#include <core/ratingBody_types.h>
#include <core/ratingValue_types.h>
#include <core/animation_types.h>
#include <core/awardType_types.h>
#include <core/awardBody_types.h>
#include <core/genre_types.h>
#include <core/productionCompany_types.h>
#include <core/advisory_types.h>
#include <core/holiday_types.h>
#include <core/playoff_types.h>
#include <core/team_types.h>
#include <core/venue_types.h>
#include <core/warning_types.h>
#include <core/conference_types.h>
#include <core/university_types.h>


using namespace gracenote::metamorphosis::data::core;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace ApiPack2;

ThriftConverter::ThriftConverter(size_t initialBuffer)
{
    mMemBuffer= boost::shared_ptr<TMemoryBuffer>(new TMemoryBuffer((uint32_t )initialBuffer));
    mProtocol=boost::shared_ptr<TBinaryProtocol>(new TBinaryProtocol(mMemBuffer));

}
ThriftConverter::~ThriftConverter()
{

}

void* ThriftConverter::createPopulated(Topic topic,const unsigned char *data, size_t sizeInBytes)
{
    switch (topic)
    {
        case Topic::Metamorphosis_Station :
        {
            return ThriftConverterUtils::createAndPopulate<StationData>(data, sizeInBytes, mProtocol, mMemBuffer,
                                                                        topic);
        }
        case Topic::Metamorphosis_Linuep :
            return new gracenote::metamorphosis::data::core::LineupData();
        case Topic::Metamorphosis_Image :
            return new gracenote::metamorphosis::data::core::ImageData();
        case Topic::Metamorphosis_Person :
            return new gracenote::metamorphosis::data::core::PersonData();
        case Topic::Metamorphosis_Program_TmsId :
            return new ProgramTMSId();
        case Topic::Metamorphosis_Affiliation :
            return new gracenote::metamorphosis::data::core::AffiliationData();
        case Topic::Metamorphosis_Market :
            return new gracenote::metamorphosis::data::core::MarketData();
        case Topic::Metamorphosis_Schedule_Day :
        {
            return ThriftConverterUtils::createAndPopulate<ScheduleDayData>(data, sizeInBytes, mProtocol, mMemBuffer,
                                                                            topic);
        }
        case Topic::Metamorphosis_Provider : return new gracenote::metamorphosis::data::core::ProviderData();
        case Topic::Metamorphosis_Headend : return new gracenote::metamorphosis::data::core::HeadendData();
        case Topic::Metamorphosis_Rating_Body : return new gracenote::metamorphosis::data::core::RatingBodyData();
        case Topic::Metamorphosis_Rating_Value : return new gracenote::metamorphosis::data::core::RatingValueData();
        case Topic::Metamorphosis_Animation : return  new gracenote::metamorphosis::data::core::AnimationData();
        case Topic::Metamorphosis_Award_Type : return  new gracenote::metamorphosis::data::core::AwardTypeData();
        case Topic::Metamorphosis_Award_Body : return  new gracenote::metamorphosis::data::core::AwardBodyData();
        case Topic::Metamorphosis_Genre : return  new gracenote::metamorphosis::data::core::GenreData();
        case Topic::Metamorphosis_Production_Company : return new gracenote::metamorphosis::data::core::ProductionCompanyData();
        case Topic::Metamorphosis_Advisory : return new gracenote::metamorphosis::data::core::AdvisoryData();
        case Topic::Metamorphosis_Holiday : return new gracenote::metamorphosis::data::core::HolidayData();
        case Topic::Metamorphosis_Playoff : return new gracenote::metamorphosis::data::core::PlayoffData();
        case Topic::Metamorphosis_Team : return new gracenote::metamorphosis::data::core::TeamData();
        case Topic::Metamorphosis_Venue : return new gracenote::metamorphosis::data::core::VenueData();
        case Topic::Metamorphosis_Warning : return  new gracenote::metamorphosis::data::core::WarningData();
        case Topic::Metamorphosis_Conference : return  new gracenote::metamorphosis::data::core::ConferenceData();
        case Topic::Metamorphosis_University : return  new gracenote::metamorphosis::data::core::UniversityData();
    };

    return nullptr;  // in case we forget to handle a new enum
}

/*
bool ThriftConverter::populate(const unsigned char *data, size_t sizeInBytes, void *object)
{
    mMemBuffer->resetBuffer((uint8_t *)data,(uint32_t)sizeInBytes);
    populateThrift((gracenote::metamorphosis::data::core::WarningData*)object,mProtocol);


    return false;
}
void* ThriftConverter::createPopulated(const unsigned char *data, size_t sizeInBytes,Topic topic)
{
    void *object=create(topic);

    if(object!= nullptr)
    {
        if(!populate(data,sizeInBytes,object))
        {
            delete object;
            object= nullptr;
        }
    }

    return object;
}

 */