/*
 * Prefs.h
 *
 *  Created on: Nov 22, 2022
 *      Author: zaremba
 */

#ifndef PREFS_H_
#define PREFS_H_

#include <stdio.h>
#include <string>
#include <yaml-cpp/yaml.h>

using namespace std;

/* This will be a singleton */

class Prefs {
public:
    virtual ~Prefs();
    double get_gain_input();
    void set_gain_input(double gain);
    static Prefs* get_instance();
    static Prefs* the_instance;
private:
    Prefs();
    void write_prefs();
    YAML::Node prefs_node;
    string m_prefs_filepath;
};


#endif /* PREFS_H_ */
