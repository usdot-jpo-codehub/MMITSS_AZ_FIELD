/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "MyModule"
 * 	found in "SRM.txt"
 * 	`asn1c -fnative-types`
 */

#ifndef	_SRM_H_
#define	_SRM_H_


#include <asn_application.h>

/* Including external dependencies */
#include "DSRCmsgID.h"
#include "MsgCount.h"
#include "SignalRequest.h"
#include "TransitStatus.h"
#include "BSMblob.h"
#include "VehicleRequestStatus.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct DTime;
struct VehicleIdent;

/* SRM */
typedef struct SRM {
	DSRCmsgID_t	 msgID;
	MsgCount_t	 msgCnt;
	SignalRequest_t	 request;
	struct DTime	*timeOfService	/* OPTIONAL */;
	struct DTime	*endOfService	/* OPTIONAL */;
	TransitStatus_t	*transitStatus	/* OPTIONAL */;
	struct VehicleIdent	*vehicleVIN	/* OPTIONAL */;
	BSMblob_t	 vehicleData;
	VehicleRequestStatus_t	*status	/* OPTIONAL */;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SRM_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SRM;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "DTime.h"
#include "VehicleIdent.h"

#endif	/* _SRM_H_ */
