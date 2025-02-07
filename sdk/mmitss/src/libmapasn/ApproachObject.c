/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "J2735MAPMESSAGE"
 * 	found in "module.asn1"
 * 	`asn1c -S/skeletons`
 */

#include <asn_internal.h>

#include "ApproachObject.h"

static asn_TYPE_member_t asn_MBR_ApproachObject_1[] = {
	{ ATF_POINTER, 4, offsetof(struct ApproachObject, refPoint),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Position3D,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"refPoint"
		},
	{ ATF_POINTER, 3, offsetof(struct ApproachObject, laneWidth),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LaneWidth,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"laneWidth"
		},
	{ ATF_POINTER, 2, offsetof(struct ApproachObject, approach),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Approach,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"approach"
		},
	{ ATF_POINTER, 1, offsetof(struct ApproachObject, egress),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Approach,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"egress"
		},
};
static ber_tlv_tag_t asn_DEF_ApproachObject_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ApproachObject_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* refPoint at 137 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* laneWidth at 140 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* approach at 143 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* egress at 145 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ApproachObject_specs_1 = {
	sizeof(struct ApproachObject),
	offsetof(struct ApproachObject, _asn_ctx),
	asn_MAP_ApproachObject_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ApproachObject = {
	"ApproachObject",
	"ApproachObject",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ApproachObject_tags_1,
	sizeof(asn_DEF_ApproachObject_tags_1)
		/sizeof(asn_DEF_ApproachObject_tags_1[0]), /* 1 */
	asn_DEF_ApproachObject_tags_1,	/* Same as above */
	sizeof(asn_DEF_ApproachObject_tags_1)
		/sizeof(asn_DEF_ApproachObject_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ApproachObject_1,
	4,	/* Elements count */
	&asn_SPC_ApproachObject_specs_1	/* Additional specs */
};

