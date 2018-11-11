#!/usr/bin/env python3
import os
import sys
import json

if __name__ == "__main__":
    f = open(sys.argv[2], 'r')
    test_txt = f.read()
    f.close()
    data = json.loads(test_txt)
    tests = data["tests"]
    responses = []
    # sys.stderr = open('errors', 'w')
    logfile = ""
    for test in tests:
        # print('test')
        # print(test)
        argy = test["input"].split(" -F ")
        # print('hell0')
        logfile = argy[1]
        # res = open('res', 'w')
        # print('before')
        # sys.stdout = res
        # print('hey')
        os.system("./" + test["input"] + " >> res")

        # print('SYSTEM')
        # if test.get("output", False):
        # sys.stdout = sys.__stdout__
        # res.close()
        # print('back')
        res = open('res', 'r')
        out = res.read()
        # print(out)
        # print('opened')
        res.close()
        print('\n' + test["input"])
        if out == test.get("output", ""):
            if out == "": continue
            # print(sys.argv[2])
            print('true')
        else:
            # print(sys.argv[2])
            print('false ')
            print('Actual:')
            print(out)
            print("Expected:")
            print(test.get("output", "error thing"))
    # print(sys.argv[2])
    # print (responses[-1])
    # sys.stdout = open('res', 'w')
    # os.system("rm " + logfile + ".txt > deleteme")
    # os.system("rm -r " + logfile + ".txt > deleteme")
    sys.stderr = sys.__stderr__
    os.system("rm res")
    # sys.stdout = sys.__stdout__

    # os.system("rm res")
