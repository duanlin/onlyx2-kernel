#ifndef BITCODEC_H
#define BITCODEC_H


#include <stdbool.h>
#include <stdint.h>


#define BIT_GET(ADDR, INDEX) ((*((volatile uint8_t*)(ADDR) + (INDEX) / 8) & (0x01 << (INDEX) % 8)) ? 0b1 : 0b0)
#define BIT_SET(BITS, INDEX, VALUE) ((VALUE) ? (*((volatile uint8_t*)(ADDR) + (INDEX) / 8) |= 0x01 << (INDEX) % 8) : (*((volatile uint8_t*)(ADDR) + (INDEX) / 8) &= ~(0x01 << (INDEX) % 8)))

#define REG32_GET(ADDR) (*(volatile uint32_t*)(ADDR))
#define REG32_SET(ADDR, VALUE) (*(volatile uint32_t*)(ADDR) = (uint32_t)(VALUE))
#define REG32_GET_BIT(ADDR, INDEX) (*(volatile uint32_t*)(ADDR) & (0x00000001 << (INDEX)))
#define REG32_SET_BIT(ADDR, INDEX, VALUE) ((VALUE) ? (*(volatile uint32_t*)(ADDR) |= 0x00000001 << (INDEX)) : (*(volatile uint32_t*)(ADDR) &= ~(0x00000001 << (INDEX))))
#define REG32_DECODE(ADDR, MASK) (*(volatile uint32_t*)(ADDR) & (uint32_t)(MASK))
#define REG32_ENCODE(ADDR, MASK, VALUE) (*(volatile uint32_t*)(ADDR) = (*(volatile uint32_t*)(ADDR) & ~(uint32_t)(MASK)) | ((uint32_t)(VALUE) & (MASK)))

#define REG64_GET(ADDR) (*(volatile uint64_t*)(ADDR))
#define REG64_SET(ADDR, VALUE) (*(volatile uint64_t*)(ADDR) = (uint64_t)(VALUE))
#define REG64_GET_BIT(ADDR, INDEX) (*(volatile uint64_t*)(ADDR) & (0x0000000000000001 << (INDEX)))
#define REG64_SET_BIT(ADDR, INDEX, VALUE) ((VALUE) ? (*(volatile uint64_t*)(ADDR) |= 0x0000000000000001 << (INDEX)) : (*(volatile uint64_t*)(ADDR) &= ~(0x0000000000000001 << (INDEX))))
#define REG64_DECODE(ADDR, MASK) (*(volatile uint64_t*)(ADDR) & (uint64_t)(MASK))
#define REG64_ENCODE(ADDR, MASK, VALUE) (*(volatile uint64_t*)(ADDR) = (*(volatile uint64_t*)(ADDR) & ~(uint64_t)(MASK)) | ((uint64_t)(VALUE) & (MASK)))

static inline uint32_t REG32_GET_RANGE(uintptr_t ADDR, unsigned HIGH, unsigned LOW)
{
	unsigned i;
	uint32_t mask;
	
	for(mask = 0, i = LOW; i <= HIGH; i ++)
		mask |= 0x00000001 << i;
	
	return REG32_DECODE(ADDR, mask) >> LOW;
}

#define REG32_SET_RANGE(ADDR, HIGH, LOW, VALUE) do \
{ \
	unsigned i; \
	uint32_t mask; \
	\
	for(mask = 0, i = LOW; i <= HIGH; i ++) \
		mask |= 0x00000001 << i; \
	\
	REG32_ENCODE(ADDR, mask, VALUE << LOW); \
	\
} while(false)

static inline uint64_t REG64_GET_RANGE(uintptr_t ADDR, unsigned HIGH, unsigned LOW)
{
	unsigned i;
	uint64_t mask;
	
	for(mask = 0, i = LOW; i <= HIGH; i ++)
		mask |= 0x0000000000000001 << i;
	
	return REG64_DECODE(ADDR, mask) >> LOW;
}

#define REG64_SET_RANGE(ADDR, HIGH, LOW, VALUE) do \
{ \
	unsigned i; \
	uint64_t mask; \
	\
	for(mask = 0, i = LOW; i <= HIGH; i ++) \
		mask |= 0x0000000000000001 << i; \
	\
	REG64_ENCODE(ADDR, mask, VALUE << LOW); \
	\
} while(false)


#endif
