/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-10-28     Jesven       first version
 * 2021-02-12     lizhirui     add 64-bit support for lwp_brk
 */
#ifndef  __LWP_USER_MM_H__
#define  __LWP_USER_MM_H__

#include <rthw.h>
#include <rtthread.h>

#ifdef ARCH_MM_MMU
#include <lwp.h>
#include <mmu.h>
#include <mm_aspace.h>
#include <mm_fault.h>
#include <mm_page.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LWP_MAP_FLAG_NONE       0x0000
#define LWP_MAP_FLAG_NOCACHE    0x0001

/**
 * @brief Map files or devices into memory
 * It will create a new mapping in the virtual address space of the target lwp
 *
 * @param lwp target process
 * @param addr address from user space
 * @param length length in bytes of mapping
 * @param prot protect attribution of mapping
 * @param flags flags of control
 * @param fd file descriptor
 * @param pgoffset offset to fd in 4096 bytes unit
 * @return void* the address is successful, otherwise return MAP_FAILED
 */
void* lwp_mmap2(void *addr, size_t length, int prot, int flags, int fd, off_t pgoffset);

/**
 * @brief Unmap memory region in user space
 *
 * @param lwp target process
 * @param addr address to unmap
 * @param length length in bytes of unmapping
 * @return int errno
 */
int lwp_munmap(void *addr);

/**
 * @brief Test if address from user is accessible address by user
 *
 * @param lwp target process
 * @param addr address from user space
 * @param size the bytes to access
 * @return int RT_FALSE/RT_TRUE
 */
int lwp_user_accessible_ext(struct rt_lwp *lwp, void *addr, size_t size);

/**
 * @brief Test if address from user is accessible address by user
 *        Same as lwp_user_accessible_ext except that lwp is current lwp
 *
 * @param addr address from user space
 * @param size the bytes to access
 * @return int RT_FALSE/RT_TRUE
 */
int lwp_user_accessable(void *addr, size_t size);

/**
 * @brief Copy n bytes data from src to dst.
 *        Same as std libc memcpy, except that both src and dst may come from
 *        user space. lwp_memcpy will test and select the implementation based
 *        on the memory attribution on run-time
 *
 * @param dst where the data writes to
 * @param src where the data comes from
 * @param size the bytes to copy
 * @return void* the destination address
 */
void *lwp_memcpy(void * __restrict dst, const void * __restrict src, size_t size);

/**
 * @brief memcpy from address in user address space to kernel space buffer
 *
 * @param lwp target process
 * @param dst kernel space address where the data writes to
 * @param src user space address where the data comes from
 * @param size the bytes to copy
 * @return size_t the bytes copied
 */
size_t lwp_data_get(struct rt_lwp *lwp, void *dst, void *src, size_t size);

/**
 * @brief lwp_data_get except that lwp is current lwp
 *
 * @param dst kernel space address where the data writes to
 * @param src user space address where the data comes from
 * @param size the bytes to copy
 * @return size_t the bytes copied
 */
size_t lwp_get_from_user(void *dst, void *src, size_t size);

/**
 * @brief memcpy from kernel space buffer to address in user address space
 *
 * @param lwp target process
 * @param dst user space address where the data writes to
 * @param src kernel space address where the data comes from
 * @param size the bytes to copy
 * @return size_t the bytes copied
 */
size_t lwp_data_put(struct rt_lwp *lwp, void *dst, void *src, size_t size);

/**
 * @brief lwp_data_put except that lwp is current lwp
 *
 * @param dst user space address where the data writes to
 * @param src kernel space address where the data comes from
 * @param size the bytes to copy
 * @return size_t the bytes copied
 */
size_t lwp_put_to_user(void *dst, void *src, size_t size);

/**
 * @brief memset to address in user address space
 *
 * @param lwp target process
 * @param dst user space address where the data writes to
 * @param c the value to write
 * @param size the bytes to copy
 * @return size_t the bytes written
 */
size_t lwp_data_set(struct rt_lwp *lwp, void *dst, int c, size_t size);

int lwp_user_space_init(struct rt_lwp *lwp, rt_bool_t is_fork);
void lwp_unmap_user_space(struct rt_lwp *lwp);

int lwp_unmap_user(struct rt_lwp *lwp, void *va);
void *lwp_map_user(struct rt_lwp *lwp, void *map_va, size_t map_size, rt_bool_t text);

rt_varea_t lwp_map_user_varea(struct rt_lwp *lwp, void *map_va, size_t map_size);

/* check LWP_MAP_FLAG_* */
rt_varea_t lwp_map_user_varea_ext(struct rt_lwp *lwp, void *map_va, size_t map_size, size_t flags);

void *lwp_map_user_phy(struct rt_lwp *lwp, void *map_va, void *map_pa, size_t map_size, rt_bool_t cached);
int lwp_unmap_user_phy(struct rt_lwp *lwp, void *va);

rt_base_t lwp_brk(void *addr);

void lwp_data_cache_flush(struct rt_lwp *lwp, void *vaddr, size_t size);

static inline void *_lwp_v2p(struct rt_lwp *lwp, void *vaddr)
{
    return rt_hw_mmu_v2p(lwp->aspace, vaddr);
}

static inline void *lwp_v2p(struct rt_lwp *lwp, void *vaddr)
{
    RD_LOCK(lwp->aspace);
    void *paddr = _lwp_v2p(lwp, vaddr);
    RD_UNLOCK(lwp->aspace);
    return paddr;
}

#ifdef __cplusplus
}
#endif

#endif

#endif  /*__LWP_USER_MM_H__*/
