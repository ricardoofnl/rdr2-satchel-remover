#pragma once
#include <stdint.h>
#include <string.h>
#include "offsets.h"

// native handler abi (from disassembly): void handler(ctx) with ctx in rdi
// ctx+0x00 = return slot ptr, ctx+0x10 = args array of 8-byte slots
typedef struct {
    void*    ret;   // +0x00
    uint32_t argc;  // +0x08
    uint32_t _pad;  // +0x0c
    void*    args;  // +0x10
} NativeCtx;

typedef void (*native_fn)(NativeCtx*);

// base is the running eboot base from sys_sdk_proc_info
static inline uint64_t nat_call(uint64_t base, uint64_t off, const uint64_t* a, uint32_t n) {
    uint64_t ret = 0;
    uint64_t argbuf[8];
    memset(argbuf, 0, sizeof(argbuf));
    for (uint32_t i = 0; i < n && i < 8; i++) argbuf[i] = a[i];
    NativeCtx ctx = {0};
    ctx.ret = &ret;
    ctx.argc = n;
    ctx.args = argbuf;
    ((native_fn)(base + off))(&ctx);
    return ret;
}

// ped handles are 32-bit, passed in the low dword of a slot
static inline int gh_get_player_ped(uint64_t base) {
    uint64_t a[1] = { 0xFF };  // 0xff -> local player
    return (int)nat_call(base, OFF_GET_PLAYER_PED, a, 1);
}
static inline int gh_is_using_satchel(uint64_t base, int ped) {
    uint64_t a[2] = { (uint32_t)ped, HASH_SATCHELS };
    return (int)nat_call(base, OFF_IS_META_PED_USING_COMPONENT, a, 2);
}
static inline void gh_remove_satchel(uint64_t base, int ped) {
    uint64_t a[3] = { (uint32_t)ped, HASH_SATCHELS, 0 };
    nat_call(base, OFF_REMOVE_TAG_FROM_META_PED, a, 3);
}
static inline void gh_update_variation(uint64_t base, int ped) {
    uint64_t a[6] = { (uint32_t)ped, 0, 0, 0, 0, 0 };
    nat_call(base, OFF_UPDATE_PED_VARIATION, a, 6);
}
static inline void gh_commit_active(uint64_t base, int ped) {
    uint64_t a[2] = { (uint32_t)ped, 0 };  // ismp = 0
    nat_call(base, OFF_SET_ACTIVE_META_COMPS_UPDATED, a, 2);
}
