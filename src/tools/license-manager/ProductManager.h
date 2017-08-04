/*
 * ProductManager.h
 *
 *  Created on: Jul 1, 2017
 *      Author: yas
 */

#ifndef PRODUCTMANAGER_H_
#define PRODUCTMANAGER_H_

#include "../../library/LicenseReader.h"
#include "../../library/api/datatypes.h"
#include "../../library/base/EventRegistry.h"

#define SI_SUPPORT_IOSTREAMS
#include "../../library/ini/SimpleIni.h"

#include "../../library/os/os.h"


using namespace std;
namespace license {


typedef struct {
int license_version;
	string product;
	string from_date;
	string to_date;
	bool has_expiry;
	unsigned int from_sw_version;
	unsigned int to_sw_version;
	bool has_versions;
	string extra_data;
	bool has_use_limit;
	double use_limit;
	string config_file_path;
	string lm_config_folder;
	bool has_client_sig;
	string client_signature;
	string server_signature;
	string pukc;
}product_config;

class ProductManager: public Crypto{

	string lm_config_file_path;

public:
	ProductManager();
	virtual ~ProductManager();
	void getProductList(const char* path,std::vector<std::string> * products_);
	int readLicense(product_config* pc);
	product_config getProduct(string product_name);
	void addProduct(string product,int license_version,string from_date,string to_date,bool has_expiry,unsigned int from_sw_version,unsigned int to_sw_version,bool has_versions,string extra_data,bool has_use_limit,double use_limit);
	void init();
	std::vector<std::string> products;
	bool addProductAsIniConfig(product_config * pc);
	bool readLMCnfig(product_config * config_folder);
	//void ProductManager::addProductAsIni(ostream & a_ostream) const ;
	std::map<std::string,product_config> product_configs;
	string getConfigFile();
	bool enablesignedvarification;
};

}

#endif /* PRODUCTMANAGER_H_ */
