mm: mastermind.c
	gcc -lm -O3 mastermind.c -o mm

test: mm
	papiex -q -e PAPI_TOT_INS -e PAPI_BR_MSP mm 75532 > new.out
	diff new.out ref.out

clean: 
	rm mm new.out mm.pap*
