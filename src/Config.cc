/**
 * gcodetimer
 *
 * Copyright © 2016 Juan Jose Gonzalez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Config.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "versioninfo.h"
#include "cfgpath.h"

using namespace std;
namespace pt = boost::property_tree;
namespace fs = boost::filesystem;


void Config::load() {
    string filename = get_path();

    // Create empty property tree object
    pt::ptree tree;

    // Parse the XML into the property tree.
    if (fs::exists(filename))
        pt::read_xml(filename, tree);

    max_print_accel = {
        tree.get("config.max_print_accel.x", 200.0f),
        tree.get("config.max_print_accel.y", 200.0f),
        tree.get("config.max_print_accel.z", 30.0f),
        tree.get("config.max_print_accel.e", 1000.0f)
        };
    max_move_accel = {
        tree.get("config.max_move_accel.x", 200.0f),
        tree.get("config.max_move_accel.y", 200.0f),
        tree.get("config.max_move_accel.z", 30.0f),
        0.0
        };
    max_jerk = {
        tree.get("config.max_jerk.x", 15.0f),
        tree.get("config.max_jerk.y", 15.0f),
        tree.get("config.max_jerk.z", 0.0f),
        tree.get("config.max_jerk.e", 1000.0f)
        };
    jerk_efficiency = tree.get("config.jerk_efficiency", 1.0f);
    accel_efficiency = tree.get("config.accel_efficiency", 1.0f);

    speed_multiplier = tree.get("config.speed_multiplier", 1.0f);
}


void Config::save() const {
    string filename = get_path();

    // Create an empty property tree object.
    pt::ptree tree;

    tree.put("config.max_print_accel.x", max_print_accel.x);
    tree.put("config.max_print_accel.y", max_print_accel.y);
    tree.put("config.max_print_accel.z", max_print_accel.z);
    tree.put("config.max_print_accel.e", max_print_accel.e);
    tree.put("config.max_move_accel.x", max_move_accel.x);
    tree.put("config.max_move_accel.y", max_move_accel.y);
    tree.put("config.max_move_accel.z", max_move_accel.z);
    tree.put("config.max_jerk.x", max_jerk.x);
    tree.put("config.max_jerk.y", max_jerk.y);
    tree.put("config.max_jerk.z", max_jerk.z);
    tree.put("config.max_jerk.e", max_jerk.e);
    tree.put("config.jerk_efficiency", jerk_efficiency);
    tree.put("config.accel_efficiency", accel_efficiency);

    tree.put("config.speed_multiplier", speed_multiplier);

    // Write property tree to XML file
    ofstream f (filename);
    pt::write_xml(f, tree, pt::xml_parser::xml_writer_make_settings<std::string>(' ', 4));
    f.close();

}

Config * Config::instance = NULL;

Config::Config() {
    load();
}

const Config* Config::get() {
    if (!instance)
        instance = new Config;
    return instance;
}

string Config::get_path() const {
    char cfgdir[MAX_PATH];
    get_user_config_folder(cfgdir, sizeof(cfgdir), Project_NAME);

    fs::path p (cfgdir);
    if (!(fs::exists(p) && fs::is_directory(p))) {
        fs::create_directories(p);
    }

    p /= CONFIG_FILENAME;

    return p.make_preferred().string();
}
