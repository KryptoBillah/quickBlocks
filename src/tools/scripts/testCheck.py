#!/usr/bin/env python

#########################################################################################################################################
# This script receives as input the exact command we have to execute and its parameters. 
# Next-to-last parameter is the output file
# Lat parameter is the gold file we need to compare with
# (Both files can be relative or absolute paths)
#
# Example: run_anc_compare.py ethName 0x1 test/tools/tests/test1.txt test-gold/tools/tests/test1.txt 
#
# The parameters are (N: total number):
# argv[0] the name of the python script
# argv[1] the name of the command we want to run (absolute path encouraged)
# argv[2] -> argv[N-3] are the parameters passed to the command
# argv[N-2] is the output file where result is stored (this one will be compared with the gold copy at the same path but under test_gold)
# argv[N-1] is the gold copy we want to compare with
#
#########################################################################################################################################

from __future__ import print_function
import os
import sys
import subprocess
import filecmp
import errno

#-------------------------------------------------------
# Print to standard error
#-------------------------------------------------------
def printe(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

#-------------------------------------------------------
# Function to compare 2 txt files
# We receive both files as input param
#-------------------------------------------------------
def cmp_files(a, b):
        return filecmp.cmp(a, b)


#-------------------------------------------------------
# Delete a file if it exists
#-------------------------------------------------------
def delete_file(file):
    #printe("Deleting file %s" % (file))
    try:
        os.remove(file)
    except OSError as e:
        # Ignore when file is not present, we skip it silently
        if(e.errno != errno.ENOENT):
            printe("ERROR: %s for file %s" % (e.strerror, file))
            exit(2)

#-------------------------------------------------------
# Delete any cache files for input addr
#-------------------------------------------------------
def clear_cache(addr):
    # Define the path where we expect the quickblocks cache files
    #
    # NOTE: THIS IS BROKEN SINCE WE MOVED ./slurp AND ./abis TO
    #       THE BINARY CACHE. THIS WON'T WORK
    #
    QUICKBLOCKS_DIR_NAME = '.quickblocks'
    SLURP_DIR_NAME = 'slurps'
    ABIS_DIR_NAME = 'abis'

    home = os.environ['HOME']

    # Build cache files path for input address
    slurp_cache = home + '/' + QUICKBLOCKS_DIR_NAME + '/' + SLURP_DIR_NAME + '/' + addr + '.bin'
    abi_cache = home + '/' + QUICKBLOCKS_DIR_NAME + '/' + ABIS_DIR_NAME + '/' + addr + '.abi'
    json_cache = home + '/' + QUICKBLOCKS_DIR_NAME + '/' + ABIS_DIR_NAME + '/' + addr + '.json'

    # Delete them
    delete_file(slurp_cache)
    delete_file(abi_cache)
    delete_file(json_cache)

    # OK
    print("Cache files for [%s] succesfully deleted" % addr)

#-------------------------------------------------------
# Main program
#-------------------------------------------------------

# Debugging input array
# printe(sys.argv)

# When at environment we have defined an address, we clear its cache before running the test
cache_addr = os.getenv('CACHE_ADDR')

if cache_addr:
    #printe("Deleting cache_addr value %s" % (cache_addr))
    clear_cache(cache_addr)

# Check input parameters number
param_number = len(sys.argv)

# We need at least the python script [0], the command to be executed [1] the output file [2] and the gold file [3]
if param_number <= 3:
    printe("ERROR: Invalid parameters number, at least 3 expected")
    exit(1)

# Get the output/gold files
output_file = sys.argv[param_number-2]
gold_file = sys.argv[param_number-1]

# Check that gold file is present
if os.path.isfile(gold_file) == False:
    printe("ERROR: Could not find gold file %s" % gold_file)
#    exit(2)

#Debug
#printe("%d parameters received, output file is %s gold file %s" % (param_number, output_file, gold_file))

# Build the exact command we want to run as list of items, pick the first ones from argv
# Then add the output file and redirections
command = sys.argv[1:-2]

# Debug only
# printe(command)

# Open output file and execute the command with redirections
with open(output_file, 'w') as f:
    os.chdir(os.path.dirname(gold_file))
#    printe(os.getcwd())
    os.environ["TEST_MODE"] = "true"
    os.environ["NO_COLOR"] = "true"
    result = subprocess.call(command, stdout=f, stderr=subprocess.STDOUT)

if result:
    printe("ERROR: Command execution failed with error %d" % result)
    exit(result)

# Command result is OK - compare output file with gold one
if cmp_files(output_file, gold_file):
    exit(0)
else:
    printe("ERROR: Differences found comparing %s with %s" % (output_file, gold_file))
    exit(3)

