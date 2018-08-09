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
 */

#include <netinet/in.h>

#include <emproto.h>

/*
 * 
 * Generic TLV: RNTI container
 * 
 */

int epf_TLV_rnti_report(
	char *       buf, 
	unsigned int size, 
	rnti_id_t *  rntis,
	uint32_t     nof_rntis)
{
	int         i;
	ep_TLV *    tlv = (ep_TLV *)buf;
	rnti_id_t * cur = (rnti_id_t *)(buf + sizeof(ep_TLV));

	/* Scan the array while incrementing the RNTIs pointer */
	for(i = 0; i < nof_rntis; i++, cur++) {
		if((char *)cur + sizeof(rnti_id_t) > buf + size) {
			ep_dbg_log(EP_DBG_3
				"F - RNTIREP TLV: Not enough space!\n");

			return -1;
		}

		*cur = htons(rntis[i]);
	}

	tlv->type   = htons(EP_TLV_RNTI_REPORT);
	tlv->length = htons((char *)cur - (buf + sizeof(ep_TLV)));

	ep_dbg_dump(EP_DBG_3"F - RNTIREP TLV: ", buf, (char *)cur - buf);

	return (char *)cur - buf;
}

int epp_TLV_rnti_report(
	char *       buf, 
	rnti_id_t *  rntis,
	uint32_t  *  nof_rntis)
{
	int         c = 0;/* Count */
	int         s;    /* Size */
	
	ep_TLV *    tlv = (ep_TLV *)buf;
	rnti_id_t * cur = (rnti_id_t *)(buf + sizeof(ep_TLV));

	s = ntohs(tlv->length);

	/* Continue until you reach the end of the given TLV */
	while((char *)cur <= buf + sizeof(ep_TLV) + s) {
		if(c >= *nof_rntis) {
			break;
		}

		rntis[c] = ntohs(*cur);
		
		cur++;  /* Go to the next rnti; pointer aritmetics */
		c++;    /* Increment the amount of TLVs read */
	}

	*nof_rntis = c;

	ep_dbg_dump(EP_DBG_3"P - RNTIREP TLV: ", buf, (char *)cur - buf);

	return EP_SUCCESS;
}