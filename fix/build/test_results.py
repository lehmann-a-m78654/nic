import sys

if __name__ == "__main__":
    init = open(sys.argv[1], 'r')
    initial_res = init.read().split('\n')

    now = open(sys.argv[2], 'r')
    new_res = now.read().split('\n')

    if len(initial_res) != len(new_res):
        print('problem, not equal')
        sys.exit(0)

    passed = 0
    failed = 0
    for i, line in enumerate(initial_res):
        if "true" in new_res[i]:
            passed += 1
        elif "false" in new_res[i]:
            failed += 1
        if initial_res[i] != new_res[i]:
            if "true" in initial_res[i] and "true" in new_res[i]:
                # print("contineu, tru")
                continue
            elif "false" in initial_res[i] and "false" in new_res[i]:
                # print("contihnue, fale", str(i))
                continue
            initres = ""
            finalres = ""
            if "true" in initial_res[i] and "false" in new_res[i]:
                initres = "passed"
                finalres = "failed"
            elif "false" in initial_res[i] and "true" in new_res[i]:
                initres = "failed"
                finalres = "passed"
            else:
                print('PROBLEM AT ' + str(i))
                print(initial_res[i])
                print(new_res[i])
                sys.exit()
            print(finalres + ': ' + initial_res[i - 1].split('/')[-1])
        
    print('total passed: ' + str(passed))
    print('total failed: ' + str(failed))