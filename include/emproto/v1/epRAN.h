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
 * limitations under the License.
 */

/*
 *
 * RADIO ACCESS NETWORK SLICING MESSAGE
 *
 *
 */

#ifndef __EMAGE_RAN_H
#define __EMAGE_RAN_H

#include <stdint.h>

#include "eppri.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#if 0
/* Possible types of schedulers handled by the RAN sharing mechanisms */
typedef enum __ep_RAN_sched_type {
	EP_RAN_SCHED_SLICE_TYPE = 0,
	EP_RAN_SCHED_USER_TYPE  = 1
}ep_ran_schedtype;
#endif
/*
 * Message structures:
 */
#if 0
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
#endif
 /*****************************************************************************
  *                                                                           *
  * RAN User                                                                  *
  *                                                                           *
  *****************************************************************************/

 /* RAN User information message */
typedef struct __ep_ran_user_info {
	rnti_id_t  rnti;          /* RNTI of the user */
	slice_id_t slice_id;      /* ID of the Slice the user belong to */
}__attribute__((packed)) ep_ran_uinf;

/* RAN User reply message */
typedef struct __ep_ran_user_rep {
	uint16_t nof_users;     /* Number of Users to take into account */
	/* Here the array of ep_ran_uinf which depends from 'nof_users' */
}__attribute__((packed)) ep_ran_urep;

/* RAN User request message */
typedef struct __ep_ran_user_req {
	rnti_id_t rnti;	        /* RNTI of the user */
 }__attribute__((packed)) ep_ran_ureq;

 /*****************************************************************************
  *                                                                           *
  * RAN Slice                                                                 *
  *                                                                           *
  *****************************************************************************/

/* RAN slice MAC information */
typedef struct __ep_ran_slice_mac {
	sched_id_t  user_sched; /* Id of the user scheduler associated */
	uint16_t    prbs;       /* PRBs assigned to the slice */
}__attribute__((packed)) ep_ran_smac;

/* RAN slice MAC information in TLV style */
typedef struct __ep_ran_slice_mac_TLV {
	ep_TLV      header;
	ep_ran_smac body;
}__attribute__((packed)) ep_ran_smac_TLV;

 /* RAN Slice info message */
typedef struct __ep_ran_slice_inf {
	slice_id_t  id;		/* ID of the Slice */
}__attribute__((packed)) ep_ran_sinf;

 /* RAN Slice reply message */
typedef struct __ep_ran_slice_rep {
	uint16_t    nof_slices;	/* Number of Slices reported */
	/* Here the array of ep_ran_tinf which depends from 'nof_Slices' */
}__attribute__((packed)) ep_ran_srep;

 /* RAN Slice request message */
typedef struct __ep_ran_slice_req {
	slice_id_t  id;		/* ID of the Slice */
 }__attribute__((packed)) ep_ran_sreq;

 /*****************************************************************************
  *                                                                           *
  * RAN SETUP                                                                 *
  *                                                                           *
  *****************************************************************************/

/* Contains information to describe the slice scheduler */
typedef struct __ep_ran_mac_sched {
	sched_id_t slice_sched;
} __attribute__((packed)) ep_ran_mac_sched;

/* Contains information to describe the slice scheduler in TLV style */
typedef struct __ep_ran_mac_sched_TLV {
	ep_TLV           header;
	ep_ran_mac_sched body;
} __attribute__((packed)) ep_ran_mac_sched_TLV;

/* Bitmask values to use in the RAN setup capabilities bitmask */
typedef enum __ep_ran_layer1_caps {
	EP_RAN_LAYER1_CAP_NOTHING = 0,
} RAN_l1_caps;

/* Bitmask values to use in the RAN setup capabilities bitmask */
typedef enum __ep_ran_layer2_caps {
	EP_RAN_LAYER2_CAP_NOTHING = 0,
	EP_RAN_LAYER2_CAP_PRB_SLICING = 1,
} RAN_l2_caps;

/* Bitmask values to use in the RAN setup capabilities bitmask */
typedef enum __ep_ran_layer3_caps {
	EP_RAN_LAYER3_CAP_NOTHING = 0,
} RAN_l3_caps;

 /* RAN Setup reply message */
typedef struct __ep_ran_setup {
	uint32_t layer1_cap;	/* Mask of capabilities at layer 1 */
	uint32_t layer2_cap;	/* Mask of capabilities at layer 2 */
	uint32_t layer3_cap;	/* Mask of capabilities at layer 3 */
}__attribute__((packed)) ep_ran_setup;

 /*****************************************************************************
  *                                                                           *
  * Opaque structures                                                         *
  *                                                                           *
  *****************************************************************************/

/* NOTE: This limits the number of Slices per message! */
#define EP_RAN_SLICE_MAX        16
/* NOTE: This limits the number of Users per message! */
#define EP_RAN_USER_MAX         64
/* NOTE: This limits the number of character per scheduler parameter name! */
#define EP_RAN_NAME_MAX         32
/* NOTE: This limits the number of character per scheduler parameter value! */
#define EP_RAN_VALUE_MAX        8192
#if 0
typedef struct __ep_ran_sched_param_details {
	char *   name;		/* Name of the parameter */
	uint8_t  name_len;	/* Length of the name */
	char *   value;		/* Value of the parameter */
	uint16_t value_len;	/* Length of the value */
}ep_ran_sparam_det;
#endif
typedef struct __ep_ran_user_details {
	rnti_id_t  id;          /* RNTI */
	slice_id_t slice;       /* Id of the Slice */
}ep_ran_user_det;

typedef struct __ep_ran_slice_l2_details {
	sched_id_t       usched; /* User scheduler ID */
	uint16_t         prbs;   /* PRBs assigned to the slice */
} ep_ran_slice_l2d;

typedef struct __ep_ran_slice_details {
	ep_ran_slice_l2d l2;    /* ID of the active User scheduler */
} ep_ran_slice_det;

/* Invalid id for a scheduler */
#define EP_RAN_SCHED_INVALID	0

typedef struct __rp_ran_mac_details {
	sched_id_t slice_sched; /* Id of the slice scheduler */
} ep_ran_mac_det;

typedef struct __rp_ran_l2_details {
	ep_ran_mac_det mac;     /* Details at MAC layer */
} ep_ran_l2_det;

typedef struct __ep_ran_setup_details {
	uint32_t      l1_mask;  /* Layer 1 capabilities */
	uint32_t      l2_mask;  /* Layer 2 capabilities */
	uint32_t      l3_mask;  /* Layer 3 capabilities */
	ep_ran_l2_det l2;       /* Layer 2 details*/
} ep_ran_det;

/******************************************************************************
 *                                                                            *
 * Operation on single-event messages                                         *
 *                                                                            *
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
#if 0
/* Format a RAN Schedule operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_schedule_fail(b, s, e, c, m)	\
	epf_single_ran_rep_fail(b, s, EP_ACT_RAN_SCHED, e, c, m)
#endif
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
#if 0
/* Format a RAN Schedule operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_schedule_ns(b, s, e, c, m)	\
	epf_single_ran_rep_ns(b, s, EP_ACT_RAN_SCHED, e, c, m)
#endif
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

/* Parses a RAN setup request message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_setup_req(
	char *       buf,
	unsigned int size);

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
int epf_single_ran_slice_req(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id);

/* Parses a RAN Slice request message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_req(
	char *       buf,
	unsigned int size,
	slice_id_t * slice_id);

/* Formats a RAN Slice reply message for multiple slice IDs.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_multi_slice_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	uint16_t           nof_slices,
	slice_id_t *       slices);

/* Parses a RAN Slice reply message for multiple slice IDs.
 * Returns EP_SUCCESS on success, otherwise a negative error code..
 */
int epp_single_ran_multi_slice_rep(
	char *             buf,
	unsigned int       size,
	uint16_t *         nof_slices,
	slice_id_t *       slices);

/* Formats a RAN Slice reply message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_slice_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det);

/* Parses a RAN Slice reply message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_rep(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det);

/* Formats a RAN Slice add message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_slice_add(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det);

/* Parses a RAN Slice add message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_add(
	char *             buf,
	unsigned int       size, 
	slice_id_t *       slice_id,
	ep_ran_slice_det * det);

/* Formats a RAN Slice remove message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_slice_rem(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id);

/* Parses a RAN Slice remove message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_rem(
	char *             buf,
	unsigned int       size,
	slice_id_t *       id);

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
	rnti_id_t    rnti);

/* Parses a RAN user request message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_usr_req(
	char *       buf,
	unsigned int size,
	rnti_id_t *  rnti);

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
