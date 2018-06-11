/* Copyright (c) 2017 Kewin Rausch
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

#include <netinet/in.h>

#include <emproto.h>

int epf_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	int i = 0;

	ep_uerep_rep * rep = (ep_uerep_rep *)buf;
	ep_uerep_det * det = (ep_uerep_det *)(buf + sizeof(ep_uerep_rep));

	if(size < sizeof(ep_uerep_rep) + (sizeof(ep_uerep_det) * nof_ues)) {
		ep_dbg_log("F - UEREP Rep: Not enough space!\n");
		return -1;
	}

	rep->nof_ues = htonl(nof_ues);

	ep_dbg_dump("F - UREP Rep: ", buf, sizeof(ep_uerep_rep));

	for(i = 0; i < nof_ues && i < max_ues; i++) {
		det[i].pci  = htons(ues[i].pci);
		det[i].rnti = htons(ues[i].rnti);
		det[i].plmn = htonl(ues[i].plmn);
		det[i].imsi = htobe64(ues[i].imsi);

		ep_dbg_dump(
			"F - UREP Det: ",
			buf + sizeof(ep_uerep_rep) + (sizeof(ep_uerep_det) * i),
			sizeof(ep_uerep_det));
	}

	return sizeof(ep_uerep_rep) + (sizeof(ep_uerep_det) * i);
}

int epp_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	int i = -1;

	ep_uerep_rep * rep = (ep_uerep_rep *)buf;
	ep_uerep_det * det = (ep_uerep_det *)(buf + sizeof(ep_uerep_rep));

	if(size < sizeof(ep_uerep_rep)) {
		ep_dbg_log("P - UEREP Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_uerep_rep) + (
		sizeof(ep_uerep_det) * rep->nof_ues))
	{
		ep_dbg_log("P - UEREP Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(nof_ues) {
		*nof_ues = ntohl(rep->nof_ues);
	}

	ep_dbg_dump("P - UREP Rep: ", buf, sizeof(ep_uerep_rep));

	if(ues) {
		for(i = 0; i < *nof_ues && i < max_ues; i++) {
			ues[i].pci  = ntohs(det[i].pci);
			ues[i].rnti = ntohs(det[i].rnti);
			ues[i].plmn = ntohl(det[i].plmn);
			ues[i].imsi = be64toh(det[i].imsi);

			ep_dbg_dump(
				"P - UREP Det: ",
				buf +
					sizeof(ep_uerep_rep) +
					(sizeof(ep_uerep_det) * i),
				sizeof(ep_uerep_det));
		}
	}

	return EP_SUCCESS;
}

int epf_uerep_req(char * buf, unsigned int size)
{
	ep_uerep_req * req = (ep_uerep_req *)buf;

	if(size < sizeof(ep_uerep_req)) {
		ep_dbg_log("F - UEREP Req: Not enough space!\n");
		return -1;
	}

	req->dummy = 0;

	ep_dbg_dump("F - UREP Req: ", buf, sizeof(ep_uerep_req));

	return sizeof(ep_uerep_req);
}

int epp_uerep_req(char * buf, unsigned int size)
{
	if(size < sizeof(ep_uerep_req)) {
		ep_dbg_log("P - UEREP Req: Not enough space!\n");
		return EP_ERROR;
	}

	ep_dbg_dump("P - UREP Req: ", buf, 0);

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_uerep_rep_fail(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Trigger UEREP Fail: Invalid buffer!\n");
		return -1;
	}

	ms += epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_REPORT,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uerep_rep(buf + ret, size - ret, 0, 0, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	uint32_t        nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Trigger UEREP Rep: Invalid buffer!\n");
		return -1;
	}

	if(nof_ues > 0 && !ues) {
		ep_dbg_log("F - Trigger UEREP Rep: Invalid UEs!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_REPORT,
		EP_OPERATION_SUCCESS,
		EP_DIR_REPLY);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uerep_rep(buf + ret, size - ret, nof_ues, max_ues, ues);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	if(!buf) {
		ep_dbg_log("P - Trigger UEREP Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_uerep_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		nof_ues,
		max_ues,
		ues);
}

int epf_trigger_uerep_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id,
	ep_op_type   op)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Trigger UEREP Req: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_REPORT,
		op,
		EP_DIR_REQUEST);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uerep_req(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_uerep_req(char * buf, unsigned int size)
{
	if(!buf) {
		ep_dbg_log("P - Trigger UEREP Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_uerep_req(buf, size);
}
