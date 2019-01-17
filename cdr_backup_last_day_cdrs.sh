#!/bin/bash


cd /home/omn/wosmex_cdrs/arh; 
data=$(/bin/date --date="41 day ago" +%Y%m%d)
# Mack arhive of files
/bin/tar -zcf CDRsZh-sgw-2-$data.tgz CDR_$data*sgw-2*
# Remove after tgz
/bin/rm -f CDR_$data*sgw-2*
# Cope file to backup srv auto remove when succssed
/usr/bin/sshpass -p gfhjkmlkzclhjd /usr/bin/rsync -avz --remove-source-files  /home/omn/wosmex_cdrs/arh/CDRsZh-sgw-2*.tgz sgw-2@10.11.2.160:cdrs_arh/
