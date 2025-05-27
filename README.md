## ESP32-C3 SuperMini Hardware Debugger
A hardware debugger command line interface for the ESP32-C3 SuperMini.
Builds on top of my [hardware debugging library](https://github.com/ChuggleBug/libswd) and provides a command line interface to communicate to ARM M-Profile processors.

## Usage
### Control Flow
- Breakpoints:
	- `break [address]`: set a hardware breakpoint at `address`. Must be aligned for the instruction. Fails if the requested address is not valid or if no hardware breakpoints are availible
	- `b`: alias for `break`
- Resetting:
	- `reset`: triggers a system reset interrupt to restart the core
		- `catch`: additionally catches the reset vector and halts
- Resuming processor:
	- `continue`: continues the core
	- `c`: alias for `continue`
- Stepping the processor:
	- `next`/`step`: single instruction step
		- Because debug symbols will not be available, stepping a single line of code would not make sense, so these instructions are the same
	- `n`/`s`: alias for `next`/`step`
### Memory Management
- Register Information:
	- `register`: Manage registers on a target. Can be architecture dependent
		- `write [reg] [data]` writes 32 bit data to register
		- `read [reg]` reads 32 bit data held in register
		- `info`: Returns all possible registers which can be accessed on the target. Listed registers might not be available on some cores as they might be extensions[^1]
	- `reg`/`r`: alias for `register`
- Memory Management: 
	- `memory`: Manage memory on a target
		- `write [word/byte] [cnt] [base_addr] [data_0] ... [data_n]`:  writes up to `cnt` words or bytes starting at `base addr`
		- `read [word/byte] [cnt] [base_addr]`: reads up to `cnt` words or bytes starting at `base addr`
	- `mem`: Alias for `memory`
### Additional Features
- Information `info`:
	- `program`/`prog`: Shows the current state of the processor
	- `register`/`reg`: Shows architecture specific registers
	- `breakpoints`/`break`: Shows current active breakpoints
- Dap port `port`:
	- `read` `[dap_reg]`: read data from a dap register
	- `write [dap_reg] [data]`: write 32 bits of data to dap register. Some bits are read only
- Host operations `host`: 
	- `reset`: resets the host. Used for issues which cant be recovered from. Additionally resets the target and catches the reset vector

## Dependencies
The current build configuration should handle the required complilation, but they are listed here for completeness:
- https://github.com/ChuggleBug/libswd.git
- https://github.com/ChuggleBug/cli.git


[^1]: Subject to change