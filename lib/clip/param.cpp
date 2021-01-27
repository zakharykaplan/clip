//
//  param.cpp
//  Command line interface abstract parameter.
//
//  Created by Zakhary Kaplan on 2020-11-12.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#include "clip/param.h"

#include <string>

namespace clip {

// class Param
// ctors
Param::Param(const char *name) : name(name), help_("") {}

// dtor
Param::~Param() = default;

// builders
Param &Param::help(const char *s) {
    this->help_ = std::string(s);
    return *this;
}

// accessors
const char *Param::help() const {
    return this->help_.data();
}

} // namespace clip
