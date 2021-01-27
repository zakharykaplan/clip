//
//  value.h
//  Command line interface abstract value.
//
//  Created by Zakhary Kaplan on 2020-12-23.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#pragma once

#include <string>

#include "clip/param.h"

namespace clip {

// class AbstractValue
class AbstractValue : public virtual Param {
private:
    // mut members
    std::string metavar_;
    bool optional_;

public:
    // ctors
    AbstractValue(const char *name);

    // dtor
    virtual ~AbstractValue() = 0;

    // builders
    virtual AbstractValue &metavar(const char *s);
    virtual AbstractValue &optional(bool b);
    // builders (override)
    virtual AbstractValue &help(const char *s) override;

    // accessors
    virtual const char *metavar() const final;
    virtual bool optional() const final;
    // accessors (using)
    using Param::help;

    // methods (pure virtual)
    virtual bool parse(const char *s) = 0;
};

// class Value<T>
template <typename T>
class Value : public virtual AbstractValue {
private:
    // impl members
    T value_;

public:
    // ctors
    Value(const char *name);

    // dtor
    virtual ~Value() = 0;

    // builders
    virtual Value<T> &value(const T &value);
    // builders (override)
    virtual Value<T> &help(const char *s) override;
    virtual Value<T> &metavar(const char *s) override;
    virtual Value<T> &optional(bool b) override;

    // accessors
    virtual const T &value() const final;
    // accessors (using)
    using AbstractValue::help;
    using AbstractValue::metavar;
    using AbstractValue::optional;

    // methods
    virtual bool parse(const char *s) final override;
};

} // namespace clip
