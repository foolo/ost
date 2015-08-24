#!/bin/bash

# Eclipse calls gdb twice. First time with --version
if [[ $2 != --version ]]; then
  qemu-system-x86_64 -gdb tcp::1234 -S -kernel $1 &
fi

# Shift and pass parameters to gdb
shift
exec gdb "$@"

