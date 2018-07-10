/* Copyright (c) 2018 Kewin Rausch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Empower Agent Protocols Type-Length-Value common components
 */

#ifndef __EMAGE_TLV_H
#define __EMAGE_TLV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* types of valid TLV token for empower protocol */
enum ep_TLV_type {
	/* Invalid TLV token */
	EP_TLV_INVALID = 0,
	/* Token contains a cell capabilities information */
	EP_TLV_CELL_CAP,
};

typedef uint16_t tlv_type_t;
typedef uint16_t tlv_length_t;

/* Structure of the TLV header common to all components */
typedef struct __ep_hdr_TLV {
	tlv_type_t   type;    /* Type of the body */
	tlv_length_t length;  /* Length of the body */
	/*
	 * Here following there will be the TLV body
	 */
}__attribute__((packed)) ep_TLV;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_TLV_H */
