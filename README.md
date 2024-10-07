opencf

ColdFire programming tool

Angelo Dureghello <angelo@kernel-space.org>

1. Commands

write mem.b reg value
write mem.w reg value
write mem.l reg value

i.e.
write mem.l 0x20000000 0x20000001

1. Programming mcf5282 evb

mcf5282 has:
64KB internal SRAM
RAMBAR is the internal SRAM address, can be set as 0x20000000 fo 0x2fff0000

SDRAM is at

$ sudo ./opencf
opencf 0.90(alpha) starting
starting driver core ...
detecting programmer ...
detected: P&E Multilink Universal
fw version 9.60
detecting connected cpu ...
found: coldfire, v.2, rev.0, isa a+, sram 64KB
starting parser ...

# IPSBAR already enabled
# Datasheet error,
# at least from BDM rambar must be enabled (bit 0 to 1)
§ write reg rambar 0x20000001
§ load cf64k-5282.elf
§ write reg vbr 0x20000000
§ go

