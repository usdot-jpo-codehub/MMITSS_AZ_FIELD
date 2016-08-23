/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "MyModule"
 * 	found in "module.asn1"
 * 	`asn1c -S/skeletons`
 */

#ifndef	_EmergencyVehicleAlert_H_
#define	_EmergencyVehicleAlert_H_


#include <asn_application.h>

/* Including external dependencies */
#include "DSRCmsgID.h"
#include "RoadSideAlert.h"
#include "MsgCRC.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EmergencyVehicleAlert */
typedef struct EmergencyVehicleAlert {
	DSRCmsgID_t	 msgID;
	RoadSideAlert_t	 rsaMsg;
	MsgCRC_t	 crc;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} EmergencyVehicleAlert_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_EmergencyVehicleAlert;

#ifdef __cplusplus
}
#endif

#endif	/* _EmergencyVehicleAlert_H_ */
