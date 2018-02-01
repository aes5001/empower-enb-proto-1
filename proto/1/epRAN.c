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

	s->sched_id = htonl(det->tenant_sched);

	return sizeof(ep_ran_setup);
}

/* Parses a RAN setup reply into RAN details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_setup_rep(char * buf, unsigned int size, ep_ran_det * det)
{
	ep_ran_setup * s = (ep_ran_setup *)buf;

	det->tenant_sched = ntohl(s->sched_id);

	return EP_SUCCESS;
}

/* Formats a RAN tenant request using Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_tenant_req(char * buf, unsigned int size, ep_ran_tenant_det * det)
{
	ep_ran_treq * r = (ep_ran_treq *)buf;

	r->id = htobe64(det->id);

	return sizeof(ep_ran_treq);
}

/* Parses a RAN tenant request into Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_tenant_req(char * buf, unsigned int size, ep_ran_tenant_det * det)
{
	ep_ran_treq * r = (ep_ran_treq *)buf;

	det->id = be64toh(r->id);

	return sizeof(ep_ran_treq);
}

/* Formats a RAN tenant reply using Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_tenant_rep(
	char * buf, unsigned int size, uint16_t nof, ep_ran_tenant_det * dets) 
{
	uint16_t i;

	ep_ran_trep * r = (ep_ran_trep *) buf;
	ep_ran_tinf * d = (ep_ran_tinf *)(buf + sizeof(ep_ran_trep));

	r->nof_tenants = htons(nof);
	
	for (i = 0; i < nof && i < RAN_TENANT_MAX; i++) {
		d[i].id    = htobe64(dets[i].id);
		d[i].sched = htonl(dets[i].sched);
	}

	return sizeof(ep_ran_trep) + (sizeof(ep_ran_tinf) * (i + 1));
}

/* Parses a RAN tenant reply into Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_tenant_rep(
	char * buf, unsigned int size, uint16_t * nof, ep_ran_tenant_det * dets)
{
	uint16_t i;

	ep_ran_trep * r = (ep_ran_trep *) buf;
	ep_ran_tinf * d = (ep_ran_tinf *)(buf + sizeof(ep_ran_trep));

	*nof = ntohs(r->nof_tenants);

	for (i = 0; i < *nof && i < RAN_TENANT_MAX; i++) {
		dets[i].id    = be64toh(d[i].id);
		dets[i].sched = ntohl(d[i].sched);
	}

	return EP_SUCCESS;
}

/* Formats a RAN tenant add using Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_tenant_add(char * buf, unsigned int size, ep_ran_tenant_det * det)
{
	ep_ran_tadd * r = (ep_ran_tadd *)buf;

	r->id    = htobe64(det->id);
	r->sched = htonl(det->sched);

	return sizeof(ep_ran_tadd);
}

/* Parses a RAN tenant add into Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_tenant_add(char * buf, unsigned int size, ep_ran_tenant_det * det)
{
	ep_ran_tadd * r = (ep_ran_tadd *)buf;

	det->id    = be64toh(r->id);
	det->sched = ntohl(r->sched);

	return EP_SUCCESS;
}

/* Formats a RAN tenant remove using Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_tenant_rem(char * buf, unsigned int size, ep_ran_tenant_det * det)
{
	ep_ran_trem * r = (ep_ran_trem *)buf;

	r->id = htobe64(det->id);

	return sizeof(ep_ran_trem);
}

/* Parses a RAN tenant remove into Tenant details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_tenant_rem(char * buf, unsigned int size, ep_ran_tenant_det * det)
{
	ep_ran_trem * r = (ep_ran_trem *)buf;

	det->id = be64toh(r->id);

	return EP_SUCCESS;
}

/* Formats a RAN user request using User details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_user_req(char * buf, unsigned int size, uint16_t rnti)
{
	ep_ran_ureq * r = (ep_ran_ureq *)buf;

	r->rnti = htons(rnti);

	return sizeof(ep_ran_ureq);
}

/* Parses a RAN user request into User details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_user_req(char * buf, unsigned int size, uint16_t * rnti)
{
	ep_ran_ureq * r = (ep_ran_ureq *)buf;

	*rnti = ntohs(r->rnti);

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

	r->nof_users = htons(nof);

	for (i = 0; i < nof && i < RAN_USER_MAX; i++) {
		u[i].rnti      = htons(det[i].id);
		u[i].tenant_id = htobe64(det[i].tenant);
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

	*nof = ntohs(r->nof_users);

	for (i = 0; i < *nof && i < RAN_USER_MAX; i++) {
		det[i].id     = ntohs(u[i].rnti);
		det[i].tenant = be64toh(u[i].tenant_id);
	}

	return EP_SUCCESS;
}

/* Formats a RAN user add using User details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_user_add(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	r->rnti      = htons(det->id);
	r->tenant_id = htobe64(det->tenant);

	return sizeof(ep_ran_uinf);
}

/* Parses a RAN user add into User details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_user_add(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	det->id     = ntohs(r->rnti);
	det->tenant = be64toh(det->tenant);

	return EP_SUCCESS;
}

/* Formats a RAN user rem using User details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_user_rem(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	r->rnti      = htons(det->id);
	r->tenant_id = htobe64(det->tenant);

	return sizeof(ep_ran_uinf);
}

/* Parses a RAN user rem into User details.
 * No checks are done and assumes valid buffers.
 */
int epp_ran_user_rem(char * buf, unsigned int size, ep_ran_user_det * det)
{
	ep_ran_uinf * r = (ep_ran_uinf *)buf;

	det->id     = ntohs(r->rnti);
	det->tenant = be64toh(det->tenant);

	return EP_SUCCESS;
}

/* Formats a RAN scheduler parameter request using Sched details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_sched_req(
	char *              buf, 
	unsigned int        size,
	uint32_t            id,
	uint64_t            tenant,
	ep_ran_sparam_det * det)
{
	size_t        n = min(det->name_len, RAN_NAME_MAX);
	ep_ran_sreq * r = (ep_ran_sreq *)buf;
	char *        a = buf + sizeof(ep_ran_sreq);

	r->id        = htonl(id);
	r->tenant_id = htobe64(tenant);
	
	if (tenant) {
		r->type = EP_RAN_SCHED_TENANT_TYPE;
	} else {
		r->type = EP_RAN_SCHED_USER_TYPE;
	}

	r->name_len = n;

	/* Copy the name just after the message */
	memcpy(a, det->name, n);

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
	uint64_t *          tenant,
	ep_ran_sparam_det * det)
{
	ep_ran_sreq * r = (ep_ran_sreq *)buf;

	*id       = ntohl(r->id);
	*tenant   = be64toh(r->tenant_id);

	if (r->name_len) {
		det->name     = buf + r->name_len;
		det->name_len = r->name_len;
	} else {
		det->name     = 0;
		det->name_len = 0;
	}
	
	return EP_SUCCESS;
}

/* Formats a RAN scheduler parameter reply using Sched details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_sched_rep(
	char *              buf,
	unsigned int        size,
	uint32_t            id,
	uint64_t            tenant,
	ep_ran_sparam_det * det)
{
	size_t        n = min(det->name_len,  RAN_NAME_MAX);
	size_t        v = min(det->value_len, RAN_VALUE_MAX);
	ep_ran_srep * r = (ep_ran_srep *)buf;
	char *        a = buf + sizeof(ep_ran_srep);

	r->id           = htonl(id);
	r->tenant_id    = htobe64(tenant);
	
	if (tenant) {
		r->type = EP_RAN_SCHED_TENANT_TYPE;
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

	return (sizeof(ep_ran_sreq) + n + v);
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
	uint64_t *          tenant,
	ep_ran_sparam_det * det)
{
	size_t        n;
	size_t        v;
	ep_ran_srep * r = (ep_ran_srep *)buf;
	char *        a = buf + sizeof(ep_ran_srep);

	n               = r->name_len;
	v               = ntohs(r->value_len);

	*id             = ntohl(r->id);
	*tenant         = be64toh(r->tenant_id);

	det->name       = a;
	det->name_len   = n;
	det->value      = a + n;
	det->value_len  = v;

	return EP_SUCCESS;
}

/* Formats a RAN scheduler parameter reply using Sched details.
 * No checks are done and assumes valid buffers.
 */
int epf_ran_sched_set(
	char *              buf,
	unsigned int        size,
	uint32_t            id,
	uint64_t            tenant,
	ep_ran_sparam_det * det)
{
	size_t        n = min(det->name_len,  RAN_NAME_MAX);
	size_t        v = min(det->value_len, RAN_VALUE_MAX);
	ep_ran_srep * r = (ep_ran_srep *)buf;
	char *        a = buf + sizeof(ep_ran_srep);

	r->id           = htonl(id);
	r->tenant_id    = htobe64(tenant);
	
	if (tenant) {
		r->type = EP_RAN_SCHED_TENANT_TYPE;
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
	uint64_t *          tenant,
	ep_ran_sparam_det * det)
{
	size_t        n;
	size_t        v;
	ep_ran_srep * r = (ep_ran_srep *)buf;
	char *        a = buf + sizeof(ep_ran_srep);

	n               = r->name_len;
	v               = ntohs(r->value_len);

	*id             = ntohl(r->id);
	*tenant         = be64toh(r->tenant_id);

	det->name       = a;
	det->name_len   = n;
	det->value      = a + n;
	det->value_len  = v;

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ran_rep_fail(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id)
{
	int ms = 0;

	/* Check of given buffer size here */

	ms += epf_head(
		buf, 
		size, 
		EP_TYPE_SINGLE_MSG, 
		enb_id, 
		cell_id, 
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		type,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epf_single_ran_rep_ns(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id)
{
	int ms = 0;

	/* Check of given buffer size here */

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		type,
		EP_OPERATION_NOT_SUPPORTED,
		EP_DIR_REPLY);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

/********** RAN Setup **********/

int epf_single_ran_setup_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id)
{
	int ms = 0;

	/* Check of given buffer size here */

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_SETUP,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REQUEST);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epf_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id,
	ep_ran_det * det)
{
	int ms = 0;

	/* Check of given buffer size here */

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_SETUP,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REPLY);

	ms += epf_ran_setup_rep(
		buf + ms,
		size - ms,
		det);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	ep_ran_det * det)
{
	return epp_ran_setup_rep(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

/********** RAN Tenant **********/

int epf_single_ran_ten_req(
	char *              buf,
	unsigned int        size,	
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	ep_ran_tenant_det * det)
{
	int ms = 0;

	/* Check of given buffer size here */

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_TENANT,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REQUEST);

	ms += epf_ran_tenant_req(
		buf + ms,
		size - ms,
		det);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_ten_req(
	char *              buf,
	unsigned int        size,
	ep_ran_tenant_det * det)
{
	return epp_ran_tenant_req(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

int epf_single_ran_ten_rep(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	uint16_t            nof_tenants,
	ep_ran_tenant_det * det)
{
	int ms = 0;

	/* Check of given buffer size here */

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_TENANT,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REPLY);

	ms += epf_ran_tenant_rep(
		buf + ms,
		size - ms,
		nof_tenants,
		det);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_ten_rep(
	char *              buf,
	unsigned int        size,
	uint16_t *          nof_tenants,
	ep_ran_tenant_det * det)
{
	return epp_ran_tenant_rep(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		nof_tenants,
		det);
}

int epf_single_ran_ten_add(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	ep_ran_tenant_det * det)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_TENANT,
		EP_OPERATION_ADD,
		EP_DIR_REQUEST);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_ten_add(
	char *              buf,
	unsigned int        size, 
	ep_ran_tenant_det * det)
{
	return epp_ran_tenant_add(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

int epf_single_ran_ten_rem(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	ep_ran_tenant_det * det)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_TENANT,
		EP_OPERATION_REM,
		EP_DIR_REQUEST);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_ten_rem(
	char *              buf,
	unsigned int        size,
	ep_ran_tenant_det * det)
{
	return epp_ran_tenant_rem(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

/********** RAN Users **********/

int epf_single_ran_usr_req(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	uint16_t            rnti)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_USER,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REQUEST);

	ms += epf_ran_user_req(
		buf + ms,
		size - ms,
		rnti);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_usr_req(
	char *              buf,
	unsigned int        size,
	uint16_t *          rnti)
{
	return epp_ran_user_req(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		rnti);
}

int epf_single_ran_usr_rep(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	uint16_t            nof_users,
	ep_ran_user_det *   dets)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_USER,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REPLY);

	ms += epf_ran_user_rep(
		buf + ms,
		size - ms,
		nof_users,
		dets);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_usr_rep(
	char *              buf,
	unsigned int        size,
	uint16_t *	    nof_users,
	ep_ran_user_det *   dets)
{
	return epp_ran_user_rep(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		nof_users, 
		dets);
}

int epf_single_ran_usr_add(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	ep_ran_user_det *   det)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_USER,
		EP_OPERATION_ADD,
		EP_DIR_REQUEST);

	ms += epf_ran_user_add(
		buf + ms,
		size - ms,
		det);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_usr_add(
	char *              buf,
	unsigned int        size,
	ep_ran_user_det *   det)
{
	return epp_ran_user_add(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

int epf_single_ran_usr_rem(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	ep_ran_user_det *   det)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_USER,
		EP_OPERATION_REM,
		EP_DIR_REQUEST);

	ms += epf_ran_user_rem(
		buf + ms,
		size - ms,
		det);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_usr_rem(
	char *              buf,
	unsigned int        size, 
	ep_ran_user_det *   det)
{
	return epp_ran_user_rem(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

/********** RAN Scheduler **********/

int epf_single_ran_sch_req(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	uint32_t            id,
	uint64_t            tenant,
	ep_ran_sparam_det * det)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_SCHED,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REQUEST);

	ms += epf_ran_sched_req(
		buf + ms,
		size - ms,
		id,
		tenant,
		det);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_sch_req(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          tenant,
	ep_ran_sparam_det * det)
{
	return epp_ran_sched_req(
		buf  + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - sizeof(ep_hdr) + sizeof(ep_s_hdr),
		id, 
		tenant,
		det);
}

int epf_single_ran_sch_rep(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	uint32_t            id,
	uint64_t            tenant,
	ep_ran_sparam_det * det)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_SCHED,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REPLY);

	ms += epf_ran_sched_rep(
		buf + ms,
		size - ms,
		id,
		tenant,
		det);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_sch_rep(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          tenant,
	ep_ran_sparam_det * det)
{
	return epp_ran_sched_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - sizeof(ep_hdr) + sizeof(ep_s_hdr),
		id,
		tenant,
		det);
}

int epf_single_ran_sch_set(
	char *              buf,
	unsigned int        size,
	uint32_t            enb_id,
	uint16_t            cell_id,
	uint32_t            mod_id,
	uint32_t            id,
	uint64_t            tenant,
	ep_ran_sparam_det * det)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_RAN_SCHED,
		EP_OPERATION_SET,
		EP_DIR_REQUEST);

	ms += epf_ran_sched_set(
		buf + ms,
		size - ms,
		id,
		tenant,
		det);

	/* Inject the message size */
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ran_sch_set(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          tenant,
	ep_ran_sparam_det * det)
{
	return epp_ran_sched_set(
		buf  + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - sizeof(ep_hdr) + sizeof(ep_s_hdr),
		id,
		tenant,
		det);
}