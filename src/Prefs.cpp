/*
 * Prefs.cpp
 *
 *  Created on: Nov 22, 2022
 *      Author: zaremba
 */

#include "Prefs.h"
#include <fstream>
#include <exception>
#include <sstream>

static const string PREFS_FILE_NAME = "smoovcontrol.yml";
static const string PREFS_GAIN_INPUT = "gain_input";

static Prefs* instance = nullptr;

Prefs* Prefs::get_instance() {
    if(nullptr == instance) {
        instance = new Prefs();
    }

    return instance;
}

Prefs::~Prefs() {
}

Prefs::Prefs() {
    const char* homedir = getenv("HOME");
    string homedirstr;
    stringstream homedirsstrm;

    if(!homedir) {
        throw runtime_error("fuck, couldn't figure out home dir!");
    }

    homedirstr = homedir;

    homedirsstrm << homedirstr << "/" << PREFS_FILE_NAME;
    m_prefs_filepath = homedirsstrm.str();

    try {
        prefs_node = YAML::LoadFile(m_prefs_filepath);
    }
    catch (const exception& ex) {
        write_prefs();
    }
}

void Prefs::write_prefs() {
    ofstream prefs_file(m_prefs_filepath);
    prefs_file << prefs_node;
}

double Prefs::get_gain_input() {
    double gain;

    if(!prefs_node[PREFS_GAIN_INPUT]) {
        prefs_node[PREFS_GAIN_INPUT] = 0.0;
    }

    gain = prefs_node[PREFS_GAIN_INPUT].as<double>();
    return gain;
}

void Prefs::set_gain_input(double gain) {
    prefs_node[PREFS_GAIN_INPUT] = gain;
    this->write_prefs();
}

