#include "device_db.h"

#include <iostream>
#include <stdexcept>
#include <asoundlib.h>


DeviceDb::DeviceDb()
{
    m_dev_names = new vector<string*>();
}

DeviceDb::~DeviceDb()
{
    vector<string*>::iterator it;
    for(it = m_dev_names->begin(); it != m_dev_names->end(); it++)
    {
        delete(*it);
    }
    delete m_dev_names;
}

size_t DeviceDb::find_bi_devices()
{
    int retval = -1;
    void** hints = 0;
    retval = snd_device_name_hint(-1, "pcm", &hints);
    
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + "getting ALSA PCM device name hints.");
    }
    
    int hint_num = 0;
    void *hint = 0;
    
    string search_str("sysdefault:");
    
    do
    {
        hint = hints[hint_num];
        
        if(NULL != hint)
        {
            char* name = snd_device_name_get_hint(hint, "NAME");
            if(NULL != name)
            {
                cout << "NAME: " << name << endl;
                char* ioid = snd_device_name_get_hint(hint, "IOID");
                if(NULL == ioid)
                {
                    //this is a candidate bi-directional card that does 
                    //input and output.  Now find the 'sysdefault' device...
                    string name_str(name);
                    size_t found = name_str.find(search_str);
                    
                    if(found == 0)
                    {
                        m_dev_names->push_back(new string(name_str));
                    }
                }
            }
            hint_num++;
        }
        
    } while(NULL != hint);
    
    snd_device_name_free_hint(hints);
    
    return m_dev_names->size();
}

void DeviceDb::get_device_name(size_t index, string** name)
{
    if((index >= 0) && (index < m_dev_names->size()))
    {
        *name = new string(*(m_dev_names->at(index)));
    }
    else
    {
        throw runtime_error("Invalid index " + to_string(index) + " in get_device_name");
    }
}

void DeviceDb::get_card_names()
{
    int card_num = 0;
    int retval = 0;
    char* card_name = 0;
    
    do
    {
        retval = snd_card_get_name(card_num, &card_name);
        if(0 == retval)
        {
            printf("Card %d name is: %s\n", card_num, card_name);
            free(card_name);
            card_name = 0;
            snd_card_get_name(card_num, &card_name);
            printf("Card %d long name is: %s\n", card_num, card_name);
            free(card_name);
            card_num++;
        }
    } while(0 == retval);
}