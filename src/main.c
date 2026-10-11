#include <GoldHEN/Common.h>   // orbis libkernel + goldhen (proc_info, sys_sdk_proc_info)
#include "offsets.h"
#include "natives.h"

#ifndef attr_public
#define attr_public __attribute__((visibility("default")))
#endif

attr_public const char *g_pluginName    = "satchel_remover";
attr_public const char *g_pluginDesc    = "hide rdr2 player satchel";
attr_public const char *g_pluginAuth    = "ricardoofnl";
attr_public uint32_t    g_pluginVersion = 0x00000100;

static volatile int g_run = 0;
static OrbisPthread  g_thr = 0;
static uint64_t      g_base = 0;

static void tick(uint64_t base) {
    int ped = gh_get_player_ped(base);
    if (!ped) return;
    if (gh_is_using_satchel(base, ped)) {
        gh_remove_satchel(base, ped);
        gh_update_variation(base, ped);
        gh_commit_active(base, ped);
    }
}

// natives from this side thread can crash - they touch pools the game thread uses.
// real fix is to run tick() from a per-frame game hook (todo)
static void *worker(void *arg) {
    uint64_t base = (uint64_t)arg;
    while (g_run) {
        tick(base);
        sceKernelUsleep(300 * 1000);  // re-apply after wardrobe reloads
    }
    return NULL;
}

int32_t module_start(size_t argc, const void *args) {
    struct proc_info pi;
    if (sys_sdk_proc_info(&pi) != 0) return 0;
    if (strcmp(pi.titleid, GAME_TITLEID) != 0) return 0;   // only rdr2 eu
    if (!strstr(pi.version, GAME_VERSION)) return 0;        // offsets are pinned to game 1.32
    if (pi.base_address == 0) return 0;

    g_base = pi.base_address;
    g_run = 1;
    scePthreadCreate(&g_thr, NULL, worker, (void *)g_base, "satchel");
    return 0;
}

int32_t module_stop(size_t argc, const void *args) {
    g_run = 0;
    if (g_thr) scePthreadJoin(g_thr, NULL);
    return 0;
}
