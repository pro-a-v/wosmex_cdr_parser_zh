#!/bin/bash

count=$(ps -ef | grep -v 'grep' | \
                 grep -v "$$.*${0##*/}" | \
                 grep -v "su  *-  *omn.*${0##*/}" | \
                 grep -c "${0##*/}")

[ $count -gt 0 ] && { echo "ERROR: $0 is already running! Exiting ..." ; exit 5; }

while true; do cd /home/omn/wosmex_cdr_parser_zh; ./cdr_parser.sh; done
