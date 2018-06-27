/*
 * Kvx.h
 *
 *  Created on: 16 jun. 2018
 *      Author: Dorus
 */

#ifndef KVX_H_
#define KVX_H_

/* Key-Value eXchange API */

#define VALUE_SIZE_UNUSED 0

typedef enum {
	/* Invalid value type. */
	VALUE_TYPE_INVALID = -1,

	/* Fixed size value types. */
	VALUE_TYPE_UINT_8 = 0,
	VALUE_TYPE_UINT_16,
	VALUE_TYPE_UINT_32,
	VALUE_TYPE_INT_8,
	VALUE_TYPE_INT_16,
	VALUE_TYPE_INT_32,

	/* Variable size value types. */
	VALUE_TYPE_STRING,
	VALUE_TYPE_UINT_8_ARRAY,
	VALUE_TYPE_MAP,
	VALUE_TYPE_PAIR,

	/* Not an actual value type.
	 * Must be the last value in this enum. */
	VALUE_TYPE_NUM
}KvxValueType_t;

struct KeyValuePair;

typedef struct KeyValuePair * KvxPair_t;

typedef U16_t KvxKey_t;
#define KEY_INVALID 0xFFFF

void *(KvxCallbackReceive_t)(KvxPair_t pair);

MpsResult_t KvxRegisterKey(KvxKey_t key, KvxCallbackReceive_t on_key);

MpsResult_t KvxUnregisterKey(KvxKey_t key);


KvxPair_t KvxPairCreate(KvxKey_t key);

MpsResult_t KvxPairDelete(KvxPair_t pair);

MpsResult_t KvxPairSend(KvxPair_t pair);


KvxKey_t KvxPairKeyGet(KvxPair_t pair);


MpsResult_t KvxPairValueAdd(KvxPair_t pair, void *value, U32_t size, KvxValueType_t type);

U32_t KvxPairValueRemove(KvxPair_t pair, void *value, KvxValueType_t *type);


/* Example usage. */

#define BYTE_0_KEY		0
#define BYTE_1_KEY		1
#define BYTE_ARRAY_KEY	2
#define MAP_KEY			3

#define BYTE_ARRAY_SIZE 20
uint8_t byte_0 = 0xDE;
uint8_t byte_1 = 0xAD;
uint8_t byte_array[BYTE_ARRAY_SIZE];

KxvPair_t pair_map = KvxPairCreate(MAP_KEY);

KvxPairValueAdd(pair_map,  &byte_0, VALUE_SIZE_UNUSED, VALUE_TYPE_UINT_8);
KvxPairValueAdd(pair_map, &byte_1, VALUE_SIZE_UNUSED, VALUE_TYPE_UINT_8);
KvxPairValueAdd(pair_map, byte_array, sizeof(byte_array), VALUE_TYPE_UINT_8_ARRAY);

KvxPairSend(pair_map);

/* Receiving Side */

void OnMapReceived(KvxPair_t pair);

KvxRegisterKey(MAP_KEY, OnMapReceived);

void OnMapReceived(KvxPair_t pair)
{
	U32_t val_size = 0;
	KvxValueType_t val_type = VALUE_TYPE_INVALID;
	KvxKey_t val_key = 0;

	val_key = KvxPairKeyGet(pair);
	if(val_key == MAP_KEY) {
		do {
			val_key = KvxPairValueRemove(pair, &pair_byte_0, &val_size, &val_type);
			switch(val_key) {
			case BYTE_0_KEY: {

			}
			case BYTE_1_KEY: {

			}
			case BYTE_ARRAY_KEY: {

			}
			}
		} while(val_key != KEY_INVALID);
	}


}

#endif /* KVX_H_ */
