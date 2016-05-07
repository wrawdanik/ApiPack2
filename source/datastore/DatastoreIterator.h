//
// Created by warmi on 5/5/16.
//

#ifndef APIPACK2_DATASTOREITERATOR_H
#define APIPACK2_DATASTOREITERATOR_H


#include "IntrusiveTracked.h"

namespace ApiPack2
{
    class DataObject;

    class DatastoreIterator : public IntrusiveTracked
    {
    public:

        DatastoreIterator();
        ~DatastoreIterator();

        DataObject* next();


    };

    using DatastoreIteratorPtr=intrusive_ptr<DatastoreIterator>;

}
#endif //APIPACK2_DATASTOREITERATOR_H
