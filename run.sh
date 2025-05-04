#!/bin/bash

if [ -z "$XDG_RUNTIME_DIR" ]; then
  export XDG_RUNTIME_DIR="/run/user/$(id -u)"
  if [ ! -d "$XDG_RUNTIME_DIR" ]; then
    mkdir -p "$XDG_RUNTIME_DIR"
    chmod 700 "$XDG_RUNTIME_DIR"
  fi
fi

if [ "$1" = "-d" ]; then
  # Flag -S: freeze CPU at startup
  # Flag -s: wait for GDB connection
  qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512
else 
  qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
fi