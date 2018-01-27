#include "command_handler.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

CommandHandler::CommandHandler()
{
    m_processors = new map<string, Processor*>();
    m_port_num = 2323;
    m_should_stop = false;
}

CommandHandler::~CommandHandler()
{
    delete m_processors;
}

void CommandHandler::add_processor(Processor* p)
{
    bool duplicate = false;
    
    map<string, Processor*>::iterator p_it = m_processors->find(p->get_name());
    
    if(p_it != m_processors->end())
    {
        duplicate = true;
    }
    
    if(false == duplicate)
    {
        m_processors->insert(make_pair(p->get_name(), p));
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
    int bytes_sent = 0;
    int bytes_recvd = 0;
    
    while(false == m_should_stop)
    {
        cout << "Waiting for connection..." << endl;
        
        listen(m_sock_fd, 10);
                
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
        
        try
        {
            while(false == m_should_stop)
            {
                try
                {
                string tx_msg = "READY\n";
                
                bytes_sent = write(cli_sock_fd, tx_msg.c_str(), tx_msg.length());
                
                if (bytes_sent < 0) 
                {
                    throw runtime_error(string("Error ") + strerror(errno) + " writing to client.");
                }
                
                memset(buffer, 0, 256);
                
                bytes_recvd = read(cli_sock_fd, buffer, 255);
                
                if (bytes_recvd < 0) 
                {
                    throw runtime_error(string("Error ") + strerror(errno) + " reading from client.");
                }
                
                if(!strncmp("stop", (char*)buffer, 4))
                {
                    m_should_stop = true;
                }
                else
                {
                    string response = process_command((char*)buffer, bytes_recvd) + "\n";
                    
                    bytes_sent = write(cli_sock_fd, response.c_str(), response.length());
                    
                    if (bytes_sent < 0) 
                    {
                        throw runtime_error(string("Error ") + strerror(errno) + " writing to client.");
                    }
                }
                
                }
                catch(out_of_range& oorex)
                {
                    string response = string("ERROR: ") + oorex.what() + "\n";
                    
                    bytes_sent = write(cli_sock_fd, response.c_str(), response.length());
                    
                    if (bytes_sent < 0) 
                    {
                        throw runtime_error(string("Error ") + strerror(errno) + " writing to client.");
                    }
                }
            }
            
            close(cli_sock_fd);
            
        }
        catch(runtime_error& err)
        {
            cout << "command_handler: " << err.what() << endl;
            close(cli_sock_fd);
        }
    }
    
    cout << "Client closed." << endl;
    
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
    map<string, Processor*>::iterator it;
    vector<string> proc_names;
    
    for(it = m_processors->begin(); it != m_processors->end(); it++)
    {
        proc_names.push_back(it->first);
    }
    
    return proc_names;
}

string CommandHandler::process_command(char* command, uint32_t len)
{
    string cmd = string(command);
    string retval = "ERROR";
    
    /* FIXME: Assumes that the command string has a \r\n at the end.  This needs
     * to be made more robust.
     */
     
    cmd.pop_back();
    cmd.pop_back();
    
    if(cmd.length() > 0)
    {
    
        cout << "Command received: " << cmd << endl;
        
        istringstream iss(cmd);
        string sub_cmd;
        vector<string> cmds;
        
        while(getline(iss, sub_cmd, ' '))
        {
            cmds.push_back(sub_cmd);
        }
        
        map<string, Processor*>::iterator it = m_processors->find(cmds.at(0));
        
        if(it != m_processors->end())
        {
            retval = it->second->do_command(cmds);
        }
    
    }
    else
    {
        cout << "command length was 0" << endl;
    }
    
    return retval;
}