#include "command_handler.h"

#include <stdexcept>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

CommandHandler::CommandHandler()
{
    m_processors = new vector<Processor*>();
    m_port_num = 2323;
    m_should_stop = false;
}

CommandHandler::~CommandHandler()
{
    delete m_processors;
}

void CommandHandler::add_processor(Processor* p)
{
    vector<Processor*>::iterator it = m_processors->begin();
    bool duplicate = false;
    
    while(it != m_processors->end())
    {
        if(0 == p->get_name().compare((*it)->get_name()))
        {
            duplicate = true;
            break;
        }
        
        it++;
    }
    
    if(false == duplicate)
    {
        m_processors->push_back(p);
    }
    else
    {
        throw runtime_error("Attempt to add duplicate processor named " + p->get_name());
    }
}

void* CommandHandler::thread_func_wrapper(void* args)
{
    CommandHandler* the_obj = static_cast<CommandHandler*>(args);
    the_obj->thread_func();
    return NULL;
}

void CommandHandler::thread_func()
{
    int cli_sock_fd = 0;
    uint8_t buffer[256];
    char ip_addr_str[32] = {'\0'};
    
    while(false == m_should_stop)
    {
        cout << "Waiting for connection..." << endl;
        
        listen(m_sock_fd, 5);
                
        socklen_t clilen = sizeof(m_cli_addr);
        cli_sock_fd = accept(m_sock_fd, 
                 (struct sockaddr *) &m_cli_addr, 
                 &clilen);
                 
        if (cli_sock_fd < 0) 
        {
            throw runtime_error(string("Error ") + strerror(errno) + " accepting connection.");
        }
        
        inet_ntop(AF_INET, &(m_cli_addr.sin_addr), ip_addr_str, 32);
        
        cout << "Connected to client at " << ip_addr_str << ":" << ntohs(m_cli_addr.sin_port) << endl;
        
        string tx_msg = "Hi, Heather!\n";
        
        write(cli_sock_fd, tx_msg.c_str(), tx_msg.length());
        
        memset(buffer, 0, 256);
        
        int n = read(cli_sock_fd,buffer,255);
        
        if (n < 0) 
        {
            throw runtime_error(string("Error ") + strerror(errno) + " reading from client.");
        }
        
        cout << "Here is the message: " << buffer << endl;
        n = write(cli_sock_fd, "I got your message\n", 18);
        
        if (n < 0) 
        {
            throw runtime_error(string("Error ") + strerror(errno) + " writing to client.");
        }
        
        if(!strncmp("stop", (char*)buffer, 4))
        {
            m_should_stop = true;
        }
        
        close(cli_sock_fd);
        
        cout << "Client closed." << endl;
    }
    
}

void CommandHandler::go()
{
    this->open_socket();
    pthread_attr_t thread_attr;
    
    pthread_attr_init(&thread_attr);
    pthread_create(&m_thread, &thread_attr, thread_func_wrapper, (void*)this);
    pthread_attr_destroy(&thread_attr);
    sleep(1);
    void* retval = 0;
    pthread_join(m_thread, &retval);
}

void CommandHandler::stop()
{
    m_should_stop = true;
}

void CommandHandler::open_socket()
{
    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(m_sock_fd < 0)
    {
        throw runtime_error(string("Error ") + strerror(errno) + " opening socket.");
    }
    
    memset(&m_serv_addr, 0, sizeof(m_serv_addr));
    
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_addr.s_addr = INADDR_ANY;
    m_serv_addr.sin_port = htons(m_port_num);
    
    int optval = 1;
    setsockopt(m_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    int retval = bind(m_sock_fd, (struct sockaddr *) &m_serv_addr, sizeof(m_serv_addr));
    
    if(retval < 0)
    {
        throw runtime_error(string("Error ") + strerror(errno) + " binding socket.");
    }
    
}

vector<string> CommandHandler::get_processor_names()
{
    vector<Processor*>::iterator it;
    vector<string> proc_names;
    
    for(it = m_processors->begin(); it != m_processors->end(); it++)
    {
        proc_names.push_back((*it)->get_name());
    }
    
    return proc_names;
}

void CommandHandler::command(string command)
{
    
}