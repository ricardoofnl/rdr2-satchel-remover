RDR2 PS4 satchel-remover - verified RE notes

target: CUSA08519 (EU), patch0 eboot = game ver 1.29, decrypted ELF x86-64
file: bin/eboot.bin (139018876 bytes, complete)
offset map: PT_LOAD exec seg file 0x4000 -> vaddr 0x0, so vaddr = fileoff - 0x4000
runtime: actual addr = sys_sdk_proc_info().base_address + vaddr (verify slide once on-console)

approach decided: GoldHEN .prx that calls RDR2 metaped natives on the player ped in a
persistent loop. asset/RPF route rejected (RPF8 encrypted TOC, read-only PFS, no loose
loader, no public repacker). the satchel is data-driven (lives in RPF), so there is no
static byte to patch - the leverage point is the native component system.

satchel identity (verified by joaat):
  category SATCHELS = joaat("satchels") = 0x94504D26   (cross-checked joaat("hats")=0x9925C067 ok)

native handler addresses - ALL VERIFIED by the anchor pattern below (hash->handler matched):
  anchor: each native registers as  48 b9 <hash:imm64>  (mov rcx,hash)  then
          48 8d 15 <disp32>  (lea rdx,[rip+handler]).  handler = rip_after_lea + disp32.
  registration stubs live ~vaddr 0x20f0000-0x2101000.

  native                                   hash                 handler_vaddr
  GET_PLAYER_PED                           0x275F255ED201B937   0x21b9140
  _GET_META_PED_TYPE                       0xEC9A1261BF0CE510   0x2145480
  _IS_META_PED_USING_COMPONENT             0xFB4891BD7578CDC1   0x21455e0
  _GET_NUM_COMPONENTS_IN_PED               0x90403E8107B60E81   0x2145e10
  _GET_CATEGORY_OF_COMPONENT_AT_INDEX      0x9B90842304C938A7   0x2145f10
  GET_META_PED_ASSET_GUIDS                 0xA9C28516A6DC9D56   0x214c320
  _SET_META_PED_TAG                        0xBC6DF00D7A4A6819   0x2145940
  REMOVE_TAG_FROM_META_PED                 0xD710A5007C2AC539   0x2145a60
  _UPDATE_PED_VARIATION                    0xCC8CA3E88256E58F   0x2146070
  _SET_ACTIVE_META_PED_COMPONENTS_UPDATED  0xAAB86462966168CE   0x214e040
  RESET_PED_COMPONENTS                     0x0BFA1BD465CDFEFD   0x2147090

native call convention (observed by decompiling the handlers above):
  handler is  void handler(NativeContext* ctx)  with ctx in RDI (PS4 = SysV AMD64 ABI)
  NativeContext layout (confirmed from REMOVE_TAG @0x2145a60 and GET_PLAYER_PED @0x21b9140):
    ctx + 0x00 : void*  m_pReturn  (handler writes result dword to *m_pReturn)
    ctx + 0x10 : void*  m_pArgs    (arg slots, 8 bytes each: arg0@+0x0, arg1@+0x8, arg2@+0x10)
  REMOVE_TAG reads arg0=ped(dword), arg1=component hash, arg2=p2 - matches native sig
  GET_PLAYER_PED: pass arg0 = 0xFF to get the LOCAL player ped (reads player globals)

core per-tick logic (pseudo):
  Ped p = GET_PLAYER_PED(0xFF)
  if (p && _IS_META_PED_USING_COMPONENT(p, 0x94504D26)) {
      REMOVE_TAG_FROM_META_PED(p, 0x94504D26, 0)        // primary; fallback _SET_META_PED_TAG->empty if coats break
      _UPDATE_PED_VARIATION(p, 0,0,0,0,0)
      _SET_ACTIVE_META_PED_COMPONENTS_UPDATED(p, 0)
  }
  loop ~250-500ms; must re-apply after wardrobe/outfit reload

still to find / do:
  - per-frame hook point on the game/script thread to run native calls from (do NOT call from
    a raw pthread). candidate: reuse illusion0001 Havana / Mizdx RDR2.prx 1.29 hook point.
  - confirm the scrNativeCallContext size to allocate (ret+args ptrs are enough for direct calls;
    allocate a generous zeroed struct and point m_pReturn/m_pArgs into local buffers)
  - build toolchain: OpenOrbis + GoldHEN_Plugins_SDK, LLVM on arm64 macOS (fix hardcoded intel
    /usr/local paths), or OpenOrbis Docker image
  - on-console tests: verify ped+category live, try REMOVE_TAG, check coats across several outfits

honest caveats:
  - coat-safety not guaranteed; plain removal MAY bug some coats (PC clean fix is an invisible
    .ydd swap we cannot do on PS4). fallback = _SET_META_PED_TAG to empty to keep component present.
  - straps baked into coat meshes are coat data, not the satchel component - some coats keep a strap.
  - everything pinned to CUSA08519 + 1.29; fail-closed on title/version mismatch.
