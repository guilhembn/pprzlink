/*
 * Copyright (C) 2019 Fabien Garcia <gautier.hattenbergerfabien.garcia@enac.fr>
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

/** \file pprzlink_messages.hpp
 *
 *   Generic message header for PPRZLINK message system in C++
 */

#ifndef PPRZLINK_MESSAGES_HPP
#define PPRZLINK_MESSAGES_HPP

#include <tuple>
#include "tuple_iteration.h"

namespace pprzlink
{
    struct MessageId {
        const std::string msgName;
        const uint8_t msgId;
        const std::string className;
        const uint8_t classId;
    };


    template<typename ...Types>
    class PprzMessage : public std::tuple<Types...> {
    public:
        using type = PprzMessage<Types...>;

        template<int id>
        auto &get ()
        {
            return std::get<id>(*this);
        }

        struct _size {
            int value=0;

            template<typename T>
            void operator() (T &item)
            {
                value+=sizeof(T);
            }

            template<typename T>
            void operator()(std::vector<T> &vec)
            {
                for (auto item: vec)
                {
                    value += sizeof(item); // Variable array
                }
            }

            template<typename T,int N>
            void operator()(std::array<T, N> &arr)
            {
                    value += sizeof(T)*N; // Static array
            }

            void operator()(std::string &str)
            {
                value += str.length();
            }
        };

        constexpr size_t size_of_fields()
        {
            if (sizeof...(Types)>0)
            {
                _size s;
                tuple_for_each(*this,s);
                return s.value;
            }
            else
            {
                return 0;
            }
        }

    };

    template<typename msg_type>
    constexpr size_t nbFields (msg_type const &m)
    {
        return std::tuple_size<typename msg_type::type>::value;
    }

    template<typename msg_type>
    void printInfo(msg_type const &m)
    {
        std::cout << "Message " << msg_type::id.msgName << "(" << (int)msg_type::id.msgId << ") class " << msg_type::id.className << "(" << (int)msg_type::id.classId << ") with "  << nbFields(m) << " Fields." << std::endl;
    }
}

namespace std {
    template<typename ...Tf>
    struct tuple_size<pprzlink::PprzMessage<Tf...> > {
        constexpr static int value = sizeof...(Tf);
    };
}
#endif //PPRZLINK_MESSAGES_HPP
