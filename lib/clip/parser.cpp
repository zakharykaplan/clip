//
//  parser.cpp
//  Command line interface parser.
//
//  Created by Zakhary Kaplan on 2020-11-12.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//

#include "clip/parser.h"

#include <fmt/core.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "clip/arg.h"
#include "clip/flag.h"
#include "clip/opt.h"
#include "clip/option.h"
#include "clip/param.h"

namespace clip {

// class Parser
// ctors
Parser::Parser(int argc, char *argv[], const App &app) :
    argc(argc - 1),
    argv(&argv[1]),
    app(app),
    autohelp(true) {}

// builders
Parser &Parser::add(const Flag &flag) {
    // Insert param within parser
    if (this->insert(static_cast<Param *>(new Flag(flag)))) {
        // Append name to flags
        this->flags.push_back(flag.name);
    }
    return *this;
}

template <typename T>
Parser &Parser::add(const Opt<T> &opt) {
    // Insert param within parser
    if (this->insert(static_cast<Param *>(new Opt<T>(opt)))) {
        // Append name to opts
        this->opts.push_back(opt.name);
    }
    return *this;
}

template <typename T>
Parser &Parser::add(const Arg<T> &arg) {
    // Insert param within parser
    if (this->insert(static_cast<Param *>(new Arg<T>(arg)))) {
        // Append name to args
        this->args.push_back(arg.name);
    }
    // Update autohelp
    if (this->autohelp && !arg.optional())
        this->autohelp = false;
    return *this;
}

// accessors
const decltype(Parser::params) &Parser::data() {
    return this->params;
}

template <typename P>
const P &Parser::get(const char *name) const {
    return *dynamic_cast<P *>(this->params.at(name).get());
}

const Flag &Parser::getFlag(const char *name) const {
    return this->get<Flag>(name);
}

template <typename T>
const Opt<T> &Parser::getOpt(const char *name) const {
    return this->get<Opt<T>>(name);
}

template <typename T>
const Arg<T> &Parser::getArg(const char *name) const {
    return this->get<Arg<T>>(name);
}

// methods
void Parser::parse() {
    // Add automatic flags
    this->addAutoflags();

    // Skip options after finding "--" terminator
    bool doneopts = false;
    // Keep track of the next positional arg
    std::size_t argidx = 0;

    // Show help if no arguments supplied
    if (!this->argc && this->autohelp) {
        std::cout << this->help_s();
        std::exit(1);
    }

    // Parse each argument
    for (int i = 0; i < this->argc; i++) {
        // Extract this argument
        std::string arg(this->argv[i]);

        // clang-format off
        // Match option terminator
        if (!doneopts && arg == "--")
            doneopts = true;
        // Match long options
        else if ((!doneopts && arg.starts_with("--")) &&
                 this->parseLongOption(i))
            ;
        // Match short options
        else if ((!doneopts && arg.starts_with("-")) &&
                 this->parseShortOption(i))
            ;
        // Match positional arguments
        else if ((argidx < this->args.size()) &&
                this->parseArg(i, argidx))
            ;
        // Handle extra values
        else
            Parser::error(1, fmt::format("unexpected token: `{}`", this->argv[i]));
        // clang-format on
    }

    // Handle missing arguments
    if (argidx < this->args.size())
        Parser::error(1, "missing arguments");
}

// static methods
void Parser::error(unsigned char ret, const std::string &msg) {
    const bool colourize = ::isatty(::fileno(stderr));
    // clang-format off
    std::cerr << (colourize ? "\033[1;31m" : "")
              << "error: "
              << (colourize ? "\033[0m" : "")
              << msg << std::endl
              << "For more information try `--help`" << std::endl;
    // clang-format on
    std::exit(ret);
}

// mutators
bool Parser::insert(Param *param) {
    // Set up options
    Option *option = dynamic_cast<Option *>(param);
    if (option) {
        // Ensure no name collisions
        if ((option->longname() && this->longnames.contains(option->longname())) ||
            (!option->shortname() && this->shortnames.contains(option->shortname())))
            return false;

        // Map longname to name
        if (option->longname())
            this->longnames.insert(std::make_pair(option->longname(), option->name));
        // Map shortname to name
        if (option->shortname())
            this->shortnames.insert(std::make_pair(option->shortname(), option->name));
    }

    // Map name to param
    if (!this->params.insert(std::make_pair(param->name, param)).second)
        return false; // name collision

    return true; // insertion successful
}

// helpers
void Parser::addAutoflags() {
    // Add automatic flags
    this->add(Flag("help").shortname('h').help("Print this message."));
    if (app.version().length())
        this->add(Flag("version").shortname('V').help("Print version information."));
}

void Parser::checkAutoflags(Option *option) const {
    // Check for automatic flags
    if (option->name == "help") {
        std::cout << this->help_s();
        std::exit(0);
    } else if (option->name == "version") {
        std::cout << this->version_s() << std::endl;
        std::exit(0);
    }
}

bool Parser::parseLongOption(int &i) {
    // Extract from argument
    std::string arg(this->argv[i]);
    std::string s = arg.substr(2);
    if (s.empty())
        return false;
    std::string longkey = s.substr(0, s.find("="));
    std::string value = (s.find('=') != std::string::npos) ? s.substr(s.find("=") + 1) :
                                                             std::string();
    // Search for a match
    bool matched = this->longnames.contains(longkey);
    if (!matched)
        Parser::error(1, fmt::format("illegal option: `--{}`", longkey));

    // Extract match
    std::string key = this->longnames.at(longkey);
    Param *match = this->params.at(key).get();

    // Should always be an `Option`
    Option *option = dynamic_cast<Option *>(match);
    if (!option)
        Parser::error(2, fmt::format("internal error: `{}` is not an `Option", key));

    // Count this match
    option->match();
    // Check for automatic flags
    this->checkAutoflags(option);

    // Keep track of if we've moved onto the next string
    bool advanced = false;

    // Check if match is an `AbstractOpt`
    AbstractOpt *opt = dynamic_cast<AbstractOpt *>(match);
    if (opt) {
        // Either use match's parsed value, or the next string
        if (value.empty()) {
            i++; // advance to next string
            advanced = true;
            if (i < this->argc)
                value = this->argv[i];
            else if (opt->optional())
                return true; // we don't need a value
            else
                Parser::error(1, fmt::format("missing value for `--{}`", longkey));
        }

        // Parse value into `Value`
        if (!opt->parse(value.data())) {
            if (advanced && opt->optional())
                i--; // return to previous string
            else
                Parser::error(1, fmt::format("invalid value for `--{}={}`", longkey, value));
        }
    }

    return true;
}

bool Parser::parseShortOption(int &i) {
    // Extract from argument
    std::string arg(this->argv[i]);
    std::string s = arg.substr(1);
    if (s.empty())
        return false;

    // Look through each character
    for (char &shortkey : s) {
        // Search for a match
        bool matched = this->shortnames.contains(shortkey);
        if (!matched)
            Parser::error(1, fmt::format("illegal option: `-{}`", shortkey));

        // Extract match (will always be an `Option`)
        std::string key = this->shortnames.at(shortkey);
        Param *match = this->params.at(key).get();

        // Should always be an `Option`
        Option *option = dynamic_cast<Option *>(match);
        if (!option)
            Parser::error(2, fmt::format("internal error: `{}` is not an `Option", key));

        // Count this match
        option->match();
        // Check for automatic flags
        this->checkAutoflags(option);

        // Keep track of if we've moved onto the next string
        bool advanced = false;

        // Check if match is an `AbstractOpt`
        AbstractOpt *opt = dynamic_cast<AbstractOpt *>(match);
        if (opt) {
            // Extract value from argument
            std::string value = &(&shortkey)[1];

            // Either use match's parsed value, or the next string
            if (value.empty()) {
                i++; // advance to next string
                advanced = true;
                if (i < this->argc)
                    value = this->argv[i];
                else if (opt->optional())
                    return true; // we don't need a value
                else
                    Parser::error(1, fmt::format("missing value for `-{}`", shortkey));
            }

            // Skip '=' in parsed value if present
            else if (value[0] == '=')
                value = value.substr(1);

            // Parse value into `Value`
            if (!opt->parse(value.data())) {
                if (advanced && opt->optional())
                    i--; // return to previous string
                else
                    Parser::error(1, fmt::format("invalid value for `-{}={}`", shortkey, value));
            }

            break; // we're done with this string
        }
    }

    return true;
}

bool Parser::parseArg(int &i, std::size_t &argidx) {
    // Extract arg
    const std::string &name = this->args.at(argidx);
    AbstractArg *arg = dynamic_cast<AbstractArg *>(this->params.at(name).get());

    // Attempt to parse into `Value`
    // NOTE: if parse failed on optional arg, continue anyways
    if (arg->parse(this->argv[i]) || arg->optional())
        argidx++;
    else
        Parser::error(1, fmt::format("invalid value for `{}`", name));

    return true;
}

// formatters
std::string Parser::help_s() const {
    // Format usage, flags, options, args
    // clang-format off
    std::string version = this->version_s();
    std::string usage   = this->usage_s();
    std::string flags   = this->flags_s();
    std::string options = this->opts_s();
    std::string args    = this->args_s();

    // Format help message
    std::stringstream ss;
    ss << version << std::endl;
    if (this->app.author().length())
        ss << this->app.author() << std::endl;
    if (this->app.about().length())
        ss << this->app.about() << std::endl;
    ss << std::endl;
    ss << "USAGE: " << std::endl
       << "\t" << usage << std::endl
       << std::endl;
    if (flags.length())
        ss << "FLAGS:" << std::endl
           << flags << std::endl;
    if (options.length())
        ss << "OPTIONS:" << std::endl
           << options << std::endl;
    if (args.length())
        ss << "ARGS:" << std::endl
           << args << std::endl;
    // clang-format on
    return ss.str();
}

std::string Parser::usage_s() const {
    std::stringstream ss;
    // clang-format off
    ss << this->app.name
       << (this->flags.size() ? " [FLAGS]"   : "")
       << (this->opts.size()  ? " [OPTIONS]" : "")
       << (this->args.size()  ? " <ARGS>"    : "");
    // clang-format on
    return ss.str();
}

std::string Parser::flags_s() const {
    const int WIDTH = 16;
    const std::string FMTWIDTH = fmt::format("{{:{}s}}", WIDTH);

    // Format all flags
    std::stringstream ss;
    for (const auto &name : this->flags) {
        const Flag *flag = dynamic_cast<Flag *>(this->params.at(name).get());
        // Format shortname
        std::string fmtshortname = flag->shortname() ? fmt::format("-{}, ", flag->shortname()) :
                                                       fmt::format("{:4s}", "");
        // Format longname
        std::string fmtlongname = fmt::format("--{}", flag->longname());
        // Format shortname + longname
        std::string fmtnames = fmtshortname + fmtlongname;
        if (fmtnames.length() < WIDTH)
            fmtnames = fmt::format(FMTWIDTH, fmtnames);
        else
            fmtnames = fmt::format("{}\n\t" + FMTWIDTH, fmtnames, "");
        // Append formatted flag
        ss << fmt::format("\t{}{}\n", fmtnames, flag->help());
    }
    return ss.str();
}

std::string Parser::opts_s() const {
    const int WIDTH = 24;
    const std::string FMTWIDTH = fmt::format("{{:{}s}}", WIDTH);

    // Format all opts
    std::stringstream ss;
    for (const auto &name : this->opts) {
        const AbstractOpt *opt = dynamic_cast<AbstractOpt *>(this->params.at(name).get());
        // Format shortname
        std::string fmtshortname = opt->shortname() ? fmt::format("-{}, ", opt->shortname()) :
                                                      fmt::format("{:4s}", "");
        // Format longname
        std::string fmtlongname = fmt::format("--{} ", opt->longname());
        // Format metavar
        std::string fmtmetavar = fmt::format(opt->optional() ? "[{}]" : "<{}>", opt->metavar());
        // Format shortname + longname + metavar
        std::string fmtnames = fmtshortname + fmtlongname + fmtmetavar;
        if (fmtnames.length() < WIDTH)
            fmtnames = fmt::format(FMTWIDTH, fmtnames);
        else
            fmtnames = fmt::format("{}\n\t" + FMTWIDTH, fmtnames, "");
        // Append formatted opt
        ss << fmt::format("\t{}{}\n", fmtnames, opt->help());
    }
    return ss.str();
}

std::string Parser::args_s() const {
    const int WIDTH = 16;
    const std::string FMTWIDTH = fmt::format("{{:{}s}}", WIDTH);

    // Format all args
    std::stringstream ss;
    for (const auto &name : this->args) {
        const AbstractArg *arg = dynamic_cast<AbstractArg *>(this->params.at(name).get());
        // Format name
        std::string fmtmetavar = fmt::format(arg->optional() ? "[{}]" : "<{}>", arg->metavar());
        if (fmtmetavar.length() < WIDTH)
            fmtmetavar = fmt::format(FMTWIDTH, fmtmetavar);
        else
            fmtmetavar = fmt::format("{}\n\t" + FMTWIDTH, fmtmetavar, "");
        // Append formatted arg
        ss << fmt::format("\t{}{}\n", fmtmetavar, arg->help());
    }
    return ss.str();
}

std::string Parser::version_s() const {
    return fmt::format("{} {}", this->app.name, this->app.version());
}

// explicit instantiations
// clang-format off
template Parser &Parser::add(const Opt<double>      &opt);
template Parser &Parser::add(const Opt<int>         &opt);
template Parser &Parser::add(const Opt<std::string> &opt);
template Parser &Parser::add(const Arg<double>      &arg);
template Parser &Parser::add(const Arg<int>         &arg);
template Parser &Parser::add(const Arg<std::string> &arg);
template const Flag             &Parser::get(const char *name) const;
template const Opt<double>      &Parser::get(const char *name) const;
template const Opt<int>         &Parser::get(const char *name) const;
template const Opt<std::string> &Parser::get(const char *name) const;
template const Arg<double>      &Parser::get(const char *name) const;
template const Arg<int>         &Parser::get(const char *name) const;
template const Arg<std::string> &Parser::get(const char *name) const;
template const Opt<double>      &Parser::getOpt(const char *name) const;
template const Opt<int>         &Parser::getOpt(const char *name) const;
template const Opt<std::string> &Parser::getOpt(const char *name) const;
template const Arg<double>      &Parser::getArg(const char *name) const;
template const Arg<int>         &Parser::getArg(const char *name) const;
template const Arg<std::string> &Parser::getArg(const char *name) const;
// clang-format on

} // namespace clip
