# Support for STM32F427 micro-architecture in OTAWA.


## Dependencies
- otawa support for armv7 ISA : [armv7t](https://github.com/statinf-otawa/armv7t)
- otawa loader for arm : [otawa-arm](https://github.com/statinf-otawa/otawa-arm)
- [otawa-clp](https://git.renater.fr/anonscm/git/otawa/otawa-clp.git)
- otawa support for dcache mem : [otawa-dcache](https://github.com/statinf-otawa/otawa-dcache)

## Compilation
```bash
    cmake . && make install
```

## Usage
### For arm cortex M4
```bash
    owcet -s stm32 path/to/elf/file functionName [-p CONFIG=x]
```

### For arm cortex M7
```bash
    owcet -s stm32 path/to/elf/file functionName -p ARCH=1 [-p CONFIG=x]
```