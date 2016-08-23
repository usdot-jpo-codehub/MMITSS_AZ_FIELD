/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "J2735MAPMESSAGE"
 * 	found in "MAP.txt"
 * 	`asn1c -fnative-types`
 */

#ifndef	_Intersection_H_
#define	_Intersection_H_


#include <asn_application.h>

/* Including external dependencies */
#include "DescriptiveName.h"
#include "IntersectionID.h"
#include "Heading.h"
#include "LaneWidth.h"
#include "IntersectionStatusObject.h"
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct Position3D;
struct ApproachObject;

/* Intersection */
typedef struct Intersection {
	DescriptiveName_t	*name	/* OPTIONAL */;
	IntersectionID_t	 id;
	struct Position3D	*refPoint	/* OPTIONAL */;
	IntersectionID_t	*refInterNum	/* OPTIONAL */;
	Heading_t	*orientation	/* OPTIONAL */;
	LaneWidth_t	*laneWidth	/* OPTIONAL */;
	IntersectionStatusObject_t	*type	/* OPTIONAL */;
	struct approaches {
		A_SEQUENCE_OF(struct ApproachObject) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} approaches;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Intersection_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Intersection;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "Position3D.h"
#include "ApproachObject.h"

#endif	/* _Intersection_H_ */
