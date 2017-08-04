#include <build_properties.h>
#include <private-key.h>
#include "license-generator.h"
#include "../base_lib/CryptoHelper.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <boost/date_time.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/assign.hpp>
#include <fstream>
#include <regex>
#include <boost/filesystem.hpp>
#include "../LicenseComm.hpp"

namespace fs = boost::filesystem;
namespace bt = boost::posix_time;
namespace po = boost::program_options;

using namespace std;

namespace license {



LicenseGenerator::LicenseGenerator() {

}

void LicenseGenerator::printHelp(const char* prog_name,
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

po::options_description LicenseGenerator::configureProgramOptions() {
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
	("end_version,n", po::value<unsigned int>()->default_value(0
	/*FullLicenseInfo.UNUSED_SOFTWARE_VERSION*/, "All Versions"),
			"Specify the last version of the software this license apply to."); //
	po::options_description visibleOptions;
	visibleOptions.add(common).add(licenseGeneration);
	return visibleOptions;
}

vector<FullLicenseInfo> LicenseGenerator::parseLicenseInfo(po::variables_map vm) {
	string begin_date = FullLicenseInfo::UNUSED_TIME;
	string end_date = FullLicenseInfo::UNUSED_TIME;
	if (vm.count("expire_date")) {
		const std::string dt_end = vm["expire_date"].as<string> ();
		try {
			end_date = normalize_date(dt_end.c_str());
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
		const std::string begin_date_str = vm["begin_date"].as<string> ();
		try {
			begin_date = normalize_date(begin_date_str.c_str());
		} catch (invalid_argument &e) {
			cerr << endl << "Begin date not recognized: " << begin_date_str
					<< " Please enter a valid date in format YYYYMMDD" << endl;
			//print_usage(vm);
			exit(2);
		}
	}
	string client_signature = "";
	if (vm.count("client_signature")) {
		client_signature = vm["client_signature"].as<string> ();
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
		extra_data = vm["extra_data"].as<string> ();
	}
	unsigned int from_sw_version = vm["start_version"].as<unsigned int> ();
	unsigned int to_sw_version = vm["end_version"].as<unsigned int> ();
	if (vm.count("product") == 0) {
		cerr << endl << "Parameter [product] not found. " << endl;
		exit(2);
	}
	vector < string > products = vm["product"].as<vector<string>> ();
	vector<FullLicenseInfo> licInfo;
	licInfo.reserve(products.size());
	for (auto it = products.begin(); it != products.end(); it++) {
		if (boost::algorithm::trim_copy(*it).length() > 0) {
			licInfo.push_back(
					FullLicenseInfo("", *it, "", PROJECT_INT_VERSION,
							begin_date, end_date, client_signature,"",
							from_sw_version, to_sw_version, extra_data, false,
							0, 0));

		}
	}
	return licInfo;
}
#if 0
vector<FullLicenseInfo> LicenseGenerator::parseLicenseInfo(LicenseComm & lc) {


	vector<FullLicenseInfo> licInfo;
	LicenseLocation licenseLocation;
	licenseLocation.openFileNearModule = false;
	licenseLocation.licenseFileLocation = lc.client_signature.c_str();
	licenseLocation.environmentVariableName = "";


	//if (lc.CammnadType == EVENT_TYPE::LICENSE_OK || lc.CammnadType == EVENT_TYPE::LICENSE_FILE_NOT_FOUND) {
		vector<FullLicenseInfo> licenseInfo;
		LicenseReader licreader(licenseLocation);


		for (auto it = lc.products.begin(); it != lc.products.end(); ++it) {
			licreader.readLicenses(*it, licenseInfo);

			cout <<__LINE__<<"Here license size "<<licenseInfo.size();
				for (int i=0; i<licenseInfo.size(); i++){

				if(!lc.server_signature.empty())
					licenseInfo[i].server_signature=lc.server_signature;
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
	           std::cout <<__LINE__ << "   has_used_num: " << licenseInfo[i].has_used_num << "\n";
				}
			licInfo.insert(licInfo.end(), licenseInfo.begin(),
					licenseInfo.end());

		}
		if(licInfo.size()>0)
			return licInfo;




		string begin_date = FullLicenseInfo::UNUSED_TIME;
		string end_date = FullLicenseInfo::UNUSED_TIME;
		bool has_use_limit;
		double use_limit;
		double has_used_num;

		char curdate[20];
		if (lc.products.size() == 0) {//vm.count("product") == 0) {
			cerr << endl << "Parameter [product] not found. " << endl;
			exit(2);
		}
		vector < string > products = lc.products;//vm["product"].as<vector<string>>();
		licInfo.reserve(products.size());
		for (auto it = products.begin(); it != products.end(); it++) {
			if (boost::algorithm::trim_copy(*it).length() > 0) {

				if (!lc.to_date.empty()) {///vm.count("expire_date")) {
					const std::string dt_end = lc.to_date;//vm["expire_date"].as<string>();
					try {
						end_date = normalize_date(dt_end.c_str());

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
				if (!lc.from_date.empty()) {//vm.count("begin_date")) {
					const std::string begin_date_str = lc.from_date;//vm["begin_date"].as<string>();
					try {
						begin_date = normalize_date(begin_date_str.c_str());
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
				if (!lc.client_signature.empty()) {//vm.count("client_signature")) {
					client_signature = lc.client_signature;//vm["client_signature"].as<string>();
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
				string server_signature = "";
								if (!lc.server_signature.empty()) {//vm.count("client_signature")) {
									server_signature = lc.server_signature;//vm["client_signature"].as<string>();
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
				if (!lc.extra_data.empty()) {//vm.count("extra_data")) {
					extra_data = lc.extra_data;//vm["extra_data"].as<string>();
				}
				unsigned int from_sw_version = lc.from_sw_version;//vm["start_version"].as<unsigned int>();
				unsigned int to_sw_version = lc.to_sw_version;//vm["end_version"].as<unsigned int>();
				if (*it == "smart-eye") { //product profile....
					time_t t1 = time(0); // get time now
					struct tm * now = localtime(&t1);
					snprintf(curdate, 20,"%4d-%02d-%02d", now->tm_year+1900, now->tm_mon,
							now->tm_mday);
					begin_date.assign(curdate);
					snprintf(curdate,20, "%4d-%02d-%02d", now->tm_year+1900 + 1,
							now->tm_mon, now->tm_mday);
					end_date.assign(curdate);
					has_use_limit=false;
					use_limit=10;
					has_used_num=0;
				}
				else{
					time_t t1 = time(0); // get time now
										struct tm * now = localtime(&t1);
										snprintf(curdate, 20,"%4d-%02d-%02d", now->tm_year+1900, now->tm_mon,
												now->tm_mday);
										begin_date.assign(curdate);
										snprintf(curdate,20, "%4d-%02d-%02d", now->tm_year+1900,
												now->tm_mon+6, now->tm_mday);
										end_date.assign(curdate);
										has_use_limit=false;
										use_limit=10;
										has_used_num=0;

				}
				licInfo.push_back(
						FullLicenseInfo("", *it, "", PROJECT_INT_VERSION,
								begin_date, end_date, client_signature,server_signature,
								from_sw_version, to_sw_version, extra_data,
								has_use_limit, use_limit, has_used_num));
			}
		}

	return licInfo;
}


vector<FullLicenseInfo> LicenseGenerator::generateAndOutputLicenses(
		LicenseComm &lc) {
	vector<FullLicenseInfo> licenseInfo = parseLicenseInfo(lc);

	unique_ptr < CryptoHelper > helper = CryptoHelper::getInstance();
	const char pkey[] = PRIVATE_KEY;
	size_t len = sizeof(pkey);
	const string fname = lc.client_signature;


	fstream ofstream(fname, std::ios::out | std::ios::trunc);
	if (!ofstream.is_open()) {
		cerr << "can't open file [" << fname << "] for Lincese." << endl
				<< " error: " << strerror(errno);
		//exit(3);
	}

	for (auto it = licenseInfo.begin(); it != licenseInfo.end(); ++it) {
		if(it->has_use_limit){
			it->has_used_num++;
			//it->param_to_update=it->param_to_update+"has_used_num";
		}
		const string license = it->printForSign();
		cout << "lincense_generate " << license << endl;


		string signature =	helper->signString((const void *) pkey, len, license);
		it->license_signature = signature;
		//license =	it->printForSignServer();
		signature = helper->signString((const void *) pkey, len, it->printForSignServer());

		cout<<"license server gen:- "<<signature<<endl;

		//it->server_signature = signature;

		//it->printAsIni(outputFile);
		it->printAsIni(ofstream);
		//#ifdef DEBUG
		it->printAsIni(cout);
		//#endif
	}
	return licenseInfo;
}

vector<licsenceCommPack> LicenseGenerator::parseLincenseToCom(
		vector<FullLicenseInfo> genresponse, EVENT_TYPE response_event,Crypto * crypto,char * remote_pub_key) {
	vector<licsenceCommPack> genresponsecom;
	//Crypto crypto;
	unique_ptr < CryptoHelper > helper = CryptoHelper::getInstance();
		const char pkey[] = PRIVATE_KEY;
		size_t len = sizeof(pkey);

		licsenceCommPack lcp;

	for (auto it = genresponse.begin(); it != genresponse.end(); ++it) {
		vector < string > products;
		products.push_back(it->product);
		//genresponsecom.push_back(LicenseComm(response_event,0,LICENSE_TYPE::REMOTE,"",it->license_version,it->client_signature,products,it->license_signature,it->from_date,it->to_date,false,it->from_sw_version,it->to_sw_version,it->has_versions,it->has_client_sig,it->extra_data,it->has_use_limit,it->use_limit,it->has_used_num));

		LicenseComm lc;
		if(it->server_signature.empty())
			lc.CammnadType = EVENT_TYPE::PRODUCT_NOT_LICENSED;
		else{
			//const string license = it->printForSignServer();

			//string signature = helper->signString((const void *) pkey, len, license+"CompanyName:softonnet.com");

			//cout << "lincense_generate " << license << endl;
			//cout<<"          server_sig: "<<it->server_signature.c_str()<<"\n print sign server:"<<it->printForSignServer().c_str();
			FUNCTION_RETURN sigVer = verifySignature(it->printForSignServer().c_str(),it->server_signature.c_str());
			bool sigVerified = sigVer == FUNC_RET_OK;
			if (sigVerified) {
				//lc.param_to_update.assign(":param_to_update");
				lc.CammnadType=LICENSE_VERIFIED;
			} else {
				//lc.param_to_update.assign(":param_to_update");
				lc.CammnadType=LICENSE_CORRUPTED;
			}

		}
		//lc.param_to_update.assign(":has_used_num");
		//cout<<" param to update: "<<lc.param_to_update<<endl;
		lc.days_left = 0;
		lc.license_type = LICENSE_TYPE::REMOTE; // Local or Remote

		lc.proprietary_data = "";
		lc.license_version = it->license_version; //license file version
		lc.client_signature = it->client_signature;

		lc.products = products;
		lc.license_signature = it->license_signature;

		//cout << endl << "lc.from_date= " << lc.from_date;
		lc.from_date = it->from_date;//it->from_date;
		lc.to_date = it->to_date;//it->to_date;
		//cout << endl << "lc.from_date= " << lc.from_date.c_str() << __LINE__

		lc.has_expiry = it->has_expiry;
		lc.from_sw_version = it->from_sw_version;
		lc.to_sw_version = it->to_sw_version;
		lc.has_versions = it->has_versions;

		lc.has_client_sig = it->has_client_sig;
		lc.extra_data = it->extra_data;
		lc.has_use_limit = it->has_use_limit;
		lc.use_limit = it->use_limit;
		lc.has_used_num = it->has_used_num;
		lc.server_signature = it->server_signature;

		//lcp.data.assign(packet_manager::objToString(lc));
//		packet_manager::encryptRsaData(crypto,&(lcp.data),&(lcp.encryptedData), &(lcp.encryptedKey),  &(lcp.iv));
		lcp.decrypted_length=packet_manager::objToCharArray((unsigned char*)&(lcp.decrypted),lc);
		lcp.encrypted_length= crypto->public_encrypt(lcp.decrypted,lcp.decrypted_length,remote_pub_key,lcp.encrypted);
		genresponsecom.push_back(lcp);

	}
	return genresponsecom;

}
#endif
void LicenseGenerator::generateAndOutputLicenses(const po::variables_map& vm,
		ostream& outputFile) {
	vector<FullLicenseInfo> licenseInfo = parseLicenseInfo(vm);
	unique_ptr < CryptoHelper > helper = CryptoHelper::getInstance();
	const char pkey[] = PRIVATE_KEY;
	size_t len = sizeof(pkey);
	for (auto it = licenseInfo.begin(); it != licenseInfo.end(); ++it) {
		const string license = it->printForSign();
		string signature =	helper->signString((const void *) pkey, len, license);
		it->license_signature = signature;
		it->printAsIni(outputFile);
	}
}

int LicenseGenerator::generateLicense(int argc, const char **argv) {

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
		const std::string fname = vm["output"].as<string> ();

		fstream ofstream(fname, std::ios::out | std::ios::trunc);
		if (!ofstream.is_open()) {
			cerr << "can't open file [" << fname << "] for output." << endl
					<< " error: " << strerror(errno);
			exit(3);
		}
		generateAndOutputLicenses(vm, ofstream);
		ofstream.close();
	} else {
		generateAndOutputLicenses(vm, cout);
	}
	return 0;
}

const std::string formats[] = { "%4u-%2u-%2u", "%4u/%2u/%2u", "%4u%2u%2u" };
const size_t formats_n = 3;

string LicenseGenerator::normalize_date(const char * s) {
	unsigned int year, month, day;
	int chread;
	bool found = false;
	for (size_t i = 0; i < formats_n && !found; ++i) {
		chread = sscanf(s, formats[i].c_str(), &year, &month, &day);
		if (chread == 3) {
			found = true;
		}
	}
	if (!found) {
		throw invalid_argument("Date not recognized.");
	}
	ostringstream oss;
	oss << year << "-" << setfill('0') << std::setw(2) << month << "-" << day;
	//delete (facet);
	return oss.str();
}
}
