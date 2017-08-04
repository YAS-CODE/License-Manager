#include <stdio.h>
#include "../base_lib/CryptoHelper.h"
#include <string>
#include <stdlib.h>
#include <iostream> 
#include "bootstrap.h"

using namespace std;

int main(int argc, char** argv) {

	if (argc != 3) {
		//print_usage();
		exit(2);
	} else {
		printf("********************************************\n");
		printf("*  Bootstrap!!!                            *\n");
		printf("********************************************\n");

	}
	string private_fname = string(argv[1]);
	string public_fname(argv[2]);

	license::Bootstrap::generatePk(private_fname, public_fname);
	return 0;
}
