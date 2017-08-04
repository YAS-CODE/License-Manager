//
// stock.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SERIALIZATION_STOCK_HPP
#define SERIALIZATION_STOCK_HPP

#include <string>
#include <boost/serialization/vector.hpp>
#include "../library/api/datatypes.h"
#include <vector>
#include "base_lib/linux/Crypto.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/serialization.hpp>
#include <iostream>
#include <sstream>
#include "../tools/base_lib/linux/base64.h"
#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>


#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <iostream>



namespace license {

#define MAX_ARR_LEN 4098

using namespace std;
using namespace boost::archive::iterators;
/// Structure to hold information about a single stock.
enum {
	max_length = 8192
};


/*typedef enum{
	GENERATE_LICENSE,
	GENERATE_LICENSE_REPLY,
	LICENSE_FILE_NOT_FOUND,
	LICENSE_FILE_NOT_FOUND_REPLY,
	STATUS_UPDATE,
	AUTHORIZE
}COMMAND_TYPE;*/

typedef struct {
	/**
	 * Detailed reason of success/failure. Reasons for a failure can be
	 * multiple (for instance, license expired and signature not verified).
	 * Only the last 5 are reported.
	 */
	//AuditEvent status[5];

	/**
	 * Eventual expiration date of the software,
	 * can be '\0' if the software don't expire
	 * */
	EVENT_TYPE CammnadType;

	unsigned int days_left;
	LICENSE_TYPE license_type; // Local or Remote

	std::string proprietary_data;
	int license_version; //license file version
	std::string client_signature;

	std::vector<std::string>  products;
	std::string license_signature;
	//std::string public_key;

	std::string from_date;
	std::string to_date;
	bool has_expiry;
	unsigned int from_sw_version;
	unsigned int to_sw_version;
	bool has_versions;

	bool has_client_sig;
	std::string extra_data;
	bool has_use_limit;
	double use_limit;
	double usage_remain;
	std::string server_signature;					//remove for demo.....
	std::vector<std::string> iniparamupdated;
	std::string param_to_insert;

	void LicenseComm(){
		CammnadType=EVENT_TYPE::LICENSE_FILE_NOT_FOUND;
			days_left=0;
			LICENSE_TYPE license_type=LICENSE_TYPE::LOCAL; // Local or Remote

			proprietary_data="";
			license_version=0; //license file version
			client_signature="";

			products={};
			license_signature="";

			from_date="";
			to_date="";
			has_expiry=true;
			from_sw_version=0;
			to_sw_version=0;
			has_versions=false;

			has_client_sig=false;
			extra_data="";
			has_use_limit=false;
			use_limit=0;
			usage_remain=0;
			server_signature="";
			//public_key="";
			iniparamupdated={};
	}

	void LicenseComm(EVENT_TYPE CammnadType_,unsigned int days_left_,LICENSE_TYPE license_type_,std::string proprietary_data_,int license_version_,std::string client_signature_,std::vector<std::string> products_,std::string license_signature_,std::string from_date_,std::string to_date_,bool has_expiry_,unsigned int from_sw_version_,unsigned int to_sw_version_,bool has_versions_,bool has_client_sig_,std::string extra_data_,bool has_use_limit_,double use_limit_,double usage_remain_,std::string server_signature_,std::vector<std::string> iniparamupdated_){
		CammnadType=CammnadType_;
		days_left=days_left_;
		license_type=license_type_; // Local or Remote

		proprietary_data=proprietary_data_;
		license_version=license_version_; //license file version
		client_signature=client_signature_;

		products=products_;
		license_signature=license_signature_;

		from_date=from_date_;
		to_date=to_date_;
		has_expiry=has_expiry_;
		from_sw_version=from_sw_version_;
		to_sw_version=to_sw_version_;
		has_versions=has_versions_;

		has_client_sig=has_client_sig_;
		extra_data=extra_data_;
		has_use_limit=has_use_limit_;
		use_limit=use_limit_;
		usage_remain=usage_remain_;
		server_signature=server_signature_;
		iniparamupdated=iniparamupdated_;
		//public_key=public_key_;

	}
	void setVal(EVENT_TYPE CammnadType_,unsigned int days_left_,LICENSE_TYPE license_type_,std::string proprietary_data_,int license_version_,std::string client_signature_,std::vector<std::string> products_,std::string license_signature_,std::string from_date_,std::string to_date_,bool has_expiry_,unsigned int from_sw_version_,unsigned int to_sw_version_,bool has_versions_,bool has_client_sig_,std::string extra_data_,bool has_use_limit_,double use_limit_,double usage_remain_,std::string server_signature_,std::vector<std::string> iniparamupdated_){
			CammnadType=CammnadType_;
			days_left=days_left_;
			license_type=license_type_; // Local or Remote

			proprietary_data=proprietary_data_;
			license_version=license_version_; //license file version
			client_signature=client_signature_;

			products=products_;
			license_signature=license_signature_;

			from_date=from_date_;
			to_date=to_date_;
			has_expiry=has_expiry_;
			from_sw_version=from_sw_version_;
			to_sw_version=to_sw_version_;
			has_versions=has_versions_;

			has_client_sig=has_client_sig_;
			extra_data=extra_data_;
			has_use_limit=has_use_limit_;
			use_limit=use_limit_;
			usage_remain=usage_remain_;
			server_signature=server_signature_;
			iniparamupdated=iniparamupdated_;
			//public_key=public_key_;
		}


	 template <typename Archive>
	  void serialize(Archive& ar, const unsigned int version)
	  {
		 ar &  CammnadType;
		 ar & days_left;
		 ar &  license_type; // Local or Remote

		 //ar &  proprietary_data;
		 ar &  license_version; //license file version
		 ar &  client_signature;


		 ar & BOOST_SERIALIZATION_NVP(products);
		 ar &  license_signature;

		 ar &  from_date;
		 ar &  to_date;
		 ar &  has_expiry;
		 ar &  from_sw_version;
		 ar &  to_sw_version;
		 ar &  has_versions;

		 ar &  has_client_sig;
		 //ar &  extra_data;
		 ar & has_use_limit;
		 ar &  use_limit;
		 ar &  usage_remain;
		 ar & server_signature;
		 //ar & BOOST_SERIALIZATION_NVP(iniparamupdated);
		 //ar & public_key;
	  }

} LicenseComm;

typedef struct{
	unsigned char encrypted[MAX_ARR_LEN]={};
	unsigned char decrypted[MAX_ARR_LEN]={};
	int encrypted_length;
	int decrypted_length;
	//std::string msgpack;
	unsigned char publicKey[MAX_ARR_LEN]={};
	//const char *encrypted_ptr;
	//std::string data;


	template <typename Archive>
		  void serialize(Archive& ar, const unsigned int version)
		  {
			ar  & encrypted;
			 ar & encrypted_length;
			 ar &  publicKey;
			 //ar & encrypted_ptr;
		}
	static int dumpStrToArr(string str, char *arr){
		/*memset(arr,0,4098);
		memcpy(arr,str,4098);*/
		for(int i=0;i<str.size();i++)
			arr[i]=str.at(i);
		return (str.size());
	}
	static void dumpArrToStr(string *str,char *arr,int len){
		/*memset(str,0,1098);
		memcpy(str,arr,1098);*/
		std::ostringstream buffer;  // add #include <sstream> at the top of
		                            // the file for this
		for(int i = 0; i < len; ++i)
		    buffer << arr[i];
		*str = buffer.str();
		}

}licsenceCommPack;

class packet_manager{

	public:
	static std::string compress(const std::string& data)
		{
			namespace bio = boost::iostreams;

			std::stringstream compressed;
			std::stringstream origin(data);

			bio::filtering_streambuf<bio::input> out;
			out.push(bio::gzip_compressor(bio::gzip_params(bio::gzip::best_compression)));
			out.push(origin);
			bio::copy(out, compressed);

			return compressed.str();
		}

	 static std::string decompress(const std::string& data)
		{
			namespace bio = boost::iostreams;

			std::stringstream compressed(data);
			std::stringstream decompressed;

			bio::filtering_streambuf<bio::input> out;
			out.push(bio::gzip_decompressor());
			out.push(compressed);
			bio::copy(out, decompressed);

			return decompressed.str();
		}

	static string objToString(LicenseComm lc){
		 std::stringstream stream;
	     boost::archive::binary_oarchive ar(stream, boost::archive::no_header);
		 ar << lc;
		 std::cout << "Size: " << stream.str().size() << "\n";
		 return stream.str();
	 }
	static void stringToObj(string str,LicenseComm *lc){
		 stringstream s;
		 s << str;
		 boost::archive::binary_iarchive ia(s, boost::archive::no_header);
		 ia >> *lc;
	 }
	static std::string decode64(const std::string &s) {
	    /*using namespace boost::archive::iterators;
	    using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
	    return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
	        return c == '\0';
	    });*/
		//using namespace boost::archive::iterators;
		typedef transform_width< binary_from_base64<remove_whitespace<string::const_iterator> >, 8, 6 > it_binary_t;
		  typedef insert_linebreaks<base64_from_binary<transform_width<string::const_iterator,6,8> >, 72 > it_base64_t;

		  /*cout << "Your string is: '"<<s<<"'"<<endl;

		  // Encode
		  unsigned int writePaddChars = (3-s.length()%3)%3;
		  string base64(it_base64_t(s.begin()),it_base64_t(s.end()));
		  base64.append(writePaddChars,'=');

		  cout << "Base64 representation: " << base64 << endl;*/
		  string base64(s);
		  // Decode
		  unsigned int paddChars = count(base64.begin(), base64.end(), '=');
		  std::replace(base64.begin(),base64.end(),'=','A'); // replace '=' by base64 encoding of '\0'
		  string result(it_binary_t(base64.begin()), it_binary_t(base64.end())); // decode
		  result.erase(result.end()-paddChars,result.end());  // erase padding '\0' characters
		  //cout << "Decoded: " << result << endl;
		  return result;
	}

	static std::string encode64(const std::string &s) {


				typedef transform_width< binary_from_base64<remove_whitespace<string::const_iterator> >, 8, 6 > it_binary_t;
				  typedef insert_linebreaks<base64_from_binary<transform_width<string::const_iterator,6,8> >, 72 > it_base64_t;



				  // Encode
				  unsigned int writePaddChars = (3-s.length()%3)%3;
				  string base64(it_base64_t(s.begin()),it_base64_t(s.end()));
				  base64.append(writePaddChars,'=');



				  return base64;

	 /*   using namespace boost::archive::iterators;
	    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
	    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
	    return tmp.append((3 - val.size() % 3) % 3, '=');*/
	}


	static int  objToCharArray(unsigned char *c_str,LicenseComm lc){
			 std::stringstream stream;
		     boost::archive::binary_oarchive ar(stream, boost::archive::no_header);
			 ar << lc;
			 string s=compress(encode64(stream.str()));

			 for(int i=0;i<s.size();i++)
				 c_str[i]=s.at(i);
//			 cout<<"Size: "<<s.size()<<"LINE: "<<__LINE__<<"File: "<<__FILE__<<endl;
			 return (s.size());
		 }
		static void charArrayToObj(unsigned char *c_str,LicenseComm *lc,int len){

			string data((char*)c_str, len);
			// cout<<"size "<<data.size()<<"LINE: "<<__LINE__<<"File: "<<__FILE__<<endl;
			string s=decode64(decompress(data));//ss.str()));


			stringstream stream;
			stream<<s;
			//cout<<"com-size: "<<len<<"str-size: "<<s.size()<<"ss-size: "<<stream.str().size()<<"LINE: "<<__LINE__<<"File: "<<__FILE__<<endl;
			 boost::archive::binary_iarchive ia(stream, boost::archive::no_header);
			 ia >> *lc;
			 //cout<<"LINE: "<<__LINE__<<"File: "<<__FILE__<<endl;

		 }

	static void encryptRsaData(Crypto *crypto,string *message_,std::string *encryptedMessage_,  std::string *encryptedKey_,  std::string *iv_){
		unsigned char *encryptedMessage = NULL;
		  unsigned char *encryptedKey;
		  unsigned char *iv;
		  size_t encryptedKeyLength;
		  size_t ivLength;

		  int encryptedMessageLength = crypto->rsaEncrypt((const unsigned char*)message_->c_str(), message_->size()+1,
		    &encryptedMessage, &encryptedKey, &encryptedKeyLength, &iv, &ivLength);

		  encryptedMessage_->assign(base64Encode(encryptedMessage,encryptedMessageLength));
		  encryptedKey_->assign(base64Encode(encryptedKey,encryptedKeyLength));
		  iv_->assign(base64Encode(iv,ivLength));

		  if(encryptedMessageLength == -1) {
		    fprintf(stderr, "Encryption failed\n");
		    return;
		  }

		#if 0
		  // Print the encrypted message as a base64 string
		  char* b64Message = base64Encode((unsigned char*)encryptedMessage_->c_str(), encryptedMessageLength);
		  //printf("Encrypted message: %s\n", b64Message);

		  // Decrypt the message
		  char *decryptedMessage = NULL;

		  int decryptedMessageLength = crypto->rsaDecrypt((unsigned char*)encryptedMessage_->c_str(), (size_t)encryptedMessage_->size(),
		    (unsigned char*)encryptedKey_->c_str(), encryptedKey_->size(), (unsigned char*)iv_->c_str(), iv_->size(), (unsigned char**)&decryptedMessage);

		  if(decryptedMessageLength == -1) {
		    fprintf(stderr, "Decryption failed\n");
		    return;
		  }

		  printf("------------------------Decrypted message: %s\n", decryptedMessage);
		#endif

		  // Clean up
		  free(encryptedMessage);
		  //free(decryptedMessage);
		  free(encryptedKey);
		  free(iv);
		  //free(b64Message);

		  encryptedMessage = NULL;
		  //decryptedMessage = NULL;
		  encryptedKey = NULL;
		  iv = NULL;
		  //b64Message = NULL;
		}



		static void decryptRsaData(Crypto *crypto,string *message_,std::string *encryptedMessage_,  std::string *encryptedKey_,  std::string *iv_){

		unsigned char *encryptedMessage = NULL;
		  unsigned char *encryptedKey;
		  unsigned char *iv;
		  size_t encryptedKeyLength;
		  size_t ivLength;

		#if 0
		  int encryptedMessageLength = crypto->rsaEncrypt((const unsigned char*)message_->c_str(), message_->size()+1,
		    &encryptedMessage, &encryptedKey, &encryptedKeyLength, &iv, &ivLength);

		  encryptedMessage_->assign(encryptedMessage,encryptedMessageLength);
		  encryptedKey_->assign(encryptedKey,encryptedKeyLength);
		  iv_->assign(iv,ivLength);

		  if(encryptedMessageLength == -1) {
		    fprintf(stderr, "Encryption failed\n");
		    return;
		  }

		  // Print the encrypted message as a base64 string
		  char* b64Message = base64Encode((unsigned char*)encryptedMessage_->c_str(), encryptedMessageLength);
		  //printf("Encrypted message: %s\n", b64Message);
		#endif
		  // Decrypt the message
		  char *decryptedMessage = NULL;

		    int encryptedMessageLength=base64Decode(encryptedMessage_->c_str(),encryptedMessage_->size(),&encryptedMessage);
		    encryptedKeyLength=base64Decode(encryptedKey_->c_str(),encryptedKey_->size(),&encryptedKey);
		    ivLength=base64Decode(iv_->c_str(),iv_->size(),&iv);
		    int decryptedMessageLength = crypto->rsaDecrypt(encryptedMessage, (size_t)encryptedMessageLength,encryptedKey, encryptedKeyLength, iv, ivLength, (unsigned char**)&decryptedMessage);

		 // int decryptedMessageLength = crypto->rsaDecrypt((unsigned char*), (size_t)encryptedMessage_->size(),(unsigned char*)base64Decode(encryptedKey_->c_str(),encryptedKey_->size()), encryptedKey_->size(), (unsigned char*)base64Decode(iv_->c_str(),iv_->size()), iv_->size(), (unsigned char**)&decryptedMessage);

		  if(decryptedMessageLength == -1) {
		    fprintf(stderr, "Decryption failed\n");
		    return;
		  }

		  message_->assign(decryptedMessage,decryptedMessageLength);


		  // Clean up
		  free(encryptedMessage);
		  free(decryptedMessage);
		  free(encryptedKey);
		  free(iv);
		  //free(b64Message);

		//  encryptedMessage = NULL;
		  //free(decryptedMessage);
		  decryptedMessage = NULL;
		//  encryptedKey = NULL;
		//  iv = NULL;
		  //b64Message = NULL;
		}

};


} // namespace s11n_example

#endif // SERIALIZATION_STOCK_HPP
