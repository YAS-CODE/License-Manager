//
// LicenseManagerClient.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include "../connection.hpp" // Must come before boost/serialization headers.
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../LicenseComm.hpp"
#include "../../library/LicenseReader.h"

#include <fstream>

#include <cstring>
#include "../src/tools/license-generator/license-generator.h"
#include "../src/library/api/license++.h"
#include <build_properties.h>
#include <boost/filesystem.hpp>
#include "../src/library/ini/SimpleIni.h"
//#include "generate-license.h"
#include "../src/library/pc-identifiers.h"
#include "../LicenseComm.hpp"
#include "../bootstrap/bootstrap.h"
#include "../base_lib/linux/Crypto.h"

using namespace std;
using namespace license;

namespace license {
/// Downloads stock quote information from a server.
class LicenseManagerClient: public LicenseReader,
		public Bootstrap,
		public Crypto {
public:
	/// Constructor starts the asynchronous connect operation.
	LicenseManagerClient(boost::asio::io_service& io_service,
			boost::asio::ip::tcp::resolver::iterator& endpoint_iterator,
			boost::asio::ssl::context& context, const std::string& host,
			const std::string& service, const LicenseLocation &licenselocation,
			const std::string product_) :
			connection_(io_service, context), LicenseReader(licenselocation), product(
					product_) {

		//server_public_key=readlocalkey("Puks.key");

		endpoint_iterator_ = endpoint_iterator;

	}
	int license_init() {
		client_public_key = readlocalkey(product + "-Pukc.key");
		client_private_key = readlocalkey(product + "-Prkc.key");
		if (client_public_key.size() < 1 || client_private_key.size() < 1) {
			cout << product + "*.keys not found.";
			return FAILURE;
		}
		return SUCCESS;
	}
	void sync() {

		boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator_;
		connection_.socket().lowest_layer().async_connect(endpoint,
				boost::bind(&LicenseManagerClient::handle_connect, this,
						boost::asio::placeholders::error,
						++(endpoint_iterator_)));
		handshaked = false;
	}
	void handle_connect(const boost::system::error_code& error,
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
		if (!error) {

			connection_.getsocket().async_handshake(
					boost::asio::ssl::stream_base::client,
					boost::bind(&LicenseManagerClient::handle_handshake, this,
							boost::asio::placeholders::error));
		} else if (endpoint_iterator
				!= boost::asio::ip::tcp::resolver::iterator()) {
			connection_.getsocket().lowest_layer().close();
			boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
			connection_.getsocket().lowest_layer().async_connect(endpoint,
					boost::bind(&LicenseManagerClient::handle_connect, this,
							boost::asio::placeholders::error,
							++endpoint_iterator));
		} else {
			std::cout << "Connect failed: " << error << "\n";
		}
	}
	void handle_handshake(const boost::system::error_code& error) {
		if (!error) {
			handshaked = true;
			connection_.async_write(licensepackets,
					boost::bind(&LicenseManagerClient::handle_write, this,
							boost::asio::placeholders::error));
			connection_.async_read(licensepackets,
					boost::bind(&LicenseManagerClient::handle_read, this,
							boost::asio::placeholders::error));
		} else {
			std::cout << "Handshake failed: " << error << "\n";
		}

	}

	void handle_write(const boost::system::error_code& error,
			size_t bytes_transferred) {
		if (!error) {
			connection_.async_read(licensepackets,
					boost::bind(&LicenseManagerClient::handle_read, this,
							boost::asio::placeholders::error));
		} else {
			//std::cout << "Write failed: " << error << "\n";
		}
	}
	int openLicenseFile(string product) {

		license_filestream.open(product, std::ios::out | std::ios::trunc);
		if (!license_filestream.is_open()) {
			cerr << "can't open file [ << license.lic << ] for output." << endl
					<< " error: " << strerror(errno);
			return false;
		}
		return true;

	}

	void closeLicenseFile() {
		if (license_filestream.is_open())
			license_filestream.close();
	}

	/// Handle completion of a read operation.
	void handle_read(const boost::system::error_code& e) {
		if (!e) {
			LicenseComm lc;
			// Print out the data that was received.
			for (std::size_t i = 0; i < licensepackets.size(); ++i) {

				licensepackets[i].decrypted_length = private_decrypt(
						licensepackets[i].encrypted,
						licensepackets[i].encrypted_length,
						(char*) client_private_key.c_str(),
						licensepackets[i].decrypted);
				packet_manager::charArrayToObj(
						(unsigned char*) &(licensepackets[i].decrypted), &lc,
						licensepackets[i].decrypted_length);
#if 0
				printf("Decrypted Text =%s\n",lc.from_date.c_str());
				printf("Decrypted Text =%s\n",licensepackets[i].decrypted);
				printf("Decrypted Length =%d\n",licensepackets[i].decrypted_length);

				std::cout << "Stock number " << i << "\n";
#endif
				std::cout << "  License Manager Reply: ";
				switch (lc.CammnadType) {
				case EVENT_TYPE::LICENSE_OK:            //license file not found
					cout << "Commnad :LICENSE_OK" << endl;
					break;
				case EVENT_TYPE::LICENSE_FILE_NOT_FOUND: //license file not found
					cout << "Commnad :LICENSE_FILE_NOT_FOUND" << endl;
					break;
				case EVENT_TYPE::LICENSE_SERVER_NOT_FOUND: //license server can't be contacted
					cout << "Commnad :LICENSE_SERVER_NOT_FOUND" << endl;
					break;
				case EVENT_TYPE::ENVIRONMENT_VARIABLE_NOT_DEFINED: //environment variable not defined
					cout << "Commnad :ENVIRONMENT_VARIABLE_NOT_DEFINED" << endl;
					break;
				case EVENT_TYPE::FILE_FORMAT_NOT_RECOGNIZED: //license file has invalid format (not .ini file)
					cout << "Commnad :FILE_FORMAT_NOT_RECOGNIZED" << endl;
					break;
				case EVENT_TYPE::LICENSE_MALFORMED: //some mandatory field are missing, or data can't be fully read.
					cout << "Commnad :LICENSE_MALFORMED" << endl;
					break;
				case EVENT_TYPE::PRODUCT_NOT_LICENSED: //this product was not licensed
					cout << "Commnad :PRODUCT_NOT_LICENSED" << endl;
					break;
				case EVENT_TYPE::PRODUCT_EXPIRED:
					cout << "Commnad :PRODUCT_EXPIRED" << endl;
					break;
				case EVENT_TYPE::LICENSE_CORRUPTED:	//License signature didn't match with current license
					cout << "Commnad :LICENSE_CORRUPTED" << endl;
					break;
				case EVENT_TYPE::IDENTIFIERS_MISMATCH: //Calculated identifier and the one provided in license didn't match
					cout << "Commnad :IDENTIFIERS_MISMATCH" << endl;
					break;
				case EVENT_TYPE::PRODUCT_NOT_CONFIGERED:
					cout << "Commnad :PRODUCT_NOT_CONFIGERED" << endl;
					break;
				case EVENT_TYPE::LICENSE_FILE_FOUND:
					cout << "Commnad :LICENSE_FILE_FOUND" << endl;
					break;
				case EVENT_TYPE::LICENSE_VERIFIED:
					cout << "Commnad :LICENSE_VERIFIED" << endl;
					break;
				}
#ifdef _DEBUG

				std::cout << "  days_left: " << lc.days_left << "\n";
				std::cout << "  license_type: " << lc.license_type << "\n";
				std::cout << "  proprietary_data: " << lc.proprietary_data
				<< "\n";
				std::cout << "  license_version: " << lc.license_version
				<< "\n";
				std::cout << "  client_signature: " << lc.client_signature
				<< "\n";
				std::cout << "  products: " << lc.products[0] << "\n";
				std::cout << "  license_signature: " << lc.license_signature
				<< "\n";
				std::cout << "  from_date: " << lc.from_date << "\n";
				std::cout << "  to_date: " << lc.to_date << "\n";
				std::cout << "  has_expiry: " << lc.has_expiry << "\n";
				std::cout << "  from_sw_version: " << lc.from_sw_version
				<< "\n";
				std::cout << "  to_sw_version: " << lc.to_sw_version << "\n";
				std::cout << "  has_versions: " << lc.has_versions << "\n";
				std::cout << "  has_client_sig: " << lc.has_client_sig << "\n";
				std::cout << "  extra_data: " << lc.extra_data << "\n";
				std::cout << "  has_use_limit: " << lc.has_use_limit << "\n";

				std::cout << "  use_limit: " << lc.use_limit << "\n";
				std::cout << "server signature : " << lc.server_signature
				<< "\n";
#endif

				std::cout << "   usage_remain: " << lc.usage_remain << "\n";

				if (lc.CammnadType == EVENT_TYPE::LICENSE_FILE_NOT_FOUND
						|| lc.CammnadType == EVENT_TYPE::LICENSE_OK
						|| lc.CammnadType == EVENT_TYPE::LICENSE_VERIFIED
						|| lc.CammnadType == EVENT_TYPE::PRODUCT_EXPIRED
						|| lc.CammnadType == EVENT_TYPE::IDENTIFIERS_MISMATCH) {
					FullLicenseInfo fulllicenseinfo("", lc.products[0],
							lc.license_signature, lc.license_version,
							lc.from_date, lc.to_date, lc.client_signature,
							lc.server_signature, lc.from_sw_version,
							lc.to_sw_version, lc.extra_data, lc.has_use_limit,
							lc.use_limit, lc.usage_remain); // todo check source param:0

					if (openLicenseFile(lc.products[0])) {
						//this->deleteLicenses(lc.products[0]);
						fulllicenseinfo.printAsIni(license_filestream);

						closeLicenseFile();

					}
				} else if (lc.CammnadType == EVENT_TYPE::PRODUCT_NOT_LICENSED) {
					FullLicenseInfo fulllicenseinfo("", lc.products[0],
							lc.license_signature, lc.license_version,
							lc.from_date, lc.to_date, lc.client_signature,
							lc.server_signature, lc.from_sw_version,
							lc.to_sw_version, lc.extra_data, lc.has_use_limit,
							lc.use_limit, lc.usage_remain); // todo check source param:0

					cout << "product " << lc.products[0]
							<< "Is not license yet....";

					if (openLicenseFile(lc.products[0])) {
						//this->deleteLicenses(lc.products[0]);
						fulllicenseinfo.printAsIni(license_filestream);
						closeLicenseFile();

					}
				} else if (lc.CammnadType == EVENT_TYPE::LICENSE_CORRUPTED) {
					FullLicenseInfo fulllicenseinfo("", lc.products[0],
							lc.license_signature, lc.license_version,
							lc.from_date, lc.to_date, lc.client_signature,
							lc.server_signature, lc.from_sw_version,
							lc.to_sw_version, lc.extra_data, lc.has_use_limit,
							lc.use_limit, lc.usage_remain); // todo check source param:0

					cout << "product " << lc.products[0]
							<< "Is expired yet....";

					if (openLicenseFile(lc.products[0])) {
						//this->deleteLicenses(lc.products[0]);
						fulllicenseinfo.printAsIni(license_filestream);
						closeLicenseFile();

					}

				} else if (lc.CammnadType == EVENT_TYPE::LICENSE_HANDSHAKE) {
					server_public_key.assign(
							(char*) licensepackets[i].publicKey, MAX_ARR_LEN);
				}
			}
		} else {
			// An error occurred.
			std::cerr << e.message() << std::endl;
		}

		// Since we are not starting a new operation the io_service will run out of
		// work to do and the LicenseManagerClient will exit.
	}
	template<typename T>
	int Write(T& t) {
		connection_.async_write(t,
				boost::bind(&LicenseManagerClient::handle_write, this,
						boost::asio::placeholders::error));

		return 0;

	}

	const char * getPubKey() {
		return client_public_key.c_str();
	}
	const char * getPriKey() {
		return client_private_key.c_str();
	}
	void handle_write(const boost::system::error_code& e) {
		// Nothing to do. The socket will be closed automatically when the last
		// reference to the connection object goes away.
		if (!e) {

		} else {
			//					std::cerr <<"Write Handler: "<< e.message() << std::endl;
		}
	}

	std::vector<licsenceCommPack> licensepackets;

	//Crypto crypto;

private:
	connection connection_;
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator_;
	std::ofstream license_filestream;
	bool handshaked;
	string server_public_key;
	string client_public_key;
	string client_private_key;
	string product;

};

} // namespace license

int main(int argc, char* argv[]) {
	try {
		Crypto crypto_;
		string product_arg;
		if (argv[3])
			product_arg.assign(argv[3]);
		else
			product_arg = "";
		// Check command line arguments.
		if (argc < 2) {
			std::cerr
					<< "Usage: LicenseManagerClient <host> <port> <product_name> <server_key_gen_at_runtime>(optional)"
					<< std::endl;
			return 1;
		}
		LicenseLocation licenseLocation;
		licenseLocation.openFileNearModule = false;
		//licenseLocation.licenseFileLocation = "license.lic";
		if (argv[3])
			licenseLocation.licenseFileLocation = argv[3];
		else
			licenseLocation.licenseFileLocation = "license.lic";
		licenseLocation.environmentVariableName = "";

		boost::asio::io_service io_service;

		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(argv[1], argv[2]);
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(
				query);

		boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
		ctx.load_verify_file("server.crt");

		license::LicenseManagerClient LicenseManagerClient(io_service, iterator,
				ctx, argv[1], argv[2], licenseLocation, product_arg);
		unsigned char *encryptedMessage = NULL;
		unsigned char *encryptedKey;
		unsigned char *iv;
		size_t encryptedKeyLength;
		size_t ivLength;

		PcSignature identifier_out;

		IDENTIFICATION_STRATEGY strategy = IDENTIFICATION_STRATEGY::ETHERNET;
		//BOOST_CHECKPOINT("Before generate");
		FUNCTION_RETURN generate_ok = generate_user_pc_signature(identifier_out,
				strategy);
		LicenseComm licensecomm;
		licsenceCommPack lcp;
		LicenseComm lc;
		LicenseInfo license;
		string server_license;

		validate_pc_signature(identifier_out);

		PcIdentifier user_identifiers[2];
		FUNCTION_RETURN result = decode_pc_id(user_identifiers[0],
				user_identifiers[1], identifier_out);
		if (result != FUNC_RET_OK) {
			return result;
		}
		int current_strategy = strategy_from_pc_id(user_identifiers[1]);

		user_identifiers[1][0] = (user_identifiers[1][0] & 15)
				& (~current_strategy);

		vector<FullLicenseInfo> licenseInfoOut;

		vector<string> products= {argv[3]};			//{"smart-eye"};

		licensecomm.products.push_back("");	//just temparary, todo remove this line later......
		for (auto it = products.begin(); it != products.end(); it++) {
			EVENT_TYPE result = acquire_license_client(it->c_str(),
					licenseLocation, &license, identifier_out);

			if (result == IDENTIFIERS_MISMATCH) {
				cout << "IDENTIFIERS_MISMATCH: Exit now!!!!" << endl;
				return 0;
			}

			licensecomm.products[0] = *it;
			if (LicenseManagerClient.license_init()) {
				cout << "DO NOT FOUND KEYS!!!" << endl;
				return 0;
			}

			LicenseManagerClient.readLicenses(licensecomm.products[0],
					licenseInfoOut);

			if (argv[4])
				server_license.assign(argv[4]);
			else
				server_license = "";

			std::stringstream stream;
			boost::archive::binary_oarchive ar(stream,
					boost::archive::no_header);
			licensecomm.setVal(result, 0, LICENSE_TYPE::REMOTE, "", 0,
					identifier_out, licensecomm.products, "", "2016-06-06",
					"2013-06-06", false, 0, 10, true, true, "", false, 100, 0,
					server_license, licensecomm.iniparamupdated);
			ar << licensecomm;

			memset(lcp.publicKey, 0, MAX_ARR_LEN);
			strncpy((char*) (lcp.publicKey),
					(char*) (LicenseManagerClient.getPubKey()),
					strlen((char*) (LicenseManagerClient.getPubKey())));

			switch (result) {
			case EVENT_TYPE::PRODUCT_NOT_LICENSED:
				cout
						<< "PRODUCT_NOT_LICENSED: from Client, Going to sync with License Manager...."
						<< endl;
				licensecomm.setVal(result, 0, LICENSE_TYPE::REMOTE, "", 0,
						identifier_out, licensecomm.products, "", "2016-06-06",
						"2013-06-06", false, 0, 10, true, true, "", false, 100,
						0, server_license, licensecomm.iniparamupdated);
				lcp.decrypted_length = packet_manager::objToCharArray(
						(unsigned char*) &(lcp.decrypted), licensecomm);
				lcp.encrypted_length = LicenseManagerClient.private_encrypt(
						lcp.decrypted, lcp.decrypted_length,
						(char *) LicenseManagerClient.getPriKey(),
						lcp.encrypted);

				LicenseManagerClient.licensepackets.push_back(lcp);

				LicenseManagerClient.sync();
				break;

			case EVENT_TYPE::LICENSE_OK:
				cout
						<< "LICENSE_OK: from Client, Going to sync with License Manager...."
						<< endl;
				licensecomm.setVal(result, 0, LICENSE_TYPE::REMOTE, "", 0,
						identifier_out, licensecomm.products, "", "2016-06-06",
						"2013-06-06", false, 0, 10, true, true, "", false, 100,
						0, server_license, licensecomm.iniparamupdated);
				lcp.decrypted_length = packet_manager::objToCharArray(
						(unsigned char*) &(lcp.decrypted), licensecomm);
				lcp.encrypted_length = LicenseManagerClient.private_encrypt(
						lcp.decrypted, lcp.decrypted_length,
						(char *) LicenseManagerClient.getPriKey(),
						lcp.encrypted);
				LicenseManagerClient.licensepackets.push_back(lcp);
				//LicenseManagerClient.Write(LicenseManagerClient.licensepackets);
				LicenseManagerClient.sync();
				break;

			case EVENT_TYPE::LICENSE_FILE_NOT_FOUND:
				cout
						<< "LICENSE_FILE_NOT_FOUND by client, Requesting to License Manager to issue for new App...."
						<< endl;
				licensecomm.setVal(result, 0, LICENSE_TYPE::REMOTE, "", 0,
						identifier_out, licensecomm.products, "", "2016-06-06",
						"2016-06-06", true, 0, 10, true, true, "", false, 100,
						0, server_license, licensecomm.iniparamupdated);

				lcp.decrypted_length = packet_manager::objToCharArray(
						(unsigned char*) &(lcp.decrypted), licensecomm);

				lcp.encrypted_length = LicenseManagerClient.private_encrypt(
						lcp.decrypted, lcp.decrypted_length,
						(char *) LicenseManagerClient.getPriKey(),
						lcp.encrypted);

				LicenseManagerClient.licensepackets.push_back(lcp);

				LicenseManagerClient.sync();
				break;

			case EVENT_TYPE::PRODUCT_EXPIRED:
				if (license.has_expiry)
					cout << "PRODUCT_EXPIRED: " << *it << endl;
				if (!license.linked_to_pc)
					cout << "License Not link to PC yet!!!" << endl;

				//cout<<"property data: "<<license.proprietary_data<<endl;
				goto exit;
				break;
			case EVENT_TYPE::LICENSE_CORRUPTED:
				goto exit;

			}

			io_service.run();
			//licensecomm.products.remove()
		}

		exit:

		cout << endl;

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

