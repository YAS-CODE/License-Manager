/*
 * Ssl_Server.h
 *
 *  Created on: Jul 1, 2017
 *      Author: yas
 */

#ifndef SSL_SERVER_H_
#define SSL_SERVER_H_
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "Session.h"

class Ssl_Server {
public:
	Ssl_Server();
	virtual ~Ssl_Server();
	//
	// server.cpp
	// ~~~~~~~~~~
	//
	// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
	//
	// Distributed under the Boost Software License, Version 1.0. (See accompanying
	// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
	//
	Ssl_Server(boost::asio::io_service& io_service, unsigned short port);
	std::string get_password() const;
	void handle_accept(Session* new_session,const boost::system::error_code& error);



private:
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ssl::context context_;

};

#endif /* SSL_SERVER_H_ */
