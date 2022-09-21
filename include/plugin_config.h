/*
 * plugin_config.h
 *
 *  Created on: Sep 19, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_CONFIG_H_
#define PLUGIN_CONFIG_H_

#include <string>
#include <map>

using namespace std;

class PluginConfigVal {
public:
	std::string name;
	float floatval;
	uint32_t uint32val;
	int32_t int32val;
};

class PluginConfig {
public:
	PluginConfig(const std::string _plugin_name);
	virtual ~PluginConfig();
	const std::string& get_name() const;
	void clear();
	void add(const std::string& name, const PluginConfigVal& val);
	void remove(const std::string& name);
	void change(const std::string& name, const PluginConfigVal& val);
	const std::map<std::string, PluginConfigVal>* const getConfig();
private:
	PluginConfig();
	std::string plugin_name;
	std::map<std::string, PluginConfigVal>& config_vals;

};


#endif /* PLUGIN_CONFIG_H_ */
