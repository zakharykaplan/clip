//
//  opt.cpp
//  Command line interface option.
//
//  Created by Zakhary Kaplan on 2020-11-29.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#include "clip/opt.h"

#include <string>

#include "clip/option.h"
#include "clip/param.h"
#include "clip/value.h"

namespace clip {

// class AbstractOpt
// ctors
AbstractOpt::AbstractOpt(const char *name) : AbstractValue(name), Option(name) {}

// dtor
AbstractOpt::~AbstractOpt() = default;

// builders (override)
AbstractOpt &AbstractOpt::help(const char *s) {
    this->Option::help(s);
    return *this;
}

AbstractOpt &AbstractOpt::longname(const char *s) {
    this->Option::longname(s);
    return *this;
}

AbstractOpt &AbstractOpt::shortname(char c) {
    this->Option::shortname(c);
    return *this;
}

AbstractOpt &AbstractOpt::metavar(const char *s) {
    this->AbstractValue::metavar(s);
    return *this;
}

AbstractOpt &AbstractOpt::optional(bool b) {
    this->AbstractValue::optional(b);
    return *this;
}

// class Opt<T>
// ctors
template <typename T>
Opt<T>::Opt(const char *name) :
    Param(name),
    AbstractValue(name),
    AbstractOpt(name),
    Value<T>(name) {}

// builders (override)
template <typename T>
Opt<T> &Opt<T>::help(const char *s) {
    this->AbstractOpt::help(s);
    return *this;
}

template <typename T>
Opt<T> &Opt<T>::longname(const char *s) {
    this->AbstractOpt::longname(s);
    return *this;
}

template <typename T>
Opt<T> &Opt<T>::shortname(char c) {
    this->AbstractOpt::shortname(c);
    return *this;
}

template <typename T>
Opt<T> &Opt<T>::metavar(const char *s) {
    this->AbstractOpt::metavar(s);
    return *this;
}

template <typename T>
Opt<T> &Opt<T>::optional(bool b) {
    this->AbstractOpt::optional(b);
    return *this;
}

template <typename T>
Opt<T> &Opt<T>::value(const T &v) {
    this->Value<T>::value(v);
    return *this;
}

// explicit instantiations
template class Opt<double>;
template class Opt<int>;
template class Opt<std::string>;

} // namespace clip
