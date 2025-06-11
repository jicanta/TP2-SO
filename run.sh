#!/bin/bash

# Default QEMU command
qemu_command="qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512"

# Handle sound options
if [[ "$1" = "nosound" ]]; then
    # No additional sound options needed
    :
else
    qemu_command+=" -audiodev pa,id=speaker -machine pcspk-audiodev=speaker"
fi

# Handle GDB option
if [[ "$2" = "gdb" ]]; then
    qemu_command+=" -s -S"
fi

# Execute the final command
eval $qemu_command