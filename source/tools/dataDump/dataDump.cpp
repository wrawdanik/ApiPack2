//
// Created by warmi on 5/14/16.
//

#include "Datastore.h"
#include "Topics.h"
#include <gflags/gflags.h>
#include <folly/Format.h>
#include <folly/FBString.h>
#include "folly/Conv.h"
#include "ThriftConverter.h"
#include <core/station_types.h>
#include <core/scheduleDay_types.h>
#include "ScheduleDay.h"
#include <folly/dynamic.h>
#include <folly/json.h>

#include <iostream>
#include <fstream>

#include "DatastoreConnectionInfo.h"
#include "SleepTimer.h"
#include "StopTimer.h"

using folly::sformat;
using folly::vformat;

using namespace folly;

using namespace ApiPack2;


void testScheduleQuery2(Datastore * store,IdArray &array, const boost::gregorian::date &dateFrom,const boost::gregorian::date &dateTo)
{
    store->setFetchSize(2);

    DatastoreIteratorPtr  iter=store->data(dateFrom,dateTo,array);

    ThriftConverter mConverter;

    if(iter.get()!= nullptr)
    {
        DataObject *data=nullptr;

        size_t count=0;

        while((data=iter->next())!=nullptr)
        {
            gracenote::metamorphosis::data::core::ScheduleDayData *sData = (gracenote::metamorphosis::data::core::ScheduleDayData *) mConverter.createPopulated(
                    Topic::Metamorphosis_Schedule_Day, (unsigned char *) data->thriftObject,
                    data->sizeInBytes);

            //LOG(INFO) << "Read schedule day id "<< sData->object.id << " action "<<sData->meta.action << " station "<< sData->object.stationId << " date " << sData->object.day;
            count++;
            delete data;

        }

        //LOG(INFO)<< "Got   "<<count << " ScheduleDay days ";

        if(iter->hasError())
        {
            LOG(INFO)<< "Error iterating over results "<<iter->error();
        }
    }
}

void testScheduleQuery(Datastore * store,IdArray &array, const std::string &dateFrom,const std::string &dateTo)
{
    store->setFetchSize(2);

    DatastoreIteratorPtr  iter=store->data(dateFrom,dateTo,array);

    ThriftConverter mConverter;

    if(iter.get()!= nullptr)
    {
        DataObject *data=nullptr;

        size_t count=0;

        while((data=iter->next())!=nullptr)
        {
                gracenote::metamorphosis::data::core::ScheduleDayData *sData = (gracenote::metamorphosis::data::core::ScheduleDayData *) mConverter.createPopulated(
                        Topic::Metamorphosis_Schedule_Day, (unsigned char *) data->thriftObject,
                        data->sizeInBytes);

                //LOG(INFO) << "Read schedule day id "<< sData->object.id << " action "<<sData->meta.action << " station "<< sData->object.stationId << " date " << sData->object.day;
            count++;
            delete data;

        }

        LOG(INFO)<< "Got   "<<count << " ScheduleDay days ";
        if(iter->hasError())
        {
            LOG(INFO)<< "Error iterating over results "<<iter->error();
        }
    }
}
void testQuery(Datastore * store,IdArray &array, Topic topic)
{
    store->setFetchSize(2);

    DatastoreIteratorPtr  iter=store->data(array,topic);

    ThriftConverter mConverter;

    if(iter.get()!= nullptr)
    {
        DataObject *data=nullptr;

        size_t count=0;

        while((data=iter->next())!=nullptr)
        {
            if(topic==Topic::Metamorphosis_Station)
            {
                gracenote::metamorphosis::data::core::StationData *sData = (gracenote::metamorphosis::data::core::StationData *) mConverter.createPopulated(
                        topic, (unsigned char *) data->thriftObject,
                        data->sizeInBytes);

                LOG(INFO) << "Read station id "<< sData->object.id << " action "<<sData->meta.action;

            }
            else if (topic==Topic::Metamorphosis_Schedule_Day)
            {
                gracenote::metamorphosis::data::core::ScheduleDayData *sData = (gracenote::metamorphosis::data::core::ScheduleDayData *) mConverter.createPopulated(
                        topic, (unsigned char *) data->thriftObject,
                        data->sizeInBytes);

                LOG(INFO) << "Read schedule day id "<< sData->object.id << " action "<<sData->meta.action << " station "<< sData->object.stationId << " date " << sData->object.day;
            }
            count++;

            delete data;

            //getchar();
        }
        if(iter->hasError())
        {
            LOG(INFO)<< "Error iterating over results "<<iter->error();
        }
    }
}


int main(int argc, char **argv)
{

    FLAGS_alsologtostderr = true;


    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    std::vector<int> bla2{12,13,14,15,16,16};


    fbstring outString; //=svformat("'{2:+}',",bla2);

    outString.reserve(2048);

    toAppend("'",bla2[0],"',",&outString);
    toAppend("'",bla2[1],"',",&outString);
    toAppend("'",bla2[2],"',",&outString);

    LOG(INFO) << "dataDump started  .... "  << outString;


    std::ifstream fileStream("/home/warmi/Development/git/ApiPack2/conf/default.conf");
    std::stringstream buffer;
    buffer << fileStream.rdbuf();

    //LOG(INFO) <<buffer.str();


    dynamic parsed = folly::parseJson(buffer.str());


    ApiPack2::DatastoreConnectionInfo connectionInfo(parsed);

    LOG(INFO) << connectionInfo.getNextUri();
    LOG(INFO) << connectionInfo.getNextUri();
    LOG(INFO) << connectionInfo.getNextUri();
    LOG(INFO) << connectionInfo.getNextUri();
    LOG(INFO) << connectionInfo.getNextUri();
    LOG(INFO) << connectionInfo.getDefaultFetchSize();

    ScheduleDay sDay(10003,"2016-55-245");

    int station=sDay.stationId();
    int day=sDay.day();
    int year=sDay.year();

    uint64_t  bla3=4402274379539;

    Datastore *store=new Datastore(parsed);

    IdArray ids{96541,96446,95856,95643,93825,92394,92160,92022,92020,92008};

    IdArray ids2{ScheduleDay::create(10003,"2016-05-08"),ScheduleDay::create(10003,"2016-05-09"),ScheduleDay::create(10003,"2016-05-10")};

    IdArray ids3{10003,10002};

    IdArray ids4{    19556,
                     10121,
                     16300,
                     17693,
                     19635,
                     76990,
                     76965,
                     66268,
                     88533,
                     61474,
                     10153,
                     15451,
                     85932,
                     95643,
                     78789,
                     47499,
                     11481,
                     65717,
                     20709,
                     34949,
                     83098,
                     59845,
                     56278,
                     82572,
                     21623,
                     48639,
                     10269,
            10760,
            59155,
            20675,
            73812,
            79632,
            16496,
            11461,
            87122,
            24125,
            70975,
            71662,
            58698,
            69073,
            72468,
            65342,
            90134,
            59368,
            66284,
            15717,
            38961,
            17683,
            71115,
            58333,
            73965,
            16320,
            12009,
            50500,
            73467,
            76126,
            59952,
            75479,
            92008,
            10991,
            89535,
            60121,
            77107,
            88801,
            71209,
            55241,
            19631,
            60150,
            17457,
            66347,
            68140,
            74484,
            10145,
            17956,
            34916,
            19520,
            64591,
            62125,
            65791,
            76367,
            90136,
            18086,
            72014,
            33725,
            14791,
            83173,
            71205,
            70337,
            11118,
            59366,
            16879,
            23392,
            26772,
            64490,
            11779,
            66379,
            72729,
            44642,
            21544,
            11299,
            26827,
            24569,
            91109,
            24521,
            21617,
            58625,
            61628,
            20655,
            70993,
            16584,
            71040,
            88770,
            37771,
            48859,
            45399,
            71214,
            71799,
            49582,
            11163,
            32281,
            74281,
            11653,
            97891,
            74290,
            34941,
            34933,
            78584,
            74278,
            73264,
            67331,
            29058,
            34290,
            15085,
            14776,
            24935,
            25270,
            59839


    };




    testQuery(store,ids,Topic::Metamorphosis_Station);

    if(store->isConnected())
    {
        testQuery(store, ids2, Topic::Metamorphosis_Schedule_Day);
    }
    else{
        LOG(INFO) <<" DataStore is not connected aborting query";
    }


    //testScheduleQuery(store,ids3,"2016-05-08","2016-05-12");

    //testScheduleQuery2(store,ids3,boost::gregorian::date(2016,5,8),boost::gregorian::date(2016,5,12));

    StopTimer timer;

    testScheduleQuery(store,ids4,"2015-01-08","2019-05-12");

    StopTimer::ElapsedTime eTime=timer.elapsedTime();

    LOG(INFO) <<" Query took :" <<eTime.first;

    delete store;
    return 0;
}




