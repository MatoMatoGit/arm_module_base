/*
 * KvxMps.h
 *
 *  Created on: 19 jun. 2018
 *      Author: Dorus
 */

#ifndef KVX_MPS_H_
#define KVX_MPS_H_

/* Key-Value eXchange MPS compliant interface */

MpsResult_t KvxInit(void);
MpsResult_t KvxDeinit(void);

MpsResult_t KvxStart(void);
MpsResult_t KvxStop(void);

void KvxError(MpsLayerHandle_t source_layer, MpsPacketHandle_t packet, uint16_t error);
MpsResult_t KvxRunloop(void);

MpsResult_t KvxTransmitRequest(MpsPacketHandle_t packet);
MpsResult_t KvxReceiveRequest(MpsPacketHandle_t packet);

MpsResult_t KvxLoopbackRequest(MpsPacketHandle_t packet, MpsLayerHandle_t layer);



#endif /* KVXMPS_H_ */
