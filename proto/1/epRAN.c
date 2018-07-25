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

#include <string.h>
#include <netinet/in.h>

#include <emproto.h>

#define min(a,b)	(a < b ? a : b)

/*
 * 
 * Parser setup primitives for RAN:
 * 
 */

/* Format EUQ, sEtup Unspecified reQuest. 
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_euq(char * buf, unsigned int size)
{
	/*
	 * Currently the RAN setup request has no body 
	 */

	ep_dbg_dump(EP_DBG_2"F - RANS Rep: ", buf, 0);

	return 0;
}

/* Parse EUQ, sEtup Unspecified reQuest. 
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_euq(char * buf, unsigned int size)
{
	/*
	 * Currently the RAN setup request has no body 
	 */

	ep_dbg_dump(EP_DBG_2"P - RANS Rep: ", buf, 0);

	return EP_SUCCESS;
}

/* Format EPQ, sEtup Unspecified rePly.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_eup(char * buf, unsigned int size, ep_ran_det * det)
{
	ep_ran_setup * s = (ep_ran_setup *)buf;

	/* Keeps the most updated current buf position */
	char *         c = buf;

	/* Possible info that can be parsed in TLV style */
	ep_ran_mac_sched_TLV * macs;

	if(size < sizeof(ep_ran_setup)) {
		ep_dbg_log(EP_DBG_2"F - RANS Rep: Not enough space!\n");
		return -1;
	}

	s->layer1_cap = htonl(det->l1_mask);
	s->layer2_cap = htonl(det->l2_mask);
	s->layer3_cap = htonl(det->l3_mask);

	c += sizeof(ep_ran_setup);

	/* TLV for MAC scheduler information */
	if(det->l2.mac.slice_sched != EP_RAN_SCHED_INVALID) {
		if((c - buf) + sizeof(ep_ran_mac_sched_TLV) > size) {
			ep_dbg_log(EP_DBG_3"F - RANS Rep: Not enough space!\n");
			return -1;
		}

		macs  = (ep_ran_mac_sched_TLV *)c;

		macs->header.type      = htons(EP_TLV_RAN_MAC_SCHED);
		macs->header.length    = htons(sizeof(ep_ran_mac_sched));
		macs->body.slice_sched = htonl(det->l2.mac.slice_sched);

		c += sizeof(ep_ran_mac_sched_TLV);
	}

	ep_dbg_dump(EP_DBG_2"F - RANS Rep: ", buf, c - buf);

	return c - buf;
}

/* Parse EPQ, sEtup Unspecified rePLy, TLV entries. 
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_eup_TLV(char * buf, ep_ran_det * det)
{
	ep_TLV *           tlv = (ep_TLV *)buf;

	ep_ran_mac_sched * macs;

	/* Decide what to do depending on the TLV type */
	switch(ntohs(tlv->type)) {
	case EP_TLV_RAN_MAC_SCHED:
		/* Points to the TLV body */
		macs = (ep_ran_mac_sched *)(buf + sizeof(ep_TLV));
		det->l2.mac.slice_sched = ntohl(macs->slice_sched);
		
		ep_dbg_dump(EP_DBG_3"P - RANS TLV: ", 
			buf, sizeof(ep_ran_mac_sched_TLV));

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - RANS TLV: Unexpected token %d!\n", tlv->type);
		break;
	}

	return EP_SUCCESS;
}

/* Parse EPQ, sEtup Unspecified rePLy. 
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_eup(char * buf, unsigned int size, ep_ran_det * det)
{
	char *         c;
	ep_ran_setup * s = (ep_ran_setup *)buf;
	ep_TLV *       tlv;

	if(size < sizeof(ep_ran_setup)) {
		ep_dbg_log(EP_DBG_2"P - RANS Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(!det) {
		return EP_SUCCESS;
	}

	det->l1_mask = ntohl(s->layer1_cap);
	det->l2_mask = ntohl(s->layer2_cap);
	det->l3_mask = ntohl(s->layer3_cap);

	ep_dbg_dump(EP_DBG_2"P - RANS Rep: ", buf, sizeof(ep_ran_setup));

	c = buf + sizeof(ep_ran_setup);

	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) >= buf + size) {
			ep_dbg_log(EP_DBG_3"P - RANS Rep: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + tlv->length,
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_ran_eup_TLV(c, det);

		c  += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/*
 * 
 * Parser slice primitives for RAN:
 * 
 */

/* Format SUQ, Slice Unspecified reQuest 
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_suq(char * buf, unsigned int size, slice_id_t id)
{
	ep_ran_sreq * r = (ep_ran_sreq *)buf;

	if(size < sizeof(ep_ran_sreq)) {
		ep_dbg_log(EP_DBG_2"F - RANS Unspec Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Unspec Req: ", buf, sizeof(ep_ran_sreq));

	return sizeof(ep_ran_sreq);
}

/* Parse SUQ, Slice Unspecified reQuest 
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_suq(char * buf, unsigned int size, slice_id_t * id)
{
	ep_ran_sreq * r = (ep_ran_sreq *)buf;

	if(size < sizeof(ep_ran_sreq)) {
		ep_dbg_log(EP_DBG_2"P - RANS Unspec Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Unspec Req: ", buf, sizeof(ep_ran_sreq));

	return EP_SUCCESS;
}

/* Format SUP, Slice Unspecified rePly.
 * This message formats a single slice with its details.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_sup(
	char * buf, unsigned int size, uint16_t id, ep_ran_slice_det * det) 
{
	int           l = sizeof(ep_ran_srep) + (sizeof(ep_ran_sinf));

	ep_ran_srep * r = (ep_ran_srep *) buf;
	ep_ran_sinf * d = (ep_ran_sinf *)(buf + sizeof(ep_ran_srep));
	
	ep_ran_smac_TLV * smac;

	if(size < l) {
		ep_dbg_log(EP_DBG_2"F - RANS Unspec Rep: Not enough space!\n");
		return -1;
	}

	r->nof_slices = htons(1);
	
	ep_dbg_dump(EP_DBG_2"F - RANS Unspec Rep: ", buf, sizeof(ep_ran_srep));

	d->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Unspec Info: ",
		(char *)d, sizeof(ep_ran_sinf));
	
	/* Time to check if to create TLVs for additional options */
	if(det) {
		smac = (ep_ran_smac_TLV *)(buf + l);
		l   += sizeof(ep_ran_smac_TLV);

		if(size < l) {
			ep_dbg_log(EP_DBG_3
				"F - RANS Unspec Rep: Not enough space!\n");
			return -1;
		}

		smac->header.type    = htons(EP_TLV_RAN_SLICE_MAC);
		smac->header.length  = htons(sizeof(ep_ran_smac));
		smac->body.prbs      = htons(det->l2.prbs);
		smac->body.user_sched= htonl(det->l2.usched);
	}

	return sizeof(ep_ran_srep) + sizeof(ep_ran_sinf);
}


/* Parse a single TLV field for Slice Unspecified Reply.
 *
 * It assumes that the checks over the size have already been done by the 
 * caller, and the area of memory is fine to access.
 */
int epp_ran_sup_TLV(char * buf, ep_ran_slice_det * det)
{
	ep_TLV *          tlv = (ep_TLV *)buf;

	ep_ran_smac_TLV * macs;

	/* Decide what to do depending on the TLV type */
	switch(ntohs(tlv->type)) {
	case EP_TLV_RAN_SLICE_MAC:
		/* Points to the TLV body */
		macs = (ep_ran_smac_TLV *)(buf);

		det->l2.prbs   = ntohs(macs->body.prbs);
		det->l2.usched = ntohl(macs->body.user_sched);

		ep_dbg_dump(EP_DBG_3"P - RANS Unspec Rep TLV: ", 
			buf, sizeof(ep_ccap_TLV));

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - RANS Unspec Rep: Unexpected TLV %d!\n", 
			tlv->type);
		break;
	}

	return EP_SUCCESS;
}


/* Parses SUP, Slice Unspecified rePly.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_sup(
	char * buf, unsigned int size, slice_id_t * id, ep_ran_slice_det * det)
{
	int           len = sizeof(ep_ran_srep) + sizeof(ep_ran_sinf);

	char *        c = buf + len;
	ep_ran_srep * r = (ep_ran_srep *) buf;
	ep_ran_sinf * d = (ep_ran_sinf *)(buf + sizeof(ep_ran_srep));
	ep_TLV *      tlv;

	if(size < len) {
		ep_dbg_log(EP_DBG_2"P - RANS Unspec Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(ntohs(r->nof_slices) != 1) {
		ep_dbg_log(EP_DBG_2"P - RANS Unspec Rep: Expected 1 slice!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(d->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Unspec Rep: ", buf, len);

	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) >= buf + size) {
			ep_dbg_log(EP_DBG_3"P - RANS Unspec Rep: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + tlv->length,
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_ran_sup_TLV(c, det);

		/* To the next tag */
		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/* Format SUP, Slice Unspecified rePly.
 * This message contains multiple slices IDs, but no specific information.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_sup_m(char * buf, unsigned int size, uint16_t nof, slice_id_t * ids) 
{
	uint16_t      i;

	ep_ran_srep * r = (ep_ran_srep *) buf;
	ep_ran_sinf * d = (ep_ran_sinf *)(buf + sizeof(ep_ran_srep));

	if(size < sizeof(ep_ran_srep) + (sizeof(ep_ran_sinf) * nof)) {
		ep_dbg_log(EP_DBG_2"F - RANS Unspec Rep: Not enough space!\n");
		return -1;
	}

	r->nof_slices = htons(nof);
	
	ep_dbg_dump(EP_DBG_2"F - RANS Unspec Rep: ", buf, sizeof(ep_ran_srep));

	for (i = 0; i < nof && i < EP_RAN_SLICE_MAX; i++) {
		d[i].id = htobe64(ids[i]);

		ep_dbg_dump(
			EP_DBG_3"F - RANS Unspec Info: ",
			buf + sizeof(ep_ran_srep) + (sizeof(ep_ran_sinf) * i),
			sizeof(ep_ran_sinf));
	}

	return sizeof(ep_ran_srep) + (sizeof(ep_ran_sinf) * i);
}

/* Parses SUP, Slice Unspecified rePly.
 * This message contains multiple slices IDs, but no specific information.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_sup_m(
	char * buf, unsigned int size, uint16_t * nof, slice_id_t * ids)
{
	uint16_t      i;
	uint16_t      n;

	ep_ran_srep * r = (ep_ran_srep *) buf;
	ep_ran_sinf * d = (ep_ran_sinf *)(buf + sizeof(ep_ran_srep));

	if(size < sizeof(ep_ran_srep)) {
		ep_dbg_log(EP_DBG_2"P - RANT Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_ran_srep) + (
		sizeof(ep_ran_sinf) * ntohs(r->nof_slices)))
	{
		ep_dbg_log(EP_DBG_2"P - RANT Rep: Not enough space!\n");
		return EP_ERROR;
	}

	n = ntohs(r->nof_slices);

	if(nof) {
		*nof = n;
	}

	ep_dbg_dump(EP_DBG_2"P - RANT Rep: ", buf, sizeof(ep_ran_srep));

	if(ids) {
		for (i = 0; i < n && i < EP_RAN_SLICE_MAX; i++) {
			ids[i] = be64toh(d[i].id);

			ep_dbg_dump(
				EP_DBG_3"P - RANT Info: ",
				buf + 
					sizeof(ep_ran_srep) +
					(sizeof(ep_ran_sinf) * i),
				sizeof(ep_ran_sinf));
		}
	}

	return EP_SUCCESS;
}

/* Format SAQ, Slice Add reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_saq(
	char * buf, unsigned int size, slice_id_t id, ep_ran_slice_det * det)
{
	int               len = sizeof(ep_ran_sinf);
	ep_ran_sinf *     r   = (ep_ran_sinf *)buf;
	ep_ran_smac_TLV * smac;

	if(size < len) {
		ep_dbg_log(EP_DBG_2"F - RANS Add Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Add Req: ", buf, len);

	if(det) {
		if(det->l2.usched == 0) {
			goto end;
		}

		smac = (ep_ran_smac_TLV *)(buf + len);
		len += sizeof(ep_ran_smac_TLV);

		smac->header.type     = EP_TLV_RAN_SLICE_MAC;
		smac->header.length   = htons(sizeof(ep_ran_smac_TLV));
		smac->body.prbs       = htons(det->l2.prbs);
		smac->body.user_sched = htonl(det->l2.usched);
	}
end:
	return len;
}

/* Parse a single TLV field for Slice Add Request.
 *
 * It assumes that the checks over the size have already been done by the 
 * caller, and the area of memory is fine to access.
 */
int epp_ran_saq_TLV(char * buf, ep_ran_slice_det * det)
{
	ep_TLV *          tlv = (ep_TLV *)buf;

	ep_ran_smac_TLV * smac;

	/* Decide what to do depending on the TLV type */
	switch(ntohs(tlv->type)) {
	case EP_TLV_RAN_SLICE_MAC:
		smac = (ep_ran_smac_TLV *)buf;

		det->l2.prbs   = ntohs(smac->body.prbs);
		det->l2.usched = ntohs(smac->body.user_sched);

		ep_dbg_dump(EP_DBG_3"P - RANS Add Req TLV: ", 
			buf, 
			sizeof(ep_ccap_TLV));

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - RANS Add Req: Unexpected token %d!\n", 
			tlv->type);
		break;
	}

	return EP_SUCCESS;
}

/* Parse SAQ, Slice Add reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_saq(
	char * buf, unsigned int size, slice_id_t * id, ep_ran_slice_det * det)
{
	char *        c = buf + sizeof(ep_ran_sinf);
	ep_ran_sinf * r = (ep_ran_sinf *)buf;
	ep_TLV *      tlv;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"P - RANS Add Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Add: ", buf, sizeof(ep_ran_sinf));

	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) >= buf + size) {
			ep_dbg_log(EP_DBG_3"P - RANS Add Req: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + tlv->length,
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_ran_saq_TLV(c, det);

		/* To the next tag */
		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/* Format SRQ, Slice Rem reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_srq(char * buf, unsigned int size, slice_id_t id) 
{
	ep_ran_sinf * r = (ep_ran_sinf *)buf;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"F - RANS Rem Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Rem Req: ", buf, sizeof(ep_ran_sinf));

	return sizeof(ep_ran_sinf);
}

/* Parse SRQ, Slice Rem reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_srq(char * buf, unsigned int size, slice_id_t * id)
{
	ep_ran_sinf * r = (ep_ran_sinf *)buf;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"P - RANS Rem: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Rem Req: ", buf, sizeof(ep_ran_sinf));

	return EP_SUCCESS;
}

/* Format SSQ, Slice Set reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_ssq(
	char * buf, unsigned int size, slice_id_t id, ep_ran_slice_det * det) 
{
	int               len = sizeof(ep_ran_sinf);
	ep_ran_sinf *     r   = (ep_ran_sinf *)buf;

	ep_ran_smac_TLV * smac;

	if(size < len) {
		ep_dbg_log(EP_DBG_2"F - RANS Set Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Set Req: ", buf, sizeof(ep_ran_sinf));

	if(det) {
		if(det->l2.usched == 0) {
			goto end;
		}

		smac = (ep_ran_smac_TLV *)(buf + len);
		len += sizeof(ep_ran_smac_TLV);
		
		if(size < len) {
			ep_dbg_log(EP_DBG_3
				"F - RANS Set Req TLV: Not enough space!\n");

			return -1;
		}

		smac->header.type     = EP_TLV_RAN_SLICE_MAC;
		smac->header.length   = htons(sizeof(ep_ran_smac_TLV));
		smac->body.prbs       = det->l2.prbs;
		smac->body.user_sched = det->l2.usched;

		ep_dbg_dump(EP_DBG_3"F - RANS Set Req TLV: ", 
			(char *)smac, sizeof(ep_ran_smac_TLV));
	}
end:
	return len;
}

/* Parse SRQ, Slice Set reQuest, TLV entry.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_ssq_TLV(char * buf, ep_ran_slice_det * det)
{
	ep_TLV *          tlv = (ep_TLV *)buf;

	ep_ran_smac_TLV * smac;

	/* Decide what to do depending on the TLV type */
	switch(ntohs(tlv->type)) {
	case EP_TLV_RAN_SLICE_MAC:
		smac = (ep_ran_smac_TLV *)buf;

		det->l2.prbs   = ntohs(smac->body.prbs);
		det->l2.usched = ntohs(smac->body.user_sched);

		ep_dbg_dump(EP_DBG_3"P - RANS Set Req: ", 
			buf, sizeof(ep_ccap_TLV));

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - RANS Set Req: Unexpected token %d!\n", 
			tlv->type);
		break;
	}

	return EP_SUCCESS;
}

/* Parse SRQ, Slice Set reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_ssq(
	char * buf, unsigned int size, slice_id_t * id, ep_ran_slice_det * det)
{
	char *        c   = buf + sizeof(ep_ran_sinf);
	ep_ran_sinf * r   = (ep_ran_sinf *)buf;
	ep_TLV *      tlv;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"P - RANS Set Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Set Req: ", buf, sizeof(ep_ran_sinf));

	/* Continue until the end of the given array */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) >= buf + size) {
			ep_dbg_log(EP_DBG_3"P - RANS Set Req:: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + tlv->length,
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_ran_ssq_TLV(c, det);

		/* To the next tag */
		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/*
 * 
 * Parser user primitives for RAN:
 * 
 */

/* Format UUQ, User Unspecified reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_uuq(char * buf, unsigned int size, rnti_id_t rnti)
{
	ep_ran_ureq * r = (ep_ran_ureq *)buf;

	if(size < sizeof(ep_ran_ureq)) {
		ep_dbg_log(EP_DBG_2"F - RANU Req: Not enough space!\n");
		return -1;
	}

	r->rnti = htons(rnti);

	ep_dbg_dump(EP_DBG_2"F - RANU Req: ", buf, sizeof(ep_ran_ureq));

	return sizeof(ep_ran_ureq);
}

/* Parse UUQ, User Unspecified reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_uuq(char * buf, unsigned int size, uint16_t * rnti)
{
	ep_ran_ureq * r = (ep_ran_ureq *)buf;

	if(size < sizeof(ep_ran_ureq)) {
		ep_dbg_log(EP_DBG_2"P - RANU Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(rnti) {
		*rnti = ntohs(r->rnti);
	}

	ep_dbg_dump(EP_DBG_2"P - RANU Req: ", buf, sizeof(ep_ran_ureq));

	return sizeof(ep_ran_ureq);
}

/* Format UUP, User Unspecified rePly.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_uup(
	char * buf, unsigned int size, uint16_t nof, ep_ran_user_det * det)
{
	uint16_t      i;
	ep_ran_urep * r = (ep_ran_urep *) buf;
	ep_ran_uinf * u = (ep_ran_uinf *)(buf + sizeof(ep_ran_urep));

	if(size < sizeof(ep_ran_ureq) + (sizeof(ep_ran_uinf) * nof)) {
		ep_dbg_log(EP_DBG_2"F - RANU Rep: Not enough space!\n");
		return -1;
	}

	r->nof_users = htons(nof);

	ep_dbg_dump(EP_DBG_2"F - RANU Rep: ", buf, sizeof(ep_ran_urep));

	for (i = 0; i < nof && i < EP_RAN_USER_MAX; i++) {
		u[i].rnti     = htons(det[i].id);
		u[i].slice_id = htobe64(det[i].slice);

		ep_dbg_dump(EP_DBG_3"F - RANU Info: ",
			buf + sizeof(ep_ran_urep)  + (sizeof(ep_ran_uinf) * i),
			sizeof(ep_ran_uinf));
	}

	return sizeof(ep_ran_ureq) + (sizeof(ep_ran_uinf) * (i + 1));
}

/* Parse UUP, User Unspecified rePly.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_uup(
	char * buf, unsigned int size, uint16_t * nof, ep_ran_user_det * det)
{
	uint16_t      i;
	ep_ran_urep * r = (ep_ran_urep *) buf;
	ep_ran_uinf * u = (ep_ran_uinf *)(buf + sizeof(ep_ran_urep));

	if(size < sizeof(ep_ran_ureq)) {
		ep_dbg_log(EP_DBG_2"P - RANU Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_ran_ureq) + (
		sizeof(ep_ran_uinf) * ntohs(r->nof_users)))
	{
		ep_dbg_log(EP_DBG_2"P - RANU Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(nof) {
		*nof = ntohs(r->nof_users);
	}

	ep_dbg_dump(EP_DBG_2"P - RANU Rep: ", buf, sizeof(ep_ran_urep));

	if(det) {
		for (i = 0; i < *nof && i < EP_RAN_USER_MAX; i++) {
			det[i].id     = ntohs(u[i].rnti);
			det[i].slice = be64toh(u[i].slice_id);

			ep_dbg_dump(EP_DBG_3"P - RANU Info: ",
				buf + sizeof(ep_ran_urep)  + (
					sizeof(ep_ran_uinf) * i),
				sizeof(ep_ran_uinf));
		}
	}

	return EP_SUCCESS;
}

/* Format UAQ, User Add reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_uaq(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	if(size < sizeof(ep_ran_uinf)) {
		ep_dbg_log(EP_DBG_2"F - RANU Add: Not enough space!\n");
		return -1;
	}

	r->rnti     = htons(det->id);
	r->slice_id = htobe64(det->slice);

	ep_dbg_dump(EP_DBG_2"F - RANU Add: ", buf, sizeof(ep_ran_uinf));

	return sizeof(ep_ran_uinf);
}

/* Parse UAQ, User Add reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_uaq(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	if(size < sizeof(ep_ran_uinf)) {
		ep_dbg_log(EP_DBG_2"P - RANU Add: Not enough space!\n");
		return EP_ERROR;
	}

	if(det) {
		det->id    = ntohs(r->rnti);
		det->slice = be64toh(det->slice);
	}

	ep_dbg_dump(EP_DBG_2"P - RANU Add: ", buf, sizeof(ep_ran_uinf));

	return EP_SUCCESS;
}

/* Format URQ, User Rem reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_urq(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	if(size < sizeof(ep_ran_uinf)) {
		ep_dbg_log(EP_DBG_2"F - RANU Rem: Not enough space!\n");
		return -1;
	}

	r->rnti      = htons(det->id);
	r->slice_id = htobe64(det->slice);

	ep_dbg_dump(EP_DBG_2"F - RANU Rem: ", buf, sizeof(ep_ran_uinf));

	return sizeof(ep_ran_uinf);
}

/* Parse URQ, User Rem reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_urq(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	if(size < sizeof(ep_ran_uinf)) {
		ep_dbg_log(EP_DBG_2"P - RANU Rem: Not enough space!\n");
		return EP_ERROR;
	}

	if(det) {
		det->id     = ntohs(r->rnti);
		det->slice = be64toh(det->slice);
	}

	ep_dbg_dump(EP_DBG_2"P - RANU Rem: ", buf, sizeof(ep_ran_uinf));

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ran_rep_fail(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;
	/* Check of given buffer size here */

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RAN Fail: Invalid buffer!\n");
		return -1;
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
		type,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_single_ran_rep_ns(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0 ;
	/* Check of given buffer size here */

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RAN NS: Invalid buffer!\n");
		return -1;
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
		type,
		EP_OPERATION_NOT_SUPPORTED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

/*
 * 
 * Parser public procedures for RAN Setup messages
 * 
 */

int epf_single_ran_setup_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0 ;

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RANS Req: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_SETUP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_euq(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_setup_req(
	char *       buf,
	unsigned int size)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RAN Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_euq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)));
}

int epf_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	ep_ran_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log(EP_DBG_2"F - Single RAN Rep: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_SETUP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_eup(
		buf + ret,
		size - ret,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	ep_ran_det * det)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RAN Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_eup(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

/*
 * 
 * Parser public procedures for RAN Slicing messages
 * 
 */

int epf_single_ran_slice_req(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Req: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_suq(
		buf + ret,
		size - ret,
		slice_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_req(
	char *       buf,
	unsigned int size,
	slice_id_t * slice_id)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RANT Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_suq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		slice_id);
}

int epf_single_ran_multi_slice_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	uint16_t           nof_slices,
	slice_id_t *       slices)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !slices) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Rep: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_sup_m(
		buf + ret,
		size - ret,
		nof_slices,
		slices);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_multi_slice_rep(
	char *             buf,
	unsigned int       size,
	uint16_t *         nof_slices,
	slice_id_t *       slices)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RANT Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_sup_m(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		nof_slices,
		slices);
}

int epf_single_ran_slice_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Rep: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_sup(
		buf + ret,
		size - ret,
		slice_id,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_rep(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RANT Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_sup(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		slice_id,
		det);
}

int epf_single_ran_slice_add(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Add: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_ADD);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_saq(
		buf + ret, 
		size - ret, 
		slice_id, 
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_add(
	char *             buf,
	unsigned int       size, 
	slice_id_t *       slice_id,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RANT Add: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_saq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		slice_id,
		det);
}

int epf_single_ran_slice_rem(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single RANT Rem: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_REM);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_srq(
		buf + ret, 
		size - ret, 
		slice_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_rem(
	char *             buf,
	unsigned int       size,
	slice_id_t *       det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANT Rem: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_srq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

int epf_single_ran_slice_set(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Rem: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_SET);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_ssq(
		buf + ret, 
		size - ret, 
		slice_id,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_set(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANT Rem: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_ssq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		slice_id,
		det);
}

/******* RAN Users ************************************************************/

int epf_single_ran_usr_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	rnti_id_t    rnti)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single RANU Req: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_USER,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_uuq(
		buf + ret,
		size - ret,
		rnti);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_usr_req(
	char *       buf,
	unsigned int size,
	rnti_id_t *  rnti)
{
	if(!buf) {
		ep_dbg_log("P - Single RANU Req: Invalid buffer!\n");
		return -1;
	}

	return epp_ran_uuq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		rnti);
}

int epf_single_ran_usr_rep(
	char *            buf,
	unsigned int      size,
	enb_id_t          enb_id,
	cell_id_t         cell_id,
	mod_id_t          mod_id,
	uint16_t          nof_users,
	ep_ran_user_det * dets)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !dets) {
		ep_dbg_log("F - Single RANU Rep: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_USER,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_uup(
		buf + ret,
		size - ret,
		nof_users,
		dets);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_usr_rep(
	char *            buf,
	unsigned int      size,
	uint16_t *        nof_users,
	ep_ran_user_det * dets)
{
	if(!buf) {
		ep_dbg_log("P - Single RANU Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_uup(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		nof_users, 
		dets);
}

int epf_single_ran_usr_add(
	char *            buf,
	unsigned int      size,
	enb_id_t          enb_id,
	cell_id_t         cell_id,
	mod_id_t          mod_id,
	ep_ran_user_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single RANU Add: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_USER,
		EP_OPERATION_ADD);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_uaq(
		buf + ret,
		size - ret,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_usr_add(
	char *            buf,
	unsigned int      size,
	ep_ran_user_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANU Add: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_uaq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

int epf_single_ran_usr_rem(
	char *            buf,
	unsigned int      size,
	enb_id_t          enb_id,
	cell_id_t         cell_id,
	mod_id_t          mod_id,
	ep_ran_user_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single RANU Rem: Invalid buffer!\n");
		return -1;
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
		EP_ACT_RAN_USER,
		EP_OPERATION_REM);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_urq(
		buf + ret,
		size - ret,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_usr_rem(
	char *            buf,
	unsigned int      size, 
	ep_ran_user_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANU Rem: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_urq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}
