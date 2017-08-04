/*
 * pc-identifiers.h
 *
 *  Created on: Apr 16, 2014
 *      
 */

#ifndef PC_IDENTIFIERS_H_
#define PC_IDENTIFIERS_H_
#include "api/datatypes.h"
#include "base/base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char PcIdentifier[7];//[6];
typedef char PcSignature[24];//[21];

FUNCTION_RETURN generate_pc_id(PcIdentifier * identifiers, unsigned int * array_size,
		IDENTIFICATION_STRATEGY strategy);

EVENT_TYPE validate_pc_signature(PcSignature str_code);

/**
 * Generates an UserPcIdentifier.
 *
 * @param identifier_out
 * @param strategy
 * @return
 */
FUNCTION_RETURN generate_user_pc_signature(PcSignature identifier_out,
		IDENTIFICATION_STRATEGY strategy);

FUNCTION_RETURN encode_pc_id(PcIdentifier identifier1, PcIdentifier identifier2,
		PcSignature pc_identifier_out);

FUNCTION_RETURN generate_ethernet_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers, int use_mac);

FUNCTION_RETURN generate_user_pc_signature_having_mac(PcSignature identifier_out,
		IDENTIFICATION_STRATEGY strategy,PcIdentifier identifier_mac);

FUNCTION_RETURN decode_pc_id(PcIdentifier identifier1_out,
		PcIdentifier identifier2_out, PcSignature pc_signature_in);

IDENTIFICATION_STRATEGY strategy_from_pc_id(PcIdentifier identifier);

void hexDump (char *desc, void *addr, int len);

#ifdef __cplusplus
}
#endif
#endif /* PC_IDENTIFIERS_H_ */
