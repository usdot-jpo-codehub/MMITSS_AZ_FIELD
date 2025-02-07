/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "MyModule"
 * 	found in "module.asn1"
 * 	`asn1c -S/skeletons`
 */

#include <asn_internal.h>

#include "Extent.h"

int
Extent_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_NativeEnumerated.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
static void
Extent_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NativeEnumerated.free_struct;
	td->print_struct   = asn_DEF_NativeEnumerated.print_struct;
	td->ber_decoder    = asn_DEF_NativeEnumerated.ber_decoder;
	td->der_encoder    = asn_DEF_NativeEnumerated.der_encoder;
	td->xer_decoder    = asn_DEF_NativeEnumerated.xer_decoder;
	td->xer_encoder    = asn_DEF_NativeEnumerated.xer_encoder;
	td->uper_decoder   = asn_DEF_NativeEnumerated.uper_decoder;
	td->uper_encoder   = asn_DEF_NativeEnumerated.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NativeEnumerated.per_constraints;
	td->elements       = asn_DEF_NativeEnumerated.elements;
	td->elements_count = asn_DEF_NativeEnumerated.elements_count;
     /* td->specifics      = asn_DEF_NativeEnumerated.specifics;	// Defined explicitly */
}

void
Extent_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	Extent_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
Extent_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	Extent_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
Extent_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	Extent_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
Extent_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	Extent_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
Extent_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	Extent_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
Extent_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	Extent_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static asn_INTEGER_enum_map_t asn_MAP_Extent_value2enum_1[] = {
	{ 0,	16,	"useInstantlyOnly" },
	{ 1,	13,	"useFor3meters" },
	{ 2,	14,	"useFor10meters" },
	{ 3,	14,	"useFor50meters" },
	{ 4,	15,	"useFor100meters" },
	{ 5,	15,	"useFor500meters" },
	{ 6,	16,	"useFor1000meters" },
	{ 7,	16,	"useFor5000meters" },
	{ 8,	17,	"useFor10000meters" },
	{ 9,	17,	"useFor50000meters" },
	{ 10,	18,	"useFor100000meters" },
	{ 127,	7,	"forever" }
};
static unsigned int asn_MAP_Extent_enum2value_1[] = {
	11,	/* forever(127) */
	10,	/* useFor100000meters(10) */
	8,	/* useFor10000meters(8) */
	6,	/* useFor1000meters(6) */
	4,	/* useFor100meters(4) */
	2,	/* useFor10meters(2) */
	1,	/* useFor3meters(1) */
	9,	/* useFor50000meters(9) */
	7,	/* useFor5000meters(7) */
	5,	/* useFor500meters(5) */
	3,	/* useFor50meters(3) */
	0	/* useInstantlyOnly(0) */
};
static asn_INTEGER_specifics_t asn_SPC_Extent_specs_1 = {
	asn_MAP_Extent_value2enum_1,	/* "tag" => N; sorted by tag */
	asn_MAP_Extent_enum2value_1,	/* N => "tag"; sorted by N */
	12,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1,	/* Strict enumeration */
	0,	/* Native long size */
	0
};
static ber_tlv_tag_t asn_DEF_Extent_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
asn_TYPE_descriptor_t asn_DEF_Extent = {
	"Extent",
	"Extent",
	Extent_free,
	Extent_print,
	Extent_constraint,
	Extent_decode_ber,
	Extent_encode_der,
	Extent_decode_xer,
	Extent_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Extent_tags_1,
	sizeof(asn_DEF_Extent_tags_1)
		/sizeof(asn_DEF_Extent_tags_1[0]), /* 1 */
	asn_DEF_Extent_tags_1,	/* Same as above */
	sizeof(asn_DEF_Extent_tags_1)
		/sizeof(asn_DEF_Extent_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* Defined elsewhere */
	&asn_SPC_Extent_specs_1	/* Additional specs */
};

