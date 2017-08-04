/*
 * Session.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: yas
 */

#include "Session.h"


	ssl_socket::lowest_layer_type& Session::socket()
	  {
	    return socket_.lowest_layer();
	  }

  void Session::start()
  {
	socket_.async_handshake(boost::asio::ssl::stream_base::server,
		boost::bind(&Session::handle_handshake, this,
		  boost::asio::placeholders::error));
  }

  void Session::handle_handshake(const boost::system::error_code& error)
  {
	if (!error)
	{
	  socket_.async_read_some(boost::asio::buffer(data_, max_length),
		  boost::bind(&Session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	else
	{
	  delete this;
	}
  }

  void Session::handle_read(const boost::system::error_code& error,size_t bytes_transferred)
  {
	if (!error)
	{
	  boost::asio::async_write(socket_,
		  boost::asio::buffer(data_, bytes_transferred),
		  boost::bind(&Session::handle_write, this,
			boost::asio::placeholders::error));
	}
	else
	{
	  delete this;
	}
  }

  void Session::handle_write(const boost::system::error_code& error)
  {
	if (!error)
	{
	  socket_.async_read_some(boost::asio::buffer(data_, max_length),
		  boost::bind(&Session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	else
	{
	  delete this;
	}
  }



Session::~Session() {
	// TODO Auto-generated destructor stub
}
