#!/bin/bash
export LD_LIBRARY_PATH=/home/suprajs/cyclonedds_install/lib:/home/suprajs/cyclonedds-cxx_install/lib:$LD_LIBRARY_PATH
exec /home/suprajs/cyclonedds_install/bin/idlc "$@"
