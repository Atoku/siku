#!/bin/sh
#
#  Syncronizing documentation with web
#
SERVER=kappa.arsc.edu
USER=kulchits
WEBPATH=/web/tier34/LUNARREG/html/siku/devdoc/

CMD="rsync -avz -e ssh html/ ${USER}@${SERVER}:${WEBPATH}"
echo $CMD
$CMD
