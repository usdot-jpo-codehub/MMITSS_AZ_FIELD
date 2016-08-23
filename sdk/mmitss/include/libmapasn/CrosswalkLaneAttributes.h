/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "J2735MAPMESSAGE"
 * 	found in "module.asn1"
 * 	`asn1c -S/skeletons`
 */

#ifndef	_CrosswalkLaneAttributes_H_
#define	_CrosswalkLaneAttributes_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum CrosswalkLaneAttributes {
	CrosswalkLaneAttributes_noData	= 0,
	CrosswalkLaneAttributes_twoWayPath	= 1,
	CrosswalkLaneAttributes_pedestrianCrosswalk	= 2,
	CrosswalkLaneAttributes_bikeLane	= 4,
	CrosswalkLaneAttributes_railRoadTrackPresent	= 8,
	CrosswalkLaneAttributes_oneWayPathOfTravel	= 16,
	CrosswalkLaneAttributes_pedestrianCrosswalkTypeA	= 32,
	CrosswalkLaneAttributes_pedestrianCrosswalkTypeB	= 64,
	CrosswalkLaneAttributes_pedestrianCrosswalkTypeC	= 128
} e_CrosswalkLaneAttributes;

/* CrosswalkLaneAttributes */
typedef long	 CrosswalkLaneAttributes_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CrosswalkLaneAttributes;
asn_struct_free_f CrosswalkLaneAttributes_free;
asn_struct_print_f CrosswalkLaneAttributes_print;
asn_constr_check_f CrosswalkLaneAttributes_constraint;
ber_type_decoder_f CrosswalkLaneAttributes_decode_ber;
der_type_encoder_f CrosswalkLaneAttributes_encode_der;
xer_type_decoder_f CrosswalkLaneAttributes_decode_xer;
xer_type_encoder_f CrosswalkLaneAttributes_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _CrosswalkLaneAttributes_H_ */
