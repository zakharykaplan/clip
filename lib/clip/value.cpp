//
//  value.cpp
//  Command line interface abstract value.
//
//  Created by Zakhary Kaplan on 2020-12-23.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#include "clip/value.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>

#include "clip/param.h"

namespace clip {

// class AbstractValue
// ctors
AbstractValue::AbstractValue(const char *name) : Param(name), metavar_(""), optional_(false) {
    // Set default metavar
    std::string metavar(name);
    std::transform(metavar.begin(), metavar.end(), metavar.begin(), ::toupper);
    this->metavar(metavar.data());
}

// dtor
AbstractValue::~AbstractValue() = default;

// builders
AbstractValue &AbstractValue::metavar(const char *s) {
    this->metavar_ = std::string(s);
    return *this;
}

AbstractValue &AbstractValue::optional(bool b) {
    this->optional_ = b;
    return *this;
}

// builders (override)
AbstractValue &AbstractValue::help(const char *s) {
    this->Param::help(s);
    return *this;
}

// accessors
const char *AbstractValue::metavar() const {
    return this->metavar_.data();
}

bool AbstractValue::optional() const {
    return this->optional_;
}

// class Value<T>
// ctors
template <typename T>
Value<T>::Value(const char *name) : Param(name), AbstractValue(name) {}

// dtor
template <typename T>
Value<T>::~Value() = default;

// builders
template <typename T>
Value<T> &Value<T>::value(const T &value) {
    this->value_ = value;
    return *this;
}

// builders (override)
template <typename T>
Value<T> &Value<T>::help(const char *s) {
    this->AbstractValue::help(s);
    return *this;
}

template <typename T>
Value<T> &Value<T>::metavar(const char *s) {
    this->AbstractValue::metavar(s);
    return *this;
}

template <typename T>
Value<T> &Value<T>::optional(bool b) {
    this->AbstractValue::optional(b);
    return *this;
}

// accessors
template <typename T>
const T &Value<T>::value() const {
    return this->value_;
}

// methods
// explicit specializations
template <>
bool Value<double>::parse(const char *s) {
    try {
        char *endstr;
        double value_ = std::strtod(s, &endstr);
        bool success = !*endstr; // check entire string was parsed
        if (success)
            this->value_ = value_;
        return success;
    } catch (...) { return false; }
}

template <>
bool Value<int>::parse(const char *s) {
    try {
        char *endstr;
        int value_ = std::strtol(s, &endstr, 10);
        bool success = !*endstr; // check entire string was parsed
        if (success)
            this->value_ = value_;
        return success;
    } catch (...) { return false; }
}

template <>
bool Value<std::string>::parse(const char *s) {
    std::string value_ = s;
    bool success = !value_.empty(); // check string is not empty
    if (success)
        this->value_ = value_;
    return success;
}

// explicit instantiations
template class Value<double>;
template class Value<int>;
template class Value<std::string>;

} // namespace clip
