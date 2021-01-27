//
//  param.h
//  Command line interface abstract parameter.
//
//  Created by Zakhary Kaplan on 2020-11-12.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#pragma once

#include <string>

namespace clip {

// class Param
class Param {
public:
    // const members
    const std::string name;

private:
    // mut members
    std::string help_;

public:
    // ctors
    Param(const char *name);

    // dtor
    virtual ~Param() = 0;

    // builders
    virtual Param &help(const char *s);

    // accessors
    virtual const char *help() const final;
};

} // namespace clip
