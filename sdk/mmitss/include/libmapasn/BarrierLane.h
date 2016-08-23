/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "J2735MAPMESSAGE"
 * 	found in "module.asn1"
 * 	`asn1c -S/skeletons`
 */

#ifndef	_BarrierLane_H_
#define	_BarrierLane_H_


#include <asn_application.h>

/* Including external dependencies */
#include "LaneNumber.h"
#include "LaneWidth.h"
#include "BarrierAttributes.h"
#include "NodeList.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BarrierLane */
typedef struct BarrierLane {
	LaneNumber_t	 laneNumber;
	LaneWidth_t	*laneWidth	/* OPTIONAL */;
	BarrierAttributes_t	 barrierAttributes;
	NodeList_t	 nodeList;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BarrierLane_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BarrierLane;

#ifdef __cplusplus
}
#endif

#endif	/* _BarrierLane_H_ */
