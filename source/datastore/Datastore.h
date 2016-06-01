//
// Created by warmi on 5/5/16.
//

#ifndef APIPACK2_DATASTORE_H
#define APIPACK2_DATASTORE_H

#include "DataObject.h"
#include <glog/logging.h>
#include <vector>
#include "Topics.h"
#include "DatastoreIterator.h"
#include <postgresql/libpq-fe.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <folly/dynamic.h>
#include "IntrusiveTracked.h"

using namespace folly;


namespace ApiPack2
{
/*! \brief Datastore driver.
 *
 *
 *  A basic, domain specific, datastore driver encapsulating access to Catapult database. I
 *  Query requests (data()) return a smart pointer to an interator which can be used to
 */


    class Datastore: public IntrusiveTracked::Creator
    {
    public:

        Datastore(dynamic &config, size_t fetchSize=500);

        const std::string connectionInfo() const
        {
            return mConnectionInfo;
        }

        bool isValid() const;
        bool isConnected() const;

        inline size_t fetchSize() const { return mFetchSize;}

        inline void setFetchSize(size_t val)
        {
            mFetchSize=val;
            if(mFetchSize<1)
                mFetchSize=1;
        }

        DatastoreIteratorPtr data(const IdArray &idArray, Topic topic);
        DatastoreIteratorPtr data(const std::string &dateFrom,const std::string &dateTo,const IdArray &stationIdArray);
        DatastoreIteratorPtr data(const boost::gregorian::date &dateFrom,const boost::gregorian::date &dateTo,const IdArray &stationIdArray);

        // enable deletion of a Derived* through a Base*
        virtual ~Datastore();

        virtual void release(IntrusiveTracked *object);

    private:


        size_t  mFetchSize;
        std::string mConnectionInfo;
        DatastoreIterator *mDataIter;
    };


}

#endif //APIPACK2_DATASTORE_H
