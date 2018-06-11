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

int epf_macrep_rep(
	char *          buf,
	unsigned int    size,
	ep_macrep_det * report)
{
	ep_macrep_rep * rep = (ep_macrep_rep *)buf;

	if(size < sizeof(ep_macrep_rep)) {
		ep_dbg_log("F - MREP Rep: Not enough space!\n");
		return -1;
	}

	rep->DL_prbs_used   = htonl(report->DL_prbs_used);
	rep->DL_prbs_total  = report->DL_prbs_total;
	rep->UL_prbs_used   = htonl(report->UL_prbs_used);
	rep->UL_prbs_total  = report->UL_prbs_total;

	ep_dbg_dump("F - MREP Rep: ", buf, sizeof(ep_macrep_rep));

	return sizeof(ep_macrep_rep);
}

int epp_macrep_rep(
	char *          buf,
	unsigned int    size,
	ep_macrep_det * report)
{
	ep_macrep_rep * rep = (ep_macrep_rep *)buf;

	if(size < sizeof(ep_macrep_rep)) {
		ep_dbg_log("P - MREP Rep: Not enough space!\n");
		return -1;
	}

	if(report) {
		report->DL_prbs_used   = ntohl(rep->DL_prbs_used);
		report->DL_prbs_total  = rep->DL_prbs_total;
		report->UL_prbs_used   = ntohl(rep->UL_prbs_used);
		report->UL_prbs_total  = rep->UL_prbs_total;
	}

	ep_dbg_dump("P - MREP Rep: ", buf, sizeof(ep_macrep_rep));

	return EP_SUCCESS;
}

int epf_macrep_req(char * buf, unsigned int size, uint16_t interval)
{
	ep_macrep_req * req = (ep_macrep_req *)buf;

	if(size < sizeof(ep_macrep_req)) {
		ep_dbg_log("F - MREP Req: Not enough space!\n");
		return -1;
	}

	req->interval = htons(interval);

	ep_dbg_dump("F - MREP Req: ", buf, sizeof(ep_macrep_req));

	return sizeof(ep_macrep_req);
}

int epp_macrep_req(char * buf, unsigned int size, uint16_t * interval)
{
	ep_macrep_req * req = (ep_macrep_req *)buf;

	if(size < sizeof(ep_macrep_req)) {
		ep_dbg_log("P - MREP Req: Not enough space!\n");
		return -1;
	}

	if(interval) {
		*interval = ntohs(req->interval);
	}

	ep_dbg_dump("P - MREP Req: ", buf, sizeof(ep_macrep_req));

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_macrep_rep_fail(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single MACREP Fail: Invalid buffer!\n");
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
		EP_ACT_MAC_REPORT,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_macrep_rep(buf + ret, size - ret, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_trigger_macrep_rep_ns(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single MACREP NS: Invalid buffer!\n");
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
		EP_ACT_MAC_REPORT,
		EP_OPERATION_NOT_SUPPORTED,
		EP_DIR_REPLY);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_macrep_rep(buf + ret, size - ret, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_trigger_macrep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	ep_macrep_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log("F - Single MACREP Rep: Invalid buffer!\n");
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
		EP_ACT_MAC_REPORT,
		EP_OPERATION_SUCCESS,
		EP_DIR_REPLY);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_macrep_rep(buf + ret, size - ret, det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_macrep_rep(
	char *          buf,
	unsigned int    size,
	ep_macrep_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single MACREP Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_macrep_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		det);
}

int epf_trigger_macrep_req(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	uint16_t        interval)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single MACREP Req: Invalid buffer!\n");
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
		EP_ACT_MAC_REPORT,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REPLY);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_macrep_req(buf + ret, size - ret, interval);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_macrep_req(
	char *          buf,
	unsigned int    size,
	uint16_t *      interval)
{
	if(!buf) {
		ep_dbg_log("P - Single MACREP Req: Invalid buffer!\n");
		return -1;
	}

	return epp_macrep_req(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		interval);
}
