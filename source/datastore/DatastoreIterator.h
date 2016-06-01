//
// Created by warmi on 5/5/16.
//

#ifndef APIPACK2_DATASTOREITERATOR_H
#define APIPACK2_DATASTOREITERATOR_H

#include <postgresql/libpq-fe.h>
#include "IntrusiveTracked.h"
#include <folly/FBString.h>
#include "DataObject.h"
#include "Topics.h"
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace folly;

namespace ApiPack2
{
    class DataObject;

    using IdArray=std::vector<uint64_t>;

    class DatastoreIterator : public IntrusiveTracked
    {

        friend class Datastore;

    public:


        ~DatastoreIterator();

        DataObject* next();

        bool hasError() const
        {
            return mWasError;
        }

        void resetError()
        {
            mWasError=false;
        }

        const std::string error() const
        {
            return mError.toStdString();
        }
        bool isValid() const
        {
            return (mConnection!= nullptr);
        }

        bool isConnected() const
        {
            return (isValid() && (PQstatus(mConnection) == CONNECTION_OK));
        }

    private:

        DatastoreIterator(const fbstring &connectionString);

        enum class State : size_t
        {
            Ready,
            Open
        };

        void setError() { mWasError=true;}



        bool isReady() const  { return mState==State::Ready;}
        void closeAndReset();

        void executeQuery(const fbstring &query,Topic topic,size_t fetchSize);

        void executeQuery(const IdArray &ids,Topic topic, size_t fetchSize);
        void executeQuery(const std::string &dateFrom,const std::string &dateTo,const IdArray &stationIdArray, size_t fetchSize);
        void executeQuery(const boost::gregorian::date &dateFrom,const boost::gregorian::date &dateTo,const IdArray &stationIdArray, size_t fetchSize);
        void    generateStringIdList(const IdArray &ids,fbstring &string);
        void    generateLongIdList(const IdArray &ids,fbstring &string);
        void createConnection();
        void    destroyConnection();

        fbstring mError;
        fbstring mFetchCommand;
        fbstring mCloseCommand;
        fbstring mQueryCommand;
        PGresult *mResult;
        State   mState;
        PGconn *mConnection;
        std::string  mThreadId;
        fbstring mConnectionString;
        size_t  mResultOffset;
        bool mWasError;
    };

    using DatastoreIteratorPtr=intrusive_ptr<DatastoreIterator>;

}
#endif //APIPACK2_DATASTOREITERATOR_H
