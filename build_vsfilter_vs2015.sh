#!/bin/sh

sh build_vsfilter.sh -conf Release -plat Win32 -action rebuild -compiler VS2015

sh build_vsfilter.sh -conf Release -plat x64 -action rebuild -compiler VS2015

