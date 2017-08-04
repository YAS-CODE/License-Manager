/*
 * Ssl_Client.h
 *
 *  Created on: Jul 1, 2017
 *      Author: yas
 */

#ifndef SSL_CLIENT_H_
#define SSL_CLIENT_H_


#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

enum { max_length = 1024 };


class Ssl_Client {
public:

	Ssl_Client(boost::asio::io_service& io_service,
	      boost::asio::ssl::context& context,
	      boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
	void handle_read(const boost::system::error_code& error,size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_handshake(const boost::system::error_code& error);
	void handle_connect(const boost::system::error_code& error);
	bool verify_certificate(bool preverified,boost::asio::ssl::verify_context& ctx);
	virtual ~Ssl_Client();


private:
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
  char request_[max_length];
  char reply_[max_length];
};

#endif /* SSL_CLIENT_H_ */
