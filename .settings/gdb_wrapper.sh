#!/bin/bash

# Usage:   launch_bash_debugger.sh  EXTRA_COMMAND  GDB_ARGS  [--version]
# Example: launch_bash_debugger.sh: "qemu-system-i386 -gdb tcp::1234 -S -kernel kernel.bin" <GDB_ARGS>

for LAST_ARG; do true; done  #get last argument

EXTRA_COMMAND=$1

# Eclipse calls gdb twice. First time with --version.
if [[ $LAST_ARG != --version ]]; then
	bash -c "$EXTRA_COMMAND &"
fi

# Shift and pass parameters to gdb
shift
exec gdb "$@"

