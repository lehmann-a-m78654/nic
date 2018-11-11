#!/usr/bin/env python3

import os
import argparse
import subprocess
import base64
import json
import binascii
from time import time
from shutil import rmtree

# 30 seconds per test case
TIME_PER_TESTCASE = 30
TIME_PER_TESTCASE_PERF = 60

def get_size(start_path = '.'):
    total_size = 0
    for dirpath, dirnames, filenames in os.walk(start_path):
        for f in filenames:
            fp = os.path.join(dirpath, f)
            total_size += os.path.getsize(fp)
    return total_size

def add_logs(logs, args):
    # keep track of logs created
    # protect against naughty -F paths
    for i in range(args.count('-F')):
        try:
            idx = args.index('-F')+1
            log = args[idx]
            idx += 1
            if os.path.exists(log):
                # if "log" file already exists
                # might not be a log file
                if log in logs:
                    # it's a log file we already know about
                    # don't add it
                    pass
                else:
                    # it's not a log file created during this test
                    # don't add it (don't delete it later)
                    pass
            else:
                # log path does not exist yet
                # OK for new log
                logs.add(log)
        except ValueError:
            # missing -F
            pass
        except IndexError:
            # nothing after -F
            pass

def mkdtemp():
    timestamp = time()
    tmp_dir = 'tmp_' + str(timestamp)
    while os.path.exists(tmp_dir):
        timestamp = time()
        tmp_dir = 'tmp_' + str(timestamp)
    try:
        os.mkdir(tmp_dir)
    except FileExistsError as err:
        # directory already exists
        # this should not have happened
        #print('[FATAL] could not create temporary directory')
        return None
    return tmp_dir

def run_test(args, time_remaining, allowshell):
    try:
        if allowshell:
            p = subprocess.Popen(' '.join(args), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        else:
            p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out,err = p.communicate(timeout=time_remaining)
        ret = p.returncode
        out = out.decode('utf-8')
        err = err.decode('utf-8')
        return out, err, ret
    except subprocess.TimeoutExpired:
        p.kill()
        _out,_err = p.communicate()
        #print('[TIMEOUT] time limit exceeded')
        return None,'[TIMEOUT] time limit exceeded',None
    except (ValueError, FileNotFoundError, PermissionError, OSError) as err:
        #print('[WARNING] error: %s' % (err))
        return None,'[WARNING] error: %s' % (repr(err)),None
    return None,None,None

def check_result(out,err,ret,test):
    expected_out = test['output'] if 'output' in test else ''
    expected_ret = test['exit'] if 'exit' in test else 0        
    expected_err = test['error'] if 'error' in test else ''
    err_mssg = ''
    if out.rstrip() != expected_out.rstrip():
        err_mssg += '[out] got "%s" expected "%s";' % (out.replace('\n','\\n'), expected_out.replace('\n','\\n'))
    if err.rstrip() != expected_err.rstrip():
        err_mssg +=  '[err] got "%s" expected "%s";' % (err.replace('\n','\\n'), expected_err.replace('\n','\\n'))
    if ret != expected_ret:
        err_mssg +=  '[ret] got %d expected %d;' % (ret, expected_ret)
    return err_mssg

def validate_test_data(test_data):
    if 'tests' not in test_data:
        return False, 'missing required property: tests'

    if type(test_data['tests']) is not list:
        return False, 'tests has wrong type: must be array'

    idx = 0
    for test in test_data['tests']:
        if 'input' not in test:
            return False, 'tests[%d] missing required property: input' % (idx)

        if type(test['input']) is not str:
            return False, 'tests[%d].input has wrong type: must be string' % (idx)

        if 'output' in test and type(test['input']) is not str:
            return False, 'tests[%d].output has wrong type: must be string' % (idx)

        if 'exit' in test:
            if type(test['exit']) is not int:
                return False, 'tests[%d].exit has wrong type: must be int' % (idx)
            if test['exit'] < 0 or test['exit'] > 255:
                return False, 'tests[%d].exit value out of bounds: must be in [0-255]' % (idx)

        if 'error' in test and type(test['error']) is not str:
            return False, 'tests[%d].error has wrong type: must be string' % (idx)

        if 'shell' in test and type(test['shell']) is not bool:
            return False, 'tests[%d].shell has wrong type: must be bool' % (idx)

        idx +=1

    if 'batch' in test_data:
        try:
            buf = base64.b64decode(test_data['batch'], validate=True)
        except binascii.Error:
            return False, 'batch value is invalid base64'

    if 'performance' in test_data:
        if type(test_data['performance']) is not str:
            return False, 'performance has wrong type: must be string'
        if test_data['performance'] not in ['time','space']:
            return False, 'performance value out of bounds: must be time or space'

    # TODO be strict about this next time
    #if 'break' in test_data:
    #    if type(test_data['break']) is not str:
    #        return False, 'break has wrong type: must be string'
    #    if test_data['break'] not in ['correctness','crash','security']:
    #        return False, 'break value out of bounds: must be correctness, crash, or security'

    return True,None

def clean_up(tmp_dir, logs):
    # rm temporary directory (contains batch and legal log files)
    try:
        # speak softly
        rmtree(tmp_dir, ignore_errors=True)
    except RuntimeError:
        # carry a big stick
        os.system('rm -rf ' + tmp_dir)

    # rm (illegal) logs that may have escaped
    for log in logs:
        if os.path.exists(log):
            os.remove(log)

def main(args):
    testin, prefix, keeplog, verbose, allowshell = args.test, args.prefix, args.keeplog, args.verbose, args.allowshell

    try:
        with open(testin, 'r') as file:
            test_data = json.load(file)
    except ValueError:
        print('{"passed":false,"error":"[FATAL] invalid JSON"}')
        return

    is_valid, reason = validate_test_data(test_data)
    if not is_valid:
        print('{"passed":false,"error":"%s"}' % (reason))
        return

    # make a temporary directory for testing
    tmp_dir = mkdtemp()
    if tmp_dir is None:
        # failed
        print('{"passed":false,"error":"[FATAL] could not create temporary directory"}')
        return

    # keep track of logs created
    logs = set();

    # write out the batch file, if present
    if 'batch' in test_data:
        buf = test_data['batch']
        try:
            buf = base64.b64decode(buf, validate=True)
        except binascii.Error:
            print('{"passed":false,"error":"[FATAL] batch value is invalid base64"}')
            clean_up(tmp_dir, logs)
            return
        buf = buf.decode('utf-8')
        with open(os.path.join(tmp_dir,'batch'), 'w') as bufout:
            bufout.write(buf)

        # logs created by batch commands
        for line in buf.split('\n'):
            args = line.strip().split()
            add_logs(logs, args)

    # run all of the commands and test their outputs
    tests = test_data['tests']
    passed = True
    total_time = 0
    err_mssg = ''
    if 'performance' in test_data:
        time_remaining = TIME_PER_TESTCASE_PERF
    else:
        time_remaining = TIME_PER_TESTCASE
    out = None
    ret = None
    for test in tests:
        inpt = test['input']
        args = inpt.strip().split(" ")

        cmd = args[0]
        # protect against naughty cmds
        use_shell = False or ('shell' in test and test['shell'])
        if cmd in ['logappend', 'logread']:
            if cmd == 'logappend':
                add_logs(logs, args)
            cmd = os.path.abspath(os.path.join(prefix,cmd))
        elif allowshell:
            # I hope you know what you're doing...
            use_shell = True
        else:
            continue
        args[0] = cmd

        if use_shell:
            with open('shell_log.txt','a') as shell_log:
                shell_log.write('%d,%s,%s\n' % (time(),testin,inpt))

        # cd to temp directory
        prev_dir = os.getcwd()
        os.chdir(os.path.abspath(tmp_dir))

        # run the test
        if verbose:
            print("[INFO] running%s %s" % ("(shell)" if use_shell else "", inpt))
        t1 = time()
        out,err,ret = run_test(args, time_remaining, use_shell)
        t2 = time()
        dt = t2-t1
        total_time += dt
        time_remaining = max(0,time_remaining - (dt))
        if out is None:
            passed = False
            err_mssg = err
            break

        # cd back
        os.chdir(prev_dir)

        # check and see if out,err,ret match what is expected in this test
        err_mssg2 = check_result(out,err,ret,test)
        passed &= err_mssg2 == ''
        err_mssg += err_mssg2
        if verbose and err_mssg2 != '':
            print(err_mssg2)
        if not verbose and not passed:
            # short circuit on fail
            break

    if passed and 'performance' in test_data and test_data['performance'] == 'space':
        size = get_size(tmp_dir)
    else:
        size = -1

    if verbose:
        print('[INFO] %s' % ('PASSED' if passed else 'FAILED'))
        print('[INFO] time = %f' % (total_time))
        print('[INFO] space = %d' % (size))
        print('[INFO] errors:\n\t%s' % ('\n\t'.join(err_mssg.split(';'))))

    if err_mssg and "TIMEOUT" in err_mssg:
        print('{"passed":%s,"timeout":true}' % (str(passed).lower()))
    elif out and out.rstrip() == "integrity violation":
        print('{"passed":%s,"integrity_violation":true}' % (str(passed).lower()))
    elif out and out.rstrip() == "invalid":
        print('{"passed":%s,"invalid":true}' % (str(passed).lower()))
    elif ret and ret not in [0,255]:
        print('{"passed":%s,"crash":true}' % (str(passed).lower()))
    else:
        print('{"passed":%s}' % (str(passed).lower()))
    if not keeplog:
        clean_up(tmp_dir, logs)

if __name__ =='__main__':
    parser = argparse.ArgumentParser(description='Test executor')
    parser.add_argument('-p', '--prefix', dest='prefix', type=str, default="./", help='program prefix, default: ./')
    parser.add_argument('-t', '--test', dest='test', type=str, default="test.json", required=True, help='test input')
    parser.add_argument('-k', '--keep-logfile', dest='keeplog', action='store_true', help='do not auto-erase the temporary directory containing log file output and batch file')
    parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='print commands as they run and any errors')
    parser.add_argument('--allow-shell', dest='allowshell', action='store_true', help='allow shell commands to be executed')

    args = parser.parse_args()
    main(args)