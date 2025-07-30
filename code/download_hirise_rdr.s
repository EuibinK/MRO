#!/bin/bash
#-----------------------------------------------------------------------------------------------
#
# Code originally written by Euibin Kim
#
# Last Modified: September 2023
#
# Code Description:
# This code will download processed HiRISE images given a list of image IDs.
#-----------------------------------------------------------------------------------------------
#
#     EXAMPLE CALL FOR THE SCRIPT FROM THE COMMAND LINE:
#          ./download_hirise.s HiRISE_ID1
#
# To set up this code on a new machine, change the path names of the '_loc' variable (Line 26).
#
#-----------------------------------------------------------------------------------------------

image_id=$1

# Cumulative index is needed for HiRISE.
# Up-to-date HiRISE index (RDRCUMINDEX.tab) is located here:
#   https://hirise-pds.lpl.arizona.edu/PDS/INDEX/

# Define location of CTX cumulative index table:
cumindex_loc="/Users/euibin/Desktop/Research/code/MRO/MRO/tab/RDRCUMINDEX.tab"
main_path_h=`echo https://hirise-pds.lpl.arizona.edu/PDS`
end_path_h=`cat ${cumindex_loc} | grep ${image_id}_RED | awk '{print substr($0,15,67)}' | sed s/"${image_id}_RED\.JP2"//`
path_h=`echo ${main_path_h}/${end_path_h}`

# Check if the cumindex file is up to date and contains the files of interest.
# If not, code will download ALL files in the HIRISE data.
image_path_check=`cat ${cumindex_loc} | grep ${image_id}_RED | wc -l`
if [ ${image_path_check} -ne 0 ]; then
  wget -r -nd -np "${path_h}" -A "*.JP2"
  wait
fi
