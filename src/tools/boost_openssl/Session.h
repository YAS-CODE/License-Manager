/*
 * Session.h
 *
 *  Created on: Jul 1, 2017
 *      Author: yas
 */

#ifndef SESSION_H_
#define SESSION_H_

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class Session {
public:
	virtual ~Session();
	ssl_socket::lowest_layer_type& socket();
	Session(boost::asio::io_service& io_service, boost::asio::ssl::context& context): socket_(io_service, context){}
	void handle_write(const boost::system::error_code& error);
	void handle_read(const boost::system::error_code& error,size_t bytes_transferred);
	void handle_handshake(const boost::system::error_code& error);
	void start();
private:
	  ssl_socket socket_;
	  enum { max_length = 1024 };
	  char data_[max_length];
};

#endif /* SESSION_H_ */
