for name in breaks/correctness/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in breaks/crash/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done
for name in breaks/security/*.json; do
    echo $name
    ./test_runner_2.py -t $name
done