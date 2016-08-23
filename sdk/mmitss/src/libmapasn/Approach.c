/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "J2735MAPMESSAGE"
 * 	found in "module.asn1"
 * 	`asn1c -S/skeletons`
 */

#include <asn_internal.h>

#include "Approach.h"

static int
memb_drivingLanes_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size <= 32)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_computedLanes_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size <= 32)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_trainsAndBuses_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size <= 32)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_barriers_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size <= 32)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_crosswalks_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size <= 32)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_drivingLanes_4[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_VehicleReferenceLane,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_drivingLanes_tags_4[] = {
	(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_drivingLanes_specs_4 = {
	sizeof(struct drivingLanes),
	offsetof(struct drivingLanes, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_drivingLanes_4 = {
	"drivingLanes",
	"drivingLanes",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_drivingLanes_tags_4,
	sizeof(asn_DEF_drivingLanes_tags_4)
		/sizeof(asn_DEF_drivingLanes_tags_4[0]) - 1, /* 1 */
	asn_DEF_drivingLanes_tags_4,	/* Same as above */
	sizeof(asn_DEF_drivingLanes_tags_4)
		/sizeof(asn_DEF_drivingLanes_tags_4[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_drivingLanes_4,
	1,	/* Single element */
	&asn_SPC_drivingLanes_specs_4	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_computedLanes_6[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_VehicleComputedLane,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_computedLanes_tags_6[] = {
	(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_computedLanes_specs_6 = {
	sizeof(struct computedLanes),
	offsetof(struct computedLanes, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_computedLanes_6 = {
	"computedLanes",
	"computedLanes",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_computedLanes_tags_6,
	sizeof(asn_DEF_computedLanes_tags_6)
		/sizeof(asn_DEF_computedLanes_tags_6[0]) - 1, /* 1 */
	asn_DEF_computedLanes_tags_6,	/* Same as above */
	sizeof(asn_DEF_computedLanes_tags_6)
		/sizeof(asn_DEF_computedLanes_tags_6[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_computedLanes_6,
	1,	/* Single element */
	&asn_SPC_computedLanes_specs_6	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_trainsAndBuses_8[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_SpecialLane,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_trainsAndBuses_tags_8[] = {
	(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_trainsAndBuses_specs_8 = {
	sizeof(struct trainsAndBuses),
	offsetof(struct trainsAndBuses, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_trainsAndBuses_8 = {
	"trainsAndBuses",
	"trainsAndBuses",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_trainsAndBuses_tags_8,
	sizeof(asn_DEF_trainsAndBuses_tags_8)
		/sizeof(asn_DEF_trainsAndBuses_tags_8[0]) - 1, /* 1 */
	asn_DEF_trainsAndBuses_tags_8,	/* Same as above */
	sizeof(asn_DEF_trainsAndBuses_tags_8)
		/sizeof(asn_DEF_trainsAndBuses_tags_8[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_trainsAndBuses_8,
	1,	/* Single element */
	&asn_SPC_trainsAndBuses_specs_8	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_barriers_10[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_BarrierLane,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_barriers_tags_10[] = {
	(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_barriers_specs_10 = {
	sizeof(struct barriers),
	offsetof(struct barriers, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_barriers_10 = {
	"barriers",
	"barriers",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_barriers_tags_10,
	sizeof(asn_DEF_barriers_tags_10)
		/sizeof(asn_DEF_barriers_tags_10[0]) - 1, /* 1 */
	asn_DEF_barriers_tags_10,	/* Same as above */
	sizeof(asn_DEF_barriers_tags_10)
		/sizeof(asn_DEF_barriers_tags_10[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_barriers_10,
	1,	/* Single element */
	&asn_SPC_barriers_specs_10	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_crosswalks_12[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_CrosswalkLane,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_crosswalks_tags_12[] = {
	(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_crosswalks_specs_12 = {
	sizeof(struct crosswalks),
	offsetof(struct crosswalks, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_crosswalks_12 = {
	"crosswalks",
	"crosswalks",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_crosswalks_tags_12,
	sizeof(asn_DEF_crosswalks_tags_12)
		/sizeof(asn_DEF_crosswalks_tags_12[0]) - 1, /* 1 */
	asn_DEF_crosswalks_tags_12,	/* Same as above */
	sizeof(asn_DEF_crosswalks_tags_12)
		/sizeof(asn_DEF_crosswalks_tags_12[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_crosswalks_12,
	1,	/* Single element */
	&asn_SPC_crosswalks_specs_12	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_Approach_1[] = {
	{ ATF_POINTER, 2, offsetof(struct Approach, name),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DescriptiveName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"name"
		},
	{ ATF_POINTER, 1, offsetof(struct Approach, id),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ApproachNumber,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"id"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Approach, drivingLanes),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		0,
		&asn_DEF_drivingLanes_4,
		memb_drivingLanes_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"drivingLanes"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Approach, computedLanes),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		0,
		&asn_DEF_computedLanes_6,
		memb_computedLanes_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"computedLanes"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Approach, trainsAndBuses),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		0,
		&asn_DEF_trainsAndBuses_8,
		memb_trainsAndBuses_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"trainsAndBuses"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Approach, barriers),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		0,
		&asn_DEF_barriers_10,
		memb_barriers_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"barriers"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Approach, crosswalks),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		0,
		&asn_DEF_crosswalks_12,
		memb_crosswalks_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"crosswalks"
		},
};
static ber_tlv_tag_t asn_DEF_Approach_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Approach_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* name at 150 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* id at 151 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* drivingLanes at 152 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* computedLanes at 153 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* trainsAndBuses at 154 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* barriers at 155 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 } /* crosswalks at 157 */
};
static asn_SEQUENCE_specifics_t asn_SPC_Approach_specs_1 = {
	sizeof(struct Approach),
	offsetof(struct Approach, _asn_ctx),
	asn_MAP_Approach_tag2el_1,
	7,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Approach = {
	"Approach",
	"Approach",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Approach_tags_1,
	sizeof(asn_DEF_Approach_tags_1)
		/sizeof(asn_DEF_Approach_tags_1[0]), /* 1 */
	asn_DEF_Approach_tags_1,	/* Same as above */
	sizeof(asn_DEF_Approach_tags_1)
		/sizeof(asn_DEF_Approach_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Approach_1,
	7,	/* Elements count */
	&asn_SPC_Approach_specs_1	/* Additional specs */
};

