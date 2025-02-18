![](opencf.png)

# opencf - a ColdFire programming tool

opencf is an opensource tool to program ColdFire cpus. Actually, 
it supports the Multilink Universal pod only.

Credits: Angelo Dureghello <angelo@kernel-space.org>

## Build
```
./configure
make
sudo make install
```

## Usage
```
sudo ./opencf
sudo ./opencf                                                          1 ✘ 
opencf 0.90(alpha) starting
starting driver core ...
detecting programmer ...
detected: P&E Multilink Universal
fw version 9.60
detecting connected cpu ...
found: coldfire, v.2, rev.0, isa a+, sram 64KB
starting parser ...
§
```

## Commands

```
§ help
Available commands:
exit
  exit application
go
  execute continuously
halt
  stop execution
help
  this help
load
  load elf executable
quit
  exit alias, exit application
read
  read memory or register:
    read mem.b location    read one byte from memory
    read mem.w location    read two bytes from memory
    read mem.l location    read four bytes from memory
    read reg name          read special register
    special registers: pc, vbr, rambar, sp, sr
regs
  dump cpu registers
st
  step alias, shorted
step
  step
write
  write memory or register:
    write mem.b location val    write one byte to memory
    write mem.w location val    write two bytes to memory
    write mem.l location val    write four bytes to memory
    write reg name val          write special register
    special registers: pc, vbr, rambar, sp, sr
Keys:
key enter
  repeat last command
§
```


## Examples

### Programming mcf5282 evb

mcf5282 has:
64KB internal SRAM
RAMBAR is the internal SRAM address, can be set as 0x20000000 to 0x2fff0000

```
$ sudo ./opencf
opencf 0.90(alpha) starting
starting driver core ...
detecting programmer ...
detected: P&E Multilink Universal
fw version 9.60
detecting connected cpu ...
found: coldfire, v.2, rev.0, isa a+, sram 64KB
starting parser ...

# If from BDM, rambar must be enabled (bit 0 to 1)
§ write reg rambar 0x20000001
# not enabling IPSBAR (default ok, not needed)
# second RAMBAR (rambar2) not needed (DMA only usage)
§ load cf64k.elf
# VBR can be set from the code, as in cf64k-5282.elf
§ go
```

