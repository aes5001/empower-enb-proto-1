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

/*    RADIO ACCESS NETWORK SHARING MESSAGE
 *
 * This message provides the request/reply for tuning the RAN features.
 */

#ifndef __EMAGE_RAN_H
#define __EMAGE_RAN_H

#include <stdint.h>

#include "eppri.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Possible types of schedulers handled by the RAN sharing mechanisms */
typedef enum __ep_RAN_sched_type {
	EP_RAN_SCHED_SLICE_TYPE = 0,
	EP_RAN_SCHED_USER_TYPE  = 1
}ep_ran_schedtype;

/*
 * Message structures:
 */

 /********** RAN SCHEDULERS  **********/

 /* RAN scheduler parameter reply message*/
typedef struct __ep_ran_sched_rep {
	uint32_t id;		/* ID of the scheduler */
	uint8_t  type;		/* Type of scheduler */
	uint64_t slice_id;	/* ID of the Slice; valid if 'type' = 1 */
	uint8_t  name_len;	/* Length of the name field */
	uint16_t value_len;	/* Length of the value field */
	/* Scheduler parameter name  will be attached here */
	/* Scheduler parameter value will be attached here */
}__attribute__((packed)) ep_ran_crep;

/* RAN scheduler parameter request message*/
typedef struct __ep_ran_sched_req {
	uint32_t id;		/* ID of the scheduler */
	uint8_t  type;		/* Type of scheduler */
	uint64_t slice_id;	/* ID of the Slice; valid if 'type' = 1 */
	uint8_t  name_len;	/* Length of the name field */
	/* Scheduler parameter name will be attached here */
 }__attribute__((packed)) ep_ran_creq;

 /********** RAN USER  **********/

 /* RAN User information message */
typedef struct __ep_ran_user_info {
	uint16_t rnti;		/* RNTI of the user */
	uint64_t slice_id;	/* ID of the Slice the user belong to */
}__attribute__((packed)) ep_ran_uinf;

/* RAN User reply message */
typedef struct __ep_ran_user_rep {
	uint16_t nof_users;	/* Number of Users to take into account */
	/* Here the array of ep_ran_uinf which depends from 'nof_users' */
}__attribute__((packed)) ep_ran_urep;

/* RAN User request message */
typedef struct __ep_ran_user_req {
	uint16_t rnti;		/* RNTI of the user */
 }__attribute__((packed)) ep_ran_ureq;

 /********** RAN Slice  **********/

typedef struct __ep_ran_slice_rem {
	uint64_t id;		/* ID of the Slice */
}__attribute__((packed)) ep_ran_srem;

typedef struct __ep_ran_slice_add {
	uint64_t id;		/* ID of the Slice */
	uint32_t sched;		/* ID of the UE scheduler of this Slice */
}__attribute__((packed)) ep_ran_sadd;

 /* RAN Slice info message */
typedef struct __ep_ran_slice_inf {
	uint64_t id;		/* ID of the Slice */
	uint32_t sched;		/* ID of the UE scheduler of this Slice */
}__attribute__((packed)) ep_ran_sinf;

 /* RAN Slice reply message */
typedef struct __ep_ran_slice_rep {
	uint16_t nof_slices;	/* Number of Slices reported */
	/* Here the array of ep_ran_tinf which depends from 'nof_Slices' */
}__attribute__((packed)) ep_ran_srep;

 /* RAN Slice request message */
typedef struct __ep_ran_slice_req {
	uint64_t id;		/* ID of the Slice */
 }__attribute__((packed)) ep_ran_sreq;

 /********** RAN SETUP  **********/

 /* RAN Setup reply/set message */
typedef struct __ep_ran_setup {
	uint32_t sched_id;	/* ID of the top-level Slices scheduler */
}__attribute__((packed)) ep_ran_setup;

/*
 * Opaque structures:
 */

/* NOTE: This limits the number of Slices per message! */
#define EP_RAN_SLICE_MAX       8
/* NOTE: This limits the number of Users per message! */
#define EP_RAN_USER_MAX         64
/* NOTE: This limits the number of character per scheduler parameter name! */
#define EP_RAN_NAME_MAX         32
/* NOTE: This limits the number of character per scheduler parameter value! */
#define EP_RAN_VALUE_MAX        8192

typedef struct __ep_ran_sched_param_details {
	char *   name;		/* Name of the parameter */
	uint8_t  name_len;	/* Length of the name */
	char *   value;		/* Value of the parameter */
	uint16_t value_len;	/* Length of the value */
}ep_ran_sparam_det;

typedef struct __ep_ran_user_details {
	uint16_t id;		/* RNTI */
	uint64_t slice;	/* Id of the Slice */
}ep_ran_user_det;

typedef struct __ep_ran_slice_details {
	uint64_t id;		/* Id of the Slice */
	uint32_t sched;		/* ID of the active User scheduler */
} ep_ran_slice_det;

typedef struct __ep_ran_setup_details {
	uint32_t slice_sched;	/* ID of the Slice scheduler */
} ep_ran_det;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/* Format a RAN setup operation failed message.
 * Returns the message size or -1 on error.
 */
 #define epf_single_ran_setup_fail(b, s, e, c, m)	\
	epf_single_ran_rep_fail(b, s, EP_ACT_RAN_SETUP, e, c, m)

/* Format a RAN Slice operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_slice_fail(b, s, e, c, m)	\
	epf_single_ran_rep_fail(b, s, EP_ACT_RAN_SLICE, e, c, m)

/* Format a RAN User operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_user_fail(b, s, e, c, m)		\
	epf_single_ran_rep_fail(b, s, EP_ACT_RAN_USER, e, c, m)

/* Format a RAN Schedule operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_schedule_fail(b, s, e, c, m)	\
	epf_single_ran_rep_fail(b, s, EP_ACT_RAN_SCHED, e, c, m)

/* Format a RAN operation failed message. 
 * Returns the message size or -1 on error. 
 */
int epf_single_ran_rep_fail(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Format a RAN setup operation failed message.
 * Returns the message size or -1 on error.
 */
 #define epf_single_ran_setup_ns(b, s, e, c, m)	\
	epf_single_ran_rep_ns(b, s, EP_ACT_RAN_SETUP, e, c, m)

/* Format a RAN Slice operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_slice_ns(b, s, e, c, m)	\
	epf_single_ran_rep_ns(b, s, EP_ACT_RAN_SLICE, e, c, m)

/* Format a RAN User operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_user_ns(b, s, e, c, m)		\
	epf_single_ran_rep_ns(b, s, EP_ACT_RAN_USER, e, c, m)

/* Format a RAN Schedule operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_schedule_ns(b, s, e, c, m)	\
	epf_single_ran_rep_ns(b, s, EP_ACT_RAN_SCHED, e, c, m)

/* Format a RAN operation not supported message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_rep_ns(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/******************************************************************************/

/* Formats a RAN setup request message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_setup_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Formats a RAN setup reply message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	ep_ran_det * ran);

/* Parses a RAN setup reply message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	ep_ran_det * ran);

/******************************************************************************/

/* Formats a RAN Slice request message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_ten_req(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	ep_ran_slice_det * det);

/* Parses a RAN Slice request message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_ten_req(
	char *             buf,
	unsigned int       size,
	ep_ran_slice_det * det);

/* Formats a RAN Slice reply message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_ten_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	uint16_t           nof_slices,
	ep_ran_slice_det * det);

/* Parses a RAN Slice reply message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_ten_rep(
	char *             buf,
	unsigned int       size,
	uint16_t *         nof_slices,
	ep_ran_slice_det * det);

/* Formats a RAN Slice add message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_ten_add(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	ep_ran_slice_det * det);

/* Parses a RAN Slice add message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_ten_add(
	char *             buf,
	unsigned int       size,
	ep_ran_slice_det * det);

/* Formats a RAN Slice remove message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_ten_rem(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	ep_ran_slice_det * det);

/* Parses a RAN Slice remove message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_ten_rem(
	char *             buf,
	unsigned int       size,
	ep_ran_slice_det * det);

/******************************************************************************/

/* Formats a RAN user request message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_usr_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint16_t     rnti);

/* Parses a RAN user request message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_usr_req(
	char *       buf,
	unsigned int size,
	uint16_t *   rnti);

/* Formats a RAN user reply message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_usr_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	uint16_t           nof_users,
	ep_ran_user_det *  dets);

/* Parses a RAN user reply message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_usr_rep(
	char *            buf,
	unsigned int      size,
	uint16_t *        nof_users,
	ep_ran_user_det * dets);

/* Formats a RAN user add message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_usr_add(
	char *            buf,
	unsigned int      size,
	enb_id_t          enb_id,
	cell_id_t         cell_id,
	mod_id_t          mod_id,
	ep_ran_user_det * det);

/* Parses a RAN user add message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_usr_add(
	char *            buf,
	unsigned int      size,
	ep_ran_user_det * det);

/* Formats a RAN user remove message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_usr_rem(
	char *            buf,
	unsigned int      size,
	enb_id_t          enb_id,
	cell_id_t         cell_id,
	mod_id_t          mod_id,
	ep_ran_user_det * det);

/* Parses a RAN user remove message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_usr_rem(
	char *            buf,
	unsigned int      size,
	ep_ran_user_det * det);

/******************************************************************************/

/* Formats a RAN scheduler parameter request message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_sch_req(
	char *              buf,
	unsigned int        size,
	enb_id_t            enb_id,
	cell_id_t           cell_id,
	mod_id_t            mod_id,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det);

/* Parses a RAN scheduler parameter request message.
 *
 * IMPORTANT: 
 * Details structure pointers will point to memory area within the given buffer.
 * Their validity is related to 'buf' validity; any free or overwriting 
 * operation will invalidate them.
 *
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_sch_req(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det);

/* Formats a RAN scheduler parameter reply message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_sch_rep(
	char *              buf,
	unsigned int        size,
	enb_id_t            enb_id,
	cell_id_t           cell_id,
	mod_id_t            mod_id,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det);

/* Parses a RAN scheduler parameter reply message.
 *
 * IMPORTANT: 
 * Details structure pointers will point to memory area within the given buffer.
 * Their validity is related to 'buf' validity; any free or overwriting 
 * operation will invalidate them.
 *
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_sch_rep(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det);

/* Formats a RAN scheduler parameter set message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_sch_set(
	char *              buf,
	unsigned int        size,
	enb_id_t            enb_id,
	cell_id_t           cell_id,
	mod_id_t            mod_id,
	uint32_t            id,
	uint64_t            slice,
	ep_ran_sparam_det * det);

/* Parses a RAN scheduler parameter set message.
 *
 * IMPORTANT: 
 * Details structure pointers will point to memory area within the given buffer.
 * Their validity is related to 'buf' validity; any free or overwriting 
 * operation will invalidate them.
 *
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_sch_set(
	char *              buf,
	unsigned int        size,
	uint32_t *          id,
	uint64_t *          slice,
	ep_ran_sparam_det * det);

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_RAN_H */
