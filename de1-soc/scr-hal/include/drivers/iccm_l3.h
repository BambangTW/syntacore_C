/*
 * Copyright (C) 2015, Syntacore Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @file
/// Syntacore SCR* framework
///
/// @copyright Copyright (C) 2015, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// @author gdi-sc, dz-sc
///
/// @brief ICCM defs and inline funcs

#ifndef SCR_BSP_ICCM_L3_H
#define SCR_BSP_ICCM_L3_H

#include "plf.h"

#if PLF_SMP_SUPPORT && PLF_ICCM_L3_SUPPORT

/* ICCM CSRs */
/* SEGMENT A */
#define SCR_ICCM_SEG_A_BASE                         (PLF_ICCM_BASE)
#define SCR_ICCM_SEG_A_BUFSTATUS                    (SCR_ICCM_SEG_A_BASE + 0)
#define SCR_ICCM_SEG_A_BUFREAD                      (SCR_ICCM_SEG_A_BASE + 4)
#define SCR_ICCM_SEG_A_SNDSTAT_E_LO                 (SCR_ICCM_SEG_A_BASE + 0x3F0)
#define SCR_ICCM_SEG_A_SNDSTAT_E_HI                 (SCR_ICCM_SEG_A_BASE + 0x3F4)
#define SCR_ICCM_SEG_A_SNDSTAT_0_LO                 (SCR_ICCM_SEG_A_BASE + 0x400)
#define SCR_ICCM_SEG_A_SNDSTAT_0_HI                 (SCR_ICCM_SEG_A_BASE + 0x404)
#define SCR_ICCM_SEG_A_SNDSTAT_N_LO(N)              (SCR_ICCM_SEG_A_SNDSTAT_0_LO + (N) * 16)
#define SCR_ICCM_SEG_A_SNDSTAT_N_HI(N)              (SCR_ICCM_SEG_A_SNDSTAT_0_HI + (N) * 16)
#define SCR_ICCM_SEG_A_BUFWRITE_0                   (SCR_ICCM_SEG_A_BASE + 0xC00)
#define SCR_ICCM_SEG_A_BUFWRITE_N(N)                (SCR_ICCM_SEG_A_BUFWRITE_0 + (N) * 4)

/* SEGMENT B */
#define SCR_ICCM_SEG_B_BASE                         (PLF_ICCM_BASE + 0x1000)
#define SCR_ICCM_SEG_B_ICCM_VERSION                 (SCR_ICCM_SEG_B_BASE + 0x0)
#define SCR_ICCM_SEG_B_ICCM_HARTS                   (SCR_ICCM_SEG_B_BASE + 0x4)
#define SCR_ICCM_SEG_B_ICCM_CONTROL                 (SCR_ICCM_SEG_B_BASE + 0x8)
#define SCR_ICCM_SEG_B_ICCM_CLEAR_LO                (SCR_ICCM_SEG_B_BASE + 0x10)
#define SCR_ICCM_SEG_B_ICCM_CLEAR_HI                (SCR_ICCM_SEG_B_BASE + 0x14)

#define SCR_ICCM_BUFSTATUS_FULL_BIT                 (1 << 0)

#define SCR_ICCM_BUFSTATUS_SENDERID_OFFS            (16)
#define SCR_ICCM_BUFSTATUS_SENDERID_MASK            (0xFF << SCR_ICCM_BUFSTATUS_SENDERID_OFFS)

#define SCR_ICCM_BUFSTATUS_OWNID_OFFS               (24)
#define SCR_ICCM_BUFSTATUS_OWNID_MASK               (0xFF << SCR_ICCM_BUFSTATUS_OWNID_OFFS)

#define SCR_ICCM_ICCM_CONTROL_EWE_BIT               (1 << 0)
#define SCR_ICCM_ICCM_CONTROL_ERE_BIT               (1 << 1)
#define SCR_ICCM_ICCM_CONTROL_EEWF_BIT              (1 << 2)
#define SCR_ICCM_ICCM_CONTROL_EIWF_BIT              (1 << 3)

#ifndef __ASSEMBLER__

#include "arch.h"

typedef uint8_t iccm_l3_id;

/**
 * \brief Returns iccm number of connected harts (reads ICCM_HARTS)
 *
 * \return iccm number of harts
 */
static inline uint32_t iccm_l3_get_harts_num(void) {
    return *((volatile const uint32_t *const) SCR_ICCM_SEG_B_ICCM_HARTS);
}

/**
 * \brief Returns iccm id of a core (reads OWNID field in BUFSTATUS register)
 *
 * \return iccm id of a core
 */
static inline iccm_l3_id iccm_l3_get_own_id(void) {
    uint32_t bufstatus = *((volatile const uint32_t *const) SCR_ICCM_SEG_A_BUFSTATUS);
    return (iccm_l3_id) ((bufstatus & SCR_ICCM_BUFSTATUS_OWNID_MASK) >> SCR_ICCM_BUFSTATUS_OWNID_OFFS);
}

/**
 * \brief Check if receiver is busy for a particular sender (i.e. receiver has unread message from sender in its buffer)
 *
 * \param sender_id iccm id of a sender
 * \param receiver_id iccm id of a receiver
 * \return 1: receiver is busy; 0: receiver is ready to accept new message from sender
 */
static inline int iccm_l3_receiver_busy(iccm_l3_id sender_id, iccm_l3_id receiver_id)
{
    volatile uint64_t *p_sndstat = (volatile uint64_t *) SCR_ICCM_SEG_A_SNDSTAT_N_LO(sender_id);
    return (*p_sndstat >> receiver_id) & 0x1;
}

/**
 * \brief Check if receiver is busy for a external sender (i.e. receiver has unread message from sender in its buffer)
 *
 * \param receiver_id iccm id of a receiver
 * \return 1: receiver is busy; 0: receiver is ready to accept new message from sender
 */
static inline int iccm_l3_receiver_external_busy(iccm_l3_id receiver_id)
{
    volatile uint64_t *p_sndstat = (volatile uint64_t *) SCR_ICCM_SEG_A_SNDSTAT_E_LO;
    return (*p_sndstat >> receiver_id) & 0x1;
}

/**
 * \brief Send message to a hart with corresponding id (i.e. write it to receiver's buffer)
 *
 * \param receiver_id iccm id of a receiver
 * \param msg message
 */
static inline void iccm_l3_write_msg(iccm_l3_id receiver_id, uint32_t msg)
{
    *(volatile uint32_t *const) SCR_ICCM_SEG_A_BUFWRITE_N(receiver_id) = msg;
}

/**
 * \brief Write to register control
 *
 * \param set_control register value to set
 */
static inline void iccm_l3_write_b_control(uint32_t set_control)
{
    *(volatile uint32_t *const) SCR_ICCM_SEG_B_ICCM_CONTROL = set_control;
}

/**
 * \brief Check if hart (caller of this function) has unread messages in a buffer
 *
 * \return 1: messages available; 0: no messages available
 */
static inline int ipi_l3_msg_available(void)
{
    volatile uint32_t *p_bufstatus = (volatile uint32_t *) SCR_ICCM_SEG_A_BUFSTATUS;
    return *p_bufstatus & SCR_ICCM_BUFSTATUS_FULL_BIT;
}

/**
 * \brief Get id of a sender whos message we are about to read
 *
 * \return iccm id of a sender
 */
static inline iccm_l3_id iccm_l3_get_sender_id(void) {
    volatile uint32_t *p_bufstatus = (volatile uint32_t *) SCR_ICCM_SEG_A_BUFSTATUS;
    return (*p_bufstatus & SCR_ICCM_BUFSTATUS_SENDERID_MASK) >> SCR_ICCM_BUFSTATUS_SENDERID_OFFS;
}

/**
 * \brief Read message from a buffer
 *
 * \return message being read
 */
static inline uint32_t iccm_l3_read_msg(void)
{
    return *(volatile const uint32_t *const) SCR_ICCM_SEG_A_BUFREAD;
}

/**
 * \brief Checking that the register clearing bits are reset to zero
 *
 * \param receiver_id iccm id of a receiver
 * \return 1: buffer not clear; 0: buffer clear
 */
static inline int iccm_l3_read_clear_reg(iccm_l3_id receiver_id)
{
    volatile uint64_t *p_sndstat = (volatile uint64_t *) SCR_ICCM_SEG_B_ICCM_CLEAR_LO;
    return (*p_sndstat >> receiver_id) & 0x1;
}

#else // __ASSEMBLER__

.macro ipi_l3_clear_mailbox
    li t0, SCR_ICCM_SEG_B_ICCM_CLEAR_LO
    li t1, -1
    sd t1, 0(t0)
.endm

.macro ipi_l3_send_wait
.Lipi_status_wait:
    // Wait if receiver is busy
    li    t0, SCR_ICCM_SEG_A_SNDSTAT_0_LO // The address of SNDSTAT_0_LO in t0
    // Read our ipi_n from hls
    // t1 = hls:sp.ipi_n
#ifndef __riscv_32e
    load_reg_offs t1, 32, sp
#else // ! __riscv_32e
    load_reg_offs t1, 16, sp
#endif // !__riscv_32e
    slli  t2, t1, 4     // Sender ID * 16 in t2
    add   t0, t0, t2    // address of SNDSTAT_0 + (Sender ID * 16) in t0
    ld    t2, 0(t0)     // load SNDSTAT_N in t2
    li    t3, 1
    sll   t3, t3, a0    // Create mask (1 << hls:a2.ipi_n (Receiver ID)) in t3
    and   t2, t2, t3    // Check REceiver's bit in SNDSTAT_N
    bnez  t2, .Lipi_status_wait  // wait until available

    li    t0, SCR_ICCM_SEG_A_BUFWRITE_0
    slli  t2, a0, 2     // Receiver ID * 4 in t2
    add   t0, t0, t2    // address of BUFWRITE_0 + (Receiver ID * 4) in t0
    sw    a1, 0(t0)     // write msg to BUFWRITE_N
.endm

#endif // __ASSEMBLER__

#endif // PLF_SMP_SUPPORT && PLF_ICCM_L3_SUPPORT
#endif // SCR_BSP_ICCM_L3_H
