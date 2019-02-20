#!/bin/bash

rm -rf hls_proj vivado_hls.log

vivado_hls -f hls_script.tcl
