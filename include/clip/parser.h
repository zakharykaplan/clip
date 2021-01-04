//
//  parser.h
//  Command line interface parser.
//
//  Created by Zakhary Kaplan on 2020-11-12.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//

#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "clip/app.h"
#include "clip/flag.h"
#include "clip/param.h"

namespace clip {

// forward declarations
class Option;
template <typename T>
class Arg;
template <typename T>
class Opt;

class Parser final {
public:
    // const members
    const int argc;
    const char *const *const argv;
    const App app;

private:
    // impl members
    std::unordered_map<std::string, std::unique_ptr<Param>> params;
    std::unordered_map<std::string, std::string> longnames;
    std::unordered_map<char, std::string> shortnames;
    std::vector<std::string> flags;
    std::vector<std::string> opts;
    std::vector<std::string> args;
    bool autohelp;

public:
    // ctors
    Parser(int argc, char *argv[], const App &app);

    // builders
    Parser &add(const Flag &flag);
    template <typename T>
    Parser &add(const Opt<T> &opt);
    template <typename T>
    Parser &add(const Arg<T> &arg);

    // accessors
    const decltype(params) &data();
    template <typename P = Param>
    const P &get(const char *name) const;
    const Flag &getFlag(const char *name) const;
    template <typename T>
    const Opt<T> &getOpt(const char *name) const;
    template <typename T>
    const Arg<T> &getArg(const char *name) const;

    // methods
    void parse();

    // methods (static)
    static void error(unsigned char ret = 1, const std::string &msg = "unknown");

private:
    // mutators
    bool insert(Param *param);

    // helpers
    void addAutoflags();
    void checkAutoflags(Option *match) const;
    bool parseLongOption(int &i);
    bool parseShortOption(int &i);
    bool parseArg(int &i, std::size_t &argidx);

    // formatters
    std::string help_s() const;
    std::string usage_s() const;
    std::string flags_s() const;
    std::string opts_s() const;
    std::string args_s() const;
    std::string version_s() const;
};

} // namespace clip
