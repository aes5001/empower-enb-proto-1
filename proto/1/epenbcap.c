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
	char *        buf, 
	unsigned int  size,
	ep_enb_det *  det)
{
	char *        c   = buf + sizeof(ep_ecap_rep);
	int           i   = 0;
	ep_ecap_rep * rep = (ep_ecap_rep *)buf;
	ep_ccap_TLV * ctlv;

	if(size < sizeof(ep_ecap_rep)) {
		ep_dbg_log("F - ECAP Rep: Not enough space!\n");
		return -1;
	}

	rep->cap       = htonl(det->capmask);

	ep_dbg_dump("F - ECAP Rep: ", buf, sizeof(ep_ecap_rep));

	for(i = 0; i < det->nof_cells && i < EP_ECAP_CELL_MAX; i++) {
		if(c + sizeof(ep_ccap_TLV) > buf + size) {
			ep_dbg_log("F - ECAP Rep: Not enough space!\n");
			return -1;
		}

		ctlv = (ep_ccap_TLV *)c;

		ctlv->header.type   = htons(EP_TLV_CELL_CAP);
		ctlv->header.length = htons(sizeof(ep_ccap_rep));

		ctlv->body.pci      = htons(det->cells[i].pci);
		ctlv->body.DL_earfcn= htons(det->cells[i].DL_earfcn);
		ctlv->body.DL_prbs  = det->cells[i].DL_prbs;
		ctlv->body.UL_earfcn= htons(det->cells[i].UL_earfcn);
		ctlv->body.UL_prbs  = det->cells[i].UL_prbs;

		ep_dbg_dump("F - CCAP TLV: ", c, sizeof(ep_ccap_TLV));

		/* Point to the next token */
		c += sizeof(ep_ccap_TLV);
	}

	return sizeof(ep_ecap_rep) + (sizeof(ep_ccap_TLV) * i);
}

/* Parse a single TLV field.
 *
 * It assumes that the checks over the size have already been done by the 
 * caller, and the area of memory is fine to access.
 */
int epp_ecap_single_TLV(char * buf, ep_enb_det * det)
{
	ep_TLV *      tlv = (ep_TLV *)buf;

	ep_ccap_rep * ccap;

	/* Decide what to do depending on the TLV type */
	switch(ntohs(tlv->type)) {
	case EP_TLV_CELL_CAP:
		/* No more cell than this */
		if(det->nof_cells >= EP_ECAP_CELL_MAX) {
			ep_dbg_log("P - CCAP TLV: Hitting max cells limit!\n");
			break;
		}

		/* Points to the TLV body */
		ccap = (ep_ccap_rep *)(buf + sizeof(ep_TLV));

		det->cells[det->nof_cells].pci       = ntohs(ccap->pci);
		det->cells[det->nof_cells].DL_earfcn = ntohs(ccap->DL_earfcn);
		det->cells[det->nof_cells].DL_prbs   = ccap->DL_prbs;
		det->cells[det->nof_cells].UL_earfcn = ntohs(ccap->UL_earfcn);
		det->cells[det->nof_cells].UL_prbs   = ccap->UL_prbs;

		/* Increase the value to use as index and counter */
		det->nof_cells++;

		ep_dbg_dump("P - CCAP TLV: ", buf, sizeof(ep_ccap_TLV));

		break;
	default:
		ep_dbg_log("P - ECAP Rep: Unknwon token %d!\n",
			tlv->type);
		break;
	}

	return EP_SUCCESS;
}

int epp_ecap_rep(
	char *        buf, 
	unsigned int  size,
	ep_enb_det *  det)
{
	char *        c   = buf + sizeof(ep_ecap_rep);
	ep_ecap_rep * rep = (ep_ecap_rep *)buf;
	ep_TLV *      tlv;

	if(size < sizeof(ep_ecap_rep)) {
		ep_dbg_log("P - ECAP Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(!det) {
		ep_dbg_log("P - ECAP Rep: Invalid pointer!\n");
		return EP_ERROR;
	}

	det->capmask = ntohl(rep->cap);

	ep_dbg_dump("P - ECAP Rep: ", buf, sizeof(ep_ecap_rep));
	
	/* We need this set to a correct value */
	det->nof_cells = 0;

	/* Continue until the end of the given array */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) >= buf + size) {
			ep_dbg_log("P - ECAP Rep: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + tlv->length,
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_ecap_single_TLV(c, det);

		/* To the next tag */
		c += sizeof(ep_TLV) + ntohs(tlv->length);
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
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id)
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
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_ECAP,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ecap_rep(buf + ret, size - ret, 0);

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
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id,
	ep_enb_det *  det)
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
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_ECAP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ecap_rep(buf + ret, size - ret, det);

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
	ep_enb_det *  det)
{
	if(!buf) {
		ep_dbg_log("P - Single ECAP Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ecap_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		det);
}

int epf_single_ecap_req(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id)
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
		mod_id,
		EP_HDR_FLAG_DIR_REQ);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_ECAP,
		EP_OPERATION_UNSPECIFIED);

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
