#pragma once
#include <stdint.h>

// rdr2 ps4 CUSA08519, game 1.29. vaddrs = file offset - 0x4000 in bin/eboot.bin.
// runtime address = proc base_address + vaddr. all handlers verified via the native
// registration pattern (mov rcx,<hash> ; lea rdx,[rip+handler]). see notes/natives.md.

#define GAME_TITLEID  "CUSA08519"
#define GAME_VERSION  "01.29"   // sys_sdk_proc_info().version, checked before we touch anything

// metaped category hash, joaat("satchels")
#define HASH_SATCHELS 0x94504D26u

// native handler offsets from eboot base
#define OFF_GET_PLAYER_PED                0x21b9140ULL
#define OFF_GET_META_PED_TYPE             0x2145480ULL
#define OFF_IS_META_PED_USING_COMPONENT   0x21455e0ULL
#define OFF_GET_NUM_COMPONENTS_IN_PED     0x2145e10ULL
#define OFF_GET_CATEGORY_OF_COMPONENT_IDX 0x2145f10ULL
#define OFF_GET_META_PED_ASSET_GUIDS      0x214c320ULL
#define OFF_SET_META_PED_TAG              0x2145940ULL
#define OFF_REMOVE_TAG_FROM_META_PED      0x2145a60ULL
#define OFF_UPDATE_PED_VARIATION          0x2146070ULL
#define OFF_SET_ACTIVE_META_COMPS_UPDATED 0x214e040ULL
#define OFF_RESET_PED_COMPONENTS          0x2147090ULL
