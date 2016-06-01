//
// Created by warmi on 5/5/16.
//

#include "DatastoreIterator.h"
#include <folly/Format.h>
#include <thread>
#include <sstream>

using namespace ApiPack2;
using folly::sformat;

DatastoreIterator::DatastoreIterator(const fbstring &connectionString):mResult(nullptr),mConnectionString(connectionString),mConnection(nullptr)
{
    mResultOffset=0;

    mQueryCommand.reserve(8192);
    mFetchCommand.reserve(128);
    mCloseCommand.reserve(128);

    std::ostringstream ss;
    ss << std::this_thread::get_id();
    mThreadId = ss.str();

    createConnection();

    mState=State::Ready;
    resetError();

}
DatastoreIterator::~DatastoreIterator()
{
    closeAndReset();
    destroyConnection();
}

DataObject* DatastoreIterator::next()
{

    if (mResult == nullptr || mResultOffset == PQntuples(mResult))
    {
        if(mResult!= nullptr)
        {
            PQclear(mResult);
        }
        mResultOffset=0;
        mResult=PQexec(mConnection, mFetchCommand.c_str());
    }
    if (PQresultStatus(mResult) != PGRES_TUPLES_OK )
    {
        LOG(INFO) << " Result is invalid.Calling closeAndreset()";
        mError=sformat("Error fetching results : {} ",PQerrorMessage(mConnection));
        setError();
        closeAndReset();
        return nullptr;
    }
    else
    {
        if(PQntuples(mResult)==0)
        {
            closeAndReset();
            return nullptr;
        }
        DataObject *nObject=new DataObject();

        if(PQgetisnull(mResult,mResultOffset, 0)==0)
        {

            nObject->sizeInBytes=PQgetlength(mResult,mResultOffset, 0);

            nObject->thriftObject=(unsigned char*)PQgetvalue(mResult,mResultOffset, 0);

            //unsigned char *bla=PQunescapeBytea(nObject->thriftObject,&nObject->sizeInBytes);
            //nObject->thriftObject=bla;

        }
        mResultOffset++;
        return nObject;
    }

}


void DatastoreIterator::closeAndReset()
{
    if(mState==State::Open)
    {
        if(mResult!=nullptr)
        {
            PQclear(mResult);
            mResult= nullptr;
        }
        if(mConnection!= nullptr)
        {
            PGresult *result = PQexec(mConnection, mCloseCommand.c_str());
            if (PQresultStatus(result) != PGRES_COMMAND_OK)
            {
                LOG(ERROR) << "Error executing close command : " << mCloseCommand << " : " << PQerrorMessage(mConnection);
                setError();
            }
            PQclear(result);
            result = PQexec(mConnection, "END");
            if (PQresultStatus(result) != PGRES_COMMAND_OK)
            {
                LOG(ERROR) << "Error executing END command  : " << PQerrorMessage(mConnection);
                setError();
            }
            PQclear(result);
        }
    }
    mState=State::Ready;
}

void DatastoreIterator::executeQuery(const IdArray &ids,Topic topic, size_t fetchSize)
{

    mQueryCommand.clear();
    if (topic != Topic::Metamorphosis_Schedule_Day)
    {
        toAppend("DECLARE cursor_", topic, "_", mThreadId,
                 " BINARY CURSOR for select thrift from catapult.general where id in (", &mQueryCommand);
        generateStringIdList(ids,mQueryCommand);
        toAppend(" ) and topic='",TopicConversion::toChar(topic),"'",&mQueryCommand);
    }
    else
    {
        toAppend("DECLARE cursor_", topic, "_", mThreadId,
                 " BINARY CURSOR for select thrift from catapult.schedule_day where schedule_day_id in (", &mQueryCommand);
        generateLongIdList(ids,mQueryCommand);
        toAppend(" ) ",&mQueryCommand);
    }

    executeQuery(mQueryCommand,topic,fetchSize);
}


void DatastoreIterator::executeQuery(const std::string &dateFrom,const std::string &dateTo,const IdArray &stationIdArray, size_t fetchSize)
{
    mQueryCommand.clear();
    toAppend("DECLARE cursor_", Topic::Metamorphosis_Schedule_Day, "_", mThreadId,
             " BINARY CURSOR for select thrift from catapult.schedule_day where(day AT TIME ZONE 'UTC'  >='",dateFrom,"' and day  AT TIME ZONE 'UTC'<='",dateTo,"' ) and station in (", &mQueryCommand);
    generateLongIdList(stationIdArray,mQueryCommand);
    toAppend(" ) order by station,day ",&mQueryCommand);

    executeQuery(mQueryCommand,Topic::Metamorphosis_Schedule_Day,fetchSize);
}

void DatastoreIterator::executeQuery(const boost::gregorian::date &dateFrom,const boost::gregorian::date &dateTo,const IdArray &stationIdArray, size_t fetchSize)
{
    mQueryCommand.clear();
    toAppend("DECLARE cursor_", Topic::Metamorphosis_Schedule_Day, "_", mThreadId,
             " BINARY CURSOR for select thrift from catapult.schedule_day where(day AT TIME ZONE 'UTC'  >='",&mQueryCommand);
    toAppend((unsigned short)dateFrom.year(),"-",(unsigned short)dateFrom.month(),"-",(unsigned short)dateFrom.day(),&mQueryCommand);
    toAppend("' and day  AT TIME ZONE 'UTC'<='",&mQueryCommand);
    toAppend((unsigned short)dateTo.year(),"-",(unsigned short)dateTo.month(),"-",(unsigned short)dateTo.day(),&mQueryCommand);
    toAppend("' ) and station in (", &mQueryCommand);
    generateLongIdList(stationIdArray,mQueryCommand);
    toAppend(" ) order by station,day ",&mQueryCommand);

    executeQuery(mQueryCommand,Topic::Metamorphosis_Schedule_Day,fetchSize);


}

void DatastoreIterator::executeQuery(const fbstring &query,Topic topic,size_t fetchSize)
{
    mResultOffset = 0;
    mResult = nullptr;


    LOG(INFO) << "Executing begin command";

    PGresult *result = PQexec(mConnection, "BEGIN");
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        LOG(ERROR) << "Error executing query for topic: " << topic << " BEGIN command failed: " <<
        PQerrorMessage(mConnection);

        mError = sformat("Error executing query for topic : {} : BEGIN command failed {}", topic,
                         PQerrorMessage(mConnection));
        PQclear(result);
        setError();
        return;
    }
    PQclear(result);
    mState = State::Open;


    mFetchCommand.clear();
    toAppend("FETCH ",fetchSize," in cursor_",topic,"_",mThreadId,&mFetchCommand);

    mCloseCommand.clear();
    toAppend("CLOSE cursor_",topic,"_",mThreadId,&mCloseCommand);

    LOG(INFO) << "Executing query command:" << mQueryCommand;

    result=PQexec(mConnection, query.c_str());


    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        LOG(ERROR) << "Error executing query for topic: " << topic << " query failed :" <<query << " : " << PQerrorMessage(mConnection);

        mError=sformat("Error executing query {} :  ",query.c_str(),PQerrorMessage(mConnection));
        PQclear(result);
        setError();
        closeAndReset();
        return;
    }
}




void    DatastoreIterator::generateStringIdList(const IdArray &ids,fbstring &string)
{
    IdArray::const_iterator curr=ids.begin();
    IdArray::const_iterator lastItem=ids.end()-1;

    while(curr!=ids.end())
    {
        if(curr!=lastItem)
            toAppend("'",*curr,"',",&string);
        else
            toAppend("'",*curr,"'",&string);
        ++curr;
    }

}

void    DatastoreIterator::generateLongIdList(const IdArray &ids,fbstring &string)
{
    IdArray::const_iterator curr=ids.begin();
    IdArray::const_iterator lastItem=ids.end()-1;

    while(curr!=ids.end())
    {
        if(curr!=lastItem)
            toAppend(*curr,",",&string);
        else
            toAppend(*curr,&string);
        ++curr;
    }

}

void    DatastoreIterator::destroyConnection()
{
    if (isValid())
    {
        LOG(INFO) << "Destroying connection to PostgreSQL user:'" << PQuser(mConnection)
        << "' server:'" << PQhost(mConnection)
        << "' database:'" << PQdb(mConnection)
        << "' thread :'" << mThreadId  << "'";
        PQfinish(mConnection);
    }
    mConnection= nullptr;

}

void DatastoreIterator::createConnection()
{
    if (isValid())
        return;

    destroyConnection();


    mConnection = PQconnectdb(mConnectionString.c_str());

    if(!isValid() || !isConnected())
    {
        setError();
        mError=sformat("Error connecting to PostgreSQL with connection string: {}: {} ",mConnectionString,PQerrorMessage(mConnection));
        LOG(ERROR) << "Error connecting to PostgreSQL with connection string: "<< mConnectionString << " : " << PQerrorMessage(mConnection);
    }
    else
    {
        LOG(INFO) << "Sucesfully connected to PostgreSQL user:'" << PQuser(mConnection)
        << "' server:'" << PQhost(mConnection)
        << "' database:'" << PQdb(mConnection)
        << "' thread :'" << mThreadId
        << "'";
    }
}