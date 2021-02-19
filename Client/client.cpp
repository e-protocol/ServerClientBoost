#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include "json.hpp"

using boost::asio::ip::tcp;

enum { max_length = 1024 };

class Client : public std::enable_shared_from_this<Client>
{
private:
        boost::asio::io_context m_io_context;
        tcp::socket m_socket;
        tcp::resolver m_resolver;
        char data_[max_length];
        char* m_argv_1;
        char* m_argv_2;
        bool is_reconnection = true;
public:
    Client(char* argv[]) : m_socket(m_io_context), m_resolver(m_io_context)
    {
        m_argv_1 = argv[1];
        m_argv_2 = argv[2];
        boost::asio::connect(m_socket, m_resolver.resolve(argv[1], argv[2]));
    }
    
    void inputParser()
    {
        do{
            //type command to send request
            std::cout << "Enter command (type \"help\" to view commands)->";
            char request[max_length];
            std::cin.getline(request, max_length);
            
            std::vector<std::string> command_list = splitter(static_cast<std::string>(request));

            if(command_list[0] == "quit")
                commands(0,"");
            else if(command_list[0] == "help")
                commands(70,"");
            else if(command_list[0] == "read")
                command_list.size() < 2 ? commands(1,"") : commands(1,command_list[1]);
            else if(command_list[0] == "write")
                command_list.size() < 3 ? commands(2,"") : commands(2,command_list[1] + 
                + " " + command_list[2]);  
            else if(command_list[0] == "disable")
                commands(3,"");
            else
                commands(4,"");
          }while(true);
    }
        
    void commands(int command_id, const std::string input)
    {
        switch(command_id)
        {
            case(0):
            {
                exit(0);
            }
            //read
            case(1):
            {
                if(input.empty())
                {
                    std::cout << "Invalid command!\n";
                    break;
                }
                
                nlohmann::json json;
            	json["Request"] = "read";
            	json["key"] = input;
            	writeBytes(json.dump() + "\n");
            	
            	//response
                response();
                break;
            }
            //write
            case(2):
            {
                std::vector<std::string> splitted = splitter(input);
                
                if(splitted.size() < 2)
                {
                    std::cout << "Invalid command!\n";
                    break;
                }
                
                nlohmann::json json;
                json["Request"] = "write";
            	json["key"] = splitted[0];
            	json["value"] = splitted[1];
            	writeBytes(json.dump() + "\n");
            	
            	//response
                response();
                break;
            }
            case(3):
            {
                is_reconnection = false;
                std::cout << "Reconnection is disabled!\n";
                break;
            }
            case(4):
            {
                std::cout << "Invalid input!\n";
                break;
            }
            default:
            {
                std::cout << "\n\"quit\" - exit programm\n" \
                             "\"read\" <key> - get value from storage\n" \
                             "\"write\" <key> <value> - to write pair string to storage\n" \
                             "\"disable\" - disable reconnection to server\n";
            }
        }
    }
    
    std::vector<std::string> splitter(std::string input)
    {
        std::vector<std::string> splitted;
        std::istringstream ss(input);
        std::string substr;
        while(std::getline(ss,substr,' '))
            splitted.push_back(substr);
        return splitted;
    }
    
    void writeBytes(const std::string in)
    {
        for(size_t k = 0; k < max_length; ++k)
        {
            if(k < in.size())
                data_[k] = in[k];
            else
                data_[k] = ' ';
        }
        
        boost::asio::write(m_socket, boost::asio::buffer(data_, max_length));
    }
    
    void response()
    {
        char reply[max_length];
        //size_t reply_length = 
        boost::asio::read(m_socket, boost::asio::buffer(reply, max_length));
        std::string input;
        
        for(size_t k = 0; k < max_length; ++k)
        {
            if(reply[k] != '\n')
                input.push_back(reply[k]);
            else
                break;
        }
        
        std::cout << input + "\n";   
        checkConnection();
    }
    
    void checkConnection()
    {
        boost::asio::streambuf res;
        boost::system::error_code ec;
        boost::asio::read(m_socket, res, ec);
        
        //reconnect
        if(ec == boost::asio::error::eof && is_reconnection)  
        {
            std::cout << "Connection closed by peer - reconnecting\n";
            m_socket.close();
            boost::asio::connect(m_socket, m_resolver.resolve(m_argv_1, m_argv_2));
        }
        else
            throw std::runtime_error("Connection closed");
    }
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage blocking_tcp_echo_client: ./file_name <host> <port>\n";
            return 1;
        }
        
        Client client(argv);
        client.inputParser();      
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

