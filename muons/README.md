# What's here?
This repo is not meant for self-contained functional code st this point (although this is the eventual goal).
It's primary role is to support documentation currently kept in the Phenix Offline Wiki. Most of the materials
were transferred from the original project:

   /gpfs02/phenix/fvtx/subsys/fvtx/ajeeta/Simulation

# Caveat Emptor
Please note that the material here may contain modifications needed for basic testing, where appropriate the scripts will contain the signage "-mxp-" to mark the code locations where (obvious) changes were made.

# Local dependencies
This is work in progress, so subject to change. The script and macro make an assumption that
the following files are local, so they have been added to the repo:
* realdataBG-run15pp_file.list
* svxPISA.par
* fvtx_geometry.root

# Updated driver script

The "full" script contains extra ROOT macros not present in the original version,
the "new" script is work in progress and is expected to be portable.
