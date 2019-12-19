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

#include <netinet/in.h>

#include <emproto.h>

int epf_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        nof_meas,
	uint32_t        max,
	ep_ue_measure * ues)
{
	int i = 0;

	ep_uemeas_rep * rep = (ep_uemeas_rep *) buf;
	ep_uemeas_det * det = (ep_uemeas_det *)(buf + sizeof(ep_uemeas_rep));

	if(size < sizeof(ep_uemeas_rep) + (sizeof(ep_uemeas_det) + nof_meas)) {
		ep_dbg_log(EP_DBG_2"F - UMEA Rep: Not enough space!\n");
		return -1;
	}

	rep->nof_meas = htonl(nof_meas);

	ep_dbg_dump(EP_DBG_2"F - UMEA Rep: ", buf, sizeof(ep_uemeas_rep));

	for(i = 0; i < nof_meas && i < max; i++) {
		det[i].meas_id = ues[i].meas_id;
		det[i].pci     = htons(ues[i].pci);
		det[i].rsrp    = htons(ues[i].rsrp);
		det[i].rsrq    = htons(ues[i].rsrq);
		
		ep_dbg_dump(
			EP_DBG_3"F - UMEA Det: ", 
			(char *)(det + i), 
			sizeof(ep_uemeas_det));
	}

	return sizeof(ep_uemeas_rep) + (sizeof(ep_uemeas_det) * i);
}

int epp_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_meas,
	uint32_t        max,
	ep_ue_measure * ues)
{
	int i;

	ep_uemeas_rep * rep = (ep_uemeas_rep *) buf;
	ep_uemeas_det * det = (ep_uemeas_det *)(buf + sizeof(ep_uemeas_rep));

	if(size < sizeof(ep_uemeas_rep)) {
		ep_dbg_log(EP_DBG_2"P - UMEA Rep: Not enough space!\n");
		return -1;
	}

	if(size < sizeof(ep_uemeas_rep) + (
		sizeof(ep_uemeas_det) + ntohl(rep->nof_meas)))
	{
		ep_dbg_log(EP_DBG_2"P - UMEA Rep: Not enough space!\n");
		return -1;
	}

	*nof_meas = ntohl(rep->nof_meas);

	ep_dbg_dump("P - UMEA Rep: ", buf, size);

	if(ues) {
		for(i = 0; i < *nof_meas && i < max; i++) {
			ues[i].meas_id = det[i].meas_id;
			ues[i].pci     = ntohs(det[i].pci);
			ues[i].rsrp    = ntohs(det[i].rsrp);
			ues[i].rsrq    = ntohs(det[i].rsrq);

			ep_dbg_dump(
				EP_DBG_3"P - UMEA Det: ",
				(char *)(ues + i),
				sizeof(ep_uemeas_det));
		}
	}

	return EP_SUCCESS;
}

int epf_uemeas_req(
	char *        buf,
	unsigned int  size,
	uint8_t       meas_id,
	uint16_t      rnti,
	uint16_t      earfcn,
	uint16_t      interval,
	int16_t       max_cells,
	int16_t       max_meas)
{
	ep_uemeas_req * req = (ep_uemeas_req *)buf;

	if(size < sizeof(ep_uemeas_req) ) {
		ep_dbg_log(EP_DBG_2"F - UMEA Req: Not enough space!\n");
		return -1;
	}

	req->meas_id   = meas_id;
	req->rnti      = htons(rnti);
	req->earfcn    = htons(earfcn);
	req->interval  = htons(interval);
	req->max_cells = htons(max_cells);
	req->max_meas  = htons(max_meas);

	ep_dbg_dump(EP_DBG_2"F - UMEA Req: ", buf, sizeof(ep_uemeas_req));

	return sizeof(ep_uemeas_req);
}

int epp_uemeas_req(
	char * buf, unsigned int size,
	uint8_t  * meas_id,
	uint16_t * rnti,
	uint16_t * earfcn,
	uint16_t * interval,
	int16_t  * max_cells,
	int16_t  * max_meas)
{
	ep_uemeas_req * req = (ep_uemeas_req *) buf;

	if(size < sizeof(ep_uemeas_req) ) {
		ep_dbg_log(EP_DBG_2"P - UMEA Req: Not enough space!\n");
		return -1;
	}

	if(meas_id) {
		*meas_id = req->meas_id;
	}

	if(rnti) {
		*rnti = ntohs(req->rnti);
	}

	if(earfcn) {
		*earfcn = ntohs(req->earfcn);
	}

	if(interval) {
		*interval = ntohs(req->interval);
	}

	if(max_cells) {
		*max_cells = ntohs(req->max_cells);
	}

	if(max_meas) {
		*max_meas = ntohs(req->max_meas);
	}

	ep_dbg_dump(EP_DBG_2"P - UMEA Req: ", buf, sizeof(ep_uemeas_req));

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_uemeas_rep_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Trigger UMEA Fail: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_MEASURE,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uemeas_rep(buf + ret, size - ret, 0, 0, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_trigger_uemeas_rep(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	uint32_t        nof_meas,
	uint32_t        max,
	ep_ue_measure * ues)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Trigger UMEA Rep: Invalid buffer!\n");
		return -1;
	}

	if(nof_meas > 0 && !ues) {
		ep_dbg_log(EP_DBG_0"F - Trigger UMEA Rep: Invalid UEs!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_MEASURE,
		EP_OPERATION_SUCCESS);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uemeas_rep(buf + ret, size - ret, nof_meas, max, ues);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max,
	ep_ue_measure * ues)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Trigger UMEA Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_uemeas_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		nof_ues,
		max,
		ues);
}

int epf_trigger_uemeas_req(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id,
	ep_op_type    op,
	uint8_t       meas_id,
	uint16_t      rnti,
	uint16_t      earfcn,
	uint16_t      interval,
	int16_t       max_cells,
	int16_t       max_meas)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Trigger UMEA Req: Invalid buffer!\n");
		return -1;
	}

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REQ);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_MEASURE,
		op);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uemeas_req(
		buf + ret,
		size - ret,
		meas_id,
		rnti,
		earfcn,
		interval,
		max_cells,
		max_meas);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_uemeas_req(
	char *       buf,
	unsigned int size,
	uint8_t  *   meas_id,
	uint16_t *   rnti,
	uint16_t *   earfcn,
	uint16_t *   interval,
	int16_t  *   max_cells,
	int16_t  *   max_meas)
{

	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Trigger UMEA Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_uemeas_req(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		meas_id,
		rnti,
		earfcn,
		interval,
		max_cells,
		max_meas);
}
