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

/** \file tuple_iteration.hpp
 *
 *   implementation of a tuple_for_each operation
 */

#ifndef PPRZLINK_TUPLE_ITERATION_HPP
#define PPRZLINK_TUPLE_ITERATION_HPP

#include <tuple>
#include <functional>

template<int size> struct SizeT {};

template<typename TupleType, typename ActionType>
void tuple_for_each(TupleType &tuple,ActionType &fn)
{
    tuple_for_each(tuple,fn,SizeT<std::tuple_size<TupleType>::value>());
}

template<typename TupleType, typename ActionType>
void tuple_for_each(TupleType &tuple,ActionType &fn, SizeT<0> s)
{}

template<typename TupleType, typename ActionType, int N>
void tuple_for_each(TupleType &tuple,ActionType &fn,SizeT<N> s)
{
    tuple_for_each(tuple,fn,SizeT<N-1>());
    fn(std::get<N-1>(tuple));
}

template<int size> struct val {
        int value = size;
    };

#endif //PPRZLINK_TUPLE_ITERATION_HPP
