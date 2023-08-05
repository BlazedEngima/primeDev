#!/bin/bash
mknod /dev/primeDev c $1 $2
chmod 666 /dev/primeDev
ls -l /dev/primeDev
