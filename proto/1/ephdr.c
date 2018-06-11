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
 * limitations under the License.
 */

#include <netinet/in.h>

#include <emproto.h>

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_head(
	char *       buf,
	unsigned int size,
	ep_msg_type  type,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id)
{
	ep_hdr * h = (ep_hdr *)buf;

	if(!buf) {
		ep_dbg_log("F - HDR: Invalid buffer!\n");
		return -1;
	}

	if(size < sizeof(ep_hdr)) {
		ep_dbg_log("F - HDR: Not enough space!\n");
		return -1;
	}

	h->type       = (uint8_t)type;
	h->vers       = (uint8_t)EMPOWER_PROTOCOL_VERS;
	h->id.enb_id  = htonl(enb_id);
	h->id.cell_id = htons(cell_id);
	h->id.mod_id  = htonl(mod_id);

	ep_dbg_dump("F - HDR:  ", buf, sizeof(ep_hdr));

	return sizeof(ep_hdr);
}

int epp_head(
	char * buf, unsigned int size,
	ep_msg_type * type,
	uint32_t * enb_id,
	uint16_t * cell_id,
	uint32_t * mod_id)
{
	ep_hdr * h = (ep_hdr *)buf;

	if(!buf) {
		ep_dbg_log("P - HDR: Invalid buffer!\n");
		return -1;
	}

	if(size < sizeof(ep_hdr)) {
		ep_dbg_log("P - HDR: Not enough space!\n");
		return -1;
	}

	if(h->vers != EMPOWER_PROTOCOL_VERS) {
		return EP_WRONG_VERSION;
	}

	if(type) {
		*type    = h->type;
	}

	if(enb_id) {
		*enb_id  = ntohl(h->id.enb_id);
	}

	if(cell_id) {
		*cell_id = ntohs(h->id.cell_id);
	}

	if(mod_id) {
		*mod_id  = ntohl(h->id.mod_id);
	}

	ep_dbg_dump("P - HDR:  ", buf, sizeof(ep_hdr));

	return EP_SUCCESS;
}

ep_msg_type epp_msg_type(char * buf, unsigned int size)
{
	ep_hdr * h = (ep_hdr *)buf;

	if(!buf) {
		ep_dbg_log("F - HDR: Invalid buffer!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_hdr)) {
		ep_dbg_log("P - HDR type: Not enough space!\n");
		return EP_ERROR;
	}

	return (ep_msg_type)h->type;
}

uint32_t epp_seq(char * buf, unsigned int size)
{
	ep_hdr * h = (ep_hdr *)buf;

	if(!buf) {
		ep_dbg_log("F - HDR: Invalid buffer!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_hdr)) {
		ep_dbg_log("P - HDR seq: Not enough space!\n");
		return EP_ERROR;
	}

	return ntohl(h->seq);
}

uint16_t epp_msg_length(char * buf, unsigned int size)
{
	ep_hdr * h = (ep_hdr *)buf;

	if(!buf) {
		ep_dbg_log("F - HDR: Invalid buffer!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_hdr)) {
		ep_dbg_log("P - HDR len: Not enough space!\n");
		return EP_ERROR;
	}

	return ntohs(h->length);
}

int epf_seq(char * buf, unsigned int size, uint32_t seq)
{
	ep_hdr * h = (ep_hdr *)buf;

	if(!buf) {
		ep_dbg_log("F - HDR: Invalid buffer!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_hdr)) {
		ep_dbg_log("F - HDR seq: Not enough space!\n");
		return EP_ERROR;
	}

	h->seq = htonl(seq);

	return EP_SUCCESS;
}

int epf_msg_length(char * buf, unsigned int size, uint16_t len)
{
	ep_hdr * h = (ep_hdr *)buf;

	if(!buf) {
		ep_dbg_log("F - HDR: Invalid buffer!\n");
		return EP_ERROR;
	}

	if(size < sizeof(ep_hdr)) {
		ep_dbg_log("F - HDR len: Not enough space!\n");
		return EP_ERROR;
	}

	h->length = htons(len);

	return EP_SUCCESS;
}
