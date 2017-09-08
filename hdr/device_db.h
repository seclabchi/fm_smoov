#ifndef __DEVICE_DB_H__
#define __DEVICE_DB_H__

#include <vector>
#include <string>

using namespace std;

class DeviceDb
{
public:
    DeviceDb();
    virtual ~DeviceDb();
    size_t find_bi_devices();
    void get_device_name(size_t index, string** name);
    void get_card_names();
private:
    vector<string*>* m_dev_names;
    
};

#endif