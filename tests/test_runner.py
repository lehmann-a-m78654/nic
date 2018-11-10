#!/usr/bin/python3

import subprocess
import base64
import json
import os
import sys

def run_test(testcase):
	with open(testcase,'r') as file:
		test_data = json.load(file)
	
	# write out the batch file, if present
	if 'batch' in test_data:
		buf = test_data['batch']
		buf = base64.b64decode(buf)
		bufout = file('batch', 'w')
		bufout.write(buf)
		bufout.close()
	
	# run all of the commands and test their outputs
	tests = test_data['tests']
	passed = True
	logs = set()
	for test in tests:
		test_input = test['input']
		args = test_input.strip().split(" ")
		
		# keep track of logs
		try:
			log = args[args.index('-F')+1]
			logs.add(log)
		except (ValueError, IndexError):
			# missing -F
			# nothing after -F
			pass
		
		# run the test
		args[0] = './' + args[0]
		p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		out,_err = p.communicate()
		ret = p.returncode
		out = out.rstrip().decode('utf-8')
		
		#check and see if out,ret match what is expected in this test
		expected_out = test['output'] if 'output' in test else ""
		expected_ret = test['exit'] if 'exit' in test else 0
		if out != expected_out\
		   or ret != expected_ret:
			print('         out: %s' % (repr(out)))
			print('expected out: %s' % (repr(expected_out)))
			passed = False
			# fail fast
			break;
	print('Passed' if passed else 'Failed')
	
	# delete batch
	if 'batch' in test_data:
		if os.path.exists('batch'):
			os.remove('batch')
	
	# delete logs
	for log in logs:
		if os.path.exists(log):
			os.remove(log)	

if __name__ == '__main__':
	if len(sys.argv) != 2:
		print('usage: %s <testcase>' % (sys.argv[0]))
	else:
		if os.path.exists('./logappend')\
		   and os.path.exists('./logread'):
			testcase = sys.argv[1]
			run_test(testcase)
		else:
			print('put %s in the same directory as logappend and logread (did you forget to run make)' % sys.argv[0])