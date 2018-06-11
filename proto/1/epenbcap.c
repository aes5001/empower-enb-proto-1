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

int epf_ecap_rep(
	char * buf, unsigned int size,
	uint32_t      cap,
	ep_cell_det * cells,
	uint32_t      nof_cells)
{
	int           i   = 0;
	ep_ecap_rep * rep = (ep_ecap_rep *)buf;
	ep_ccap_rep * cel = (ep_ccap_rep *)(buf + sizeof(ep_ecap_rep));

	if(size < sizeof(ep_ecap_rep) + (sizeof(ep_ccap_rep) * nof_cells)) {
		ep_dbg_log("F - ECAP Rep: Not enough space!\n");
		return -1;
	}

	rep->cap       = htonl(cap);
	rep->nof_cells = htonl(nof_cells);

	ep_dbg_dump("F - ECAP Rep: ", buf, sizeof(ep_ecap_rep));

	for(i = 0; i < nof_cells && i < EP_ENCAP_MAX_CELLS; i++) {
		cel[i].pci       = htons(cells[i].pci);
		cel[i].DL_earfcn = htons(cells[i].DL_earfcn);
		cel[i].DL_prbs   = cells[i].DL_prbs;
		cel[i].UL_earfcn = htons(cells[i].UL_earfcn);
		cel[i].UL_prbs   = cells[i].UL_prbs;

		ep_dbg_dump(
			"F - CCAP Rep: ",
			buf + sizeof(ep_ecap_rep) + (sizeof(ep_ccap_rep) * i),
			sizeof(ep_ccap_rep));
	}

	return sizeof(ep_ecap_rep) + (sizeof(ep_ccap_rep) * i);
}

int epp_ecap_rep(
	char * buf, unsigned int size,
	uint32_t *    cap,
	ep_cell_det * cells,
	uint32_t *    nof_cells)
{
	int           i   = 0;
	ep_ecap_rep * rep = (ep_ecap_rep *)buf;
	ep_ccap_rep * cel = (ep_ccap_rep *)buf + sizeof(ep_ecap_rep);

	if(size < sizeof(ep_ecap_rep)) {
		ep_dbg_log("P - ECAP Rep: Not enough space!\n");
		return -1;
	}

	if(size < sizeof(ep_ecap_rep) + (
		sizeof(ep_ccap_rep) * ntohl(rep->nof_cells)))
	{
		ep_dbg_log("P - ECAP Rep: Not enough space!\n");
		return -1;
	}

	if(cap) {
		*cap       = ntohl(rep->cap);
	}

	if(nof_cells) {
		*nof_cells = ntohl(rep->nof_cells);
	}

	ep_dbg_dump("P - ECAP Rep: ", buf, sizeof(ep_ecap_rep));

	if(cells) {
		for(i = 0; i < *nof_cells && i < EP_ENCAP_MAX_CELLS; i++) {
			cells[i].pci      = ntohs(cel[i].pci);
			cells[i].cap      = ntohl(cel[i].cap);
			cells[i].DL_earfcn= ntohs(cel[i].DL_earfcn);
			cells[i].DL_prbs  = cel[i].DL_prbs;
			cells[i].UL_earfcn= ntohs(cel[i].UL_earfcn);
			cells[i].UL_prbs  = cel[i].UL_prbs;

			ep_dbg_dump("P - CCAP Rep: ",
				buf + sizeof(ep_ecap_rep) + (
					sizeof(ep_ccap_rep) * i),
				sizeof(ep_ccap_rep));
		}
	}

	return EP_SUCCESS;
}

int epf_ecap_req(char * buf, unsigned int size)
{
	ep_ecap_req * rep = (ep_ecap_req *)buf;

	if(size < sizeof(ep_ecap_req)) {
		ep_dbg_log("F - ECAP Req: Not enough space!\n");
		return -1;
	}

	rep->dummy = 0;

	ep_dbg_dump("F - ECAP Req: ", buf, sizeof(ep_ecap_req));

	return sizeof(ep_ecap_req);
}

int epp_ecap_req(char * buf, unsigned int size)
{
	if(size < sizeof(ep_ecap_req)) {
		ep_dbg_log("P - ECAP Rep: Not enough space!\n");
		return -1;
	}

	ep_dbg_dump("P - ECAP Req: ", buf, 0);

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ecap_rep_fail(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single ECAP Fail: Invalid buffer!\n");
		return EP_ERROR;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_ECAP,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ecap_rep(buf + ret, size - ret, 0, 0, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id,
	uint32_t      cap_mask,
	ep_cell_det * cells,
	uint32_t      nof_cells)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single ECAP Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_ECAP,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REPLY);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ecap_rep(buf + ret, size - ret, cap_mask, cells, nof_cells);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	uint32_t *    cap_mask,
	ep_cell_det * cells,
	uint32_t *    nof_cells)
{
	if(!buf) {
		ep_dbg_log("P - Single ECAP Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ecap_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		cap_mask,
		cells,
		nof_cells);
}

int epf_single_ecap_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single ECAP Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_ECAP,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REQUEST);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ecap_req(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ecap_req(char * buf, unsigned int size)
{
	if(!buf) {
		ep_dbg_log("P - Single ECAP Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ecap_req(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size);
}
