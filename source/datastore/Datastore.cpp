//
// Created by warmi on 5/5/16.
//

#include "Datastore.h"

#include <folly/Format.h>

using namespace ApiPack2;
using folly::sformat;


Datastore::Datastore(dynamic &config, size_t fetchSize)
{


    //mDataIter=new DatastoreIterator(mConnectionStrings[0]);
    mDataIter->setCreator(this);
}
Datastore::~Datastore()
{
    if(mDataIter->referenceCount()==0)
    {
        delete mDataIter;
    }
}

/*
 *  Right now just one DatastoreIterator, meaning an instance of Datastore can only do one query at a time
 */

DatastoreIteratorPtr Datastore::data(const IdArray &idArray,Topic topic)
{
    DatastoreIteratorPtr iter;

    if(!mDataIter->isReady())
        return iter;
    mDataIter->executeQuery(idArray,topic,mFetchSize);
    iter=mDataIter;
    return iter;
}

DatastoreIteratorPtr Datastore::data(const std::string &dateFrom,const std::string &dateTo,const IdArray &stationIdArray)
{
    DatastoreIteratorPtr iter;

    if(!mDataIter->isReady())
        return iter;
    mDataIter->executeQuery(dateFrom,dateTo,stationIdArray,mFetchSize);
    iter=mDataIter;
    return iter;


    return iter;
}
DatastoreIteratorPtr Datastore::data(const boost::gregorian::date &dateFrom,const boost::gregorian::date &dateTo,const IdArray &stationIdArray)
{
    DatastoreIteratorPtr iter;

    if(!mDataIter->isReady())
        return iter;
    mDataIter->executeQuery(dateFrom,dateTo,stationIdArray,mFetchSize);
    iter=mDataIter;
    return iter;
}


bool Datastore::isValid() const
{
    return (mDataIter!=nullptr && mDataIter->isValid());
}

bool Datastore::isConnected() const
{
    return (mDataIter!=nullptr && mDataIter->isConnected());
}





void Datastore::release(IntrusiveTracked *object)
{
    if(object!= nullptr)
    {
        if(object==mDataIter)
        {
            mDataIter->closeAndReset();
        }
    }
}