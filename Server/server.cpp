#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "safeMap.h"
#include "jsonObj.h"

using boost::asio::ip::tcp;
    //depending on app interface this function can be used
    /*void quitFunc()
    {
        do{
            std::cout << "server is on\nTo exit type \"quit\"";
            char request[max_length];
            std::cin.getline(request, max_length);
            
            if(static_cast<std::string>(request) == "quit")
            exit(0);
                
            std::cout << "\nInvalid input!\n";
        }while(true);
    }*/

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(SafeMap<std::string,std::string> *safeMap,tcp::socket socket)
    : m_safeMap(safeMap), socket_(std::move(socket)) 
    {
        
    }
    
    void start()
    {
        do_read();
    }

private:
    SafeMap<std::string,std::string> *m_safeMap;
    tcp::socket socket_;
    char data_[max_length];
    
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {   
		        responseFunc();
                do_write(length);
            }
        });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                socket_.close();
                do_read();
            }
        });
    }
    
    void responseFunc()
    {
        JsonObj jsonObj(data_,m_safeMap);
        
        std::string response = jsonObj.getResponse() + "\n";
        std::cout << jsonObj.getRequest() << "\n";
        std::cout << response;
            
        for(size_t k = 0; k < max_length; ++k)
            k < response.size() ? data_[k] = response[k] :
                data_[k] = ' ';
    }
};

class Server
{
public:
    Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }
    ~Server()
    {
        delete safeMap;
    }

private:
    SafeMap<std::string,std::string> *safeMap = new SafeMap<std::string,std::string>;
    
    void do_accept()
    {
        acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<Session>(safeMap, std::move(socket))->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage async_tcp_echo_server: ./file_name <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        Server s(io_context, std::atoi(argv[1]));
        std::cout << "Server is on\n";
        io_context.run();
        //s.~Server(); //actually doesn't need here 
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

  return 0;
}
