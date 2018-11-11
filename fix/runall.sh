# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork1.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork2.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork3.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork4.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork5.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork6.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork7.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork8.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork9.json
# ./test_runner_2.py -t tests/tests/core/doesitwork/doesitwork10.json
# ./test_runner_2.py -t tests/tests/core/batch/batch1.json
# ./test_runner_2.py -t tests/tests/core/batch/batch2.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs1.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs2.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs3.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs4.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs5.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs6.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs7.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs8.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs9.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs10.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs11.json
# ./test_runner_2.py -t tests/tests/core/edge/badargs/badargs12.json
# ./test_runner_2.py -t tests/tests/core/edge/badstate/badstate1.json
# ./test_runner_2.py -t tests/tests/core/edge/badstate/badstate2.json
# ./test_runner_2.py -t tests/tests/core/edge/badstate/badstate3.json
# ./test_runner_2.py -t tests/tests/core/features/rooms/rooms1.json
# ./test_runner_2.py -t tests/tests/core/features/rooms/rooms2.json
# ./test_runner_2.py -t tests/tests/core/features/summary/summary1.json
# ./test_runner_2.py -t tests/tests/core/features/summary/summary2.json
# ./test_runner_2.py -t tests/tests/perf/size/size1.json
# ./test_runner_2.py -t tests/tests/perf/size/size2.json
# ./test_runner_2.py -t tests/tests/perf/size/size3.json
# ./test_runner_2.py -t tests/tests/perf/size/size4.json
# ./test_runner_2.py -t tests/tests/perf/size/size5.json
# ./test_runner_2.py -t tests/tests/perf/speed/speed1.json
# ./test_runner_2.py -t tests/tests/perf/speed/speed2.json
# ./test_runner_2.py -t tests/tests/perf/speed/speed3.json
# ./test_runner_2.py -t tests/tests/perf/speed/speed4.json
# ./test_runner_2.py -t tests/tests/perf/speed/speed5.json
cd build
make >> trash
cd ..
cp build/logappend logappend
cp build/logread logread
cp tests/test_runner_2.py test_runner_2.py
for name in tests/core/doesitwork/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in tests/core/batch/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in tests/core/edge/badargs/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in tests/core/edge/badstate/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in tests/core/features/rooms/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in tests/core/features/summary/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in tests/perf/size/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in tests/perf/speed/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in tests/student_tests/*.json; do
    echo $name 
    ./test_runner_2.py -t $name
done
