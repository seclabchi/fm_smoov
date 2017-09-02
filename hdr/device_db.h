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
private:
    vector<string> m_dev_names;
    
};

#endif