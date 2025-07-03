#ifndef ASN1BER_H
#define ASN1BER_H


#include <stdbool.h>
#include <stdint.h>

#include "ComCodec.h"


#define ASN1_TAG_BOOLEAN 1
#define ASN1_TAG_INTEGER 2
#define ASN1_TAG_BIT_STRING 3
#define ASN1_TAG_OCTECT_STRING 4
#define ASN1_TAG_NULL 5
#define ASN1_TAG_OBJECT_IDENTIFIER 6
#define ASN1_TAG_OBJECT_DESCRIPTION 7
#define ASN1_TAG_EXTERNAL 8
#define ASN1_TAG_REAL 9
#define ASN1_TAG_ENUMERATED 10
#define ASN1_TAG_EMBEDDED_PDV 11
#define ASN1_TAG_UTF8_STRING 12
#define ASN1_TAG_RELATIVE_OID 13
#define ASN1_TAG_SEQUENCE 16
#define ASN1_TAG_SET 17
#define ASN1_TAG_NUMERIC_STRING 18
#define ASN1_TAG_PRINTABLE_STRING 19
#define ASN1_TAG_TELETEX_STRING 20
#define ASN1_TAG_VIDEOTEX_STRING 21
#define ASN1_TAG_IA5_STRING 22
#define ASN1_TAG_UTC_TIME 23
#define ASN1_TAG_GENERALIZED_TIME 24
#define ASN1_TAG_GRAPHIC_STRING 25
#define ASN1_TAG_VISIBLE_STRING 26
#define ASN1_TAG_GENERAL_STRING 27
#define ASN1_TAG_UNIVERSAL_STRING 28
#define ASN1_TAG_CHARACTER_STRING 29
#define ASN1_TAG_BMP_STRING 30

#define ASN1_TAG_ASFOLLOW 31


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	bool implicit;
	bool application;
	bool construct;
	
	uint8_t tag;
	uint8_t wide;
	
} Asn1Tag;

typedef struct
{
	unsigned length;
	uint8_t wide;
	
} Asn1Len;


static inline int ASN1_ENCODE_TAG(Asn1Tag* asn1tag, uint8_t* p, size_t size)
{
	if(asn1tag->tag > 0x7F)
		return -1;
	else if(asn1tag->tag > ASN1_TAG_ASFOLLOW)
	{
		if(size < 2)
			return -2;
		
		ENCODE_BYTE(p, ASN1_TAG_ASFOLLOW);
		ENCODE_BYTE(p + 1, asn1tag->tag);
		
		asn1tag->wide = 2;
	}
	else if(asn1tag->tag == ASN1_TAG_ASFOLLOW)
		return -3;
	else
	{
		if(size < 1)
			return -4;
		
		ENCODE_BYTE(p, asn1tag->tag);
		
		asn1tag->wide = 1;
	}
	
	if(asn1tag->implicit)
		*p |= 0x80;
	if(asn1tag->application)
		*p |= 0x40;
	if(asn1tag->construct)
		*p |= 0x20;
	
	return 0;
}
static inline int ASN1_DECODE_TAG(const uint8_t* p, size_t size, Asn1Tag* asn1tag)
{
	if(size < 1)
		return -1;
	
	asn1tag->implicit = (DECODE_BYTE(p) & 0x80) ? true : false;
	asn1tag->application = (DECODE_BYTE(p) & 0x40) ? true : false;
	asn1tag->construct = (DECODE_BYTE(p) & 0x20) ? true : false;
	
	asn1tag->tag = DECODE_BYTE(p) & 0x1F;
	
	if(asn1tag->tag == ASN1_TAG_ASFOLLOW)
	{
		if(size < 2)
			return -2;
		
		asn1tag->tag = DECODE_BYTE(p + 1);
		
		if(asn1tag->tag & 0x80)
			return -3;
		
		asn1tag->wide = 2;
	}
	else
		asn1tag->wide = 1;
	
	return 0;
}

static inline int ASN1_ENCODE_LEN(uint8_t* p, size_t size, Asn1Len* asn1len)
{
	if(asn1len->length < 0x80)
	{
		if(size < 1)
			return -1;
		
		ENCODE_BYTE(p, asn1len->length);
		
		asn1len->wide = 1;
	}
	else if(asn1len->length < 0x100)
	{
		if(size < 2)
			return -2;
		
		ENCODE_BYTE(p, 0x81);
		ENCODE_BYTE(p + 1, asn1len->length);
		
		asn1len->wide = 2;
	}
	else if(asn1len->length < 0x10000)
	{
		if(size < 3)
			return -3;
		
		ENCODE_BYTE(p, 0x82);
		ENCODE_WORD(p + 1, asn1len->length);
		
		asn1len->wide = 3;
	}
	else
		return -4;
	
	return 0;
}
static inline int ASN1_DECODE_LEN(const uint8_t* p, size_t size, Asn1Len* asn1len)
{
	if(size < 1)
		return -1;
	
	if(DECODE_BYTE(p) & 0x80)
	{
		switch(DECODE_BYTE(p) & 0x7F)
		{
		case 0:
			return -2;
			
		case 1:
			if(size < 2)
				return -3;
			
			asn1len->length = DECODE_BYTE(p + 1);
			
			asn1len->wide = 2;
			
			break;
			
		case 2:
			if(size < 3)
				return -4;
			
			asn1len->length = DECODE_WORD(p + 1);
			
			asn1len->wide = 3;
			
			break;
			
		default:
			return -5;
		}
	}
	else
	{
		asn1len->length = DECODE_BYTE(p);
		
		asn1len->wide = 1;
	}
	
	return 0;
}

static inline uint8_t ASN1_SIGNED_SIZE(int64_t s)
{
	if((s > (int8_t)(0x80)) && (s < (int8_t)(0x7F)))
		return 1;
	else if((s > (int16_t)(0x8000)) && (s < (int16_t)(0x7FFF)))
		return 2;
	else if((s > (int32_t)(0xFF800000)) && (s < (int32_t)(0x007FFFFF)))
		return 3;
	else if((s > (int32_t)(0x80000000)) && (s < (int32_t)(0x7FFFFFFF)))
		return 4;
	else if((s > (int64_t)(0xFFFFFF8000000000)) && (s < (int64_t)(0x0000007FFFFFFFFF)))
		return 5;
	else if((s > (int64_t)(0xFFFF800000000000)) && (s < (int64_t)(0x00007FFFFFFFFFFF)))
		return 6;
	else if((s > (int64_t)(0xFF80000000000000)) && (s < (int64_t)(0x007FFFFFFFFFFFFF)))
		return 7;
	else
		return 8;
}
static inline uint8_t ASN1_UNSIGNED_SIZE(uint64_t u)
{
	if(u < (uint8_t)(0x80))
		return 1;
	else if(u < (uint16_t)(0x8000))
		return 2;
	else if(u < (uint32_t)(0x800000))
		return 3;
	else if(u < (uint32_t)(0x80000000))
		return 4;
	else if(u < (uint64_t)(0x8000000000))
		return 5;
	else if(u < (uint64_t)(0x800000000000))
		return 6;
	else if(u < (uint64_t)(0x80000000000000))
		return 7;
	else
		return 8;
}


#ifdef __cplusplus
}
#endif


#endif
