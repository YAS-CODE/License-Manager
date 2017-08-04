// LicenseManagerServer.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>
#include "../connection.hpp" // Must come before boost/serialization headers.
#include <boost/serialization/vector.hpp>
#include "../LicenseComm.hpp"
#include "../../library/LicenseReader.h"
#include "license-generator.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <build_properties.h>
#include <boost/filesystem.hpp>
#include "../base_lib/CryptoHelper.h"
#include <private-key.h>
#include <iostream>
#include<fstream>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/assign.hpp>
#include "ProductManager.h"

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>



namespace license {


/// Serves stock quote information to any client that connects to it.
class LicenseManagerServer: public LicenseGenerator, public ProductManager {
public:
	/// Constructor opens the acceptor and starts waiting for the first incoming
	/// connection.
	LicenseManagerServer(boost::asio::io_service& io_service,
			unsigned short port) :
			acceptor_(io_service,
					boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
							port)), io_service_(io_service), context_(
					io_service, boost::asio::ssl::context::tlsv12_server), LicenseGenerator(), ProductManager() {
		context_.set_options(
				boost::asio::ssl::context::default_workarounds
						| boost::asio::ssl::context::no_sslv2
						| boost::asio::ssl::context::single_dh_use);
		context_.set_password_callback(
				boost::bind(&LicenseManagerServer::get_password, this));
		//writekeyspair("Puks.key","Prks.key");
		context_.use_certificate_chain_file("server.crt");
		context_.use_private_key_file("server.key",
				boost::asio::ssl::context::pem);
		context_.use_tmp_dh_file("dh512.pem");

		//session* new_session = new session(io_service_, context_);
		//connection_ptr conn = new connection(io_service_, context_);
		connection_ptr conn(new connection(io_service_, context_));
		acceptor_.async_accept(conn->socket(),
				boost::bind(&LicenseManagerServer::handle_accept, this, conn,
						boost::asio::placeholders::error));


	}

	std::string get_password() const {
		return "test";
	}

	void handle_accept(connection_ptr conn,
			const boost::system::error_code& error) {
		if (!error) {
			start(conn);
			//conn = new connection(io_service_, context_);
			connection_ptr conn(new connection(io_service_, context_));
			acceptor_.async_accept(conn->socket(),
					boost::bind(&LicenseManagerServer::handle_accept, this,
							conn, boost::asio::placeholders::error));
		}

	}

	void start(connection_ptr conn) {
		conn->getsocket().async_handshake(boost::asio::ssl::stream_base::server,
				boost::bind(&LicenseManagerServer::handle_handshake, this,
						boost::asio::placeholders::error, conn));
	}

	void handle_handshake(const boost::system::error_code& e,
			connection_ptr conn) {
		if (!e) {

			conn->async_read(licensepackets,
					boost::bind(&LicenseManagerServer::handle_read, this,
							boost::asio::placeholders::error, conn));

		} else {
#ifdef _DEBUG
			std::cerr << "Reciecve error:" <<e<<endl;
#endif
			//delete this;
		}
	}
	void handle_write(const boost::system::error_code& error,
			connection_ptr conn) {
		if (!error) {
			conn->getsocket().async_read_some(
					boost::asio::buffer(data_, max_length),
					boost::bind(&LicenseManagerServer::handle_read, this,
							boost::asio::placeholders::error, conn));
		}
	}

	vector<FullLicenseInfo> parseLicenseInfo(LicenseComm & lc) {

		vector<FullLicenseInfo> licInfo;
		LicenseLocation licenseLocation;
		licenseLocation.openFileNearModule = false;
		licenseLocation.licenseFileLocation = lc.products[0].c_str(); //lc.client_signature.c_str();
		licenseLocation.environmentVariableName = "";

		//if (lc.CammnadType == EVENT_TYPE::LICENSE_OK || lc.CammnadType == EVENT_TYPE::LICENSE_FILE_NOT_FOUND) {
		vector<FullLicenseInfo> licenseInfo;
		LicenseReader licreader(licenseLocation);

		for (auto it = lc.products.begin(); it != lc.products.end(); ++it) {
			licreader.readLicenses(*it, licenseInfo);


			for (int i = 0; i < licenseInfo.size(); i++) {

				if (!lc.server_signature.empty())
					licenseInfo[i].server_signature = lc.server_signature;
#ifdef _DEBUG
				std::cout <<__LINE__ << "  days_left: " << licenseInfo[i].source << "\n";
				//std::cout << "  license_type: " << it->prod << "\n";
				//std::cout << "  proprietary_data: " << licenseInfo[i].proprietary_data << "\n";
				std::cout <<__LINE__ << "  license_version: " << licenseInfo[i].license_version << "\n";
				std::cout <<__LINE__ << "  client_signature: " << licenseInfo[i].client_signature << "\n";
				std::cout <<__LINE__ << "  products: " << licenseInfo[i].product << "\n";
				std::cout <<__LINE__ << "  license_signature: " << licenseInfo[i].license_signature << "\n";
				std::cout <<__LINE__ << "  server_signature: " << licenseInfo[i].server_signature << "\n";
				std::cout <<__LINE__ << "  from_date: " << licenseInfo[i].from_date << "\n";
				std::cout <<__LINE__ << "  to_date: " << licenseInfo[i].to_date << "\n";
				std::cout <<__LINE__ << "  has_expiry: " << licenseInfo[i].has_expiry << "\n";
				std::cout <<__LINE__ << "  from_sw_version: " << licenseInfo[i].from_sw_version << "\n";
				std::cout <<__LINE__ << "  to_sw_version: " << licenseInfo[i].to_sw_version << "\n";
				std::cout <<__LINE__ << "  has_versions: " << licenseInfo[i].has_versions << "\n";
				std::cout <<__LINE__ << "  has_client_sig: " << licenseInfo[i].has_client_sig << "\n";
				std::cout <<__LINE__ << "  extra_data: " << licenseInfo[i].extra_data << "\n";
				std::cout <<__LINE__ << "  has_use_limit: " << licenseInfo[i].has_use_limit << "\n";
				std::cout <<__LINE__ << "  use_limit: " << licenseInfo[i].use_limit << "\n";
				std::cout <<__LINE__ << "   usage_remain: " << licenseInfo[i].usage_remain << "\n";
#endif
			}
			licInfo.insert(licInfo.end(), licenseInfo.begin(),
					licenseInfo.end());

		}
		if (licInfo.size() > 0)
			return licInfo;

		string begin_date = FullLicenseInfo::UNUSED_TIME;
		string end_date = FullLicenseInfo::UNUSED_TIME;
		bool has_use_limit;
		double use_limit;
		double usage_remain = 0;

		char curdate[20];
		if (lc.products.size() == 0) {  	       //vm.count("product") == 0) {
			cerr << endl << "Parameter [product] not found. " << endl;
			exit(2);
		}
		vector < string > products = lc.products; //vm["product"].as<vector<string>>();
		licInfo.reserve(products.size());
		for (auto it = products.begin(); it != products.end(); it++) {
			if (boost::algorithm::trim_copy(*it).length() > 0) { //todo take it back

				if (!lc.to_date.empty()) {		///vm.count("expire_date")) {
					const std::string dt_end = lc.to_date;//vm["expire_date"].as<string>();
					try {
						//end_date = normalize_date(dt_end.c_str());
						end_date = dt_end;

						time_t curtime = time(NULL);
						strftime(curdate, 20, "%Y-%m-%d", localtime(&curtime));
						begin_date.assign(curdate);
					} catch (const invalid_argument &e) {
						cerr << endl << "End date not recognized: " << dt_end
								<< " Please enter a valid date in format YYYYMMDD"
								<< endl;
						exit(2);
					}
				}
				if (!lc.from_date.empty()) {		//vm.count("begin_date")) {
					const std::string begin_date_str = lc.from_date;//vm["begin_date"].as<string>();
					try {
						//begin_date = normalize_date(begin_date_str.c_str());
						begin_date = begin_date_str;
					} catch (invalid_argument &e) {
						cerr << endl << "Begin date not recognized: "
								<< begin_date_str
								<< " Please enter a valid date in format YYYYMMDD"
								<< endl;
						//print_usage(vm);
						exit(2);
					}
				}
				string client_signature = "";
				if (!lc.client_signature.empty()) {	//vm.count("client_signature")) {
					client_signature = lc.client_signature;	//vm["client_signature"].as<string>();
				}
				string server_signature = "";
				if (!lc.server_signature.empty()) {	//vm.count("client_signature")) {
					server_signature = lc.server_signature;	//vm["client_signature"].as<string>();
				}
				string extra_data = "";
				if (!lc.extra_data.empty()) {		//vm.count("extra_data")) {
					extra_data = lc.extra_data;	//vm["extra_data"].as<string>();
				}
				unsigned int from_sw_version = lc.from_sw_version;//vm["start_version"].as<unsigned int>();
				unsigned int to_sw_version = lc.to_sw_version;//vm["end_version"].as<unsigned int>();
				if (*it == "smart-eye") { //product profile....
					time_t t1 = time(0); // get time now
					struct tm * now = localtime(&t1);
					snprintf(curdate, 20, "%4d-%02d-%02d", now->tm_year + 1900,
							now->tm_mon, now->tm_mday);
					begin_date.assign(curdate);
					snprintf(curdate, 20, "%4d-%02d-%02d",
							now->tm_year + 1900 + 1, now->tm_mon, now->tm_mday);
					end_date.assign(curdate);
					has_use_limit = false;
					use_limit = 10;
					usage_remain = 0;
					licInfo.push_back(
							FullLicenseInfo("", *it, "", PROJECT_INT_VERSION,
									begin_date, end_date, client_signature,
									server_signature, from_sw_version,
									to_sw_version, extra_data, has_use_limit,
									use_limit, usage_remain));
				} else {
					product_config pc = getProduct(*it);
					if (pc.product == *it) {
						//if(pc!=NULL)
//  						cout<<".................................begin date:"<<pc.from_date.c_str()<<endl;
						licInfo.push_back(
								FullLicenseInfo("", pc.product, "",
								PROJECT_INT_VERSION, pc.from_date, pc.to_date,
										client_signature, server_signature,
										pc.from_sw_version, pc.to_sw_version,
										pc.extra_data, pc.has_use_limit,
										pc.use_limit, usage_remain));
					} else {
						licInfo.push_back(
								FullLicenseInfo("", *it, "",
								PROJECT_INT_VERSION, begin_date, end_date,
										client_signature, server_signature,
										from_sw_version, to_sw_version,
										extra_data, has_use_limit, use_limit,
										usage_remain));
					}
					//continue;

				}

			}
		}

		return licInfo;
	}

	vector<FullLicenseInfo> generateAndOutputLicenses(LicenseComm &lc) {
		vector<FullLicenseInfo> licenseInfo = parseLicenseInfo(lc);

		return licenseInfo;

	}
	vector<licsenceCommPack> parseLincenseToCom(
			vector<FullLicenseInfo> genresponse, EVENT_TYPE response_event,
			Crypto * crypto, char * remote_pub_key) {
		vector<licsenceCommPack> genresponsecom;
		//Crypto crypto;
		unique_ptr<CryptoHelper> helper = CryptoHelper::getInstance();
		const char pkey[] = PRIVATE_KEY;
		size_t len = sizeof(pkey);

		licsenceCommPack lcp;

		for (auto it = genresponse.begin(); it != genresponse.end(); ++it) {
			vector < string > products;
			products.push_back(it->product);


			LicenseComm lc;
			product_config pc = getProduct(it->product);
			if (pc.product != it->product) {
				lc.CammnadType = EVENT_TYPE::PRODUCT_NOT_CONFIGERED;
			} else if (it->client_signature != pc.client_signature) {
				cout << "mismatch :" << it->client_signature << " and "
						<< pc.client_signature << endl;
				lc.CammnadType = EVENT_TYPE::IDENTIFIERS_MISMATCH;
			} else if (it->server_signature.empty() && enablesignedvarification) {
				lc.CammnadType = EVENT_TYPE::PRODUCT_NOT_LICENSED;

			} else {
				bool sigVerified=true;
				if(enablesignedvarification){
				FUNCTION_RETURN sigVer = verifySignature(
						it->printForSignServer().c_str(),
						it->server_signature.c_str());
				sigVerified = sigVer == FUNC_RET_OK;
				}
				if (sigVerified) {
					//lc.param_to_update.assign(":param_to_update");
					//lc.CammnadType = LICENSE_VERIFIED;
					if (strncmp(remote_pub_key, pc.pukc.c_str(), pc.pukc.size())
							== 0) {
						lc.CammnadType = LICENSE_VERIFIED;
						time_t now = time(NULL);
						if (it->expires_on() < now || it->valid_from() > now ) {
							lc.CammnadType = EVENT_TYPE::PRODUCT_EXPIRED;
						}

						else if (it->has_use_limit) {
							if (it->use_limit <= it->usage_remain)
								lc.CammnadType = EVENT_TYPE::PRODUCT_EXPIRED;
							else
								it->usage_remain++;
							//it->param_to_update=it->param_to_update+"usage_remain";
						}
					} else
						lc.CammnadType = LICENSE_KEYMISMATCH;

				} else {
					//lc.param_to_update.assign(":param_to_update");
					lc.CammnadType = LICENSE_CORRUPTED;
				}

			}


			const string fname = it->product;


			fstream ofstream(fname, std::ios::out | std::ios::trunc);
			if (!ofstream.is_open()) {
				cerr << "can't open file [" << fname << "] for Lincese." << endl
						<< " error: " << strerror(errno);
				//exit(3);
			}

			const string license = it->printForSign();
#ifdef _DEBUG
			cout << "lincense_generate " << license << endl;
#endif

			string signature = helper->signString((const void *) pkey, len,
					license);
			it->license_signature = signature;

			signature = helper->signString((const void *) pkey, len,
					it->printForSignServer());
#ifdef _DEBUG
			cout << "license server gen:- " << signature << endl;
#endif

			it->printAsIni(ofstream);
#ifdef _DEBUG
			it->printAsIni(cout);
#endif

			if (ofstream.is_open())
				ofstream.close();

			//genresponsecom.push_back(LicenseComm(response_event,0,LICENSE_TYPE::REMOTE,"",it->license_version,it->client_signature,products,it->license_signature,it->from_date,it->to_date,false,it->from_sw_version,it->to_sw_version,it->has_versions,it->has_client_sig,it->extra_data,it->has_use_limit,it->use_limit,it->usage_remain));

			lc.days_left = 0;
			lc.license_type = LICENSE_TYPE::REMOTE; // Local or Remote

			lc.proprietary_data = "";
			lc.license_version = it->license_version; //license file version
			lc.client_signature = it->client_signature;

			lc.products = products;
			lc.license_signature = it->license_signature;

			//cout << endl << "lc.from_date= " << lc.from_date;
			lc.from_date = it->from_date; //it->from_date;
			lc.to_date = it->to_date; //it->to_date;


			lc.has_expiry = it->has_expiry;
			lc.from_sw_version = it->from_sw_version;
			lc.to_sw_version = it->to_sw_version;
			lc.has_versions = it->has_versions;

			lc.has_client_sig = it->has_client_sig;
			lc.extra_data = it->extra_data;
			lc.has_use_limit = it->has_use_limit;
			lc.use_limit = it->use_limit;
			lc.usage_remain = it->usage_remain;
			lc.server_signature = it->server_signature;

			lcp.decrypted_length = packet_manager::objToCharArray(
					(unsigned char*) &(lcp.decrypted), lc);
			lcp.encrypted_length = crypto->public_encrypt(lcp.decrypted,
					lcp.decrypted_length, remote_pub_key, lcp.encrypted);
			genresponsecom.push_back(lcp);

		}
		return genresponsecom;

	}

//#if 0
	virtual void handle_read(const boost::system::error_code& e,
			connection_ptr conn) {

		if (!e) {
			parseData(conn);
		} else {
			// An error occurred.
#ifdef _DEBUG
			std::cerr << __LINE__ << "Reciecve error:" << e.message()
					<< std::endl;
#endif
		}

		// Since we are not starting a new operation the io_service will run out of
		// work to do and the client will exit.
	}
//#endif
	template<typename T>
	void Write(T& t) {
		//connection_.async_write(t,boost::bind(&server::handle_write, this,boost::asio::placeholders::error));
	}
	void parseData(connection_ptr conn) {
		Crypto crypto;
		LicenseComm lc;
#if 1

		for (std::size_t i = 0; i < licensepackets.size(); ++i) {

			licensepackets[i].decrypted_length = crypto.public_decrypt(
					licensepackets[i].encrypted,
					licensepackets[i].encrypted_length,
					(char *) &(licensepackets[i].publicKey),
					licensepackets[i].decrypted);

			packet_manager::charArrayToObj(
					(unsigned char*) &(licensepackets[i].decrypted), &lc,
					licensepackets[i].decrypted_length);

#ifdef _DEBUG
			printf("Decrypted Text =%s\n",lc.from_date.c_str());
			printf("Decrypted Text =%s\n",licensepackets[i].decrypted);
			printf("Decrypted Length =%d\n",licensepackets[i].decrypted_length);
			std::cout << "Stock number " << i << "\n";
			std::cout << "  CammnadType: " << lc.CammnadType << "\n";
			std::cout << "  days_left: " << lc.days_left << "\n";
			std::cout << "  license_type: " << lc.license_type << "\n";
			std::cout << "  proprietary_data: " << lc.proprietary_data << "\n";
			std::cout << "  license_version: " << lc.license_version << "\n";
			std::cout << "  client_signature: " << lc.client_signature << "\n";
			std::cout << "  products: " << lc.products[0] << "\n";
			std::cout << "  license_signature: " << lc.license_signature << "\n";
			std::cout << "  from_date: " << lc.from_date << "\n";
			std::cout << "  to_date: " << lc.to_date << "\n";
			std::cout << "  has_expiry: " << lc.has_expiry << "\n";
			std::cout << "  from_sw_version: " << lc.from_sw_version << "\n";
			std::cout << "  to_sw_version: " << lc.to_sw_version << "\n";
			std::cout << "  has_versions: " << lc.has_versions << "\n";
			std::cout << "  has_client_sig: " << lc.has_client_sig << "\n";
			std::cout << "  extra_data: " << lc.extra_data << "\n";
			std::cout << "  has_use_limit: " << lc.has_use_limit << "\n";
			std::cout << "  use_limit: " << lc.use_limit << "\n";
			std::cout << "   usage_remain: " << lc.usage_remain << "\n";
			std::cout << "   server_signature: " << lc.server_signature << "\n";
			std::cout << "   iniparamupdated: " << lc.iniparamupdated.size() << "\n";
			//std::cout << "   public key: " << lc.public_key << "\n";
#endif

			if (lc.CammnadType == EVENT_TYPE::LICENSE_FILE_NOT_FOUND
					|| lc.CammnadType == EVENT_TYPE::PRODUCT_NOT_LICENSED) {

				vector<FullLicenseInfo> genresponse = generateAndOutputLicenses(
						lc);
				vector<licsenceCommPack> genresponsecom = parseLincenseToCom(
						genresponse, lc.CammnadType, &(crypto),
						(char*) &(licensepackets[i].publicKey));
				//cout<<"genresponsecom[i].has_expiry "<<genresponsecom[i].has_expiry;
				conn->async_write(genresponsecom,
						boost::bind(&LicenseManagerServer::handle_write, this,
								boost::asio::placeholders::error, conn));
			}

			if (lc.CammnadType == EVENT_TYPE::LICENSE_OK) {
				vector<FullLicenseInfo> genresponse = generateAndOutputLicenses(
						lc);
				vector<licsenceCommPack> genresponsecom = parseLincenseToCom(
						genresponse, lc.CammnadType, &(crypto),
						(char*) &(licensepackets[i].publicKey));
				conn->async_write(genresponsecom,
						boost::bind(&LicenseManagerServer::handle_write, this,
								boost::asio::placeholders::error, conn));
			}
			if (lc.CammnadType == EVENT_TYPE::LICENSE_HANDSHAKE) {
				vector<licsenceCommPack> genresponsecom;
				product_config pc = getProduct(lc.products[0]);
				if (pc.product == lc.products[0]) {
					/*lc.proprietary_data=pc.pukc;
					 licensepackets[i].publicKey*/
					strncpy((char*) (licensepackets[i].publicKey),
							(char*) pc.pukc.c_str(), sizeof(pc.pukc.size()));
					licensepackets[i].publicKey[MAX_ARR_LEN] = 0;

					genresponsecom.push_back(licensepackets[i]);
				}
				conn->async_write(genresponsecom,
						boost::bind(&LicenseManagerServer::handle_write, this,
								boost::asio::placeholders::error, conn));
			}

		}
#endif

	}

	//Crypto crypto;
private:
	/// The acceptor object used to accept incoming socket connections.
	boost::asio::ip::tcp::acceptor acceptor_;

	std::vector<licsenceCommPack> licensepackets;

	boost::asio::ssl::context context_;
	char data_[max_length];
	boost::asio::io_service& io_service_;
	string local_private_key;
	string local_public_key;
};

} // namespace license

int main(int argc, char* argv[]) {
	try {
		// Check command line arguments.

		if (argc != 2) {
			std::cerr << "Usage: LicenseManagerServer <port>" << std::endl;
			return 1;
		}

		license::product_config pc;

		unsigned short port = boost::lexical_cast<unsigned short>(argv[1]);

		boost::asio::io_service io_service;
		license::LicenseManagerServer LicenseManagerServer(io_service, port);

		LicenseManagerServer.init();

		io_service.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
