//
//  arg.h
//  Command line interface argument.
//
//  Created by Zakhary Kaplan on 2020-12-23.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#pragma once

#include "clip/value.h"

namespace clip {

class AbstractArg : public virtual AbstractValue {
public:
    // ctors
    AbstractArg(const char *name);

    // dtor
    ~AbstractArg() = 0;

    // builders (override)
    virtual AbstractArg &help(const char *s) override;
    virtual AbstractArg &metavar(const char *s) override;
    virtual AbstractArg &optional(bool b) override;

    // accessors (using)
    using AbstractValue::help;
    using AbstractValue::metavar;
    using AbstractValue::optional;

    // methods (using)
    using AbstractValue::parse;
};

template <typename T>
class Arg final : public AbstractArg, public Value<T> {
public:
    // ctors
    Arg(const char *name);

    // builders (override)
    virtual Arg<T> &help(const char *s) override;
    virtual Arg<T> &metavar(const char *s) override;
    virtual Arg<T> &optional(bool b) override;
    virtual Arg<T> &value(const T &v) override;

    // accessors (using)
    using AbstractArg::help;
    using AbstractArg::metavar;
    using AbstractArg::optional;
    using Value<T>::value;

    // methods (using)
    using Value<T>::parse;
};

} // namespace clip
