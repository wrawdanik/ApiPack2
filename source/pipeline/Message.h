//
// Created by warmi on 5/6/16.
//

#ifndef APIPACK2_PUBLICMESSAGE_H
#define APIPACK2_PUBLICMESSAGE_H

#include <cstddef>

namespace ApiPack2
{
    enum class PublicMsgType : size_t
    {
        Update,
        OpenRequest,
        CloseRequest,
        Terminate
    };

    union PublicMsgFlags
    {
        struct
        {
            size_t destroy:             1;
            size_t type:                4;
            size_t id:                  32;
        };
        uint32_t value;
    };


    class PublicMsg
    {
    public:
        PublicMsgFlags flags;
        void *ptrData;

        bool shouldDestroy() const
        {
            return (flags.destroy==1);
        }

        PublicMsgType type() const
        {
            return (PublicMsgType)flags.type;
        }

        size_t    id() const
        {
            return flags.id;
        }

    };

}

#endif //APIPACK2_PUBLICMESSAGE_H
