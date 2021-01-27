//
//  opt.h
//  Command line interface option.
//
//  Created by Zakhary Kaplan on 2020-11-29.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#pragma once

#include "clip/option.h"
#include "clip/value.h"

namespace clip {

class AbstractOpt : public Option, public virtual AbstractValue {
public:
    // ctors
    AbstractOpt(const char *name);

    // dtor
    ~AbstractOpt() = 0;

    // builders (override)
    virtual AbstractOpt &help(const char *s) override;
    virtual AbstractOpt &longname(const char *s) override;
    virtual AbstractOpt &shortname(char c) override;
    virtual AbstractOpt &metavar(const char *s) override;
    virtual AbstractOpt &optional(bool b) override;

    // accessors (using)
    using AbstractValue::metavar;
    using AbstractValue::optional;
    using Option::count;
    using Option::help;
    using Option::longname;
    using Option::shortname;

    // methods (using)
    using AbstractValue::parse;
    using Option::match;
};

template <typename T>
class Opt final : public AbstractOpt, public Value<T> {
public:
    // ctors
    Opt(const char *name);

    // builders (override)
    virtual Opt<T> &help(const char *s) override;
    virtual Opt<T> &longname(const char *s) override;
    virtual Opt<T> &shortname(char c) override;
    virtual Opt<T> &metavar(const char *s) override;
    virtual Opt<T> &optional(bool b) override;
    virtual Opt<T> &value(const T &v) override;

    // accessors (using)
    using AbstractOpt::count;
    using AbstractOpt::help;
    using AbstractOpt::longname;
    using AbstractOpt::metavar;
    using AbstractOpt::optional;
    using AbstractOpt::shortname;
    using Value<T>::value;

    // methods (using)
    using AbstractOpt::match;
    using Value<T>::parse;
};

} // namespace clip
