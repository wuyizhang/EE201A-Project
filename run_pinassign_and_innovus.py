#!/usr/bin/env python

import sys
import os

if len(sys.argv) != 8:
	print "Usage: run_pinassign_and_innovus.py <INPUT_DESIGN_NAME> <OUTPUT_DESIGN_NAME> <BENCHMARK_PATH>"
	print "                                    ...  <HAS_CLK> <INPUT_RULE_FILE> <LOG_PATH> <OUTPUT_PATH>"
	print "Assumes input_design exists in OA DB"
	exit(1)

# Parse args
bench_name = sys.argv[1]
output_bench_name = sys.argv[2]
bench_path = sys.argv[3]
has_clk = sys.argv[4]
input_rule_file = sys.argv[5]
log_path = sys.argv[6]
output_path = sys.argv[7]

macro_info_file = log_path+"/pinassign_"+output_bench_name+".macros"

# ===========================================================================================================
# You may customize the flow below to have several iterations over PinAssign + Innovus
	# You must keep the structure of each command below intact in order to maintain the global flow
	# You may loop over the 3-step flow as necessary
	# You are responsible for any Innovus output log file parsing for PinAssign
		# For instance: You would want to parse Innovus Run1 log info for PinAssign Run2
	# Maximum Innovus runs = 5
	# It is NOT necessary to loop over the flow - every iteration will have a significant hit on runtime

# 1. Run compiled OA source code
os.system("./PinAssign "+bench_name+" "+output_bench_name+" "+input_rule_file+ \
		  " "+macro_info_file+" > "+log_path+"/pinassign_"+output_bench_name+".log")

# 2. Set up Innovus script 
os.chdir("innovus")
os.system("pwd")
os.system("./clean_invs.sh")
os.system("./setup_invs_script.py run_innovus.template run_innovus.tcl "+bench_name+ \
		  " "+output_bench_name+" ../"+input_rule_file+" "+has_clk+ \
		  " ../"+macro_info_file+" ../"+bench_path+" ../"+output_path+" ../"+log_path)
		
# 3. Run Innovus
os.system("innovus -nowin -init run_innovus.tcl -log ../"+log_path+"/innovus_"+output_bench_name)
os.chdir("..")

# ===========================================================================================================
