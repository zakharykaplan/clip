//
//  flag.h
//  Command line interface flag.
//
//  Created by Zakhary Kaplan on 2020-12-01.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#pragma once

#include "clip/option.h"

namespace clip {

class Flag final : public Option {
public:
    // ctors
    Flag(const char *name);

    // builders (override)
    virtual Flag &help(const char *s) override;
    virtual Flag &longname(const char *s) override;
    virtual Flag &shortname(char c) override;

    // accessors (using)
    using Option::count;
    using Option::help;
    using Option::longname;
    using Option::shortname;

    // methods (using)
    using Option::match;
};

} // namespace clip
