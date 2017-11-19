#ifndef __COMMAND_HANDLER_H__
#define __COMMAND_HANDLER_H__

#include <string>
#include <vector>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "processor.h"

using namespace std;

class CommandHandler
{
public:
    CommandHandler();
    virtual ~CommandHandler();
    void add_processor(Processor* p);
    void go();
    void stop();
private:
    vector<string> get_processor_names();
    void command(string command);
    void open_socket();
    
    static void* thread_func_wrapper(void* args);
    void thread_func();
    
    vector<Processor*>* m_processors;
    
    pthread_t m_thread;
    int m_sock_fd;
    struct sockaddr_in m_serv_addr;
    struct sockaddr_in m_cli_addr;
    int m_port_num;
    
    bool m_should_stop;
};

#endif /* __COMMAND_HANDLER_H__ */