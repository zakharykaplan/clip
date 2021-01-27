//
//  timer.cpp
//  Clip demo application.
//
//  Created by Zakhary Kaplan on 2021-01-03.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "clip/clip.h"

using namespace std;

int main(int argc, char *argv[]) {
    // Create parser
    clip::Parser parser(argc,
                        argv,
                        clip::App("timer")
                            .about("My awesome timer app. Parsed by clip!")
                            .author("Zakhary Kaplan <zakharykaplan@gmail.com>")
                            .version("0.1.0"));
    // Add parser options
    parser.add(clip::Opt<int>("repeat")
                   .shortname('n')
                   .metavar("INT")
                   .help("Number of times to repeat the delay.")
                   .value(1));
    parser.add(clip::Opt<string>("message")
                   .shortname('m')
                   .help("Message to print after each delay.")
                   .metavar("STRING")
                   .value("Time's up!"));
    parser.add(clip::Arg<double>("delay").help("Desired delay length (in seconds).").value(0.));
    parser.add(clip::Flag("verbose").shortname('v').help("Set verbosity level."));
    // Parse args
    parser.parse();

    // Retrieve args
    const auto &delay = parser.getArg<double>("delay");
    const auto &repeat = parser.getOpt<int>("repeat");
    const auto &message = parser.getOpt<string>("message");
    const auto &verbose = parser.getFlag("verbose");

    // Validate args
    if (delay.value() < 0)
        clip::Parser::error(1, "delay must be greater than 0");
    if (repeat.value() < 0)
        clip::Parser::error(1, "repeat must be greater than 0");

    // Handle verbose output
    if (verbose.count()) // any verbosity level
        cout << "Setting " << repeat.value() << " timers for " << delay.value() << " seconds."
             << endl;

    // Repeat for each timer
    chrono::duration<double> dur(delay.value());
    for (int i = 0; i < repeat.value(); i++) {
        if (verbose.count() >= 2) // verbosity level 2+
            cout << "Starting timer: " << i << "/" << repeat.value() << "..." << endl;

        this_thread::sleep_for(dur);

        cout << message.value() << endl;
    }
}
