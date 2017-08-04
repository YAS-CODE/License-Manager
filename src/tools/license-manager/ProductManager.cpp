/*
 * ProductManager.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: yas
 */
 #include <iostream>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
#include <vector>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#define SI_SUPPORT_IOSTREAMS
#include "../../library/ini/SimpleIni.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include<fstream>
#include <string>
#include <build_properties.h>
#include "../../library/base/StringUtils.h"
#include "../../library/base/logger.h"
#include "../base_lib/CryptoHelper.h"

#include "ProductManager.h"


namespace fs = boost::filesystem;
namespace license {


void ProductManager::init(){


	struct stat statbuf;
	//cout<<"init called now!!!";
	product_config pc;
	readLMCnfig(&pc);
	//cout<<"here config :"<<pc.lm_config_folder.c_str()<<endl;
	lm_config_file_path=pc.lm_config_folder;
	if (fs::is_directory(pc.lm_config_folder.c_str())){
		//cout<<"config folder found"<<endl;
		   getProductList(pc.lm_config_folder.c_str(),&products);


	}
	else{
		if(!boost::filesystem::create_directories(pc.lm_config_folder.c_str()))
			cout<<"Can not create Config Folder!!!!\n";
		//int status;
		//status = mkdir(pc.lm_config_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}


}
string ProductManager::getConfigFile(){
	return lm_config_file_path;
}

ProductManager::ProductManager() {
	// TODO Auto-generated constructor stub

	//init();

}


bool ProductManager::addProductAsIniConfig(product_config * pc) {
	//string fname("~/data/LM/config/");
	if(pc->lm_config_folder.empty())
		readLMCnfig(pc);
	pc->config_file_path=pc->lm_config_folder+(pc->product)+"/config";

	if (!fs::is_directory(pc->lm_config_folder+(pc->product))){
		if(!boost::filesystem::create_directories(string(pc->lm_config_folder+(pc->product)).c_str()))
					cout<<"Can not create Product Folder!!!!\n";
	}



	fstream ofstream(pc->config_file_path, std::ios::out | std::ios::trunc);
	  	if (!ofstream.is_open()) {
	  		cerr << "can't open file [" << pc->config_file_path << "] for Lincese config" << endl
	  				<< " error: " << strerror(errno);
	  		//exit(3);
	  	}
	CSimpleIniA ini;
	string result;
	string product = toupper_copy(trim_copy(pc->product));
	std::ofstream configfile;
	configfile.open(pc->config_file_path,std::ios::out | std::ios::trunc);
	if (!configfile.is_open()) {
		cerr << "can't open file [" << pc->config_file_path << " ] for output." << endl
			<< " error: " << strerror(errno);
		return false;
	}
	CSimpleIniA::StreamWriter sw(configfile);
	ini.SetLongValue(product.c_str(), "license_version",
	PROJECT_INT_VERSION);
	if (pc->has_versions) {
		ini.SetLongValue(product.c_str(), "from_sw_version", pc->from_sw_version);
		ini.SetLongValue(product.c_str(), "to_sw_version", pc->to_sw_version);
	}

	if (pc->from_date != "0000-00-00") {
		ini.SetValue(product.c_str(), "from_date", pc->from_date.c_str());
	}
	if (pc->to_date != "0000-00-00") {
		ini.SetValue(product.c_str(), "to_date", pc->to_date.c_str());
	}
	if (pc->extra_data.length() > 0) {
		ini.SetValue(product.c_str(), "extra_data", pc->extra_data.c_str());
	}

	if (pc->has_use_limit) {
			ini.SetDoubleValue(product.c_str(), "use_limit", pc->use_limit);
	}
	if (pc->has_client_sig) {
			ini.SetValue(product.c_str(), "client_signature", pc->client_signature.c_str());
	}
	if (pc->server_signature.size())
			ini.SetValue(product.c_str(), "server_signature", pc->server_signature.c_str());
	else
			ini.SetValue(product.c_str(), "server_signature", "");

	if (pc->pukc.size())
				ini.SetValue(product.c_str(), "pukc", pc->pukc.c_str());
		else
				ini.SetValue(product.c_str(), "pukc", "");


	ini.Save(sw);
	return true;
}


bool ProductManager::readLMCnfig(product_config* config_folder) {

	CSimpleIniA ini;
	bool loadAtLeastOneFile = false;
		bool atLeastOneProductLicensed = false;
		bool atLeastOneLicenseComplete = false;
		ini.Reset();
		SI_Error rc = ini.LoadFile("lm.config");
		if (rc < 0) {
			return FILE_FORMAT_NOT_RECOGNIZED;
		} else {
			loadAtLeastOneFile = true;
		}
		const char* productNamePtr = "LM_CONFIG";//pc->product.c_str();
		int sectionSize = ini.GetSectionSize(productNamePtr);
		if (sectionSize <= 0) {
			return PRODUCT_NOT_LICENSED;
		} else {
			atLeastOneProductLicensed = true;
		}
		/*
		 *  sw_version_from = (optional int)
		 *  sw_version_to = (optional int)
		 *  from_date = YYYY-MM-DD (optional)
		 *  to_date  = YYYY-MM-DD (optional)
		 *  client_signature = XXXX-XXXX-XXXX-XXXX (optional string 16)
		 *  license_signature = XXXXXXXXXX (mandatory, 1024)
		 *  application_data = xxxxxxxxx (optional string 16)
		 */
		config_folder->lm_config_folder = ini.GetValue(productNamePtr,"config_dir", "/usr/data/LM/config");

			atLeastOneLicenseComplete = true;
		enablesignedvarification=ini.GetValue(productNamePtr,"singed_varification", false);


	if (!loadAtLeastOneFile) {
		return FILE_FORMAT_NOT_RECOGNIZED;
	}
	if (!atLeastOneProductLicensed) {
		return PRODUCT_NOT_LICENSED;
	}
	if (!atLeastOneLicenseComplete) {
		return LICENSE_MALFORMED;
	}
	return LICENSE_FILE_FOUND;
}



product_config ProductManager::getProduct(string product_name){

	/*product_config productconf = product_configs.find(product_name);
	    if(productconf.product != product_configs.end().second.product) {
	    	 return productconf.second ;//std::cout << "Found " << product->first << " " << product->second << '\n';
	    }
	    else {
	        return NULL;//std::cout << "Not found\n";
	    }*/
		return product_configs[product_name];
}

void ProductManager::getProductList(const char* path,std::vector<std::string> * product_){
	DIR *dir = opendir(path);

		    struct dirent *entry = readdir(dir);


		    while (entry != NULL)
		    {
		        if (entry->d_type == DT_DIR && (strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0)){
		            product_->push_back(entry->d_name);
		            //cout<<"Folder found: "<<entry->d_name<<endl;
		            product_config pc;
					if(pc.lm_config_folder.empty()){
							readLMCnfig(&pc);

					}

		            pc.product=entry->d_name;
		        	pc.config_file_path=pc.lm_config_folder+pc.product+"/config";
		            /*if ( !boost::filesystem::exists( "myfile.txt" ) )
		            {
		              addProductAsIniConfig(&pc);
		              std::cout << "Can't find my file!" << std::endl;
		            }*/
		            if(!pc.product.empty())
		            	readLicense(&pc);
		            pc.pukc=readclientPubkey(pc.lm_config_folder+pc.product+"/"+pc.product);
		            product_configs[entry->d_name]=pc;
		        }

		        entry = readdir(dir);
		    }

		    closedir(dir);
}
ProductManager::~ProductManager() {
	// TODO Auto-generated destructor stub
}

int ProductManager::readLicense(product_config* pc) {

	CSimpleIniA ini;
	bool loadAtLeastOneFile = false;
		bool atLeastOneProductLicensed = false;
		bool atLeastOneLicenseComplete = false;
		ini.Reset();
		SI_Error rc = ini.LoadFile(pc->config_file_path.c_str());
		if (rc < 0) {
			return FILE_FORMAT_NOT_RECOGNIZED;
		} else {
			loadAtLeastOneFile = true;
		}
		const char* productNamePtr = pc->product.c_str();
		int sectionSize = ini.GetSectionSize(productNamePtr);
		if (sectionSize <= 0) {
			return PRODUCT_NOT_LICENSED;
		} else {
			atLeastOneProductLicensed = true;
		}
		/*
		 *  sw_version_from = (optional int)
		 *  sw_version_to = (optional int)
		 *  from_date = YYYY-MM-DD (optional)
		 *  to_date  = YYYY-MM-DD (optional)
		 *  client_signature = XXXX-XXXX-XXXX-XXXX (optional string 16)
		 *  license_signature = XXXXXXXXXX (mandatory, 1024)
		 *  application_data = xxxxxxxxx (optional string 16)
		 */
		pc->license_version = ini.GetLongValue(productNamePtr,
				"license_version", -1);
		pc->use_limit = ini.GetLongValue(productNamePtr,
						"use_limit", -1);
		//pc->has_used_num = ini.GetLongValue(productNamePtr,"has_used_num", -1);
		if (pc->license_version > 0) {
			pc->from_date = trim_copy(
					ini.GetValue(productNamePtr, "from_date",
							"0000-00-00"));
			pc->to_date = trim_copy(
					ini.GetValue(productNamePtr, "to_date",
							"0000-00-00"));
			/*pc->client_signature = trim_copy(
					ini.GetValue(productNamePtr, "client_signature", ""));
			client_signature.erase(
			 std::remove(client_signature.begin(), client_signature.end(), '-'),
			 client_signature.end());*/
			pc->from_sw_version = ini.GetLongValue(productNamePtr,
					"from_sw_version",
					FullLicenseInfo::UNUSED_SOFTWARE_VERSION);
			pc->has_use_limit = ini.GetBoolValue(productNamePtr,
								"has_use_limit",
								false);
			pc->use_limit = ini.GetDoubleValue(productNamePtr,
											"use_limit",
											-1);
			//pc->has_used_num = ini.GetDoubleValue(productNamePtr,	"has_used_num",-1);
			pc->to_sw_version = ini.GetLongValue(productNamePtr,
					"to_sw_version", 0);
			pc->client_signature = ini.GetValue(productNamePtr,	"client_signature", "");
			if(pc->client_signature.empty())
				pc->has_client_sig=true;


			atLeastOneLicenseComplete = true;
		} else {
			return LICENSE_MALFORMED;
		}

	if (!loadAtLeastOneFile) {
		return FILE_FORMAT_NOT_RECOGNIZED;
	}
	if (!atLeastOneProductLicensed) {
		return PRODUCT_NOT_LICENSED;
	}
	if (!atLeastOneLicenseComplete) {
		return LICENSE_MALFORMED;
	}
	return LICENSE_FILE_FOUND;
}
}
