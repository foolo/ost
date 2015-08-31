#!/bin/bash

# Eclipse calls gdb twice. First time with --version
if [[ $2 != --version ]]; then
  qemu-system-i386 -gdb tcp::1234 -S -cdrom $1 &
fi

# Shift and pass parameters to gdb
shift
exec gdb "$@"

