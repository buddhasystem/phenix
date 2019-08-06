# This file should be modified by the user according to their
# set up and needs, it is best to create a copy, modify it
# and run.
#
# Please DON'T check the modified version into the repository.


#!/usr/local/bin/tcsh -f

setenv EMBEDDING_SET	"YES"

setenv DATADIR		/phenix/u/mxmp/embedding_data
setenv MYINSTALL	/phenix/u/mxmp/install

if ( -d $DATADIR) then
    echo Data directory $DATADIR found

else
    echo Data directory $DATADIR not found, please check, existing
    exit -1
endif

if ( -d $MYINSTALL) then
    echo Data directory $MYINSTALL found

else
    echo Install directory $MYINSTALL not found, please check, existing
    exit -1
endif

exit

