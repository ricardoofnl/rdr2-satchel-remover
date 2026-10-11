#include <GoldHEN/Common.h>   // orbis libkernel, goldhen (proc_info), detour api
#include "offsets.h"
#include "natives.h"

#ifndef attr_public
#define attr_public __attribute__((visibility("default")))
#endif

attr_public const char *g_pluginName    = "satchel_remover";
attr_public const char *g_pluginDesc    = "hide rdr2 player satchel";
attr_public const char *g_pluginAuth    = "ricardoofnl";
attr_public uint32_t    g_pluginVersion = 0x00000100;

static Detour   g_detour;
static int      g_hooked = 0;
static uint64_t g_base = 0;

typedef void (*get_player_ped_t)(NativeCtx *);

// hook on the GET_PLAYER_PED handler. scripts ask for the player ped constantly, so this runs
// on the game script thread - the only safe place to call these natives. let the original run,
// read the ped it returned, then strip the satchel. self-limiting: once gone IS_USING is false.
static void hk_get_player_ped(NativeCtx *ctx) {
    Detour_Stub(&g_detour, get_player_ped_t, ctx);   // original fills ctx->ret with the ped
    if (!g_base || !ctx || !ctx->ret) return;

    static unsigned n = 0;
    if ((++n & 31) != 0) return;   // throttle, it is called many times per frame

    int ped = *(int *)ctx->ret;
    if (!ped) return;
    if (gh_is_using_satchel(g_base, ped)) {
        gh_remove_satchel(g_base, ped);
        gh_update_variation(g_base, ped);
        gh_commit_active(g_base, ped);
    }
}

int32_t module_start(size_t argc, const void *args) {
    struct proc_info pi;
    if (sys_sdk_proc_info(&pi) != 0) return 0;
    if (strcmp(pi.titleid, GAME_TITLEID) != 0) return 0;   // only rdr2 eu
    if (!strstr(pi.version, GAME_VERSION)) return 0;        // offsets are pinned to game 1.32
    if (pi.base_address == 0) return 0;

    g_base = pi.base_address;
    Detour_Construct(&g_detour, DetourMode_x64);
    Detour_DetourFunction(&g_detour, g_base + OFF_GET_PLAYER_PED, (void *)hk_get_player_ped);
    g_hooked = 1;
    return 0;
}

int32_t module_stop(size_t argc, const void *args) {
    if (g_hooked) {
        Detour_RestoreFunction(&g_detour);
        Detour_Destroy(&g_detour);
        g_hooked = 0;
    }
    return 0;
}
