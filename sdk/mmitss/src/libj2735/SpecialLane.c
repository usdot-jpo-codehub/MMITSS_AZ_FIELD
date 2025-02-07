/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "J2735MAPMESSAGE"
 * 	found in "MAP.txt"
 * 	`asn1c -fnative-types`
 */

#include <asn_internal.h>

#include "SpecialLane.h"

static asn_TYPE_member_t asn_MBR_SpecialLane_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialLane, laneNumber),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LaneNumber,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"laneNumber"
		},
	{ ATF_POINTER, 1, offsetof(struct SpecialLane, laneWidth),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LaneWidth,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"laneWidth"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialLane, laneAttributes),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SpecialLaneAttributes,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"laneAttributes"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialLane, nodeList),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NodeList,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"nodeList"
		},
	{ ATF_POINTER, 2, offsetof(struct SpecialLane, keepOutList),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NodeList,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keepOutList"
		},
	{ ATF_POINTER, 1, offsetof(struct SpecialLane, connectsTo),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ConnectsTo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"connectsTo"
		},
};
static ber_tlv_tag_t asn_DEF_SpecialLane_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SpecialLane_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* laneNumber at 254 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* laneWidth at 255 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* laneAttributes at 256 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* nodeList at 257 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* keepOutList at 259 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* connectsTo at 261 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SpecialLane_specs_1 = {
	sizeof(struct SpecialLane),
	offsetof(struct SpecialLane, _asn_ctx),
	asn_MAP_SpecialLane_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SpecialLane = {
	"SpecialLane",
	"SpecialLane",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SpecialLane_tags_1,
	sizeof(asn_DEF_SpecialLane_tags_1)
		/sizeof(asn_DEF_SpecialLane_tags_1[0]), /* 1 */
	asn_DEF_SpecialLane_tags_1,	/* Same as above */
	sizeof(asn_DEF_SpecialLane_tags_1)
		/sizeof(asn_DEF_SpecialLane_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SpecialLane_1,
	6,	/* Elements count */
	&asn_SPC_SpecialLane_specs_1	/* Additional specs */
};

