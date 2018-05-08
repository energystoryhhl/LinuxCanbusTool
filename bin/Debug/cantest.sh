#!/bin/sh
echo ***canbus test script
ip -details -statistics link show can0
ip -details -statistics link show can1

