#!/bin/bash

PROGRAMDIR="/home/omn/wosmex_cdr_parser_zh"
CDRDIR="/home/omn/wosmex_cdrs"

cd $PROGRAMDIR

d=$(date +%Y-%m-%d)
for f in $CDRDIR/CDR*sgw-2*
do
  if [ -f $f ]; then
    echo -e "\nProcessing $f file..."  >> "$PROGRAMDIR"/logs/"$d.log"
    SECONDS=0
    $PROGRAMDIR/cdr_parser.bin $f  >> "$PROGRAMDIR"/logs/"$d.log"
    if [ $? -eq 0 ];
    then
    echo "\nParse to DB correctly in $SECONDS seconds!"  >> "$PROGRAMDIR"/logs/"$d.log"
    rm -f $f
    sleep 1
    else
    echo "\nFail parse to DB!"  >> "$PROGRAMDIR"/logs/"$d.log"
    fi
  fi
done

exit 0