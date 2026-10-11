# satchel-remover

A GoldHEN `.prx` plugin that removes the player satchel in Red Dead Redemption 2 on PS4
(`CUSA08519`, game 1.32). It is a runtime mod: it drives RDR2's metaped component natives on
the player ped from an in-game hook, with no asset or RPF editing.

## Why a plugin, not an asset mod

- RDR2 PS4 archives are `RPF8` with an AES-encrypted table of contents, and no public tool
  repacks them.
- The PFS mount is read-only and there is no loose-file loader, so the PC "invisible `.ydd`"
  swap cannot be reproduced on console.
- The satchel is a data-driven metaped component (category `SATCHELS`, joaat `0x94504D26`),
  reachable only through the engine's native system.

## How it works

- Hooks the `GET_PLAYER_PED` native handler, which the game's scripts call constantly on the
  script thread, so the plugin's own native calls run on a safe thread.
- When the player ped carries the `SATCHELS` component it calls `REMOVE_TAG_FROM_META_PED`,
  then commits with `_UPDATE_PED_VARIATION` and `_SET_ACTIVE_META_PED_COMPONENTS_UPDATED`.
- It is self-limiting, and re-applies after a wardrobe change streams the satchel back in.

## Building

The plugin is built in CI (GitHub Actions) with the OpenOrbis PS4 Toolchain and the GoldHEN
SDK, which produces `satchel_remover.prx` as a build artifact. See `.github/workflows/build.yml`.

## Install

1. Copy `satchel_remover.prx` to `/data/GoldHEN/plugins/` on the PS4.
2. Add it under the `[CUSA08519]` section of `/data/GoldHEN/plugins.ini`.
3. Enable the GoldHEN Plugin Loader and relaunch RDR2.

## Caveats

- Coat-safety is not guaranteed, some coats may deform. The clean PC invisible-model trick
  needs an asset swap that is not possible on PS4.
- Straps baked into coat meshes are part of the coat data, not the satchel component, so some
  coats will still show a strap.
- All offsets are pinned to `CUSA08519` game 1.32, so a game update will break them.
