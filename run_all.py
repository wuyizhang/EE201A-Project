#!/usr/bin/env python

# EE201A Winter 2018 Course Project
# This script automates the complete pin assignment flow.

import sys
import os
import re

#==========================================================================================

# Complete list of benchmarks stored as ["design_name", has_clk], and list of input rules 
	# where "has_clk" indicates whether the design contains a clock - useful for Innovus.
bench_list = [ ["sbox_x2", False], ["sbox_x2_vert", False] ]
rule_list = ["min", "rand", "max"]

# You can comment the above and run a subset instead for debug. For example:
#bench_list = [ ["sbox_x2", False] ]
#rule_list = ["min"]

#==========================================================================================

# Path definitions
bench_parent_path = "benchmarks"
log_parent_path = "logs"
output_parent_path = "outputs"

def main():
	print "\nCleaning old files..."
	os.system("./clean_all.sh")

	print "\nInitializing directories..."
	os.system("mkdir "+log_parent_path)
	os.system("mkdir "+output_parent_path)

	print "\nInitiating runs..."
	print "\n========================================================================"

	for i, bench in enumerate(bench_list):
		[bench_name, has_clk] = bench
		bench_path = bench_parent_path+"/"+bench_name
		log_path = log_parent_path+"/"+bench_name
		output_path = output_parent_path+"/"+bench_name
		os.system("mkdir "+log_path)
		os.system("mkdir "+output_path)

		for rule in rule_list:
			print "\nRunning benchmark "+bench_name+" with "+rule+" input rules..." 
			os.system("./clean.sh")
			output_bench_name = bench_name+"_"+rule+"rule"
			input_rule_file = bench_path+"/"+rule+".inputrules"

			print "\nImporting design to OA..."
			os.system("./oaDBCreator.py "+bench_path+" "+bench_name+" >> " \
					   +log_path+"/oaDBCreator_"+output_bench_name+".log")	

			print "\nBeginning source code + Innovus run..."
			os.system("./run_pinassign_and_innovus.py "+bench_name+" "+output_bench_name+" " \
					   +bench_path+" "+str(has_clk)+" "+input_rule_file+" " \
					   +log_path+" "+output_path)

			print "\n========================================================================"


	print "\n==============="
	print "Flow complete!"
	print "==============="


if __name__ == "__main__":
    main()
