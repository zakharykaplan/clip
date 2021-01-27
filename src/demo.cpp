//
//  demo.cpp
//  Clip demo application.
//
//  Created by Zakhary Kaplan on 2020-12-17.
//  Copyright © 2020 Zakhary Kaplan. All rights reserved.
//
//  SPDX-License-Identifier: MIT
//

#include <iostream>
#include <string>

#include "clip/clip.h"

using namespace std;

int main(int argc, char *argv[]) {
    // Create parser
    clip::Parser parser(argc,
                        argv,
                        clip::App("demo")
                            .about("My awesome app. Parsed by clip!")
                            .author("Zakhary Kaplan <zakharykaplan@gmail.com>")
                            .version("0.1.0"));
    // Add parser options
    parser.add(clip::Opt<double>("double").shortname('d').help("Sample double.").value(0.));
    parser.add(clip::Opt<int>("int").shortname('i').help("Sample integer.").value(0));
    parser.add(
        clip::Opt<string>("string").shortname('s').help("Sample string.").value("Hello, world!"));
    parser.add(clip::Flag("flag").shortname('f').help("Sample flag."));
    // Parse args
    parser.parse();

    // Retrieve args
    const auto &d = parser.getOpt<double>("double");
    const auto &i = parser.getOpt<int>("int");
    const auto &s = parser.getOpt<string>("string");
    const auto &f = parser.getFlag("flag");

    // Print supplied args
    if (d.count())
        cout << d.name << ": .count = " << d.count() << ", .value = " << d.value() << endl;
    if (i.count())
        cout << i.name << ": .count = " << i.count() << ", .value = " << i.value() << endl;
    if (s.count())
        cout << s.name << ": .count = " << s.count() << ", .value = " << s.value() << endl;
    if (f.count())
        cout << f.name << ": .count = " << f.count() << endl;
}
