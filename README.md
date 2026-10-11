satchel-remover

GoldHEN .prx plugin that removes the player satchel in Red Dead Redemption 2
on PS4 (CUSA08519, game 1.29). Runtime mod: it calls RDR2's metaped component
natives on the player ped in a loop, no asset/RPF editing.

why a plugin and not an asset mod
RDR2 PS4 archives are RPF8 with an AES-encrypted TOC, the PFS mount is read-only,
and there is no loose-file loader - so the PC "invisible .ydd swap" cannot be
reproduced on console. The satchel is a data-driven metaped component (category
SATCHELS, joaat 0x94504D26), reachable only through the engine's native system.

how it works
loop ~300ms: get player ped, if it uses the SATCHELS component call
REMOVE_TAG_FROM_META_PED(ped, 0x94504D26, 0) then commit with _UPDATE_PED_VARIATION
and _SET_ACTIVE_META_PED_COMPONENTS_UPDATED. Re-applies after wardrobe reloads.
Fallback if coats break: _SET_META_PED_TAG to an empty asset (keeps component present).

layout
  notes/      verified RE notes (native addresses, NativeContext, offset map)
  src/        plugin source
  bin/        local decrypted eboot (gitignored, not committed)

caveats
  - coat-safety is not guaranteed; some coats may deform. fallback exists but
    perfect parity with the PC invisible-model trick is not possible on PS4
  - straps baked into coat meshes are coat data, so some coats keep a strap
  - all offsets pinned to CUSA08519 + game 1.29; a game update breaks them

status: RE done and verified, toolchain + plugin scaffold in progress
