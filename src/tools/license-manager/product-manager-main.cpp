// LicenseManagerConfig.cpp
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
//#include "../LicenseComm.hpp"
//#include "../../library/LicenseReader.h"
//#include "license-generator.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <build_properties.h>

#include <iostream>
#include<fstream>

#include "ProductManager.h"

#include <build_properties.h>
#include <private-key.h>
#include "license-generator.h"
#include "../base_lib/CryptoHelper.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iostream>
#include <boost/date_time.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/assign.hpp>
#include <fstream>
#include <regex>
#include <boost/filesystem.hpp>
#include "../src/library/pc-identifiers.h"
#include <valgrind/memcheck.h>


namespace license {

/*class licensepack{
 std::vector<LicenseComm> licensecoms;
 bool packREceived;
 }*/

/// Serves stock quote information to any client that connects to it.
class LicenseManagerConfig: public LicenseGenerator , public Crypto{
	/// Constructor opens the acceptor and starts waiting for the first incoming
	/// connection.
private:

	ProductManager pm;

public:
	 LicenseManagerConfig(){}
	void printHelp(const char* prog_name,
			const po::options_description& options) {
		cout << endl;
		cout << prog_name << " Version " << PROJECT_VERSION << endl << ". Usage:"
				<< endl;
		cout << prog_name << " [options] product_name1 product_name2 ... " << endl
				<< endl;
		cout
				<< " product_name1 ... = Product name. This string must match the one passed by the software."
				<< endl;
		cout << options << endl;
	}

	void convertStringToHex(string mac,PcIdentifier octets){
		  //unsigned char octets[6];
		  unsigned int value;
		  char ignore;

		  using namespace std;

		  istringstream iss(mac,istringstream::in);

		  iss >> hex;

		  for(int i=0;i<5;i++) {
		    iss >> value >> ignore;
		    octets[i]=value;
		  }
		  iss >> value;
		  octets[5]=value;

		  // validate

		  for(int i=0;i<sizeof(octets)/sizeof(octets[0]);i++)
		    cout << hex << static_cast<unsigned int>(octets[i]) << " ";

		  cout << endl;
	}
	po::options_description configureProgramOptions() {
		po::options_description common("General options");
		common.add_options()("help,h", "print help message and exit.") //
		("verbose,v", "print more information.") //
		("output,o", po::value<string>(), "Output file name. If not specified the "
				"license will be printed in standard output"); //
		po::options_description licenseGeneration("License Generation");
		licenseGeneration.add_options()("private_key,p", po::value<string>(),
				"Specify an alternate file for the primary key to be used. "
						"If not specified the internal primary key will be used.") //
		("begin_date,b", po::value<string>(),
				"Specify the start of the validity for this license. "
						" Format YYYYMMDD. If not specified defaults to today") //
		("expire_date,e", po::value<string>(),
				"Specify the expire date for this license. "
						" Format YYYYMMDD. If not specified the license won't expire") //
		("client_signature,s", po::value<string>(),
				"The signature of the pc that requires the license. "
						"It should be in the format XXXX-XXXX-XXXX-XXXX."
						" If not specified the license "
						"won't be linked to a specific pc.") //
		("start_version,t", po::value<unsigned int>()->default_value(0
		/*FullLicenseInfo.UNUSED_SOFTWARE_VERSION*/, "All Versions"),
				"Specify the first version of the software this license apply to.") //
		("usage_limit,u", po::value<double>()->default_value(0
				/*FullLicenseInfo.UNUSED_SOFTWARE_VERSION*/, "User Limit"),
						"Specify the User Limit of the software this license apply to.") //

		("end_version,n", po::value<unsigned int>()->default_value(0
		/*FullLicenseInfo.UNUSED_SOFTWARE_VERSION*/, "All Versions"),
				"Specify the last version of the software this license apply to."); //
		po::options_description visibleOptions;
		visibleOptions.add(common).add(licenseGeneration);
		return visibleOptions;
	}

	vector<FullLicenseInfo> parseproductInfo(
			po::variables_map vm) {
		string begin_date = FullLicenseInfo::UNUSED_TIME;
		string end_date = FullLicenseInfo::UNUSED_TIME;
		if (vm.count("expire_date")) {
			const std::string dt_end = vm["expire_date"].as<string>();
			try {
				//end_date = normalize_date(dt_end.c_str());
				end_date = dt_end;
				char curdate[20];
				time_t curtime = time(NULL);
				strftime(curdate, 20, "%Y-%m-%d", localtime(&curtime));
				begin_date.assign(curdate);
			} catch (const invalid_argument &e) {
				cerr << endl << "End date not recognized: " << dt_end
						<< " Please enter a valid date in format YYYYMMDD" << endl;
				exit(2);
			}
		}
		if (vm.count("begin_date")) {
			const std::string begin_date_str = vm["begin_date"].as<string>();
			try {
				//begin_date = normalize_date(begin_date_str.c_str());
				begin_date = begin_date_str;
			} catch (invalid_argument &e) {
				cerr << endl << "Begin date not recognized: " << begin_date_str
						<< " Please enter a valid date in format YYYYMMDD" << endl;
				//print_usage(vm);
				exit(2);
			}
		}
		string client_signature = "";
		if (vm.count("client_signature")) {
			client_signature = vm["client_signature"].as<string>();
			PcSignature identifier_out;
			PcIdentifier identifier;
			unsigned int identfier_num=2;
			//generate_ethernet_pc_id(identifier,&identfier_num,true);

			convertStringToHex(client_signature,identifier);
			/*if(identifier[0]==0)
				identifier[0]=32;*/
			//identifier[6]=01;
			//identifier[6] = (identifier[6] & 15) | IDENTIFICATION_STRATEGY::ETHERNET;
			//identifier[6] = (identifier[6] & 15) | IDENTIFICATION_STRATEGY::ETHERNET;

			generate_user_pc_signature_having_mac(identifier_out,IDENTIFICATION_STRATEGY::ETHERNET,identifier);
			/*VALGRIND_CHECK_VALUE_IS_DEFINED(identifier[0]);
			VALGRIND_CHECK_VALUE_IS_DEFINED(identifier[1]);
			encode_pc_id(identifier[0],identifier[1],identifier_out);
			VALGRIND_CHECK_VALUE_IS_DEFINED(identifier_out);*/


			IDENTIFICATION_STRATEGY strategy = IDENTIFICATION_STRATEGY::ETHERNET;
			//cout<<"Get MAC now..."<<endl;
			//printf("%02x:%02x:%02x:%02x:%02x:%02x",identifier[1][0],identifier[1][1],identifier[1][2],identifier[1][3],identifier[1][4],identifier[1][5]);
			//printf("%02x:%02x:%02x:%02x:%02x:%02x",identifier[0][0],identifier[0][1],identifier[0][2],identifier[0][3],identifier[0][4],identifier[0][5]);
			//cout<<"Get identifier now...";
			//cout<<"identifier_out: "<<identifier_out<<endl;
			client_signature.assign(identifier_out);


			//BOOST_CHECKPOINT("Before generate");
			//FUNCTION_RETURN generate_ok = generate_user_pc_signature(identifier_out, strategy,identifier);

			//fixme match + and /
			/*regex e("(A-Za-z0-9){4}-(A-Za-z0-9){4}-(A-Za-z0-9){4}-(A-Za-z0-9){4}");
			 if (!regex_match(client_signature, e)) {
			 cerr << endl << "Client signature not recognized: "
			 << client_signature
			 << " Please enter a valid signature in format XXXX-XXXX-XXXX-XXXX"
			 << endl;
			 exit(2);
			 }*/
		}
		string extra_data = "";
		if (vm.count("extra_data")) {
			extra_data = vm["extra_data"].as<string>();
		}
		unsigned int from_sw_version = vm["start_version"].as<unsigned int>();
		unsigned int to_sw_version = vm["end_version"].as<unsigned int>();
		if (vm.count("product") == 0) {
			cerr << endl << "Parameter [product] not found. " << endl;
			exit(2);
		}
		bool has_use_limit=false;
		double use_limit;

		if (vm.count("usage_limit")) {
			has_use_limit=true;
			use_limit = vm["usage_limit"].as<double>();
		}
		string server_signature="";
		vector<string> products = vm["product"].as<vector<string>>();
		vector<FullLicenseInfo> licInfo;
		licInfo.reserve(products.size());
		const char pkey[] = PRIVATE_KEY;
		size_t len = sizeof(pkey);
		//Crypto crypto;
		unique_ptr<CryptoHelper> helper = CryptoHelper::getInstance();
		for (auto it = products.begin(); it != products.end(); it++) {
			if (boost::algorithm::trim_copy(*it).length() > 0) {
				//string server_signature = helper->signString((const void *) pkey, len,fli.printForSignServer());


				FullLicenseInfo fli("", *it, "",
										PROJECT_INT_VERSION, begin_date,
										end_date, client_signature,
										server_signature, from_sw_version,
										to_sw_version, extra_data,
										has_use_limit, use_limit,
										0);
				string server_signature = helper->signString((const void *) pkey, len,fli.printForSignServer());

				fli.server_signature=server_signature;
							cout <<"Product["<<*it<< "]license:- " << server_signature << endl;
				licInfo.push_back(fli);

			}
		}
		return licInfo;
	}

	void generateAndOutputProduct(const po::variables_map& vm,
			ostream& outputFile) {
		vector<FullLicenseInfo> licenseInfo = parseproductInfo(vm);

		pm.init();
		Generatelocalkeys();

		for (auto it = licenseInfo.begin(); it != licenseInfo.end(); ++it) {
			license::product_config pc;


							pc.license_version = it->license_version;
							pc.product = it->product;
							pc.from_date = it->from_date;
							pc.to_date = it->to_date;
							pc.has_expiry = it->has_expiry;
							pc.from_sw_version = it->from_sw_version;
							pc.to_sw_version = it->to_sw_version;
							pc.has_versions = it->has_versions;
							pc.extra_data = it->extra_data;
							pc.has_use_limit = it->has_use_limit;
							pc.use_limit = it->use_limit;
							pc.has_client_sig = it->has_client_sig;
							pc.client_signature = it->client_signature;
							pc.server_signature = it->server_signature;
							//pc.pukc = string data((char*)c_str, len);
							//string data((char*)c_str, len);
							pm.addProductAsIniConfig(&pc);
							writekeyspair(pm.getConfigFile()+it->product+"/"+it->product+"-Pukc.key",pm.getConfigFile()+it->product+"/"+it->product+"-Prkc.key");

		}
	}

	int generateproduct(int argc, char **argv) {

		po::options_description visibleOptions = configureProgramOptions();
		//positional options must be addedd to standard options
		po::options_description allOptions;
		allOptions.add(visibleOptions).add_options()("product",
				po::value<vector<string>>(), "product names");

		po::positional_options_description p;
		p.add("product", -1);

		po::variables_map vm;
		po::store(
				po::command_line_parser(argc, argv).options(allOptions).positional(
						p).run(), vm);
		po::notify(vm);
		if (vm.count("help") || argc == 1) {
			printHelp(argv[0], visibleOptions);
			return 0;
		}

		if (vm.count("output")) {
			const std::string fname = vm["output"].as<string>();

			fstream ofstream(fname, std::ios::out | std::ios::trunc);
			if (!ofstream.is_open()) {
				cerr << "can't open file [" << fname << "] for output." << endl
						<< " error: " << strerror( errno);
				exit(3);
			}
			generateAndOutputProduct(vm, ofstream);
			ofstream.close();
		} else {
			generateAndOutputProduct(vm, cout);
		}
		return 0;
	}

};

} // namespace license

int main(int argc, char* argv[]) {
	license::LicenseManagerConfig lmc;
	lmc.generateproduct(argc, argv);


	return 0;
}
