//
//  arg.cpp
//  Command line interface argument.
//
//  Created by Zakhary Kaplan on 2020-12-23.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#include "clip/arg.h"

#include <string>

#include "clip/param.h"
#include "clip/value.h"

namespace clip {

// class AbstractArg
// ctors
AbstractArg::AbstractArg(const char *name) : AbstractValue(name) {}

// dtor
AbstractArg::~AbstractArg() = default;

// builders (override)
AbstractArg &AbstractArg::help(const char *s) {
    this->Param::help(s);
    return *this;
}

AbstractArg &AbstractArg::metavar(const char *s) {
    this->AbstractValue::metavar(s);
    return *this;
}

AbstractArg &AbstractArg::optional(bool b) {
    this->AbstractValue::optional(b);
    return *this;
}

// class Arg<T>
// ctors
template <typename T>
Arg<T>::Arg(const char *name) :
    Param(name),
    AbstractValue(name),
    AbstractArg(name),
    Value<T>(name) {}

// builders (override)
template <typename T>
Arg<T> &Arg<T>::help(const char *s) {
    this->AbstractArg::help(s);
    return *this;
}

template <typename T>
Arg<T> &Arg<T>::metavar(const char *s) {
    this->AbstractArg::metavar(s);
    return *this;
}

template <typename T>
Arg<T> &Arg<T>::optional(bool b) {
    this->AbstractArg::optional(b);
    return *this;
}

template <typename T>
Arg<T> &Arg<T>::value(const T &v) {
    this->Value<T>::value(v);
    return *this;
}

// explicit instantiations
template class Arg<double>;
template class Arg<int>;
template class Arg<std::string>;

} // namespace clip
