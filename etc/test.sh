datum=`date +%s`
gcc -pg -O3 -Wall mastermind.c -o mastermind -lm && papiex -q -e PAPI_TOT_INS -e PAPI_BR_MSP mastermind 75532 &> output.txt-$datum
gprof mastermind

