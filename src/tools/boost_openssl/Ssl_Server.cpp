/*
 * Ssl_Server.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: yas
 */

#include "Ssl_Server.h"


  Ssl_Server::Ssl_Server(boost::asio::io_service& io_service, unsigned short port)
    : io_service_(io_service),
      acceptor_(io_service,
          boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      context_(io_service, boost::asio::ssl::context::tlsv12_server)
  {
    context_.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    context_.set_password_callback(boost::bind(&Ssl_Server::get_password, this));
    context_.use_certificate_chain_file("server.crt");
    context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);
    context_.use_tmp_dh_file("dh512.pem");

    Session* new_session = new Session(io_service_, context_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&Ssl_Server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  std::string Ssl_Server::get_password() const
  {
    return "test";
  }

  void Ssl_Server::handle_accept(Session* new_session,const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session->start();
      new_session = new Session(io_service_, context_);
      acceptor_.async_accept(new_session->socket(),
          boost::bind(&Ssl_Server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
    }
    else
    {
      delete new_session;
    }
  }



Ssl_Server::~Ssl_Server() {
	// TODO Auto-generated destructor stub
}


int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: Ssl_Server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    Ssl_Server s(io_service, atoi(argv[1]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
