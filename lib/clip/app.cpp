//
//  app.cpp
//  Command line interface app.
//
//  Created by Zakhary Kaplan on 2020-12-20.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#include "clip/app.h"

#include <string>

namespace clip {

// class App
// ctors
App::App(const char *name) : name(name) {}

// builders
App &App::about(const char *s) {
    this->about_ = s;
    return *this;
}

App &App::author(const char *s) {
    this->author_ = s;
    return *this;
}

App &App::version(const char *s) {
    this->version_ = s;
    return *this;
}

// accessors
const std::string &App::about() const {
    return this->about_;
}

const std::string &App::author() const {
    return this->author_;
}

const std::string &App::version() const {
    return this->version_;
}

} // namespace clip
