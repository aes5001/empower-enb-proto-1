/* Copyright (c) 2017-2018 Kewin Rausch
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

/* Formats a RAN setup reply using RAN details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_setup_rep(char * buf, unsigned int size, ep_ran_det * det)
{
	ep_ran_setup * s = (ep_ran_setup *)buf;

	if(size < sizeof(ep_ran_setup)) {
		ep_dbg_log("F - RANS Rep: Not enough space!\n");
		return -1;
	}

	s->sched_id = htonl(det->slice_sched);

	ep_dbg_dump("F - RANS Rep: ", buf, sizeof(ep_ran_setup));

	return sizeof(ep_ran_setup);
}

/* Parses a RAN setup reply into RAN details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_setup_rep(char * buf, unsigned int size, ep_ran_det * det)
{
	ep_ran_setup * s = (ep_ran_setup *)buf;

	if(size < sizeof(ep_ran_setup)) {
		ep_dbg_log("P - RANS Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(det) {
		det->slice_sched = ntohl(s->sched_id);
	}

	ep_dbg_dump("P - RANS Rep: ", buf, sizeof(ep_ran_setup));

	return EP_SUCCESS;
}

/* Formats a RAN slice request using Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_slice_req(char * buf, unsigned int size, ep_ran_slice_det * det)
{
	ep_ran_treq * r = (ep_ran_treq *)buf;

	if(size < sizeof(ep_ran_treq)) {
		ep_dbg_log("F - RANT Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(det->id);

	ep_dbg_dump("F - RANT Req: ", buf, sizeof(ep_ran_treq));

	return sizeof(ep_ran_treq);
}

/* Parses a RAN slice request into Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_slice_req(char * buf, unsigned int size, ep_ran_slice_det * det)
{
	ep_ran_treq * r = (ep_ran_treq *)buf;

	if(size < sizeof(ep_ran_treq)) {
		ep_dbg_log("P - RANT Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(det) {
		det->id = be64toh(r->id);
	}

	ep_dbg_dump("P - RANT Req: ", buf, sizeof(ep_ran_treq));

	return sizeof(ep_ran_treq);
}

/* Formats a RAN slice reply using Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_slice_rep(
	char * buf, unsigned int size, uint16_t nof, ep_ran_slice_det * dets) 
{
	uint16_t i;

	ep_ran_trep * r = (ep_ran_trep *) buf;
	ep_ran_tinf * d = (ep_ran_tinf *)(buf + sizeof(ep_ran_trep));

	if(size < sizeof(ep_ran_trep) + (sizeof(ep_ran_tinf) * nof)) {
		ep_dbg_log("F - RANT Rep: Not enough space!\n");
		return -1;
	}

	r->nof_slices = htons(nof);
	
	ep_dbg_dump("F - RANT Rep: ", buf, sizeof(ep_ran_trep));

	for (i = 0; i < nof && i < EP_RAN_SLICE_MAX; i++) {
		d[i].id    = htobe64(dets[i].id);
		d[i].sched = htonl(dets[i].sched);

		ep_dbg_dump(
			"    F - RANT Info: ",
			buf + sizeof(ep_ran_trep) + (sizeof(ep_ran_tinf) * i),
			sizeof(ep_ran_tinf));
	}

	return sizeof(ep_ran_trep) + (sizeof(ep_ran_tinf) * (i + 1));
}

/* Parses a RAN slice reply into Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_slice_rep(
	char * buf, unsigned int size, uint16_t * nof, ep_ran_slice_det * dets)
{
	uint16_t i;

	ep_ran_trep * r = (ep_ran_trep *) buf;
	ep_ran_tinf * d = (ep_ran_tinf *)(buf + sizeof(ep_ran_trep));

	if(size < sizeof(ep_ran_trep)) {
		ep_dbg_log("P - RANT Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_ran_trep) + (
		sizeof(ep_ran_tinf) * ntohs(r->nof_slices)))
	{
		ep_dbg_log("P - RANT Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(nof) {
		*nof = ntohs(r->nof_slices);
	}

	ep_dbg_dump("P - RANT Rep: ", buf, sizeof(ep_ran_trep));

	if(dets) {
		for (i = 0; i < *nof && i < EP_RAN_SLICE_MAX; i++) {
			dets[i].id    = be64toh(d[i].id);
			dets[i].sched = ntohl(d[i].sched);

			ep_dbg_dump(
				"    P - RANT Info: ",
				buf + sizeof(ep_ran_trep) + (
					sizeof(ep_ran_tinf) * i),
				sizeof(ep_ran_tinf));
		}
	}

	return EP_SUCCESS;
}

/* Formats a RAN slice add using Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_slice_add(char * buf, unsigned int size, ep_ran_slice_det * det)
{
	ep_ran_tadd * r = (ep_ran_tadd *)buf;

	if(size < sizeof(ep_ran_tadd)) {
		ep_dbg_log("F - RANT Add: Not enough space!\n");
		return -1;
	}

	r->id    = htobe64(det->id);
	r->sched = htonl(det->sched);

	ep_dbg_dump("F - RANT Add: ", buf, sizeof(ep_ran_tadd));

	return sizeof(ep_ran_tadd);
}

/* Parses a RAN slice add into Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_slice_add(char * buf, unsigned int size, ep_ran_slice_det * det)
{
	ep_ran_tadd * r = (ep_ran_tadd *)buf;

	if(size < sizeof(ep_ran_tadd)) {
		ep_dbg_log("P - RANT Add: Not enough space!\n");
		return EP_ERROR;
	}

	if(det) {
		det->id    = be64toh(r->id);
		det->sched = ntohl(r->sched);
	}

	ep_dbg_dump("P - RANT Add: ", buf, sizeof(ep_ran_tadd));

	return EP_SUCCESS;
}

/* Formats a RAN slice remove using Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_slice_rem(char * buf, unsigned int size, ep_ran_slice_det * det)
{
	ep_ran_trem * r = (ep_ran_trem *)buf;

	if(size < sizeof(ep_ran_trem)) {
		ep_dbg_log("F - RANT Rem: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(det->id);

	ep_dbg_dump("F - RANT Rem: ", buf, sizeof(ep_ran_trem));

	return sizeof(ep_ran_trem);
}

/* Parses a RAN slice remove into Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_slice_rem(char * buf, unsigned int size, ep_ran_slice_det * det)
{
	ep_ran_trem * r = (ep_ran_trem *)buf;

	if(size < sizeof(ep_ran_trem)) {
		ep_dbg_log("P - RANT Rem: Not enough space!\n");
		return EP_ERROR;
	}

	if(det) {
		det->id = be64toh(r->id);
	}

	ep_dbg_dump("P - RANT Rem: ", buf, sizeof(ep_ran_trem));

	return EP_SUCCESS;
}

/* Formats a RAN user request using User details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_user_req(char * buf, unsigned int size, uint16_t rnti)
{
	ep_ran_ureq * r = (ep_ran_ureq *)buf;

	if(size < sizeof(ep_ran_ureq)) {
		ep_dbg_log("F - RANU Req: Not enough space!\n");
		return -1;
	}

	r->rnti = htons(rnti);

	ep_dbg_dump("F - RANU Req: ", buf, sizeof(ep_ran_ureq));

	return sizeof(ep_ran_ureq);
}

/* Parses a RAN user request into User details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_user_req(char * buf, unsigned int size, uint16_t * rnti)
{
	ep_ran_ureq * r = (ep_ran_ureq *)buf;

	if(size < sizeof(ep_ran_ureq)) {
		ep_dbg_log("P - RANU Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(rnti) {
		*rnti = ntohs(r->rnti);
	}

	ep_dbg_dump("P - RANU Req: ", buf, sizeof(ep_ran_ureq));

	return sizeof(ep_ran_ureq);
}

/* Formats a RAN user reply using User details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_user_rep(
	char * buf, unsigned int size, uint16_t nof, ep_ran_user_det * det)
{
	uint16_t      i;
	ep_ran_urep * r = (ep_ran_urep *) buf;
	ep_ran_uinf * u = (ep_ran_uinf *)(buf + sizeof(ep_ran_urep));

	if(size < sizeof(ep_ran_ureq) + (sizeof(ep_ran_uinf) * nof)) {
		ep_dbg_log("F - RANU Rep: Not enough space!\n");
		return -1;
	}

	r->nof_users = htons(nof);

	ep_dbg_dump("F - RANU Rep: ", buf, sizeof(ep_ran_urep));

	for (i = 0; i < nof && i < EP_RAN_USER_MAX; i++) {
		u[i].rnti      = htons(det[i].id);
		u[i].slice_id = htobe64(det[i].slice);

		ep_dbg_dump(
			"    F - RANU Info: ",
			buf + sizeof(ep_ran_urep)  + (sizeof(ep_ran_uinf) * i),
			sizeof(ep_ran_uinf));
	}

	return sizeof(ep_ran_ureq) + (sizeof(ep_ran_uinf) * (i + 1));
}

/* Parses a RAN user reply into User details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_user_rep(
	char * buf, unsigned int size, uint16_t * nof, ep_ran_user_det * det)
{
	uint16_t      i;
	ep_ran_urep * r = (ep_ran_urep *) buf;
	ep_ran_uinf * u = (ep_ran_uinf *)(buf + sizeof(ep_ran_urep));

	if(size < sizeof(ep_ran_ureq)) {
		ep_dbg_log("P - RANU Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_ran_ureq) + (
		sizeof(ep_ran_uinf) * ntohs(r->nof_users)))
	{
		ep_dbg_log("P - RANU Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(nof) {
		*nof = ntohs(r->nof_users);
	}

	ep_dbg_dump("P - RANU Rep: ", buf, sizeof(ep_ran_urep));

	if(det) {
		for (i = 0; i < *nof && i < EP_RAN_USER_MAX; i++) {
			det[i].id     = ntohs(u[i].rnti);
			det[i].slice = be64toh(u[i].slice_id);

			ep_dbg_dump(
				"    P - RANU Info: ",
				buf + sizeof(ep_ran_urep)  + (
					sizeof(ep_ran_uinf) * i),
				sizeof(ep_ran_uinf));
		}
	}

	return EP_SUCCESS;
}

/* Formats a RAN user add using User details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_user_add(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	if(size < sizeof(ep_ran_uinf)) {
		ep_dbg_log("F - RANU Add: Not enough space!\n");
		return -1;
	}

	r->rnti      = htons(det->id);
	r->slice_id = htobe64(det->slice);

	ep_dbg_dump("F - RANU Add: ", buf, sizeof(ep_ran_uinf));

	return sizeof(ep_ran_uinf);
}

/* Parses a RAN user add into User details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_user_add(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	if(size < sizeof(ep_ran_uinf)) {
		ep_dbg_log("P - RANU Add: Not enough space!\n");
		return EP_ERROR;
	}

	if(det) {
		det->id     = ntohs(r->rnti);
		det->slice = be64toh(det->slice);
	}

	ep_dbg_dump("P - RANU Add: ", buf, sizeof(ep_ran_uinf));

	return EP_SUCCESS;
}

/* Formats a RAN user rem using User details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_user_rem(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	if(size < sizeof(ep_ran_uinf)) {
		ep_dbg_log("F - RANU Rem: Not enough space!\n");
		return -1;
	}

	r->rnti      = htons(det->id);
	r->slice_id = htobe64(det->slice);

	ep_dbg_dump("F - RANU Rem: ", buf, sizeof(ep_ran_uinf));

	return sizeof(ep_ran_uinf);
}

/* Parses a RAN user rem into User details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_user_rem(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	if(size < sizeof(ep_ran_uinf)) {
		ep_dbg_log("P - RANU Rem: Not enough space!\n");
		return EP_ERROR;
	}

	if(det) {
		det->id     = ntohs(r->rnti);
		det->slice = be64toh(det->slice);
	}

	ep_dbg_dump("P - RANU Rem: ", buf, sizeof(ep_ran_uinf));

	return EP_SUCCESS;
}

/* Formats a RAN scheduler parameter request using Sched details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_sched_req(
	char *              buf, 
	unsigned int        size,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det)
{
	size_t        n = min(det->name_len, EP_RAN_NAME_MAX);
	ep_ran_sreq * r = (ep_ran_sreq *)buf;
	char *        a = buf + sizeof(ep_ran_sreq);

	if(size < sizeof(ep_ran_sreq) + det->name_len) {
		ep_dbg_log("F - RANC Req: Not enough space!\n");
		return -1;
	}

	r->id        = htonl(id);
	r->slice_id = htobe64(slice);
	
	if (slice) {
		r->type = EP_RAN_SCHED_SLICE_TYPE;
	} else {
		r->type = EP_RAN_SCHED_USER_TYPE;
	}

	r->name_len = n;

	/* Copy the name just after the message */
	memcpy(a, det->name, n);

	ep_dbg_dump("F - RANC Req: ", buf, sizeof(ep_ran_uinf) + det->name_len);

	return (sizeof(ep_ran_sreq) + n);
}

/* Parses a RAN scheduler parameter request into Sched details.
 * No checks are done and assumes valid buffers.
 *
 * WARNING:
 * 'det' values will point to buf, and to have a persistent element you will 
 * need to copy it, otherwise it will be invalid if 'buff is re-used or freed.
 */
int epp_ran_sched_req(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det)
{
	ep_ran_sreq * r = (ep_ran_sreq *)buf;

	/* First evaluate the space for the header... */
	if(size < sizeof(ep_ran_sreq)) {
		ep_dbg_log("P - RANC Req: Not enough space!\n");
		return EP_ERROR;
	}

	/* ...then evaluate the space for the entire packet */
	if(size < sizeof(ep_ran_sreq) + r->name_len) {
		ep_dbg_log("P - RANC Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id       = ntohl(r->id);
	}

	if(slice) {
		*slice   = be64toh(r->slice_id);
	}

	if(det) {
		if (r->name_len) {
			det->name     = buf + r->name_len;
			det->name_len = r->name_len;
		} else {
			det->name     = 0;
			det->name_len = 0;
		}
	}
	
	ep_dbg_dump("P - RANC Req: ", buf, sizeof(ep_ran_sreq) + r->name_len);

	return EP_SUCCESS;
}

/* Formats a RAN scheduler parameter reply using Sched details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_sched_rep(
	char *              buf,
	unsigned int        size,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det)
{
	size_t        n = min(det->name_len,  EP_RAN_NAME_MAX);
	size_t        v = min(det->value_len, EP_RAN_VALUE_MAX);
	ep_ran_srep * r = (ep_ran_srep *)buf;
	char *        a = buf + sizeof(ep_ran_srep);

	/* First evaluate the space for the header... */
	if(size < sizeof(ep_ran_srep) + det->name_len + det->value_len) {
		ep_dbg_log("F - RANC Rep: Not enough space!\n");
		return -1;
	}

	r->id           = htonl(id);
	r->slice_id    = htobe64(slice);
	
	if (slice) {
		r->type = EP_RAN_SCHED_SLICE_TYPE;
	} else {
		r->type = EP_RAN_SCHED_USER_TYPE;
	}

	r->name_len     = n;
	r->value_len    = htons(v);

	/* Copy the name just after the message */
	memcpy(a, det->name, n);
	/* Move forward to prepare value copy */
	a += n;
	/* copy the value after the name */
	memcpy(a, det->value, v);

	ep_dbg_dump(
		"F - RANC Rep: ",
		buf,
		sizeof(ep_ran_srep) + n + v);

	return (sizeof(ep_ran_srep) + n + v);
}

/* Parses a RAN scheduler parameter reply into Sched details.
 * No checks are done and assumes valid buffers.
 *
 * WARNING:
 * 'det' values will point to buf, and to have a persistent element you will 
 * need to copy it, otherwise it will be invalid if 'buff is re-used or freed.
 */
int epp_ran_sched_rep(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det)
{
	size_t        n;
	size_t        v;
	ep_ran_srep * r = (ep_ran_srep *)buf;
	char *        a = buf + sizeof(ep_ran_srep);

	/* First evaluate the space for the header... */
	if(size < sizeof(ep_ran_srep)) {
		ep_dbg_log("P - RANC Rep: Not enough space!\n");
		return EP_ERROR;
	}

	/* ...then evaluate the space for the entire packet */
	if(size < sizeof(ep_ran_srep) + r->name_len + ntohs(r->value_len)) {
		ep_dbg_log("P - RANC Rep: Not enough space!\n");
		return EP_ERROR;
	}

	n               = r->name_len;
	v               = ntohs(r->value_len);

	if(id) {
		*id     = ntohl(r->id);
	}

	if(slice) {
		*slice = be64toh(r->slice_id);
	}

	if(det) {
		det->name       = a;
		det->name_len   = n;
		det->value      = a + n;
		det->value_len  = v;
	}

	ep_dbg_dump(
		"P - RANC Rep: ",
		buf,
		sizeof(ep_ran_srep) + det->name_len + det->value_len);

	return EP_SUCCESS;
}

/* Formats a RAN scheduler parameter reply using Sched details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_sched_set(
	char *              buf,
	unsigned int        size,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det)
{
	size_t        n = min(det->name_len,  EP_RAN_NAME_MAX);
	size_t        v = min(det->value_len, EP_RAN_VALUE_MAX);
	ep_ran_srep * r = (ep_ran_srep *)buf;
	char *        a = buf + sizeof(ep_ran_srep);

	if(size < sizeof(ep_ran_srep) + det->name_len + det->value_len) {
		ep_dbg_log("F - RANC Set: Not enough space!\n");
		return -1;
	}

	r->id           = htonl(id);
	r->slice_id    = htobe64(slice);
	
	if (slice) {
		r->type = EP_RAN_SCHED_SLICE_TYPE;
	} else {
		r->type = EP_RAN_SCHED_USER_TYPE;
	}

	r->name_len     = n;
	r->value_len    = htons(v);

	/* Copy the name just after the message */
	memcpy(a, det->name, n);
	/* Move forward to prepare value copy */
	a += n;
	/* copy the value after the name */
	memcpy(a, det->value, v);

	ep_dbg_dump(
		"F - RANC Set: ",
		buf,
		sizeof(ep_ran_srep) + n + v);

	return (sizeof(ep_ran_srep) + n + v);
}

/* Parses a RAN scheduler parameter set into Sched details.
 * No checks are done and assumes valid buffers.
 *
 * WARNING:
 * 'det' values will point to buf, and to have a persistent element you will 
 * need to copy it, otherwise it will be invalid if 'buff is re-used or freed.
 */
int epp_ran_sched_set(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det)
{
	size_t        n;
	size_t        v;
	ep_ran_srep * r = (ep_ran_srep *)buf;
	char *        a = buf + sizeof(ep_ran_srep);

	/* First evaluate the space for the header... */
	if(size < sizeof(ep_ran_srep)) {
		ep_dbg_log("P - RANC Set: Not enough space!\n");
		return EP_ERROR;
	}

	/* ...then evaluate the space for the entire packet */
	if(size < sizeof(ep_ran_srep) + r->name_len + ntohs(r->value_len)) {
		ep_dbg_log("P - RANC Set: Not enough space!\n");
		return EP_ERROR;
	}

	n               = r->name_len;
	v               = ntohs(r->value_len);

	if(id) {
		*id     = ntohl(r->id);
	}

	if(slice) {
		*slice = be64toh(r->slice_id);
	}

	if(det) {
		det->name       = a;
		det->name_len   = n;
		det->value      = a + n;
		det->value_len  = v;
	}

	ep_dbg_dump(
		"P - RANC Set: ",
		buf,
		sizeof(ep_ran_srep) + n + v);

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
		ep_dbg_log("F - Single RAN Fail: Invalid buffer!\n");
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
		ep_dbg_log("F - Single RAN NS: Invalid buffer!\n");
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

/******* RAN Setup ************************************************************/

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
		ep_dbg_log("F - Single RANS Req: Invalid buffer!\n");
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

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
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
		ep_dbg_log("F - Single RAN Rep: Invalid buffer!\n");
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
	ms   = epf_ran_setup_rep(
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
		ep_dbg_log("P - Single RAN Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_setup_rep(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

/******* RAN Tenant ***********************************************************/

int epf_single_ran_ten_req(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single RANT Req: Invalid buffer!\n");
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
	ms   = epf_ran_slice_req(
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

int epp_single_ran_ten_req(
	char *             buf,
	unsigned int       size,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANT Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_slice_req(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

int epf_single_ran_ten_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	uint16_t           nof_slices,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single RANT Rep: Invalid buffer!\n");
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
	ms   = epf_ran_slice_rep(
		buf + ret,
		size - ret,
		nof_slices,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_ten_rep(
	char *             buf,
	unsigned int       size,
	uint16_t *         nof_slices,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANT Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_slice_rep(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		nof_slices,
		det);
}

int epf_single_ran_ten_add(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single RANT Add: Invalid buffer!\n");
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

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_ten_add(
	char *             buf,
	unsigned int       size, 
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANT Add: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_slice_add(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

int epf_single_ran_ten_rem(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
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

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_ten_rem(
	char *             buf,
	unsigned int       size,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANT Rem: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_slice_rem(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

/******* RAN Users ************************************************************/

int epf_single_ran_usr_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint16_t     rnti)
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
	ms   = epf_ran_user_req(
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
	uint16_t *   rnti)
{
	if(!buf) {
		ep_dbg_log("P - Single RANU Req: Invalid buffer!\n");
		return -1;
	}

	return epp_ran_user_req(
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
	ms   = epf_ran_user_rep(
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

	return epp_ran_user_rep(
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
	ms   = epf_ran_user_add(
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

	return epp_ran_user_add(
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
	ms   = epf_ran_user_rem(
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

	return epp_ran_user_rem(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

/******* RAN Scheduler ********************************************************/

int epf_single_ran_sch_req(
	char *              buf,
	unsigned int        size,
	enb_id_t            enb_id,
	cell_id_t           cell_id,
	mod_id_t            mod_id,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single RANC Req: Invalid buffer!\n");
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
		EP_ACT_RAN_SCHED,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_sched_req(
		buf + ret,
		size - ret,
		id,
		slice,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_sch_req(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANC Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_sched_req(
		buf  + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - sizeof(ep_hdr) + sizeof(ep_s_hdr),
		id, 
		slice,
		det);
}

int epf_single_ran_sch_rep(
	char *              buf,
	unsigned int        size,
	enb_id_t            enb_id,
	cell_id_t           cell_id,
	mod_id_t            mod_id,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single RANC Rep: Invalid buffer!\n");
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
		EP_ACT_RAN_SCHED,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_sched_rep(
		buf + ret,
		size - ret,
		id,
		slice,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_sch_rep(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANC Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_sched_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - sizeof(ep_hdr) + sizeof(ep_s_hdr),
		id,
		slice,
		det);
}

int epf_single_ran_sch_set(
	char *              buf,
	unsigned int        size,
	enb_id_t            enb_id,
	cell_id_t           cell_id,
	mod_id_t            mod_id,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single RANC Set: Invalid buffer!\n");
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
		EP_ACT_RAN_SCHED,
		EP_OPERATION_SET);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_sched_set(
		buf + ret,
		size - ret,
		id,
		slice,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_sch_set(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANC Set: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_sched_set(
		buf  + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - sizeof(ep_hdr) + sizeof(ep_s_hdr),
		id,
		slice,
		det);
}
