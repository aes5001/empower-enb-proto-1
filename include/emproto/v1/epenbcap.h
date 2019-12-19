/* Copyright (c) 2019 @ FBK - Fondazione Bruno Kessler
 * Author: Kewin Rausch
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

/*    ENB CAPABILITIES MESSAGE
 *
 * This message provides the request/reply for/from eNB capabilities, which are
 * the possible operation which can be done within the base station.
 */

#ifndef __EMAGE_ENB_CAPABILITIES_H
#define __EMAGE_ENB_CAPABILITIES_H

#include <stdint.h>

#include "eppri.h"
#include "epcelcap.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* NOTE: Definitions of a bitmask fields */
typedef enum __ep_enb_capabilities_types {
	/* Can't do anything except present himself */
	EP_ECAP_NOTHING    = 0x0,
	/* Can report UEs */
	EP_ECAP_UE_REPORT  = 0x1,
	/* Can report UEs signal power */
	EP_ECAP_UE_MEASURE = 0x2,
	/* Can hand-over UE to other eNBs */
	EP_ECAP_HANDOVER   = 0x4,
} ep_ecap_type;

/*
 * eNB capabilities message
 */

typedef struct __ep_enb_capabilities_reply {
	uint32_t cap;        /* eNB capabilities */
	/* Cells details are appended here, at the end of eNB message */
}__attribute__((packed)) ep_ecap_rep;

typedef struct __ep_enb_capabilities_request {
	uint8_t dummy;
}__attribute__((packed)) ep_ecap_req;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/* Maximum number of expected cells in an eNB capabilities message */
#define EP_ECAP_CELL_MAX	8

typedef struct __ep_enb_details {
	/* Bitmask of the available capabilities; see 'ep_ecap_type' */
	uint32_t    capmask;
	/* Array of the valid cells */
	ep_cell_det cells[EP_ECAP_CELL_MAX];
	/* Number of valid cells */
	uint32_t    nof_cells;
} ep_enb_det;

/* Format an eNB capabilities negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_rep_fail(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id);

/* Format an eNB capabilities reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id,
	ep_enb_det *  det);

/* Parse an eNB capabilities reply looking for the desired fields */
int epp_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	ep_enb_det *  det);

/* Format an eNB capabilities request.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_req(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id);

/* Parse an eNB capabilities request for the desired fields */
int epp_single_ecap_req(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_ENB_CAPABILITIES_H */
