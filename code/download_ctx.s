#!/bin/bash
#-----------------------------------------------------------------------------------------------
#
# Code originally written by Euibin Kim
#
# Last Modified: JULY 2025
#
# Code Description:
# This code will download CTX images given a list of image IDs.
#-----------------------------------------------------------------------------------------------
#
#     EXAMPLE CALL FOR THE SCRIPT FROM THE COMMAND LINE:
#          ./download_ctx.s CTX_ID1
#
# To set up this code on a new machine, change the path names of the '_loc' variable (Line 27).
#
#-----------------------------------------------------------------------------------------------

image_id=$1

# Cumulative index is needed for CTX.
# Up-to-date versions can be downloaded here by navigating to the
# most recent orbit and then looking in the folder 'index' for cumindex.tab
#    https://pds-imaging.jpl.nasa.gov/data/mro/ctx/

# Define location of CTX cumulative index table:
cumindex_loc="/Users/euibin/Desktop/Research/code/MRO/MRO/tab/cumindex.tab"
main_path_c=`echo http://pds-imaging.jpl.nasa.gov/data/mro/ctx/`

# Get volumes and file names for the CTX image
volume=`cat ${cumindex_loc} | grep ${image_id}.IMG | awk '{print tolower(substr($0,2,9))}'`
file=`cat ${cumindex_loc} | grep ${image_id}.IMG | awk '{print substr($0,14,35)}' | sed s/"DATA\/"//`

# Download the IMG file
wget ${main_path_c}${volume}/data/${file}
wait
