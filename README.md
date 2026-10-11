satchel-remover

goldhen `.prx` plugin that removes the player satchel in red dead redemption 2 on ps4,
target `CUSA08519` game 1.29

runtime mod: it calls rdr2's metaped component natives on the player ped in a loop,
no asset or rpf editing

why a plugin, not an asset mod

- rdr2 ps4 archives are `RPF8` with an aes-encrypted toc and no public tool repacks them
- the pfs mount is read-only and there is no loose-file loader, so the pc invisible-`.ydd`
  swap cannot be reproduced on console
- the satchel is a data-driven metaped component, category `SATCHELS` joaat `0x94504D26`,
  reachable only through the engine native system

how it works

- loop ~300ms: get the player ped, if it uses `SATCHELS` call `REMOVE_TAG_FROM_META_PED`,
  then commit with `_UPDATE_PED_VARIATION` and `_SET_ACTIVE_META_PED_COMPONENTS_UPDATED`
- re-applies after wardrobe reloads
- fallback if coats break: `_SET_META_PED_TAG` to an empty asset, keeps the component present

layout

- `notes/` verified re notes: native addresses, nativecontext, offset map
- `src/` plugin source
- `bin/` local decrypted eboot, gitignored, not committed

caveats

- coat-safety is not guaranteed, some coats may deform, the fallback is a workaround not a
  perfect match for the pc invisible-model trick
- straps baked into coat meshes are coat data, so some coats keep a strap
- all offsets are pinned to `CUSA08519` game 1.29, a game update breaks them

status: re done and verified, toolchain and plugin scaffold in progress
