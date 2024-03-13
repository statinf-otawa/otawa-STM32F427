# Support for STM32F427 micro-architecture in OTAWA.


## Dependencies
- otawa support for armv7 ISA : [armv7t](https://github.com/statinf-otawa/armv7t)
- otawa loader for arm : [otawa-arm](https://github.com/statinf-otawa/otawa-arm)

## Compilation
```bash
    cmake . && make install
```

## Usage
```bash
    owcet -s stm32 path/to/elf/file functionName [-p CONFIG=x]
```
