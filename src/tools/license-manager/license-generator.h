/*
 * LicenseSigner.h
 *
 *  Created on: Apr 6, 2014
 *      
 */

#ifndef LICENSE_GENERATOR_H_
#define LICENSE_GENERATOR_H_

#include <boost/program_options.hpp>
#include "../../library/LicenseReader.h"
#include "../LicenseComm.hpp"

namespace license {

namespace po = boost::program_options;
/**
 * This class contains all the logic used to generate a new license.
 * Since it has no method "main", can be easily used in unit tests.
 */
class LicenseGenerator : public FullLicenseInfo {

protected:
	LicenseGenerator();

	static void printHelp(const char* prog_name, const po::options_description& options);
	static po::options_description configureProgramOptions();
	static vector<FullLicenseInfo> parseLicenseInfo(po::variables_map vm);
	//static vector<FullLicenseInfo> parseLicenseInfo(LicenseComm &lc);
	static void generateAndOutputLicenses(const po::variables_map& vm,
			ostream& outputFile);
	static string normalize_date(const char * s);
public:
	/**
	 * Available options:
	 * <ul>
	 * 	<li>-s : use Client Signature.</li>
	 * </ul>
	 * @param argc
	 * 			count of arguments.
	 * @param argv
	 * 			char** of parameters.
	 * @return
	 */
	static int generateLicense(int argc, const char** argv);

	 //static vector<FullLicenseInfo> generateAndOutputLicenses(LicenseComm &lc);

	 //static vector<licsenceCommPack> parseLincenseToCom(vector<FullLicenseInfo> genresponse,EVENT_TYPE response_event, Crypto * crypto,char * remote_pub_key);
};

} /* namespace license */

#endif /* LICENSE_GENERATOR_H_ */
