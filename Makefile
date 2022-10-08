PLAT=xcu55c-fsvh2892-2L-e
FREQ=450
TUNE=""
SYNTH_DIRECTIVE=sdx_optimization_effort_high
PERF_EVENTS=task-clock,cycles,instructions,context-switches,cpu-migrations,page-faults,stalled-cycles-frontend,stalled-cycles-backend,branches,branch-misses,cache-references,cache-misses,duration_time
BUILD_TOPS=blake512 bmw512 cubehash512 echo512 groestl512 hamsi512 jh512 keccak512
.SECONDARY:

build/csynth_%_f$(FREQ)_perf.csv:
	mkdir -p ./build; \
	cd ./build; \
	perf stat -e $(PERF_EVENTS) -o csynth_$*_f$(FREQ)_perf.csv -x , vitis_hls -f ../scripts/csynth.tcl $* $(PLAT) $(FREQ) $(TUNE)

build/synth_%_f$(FREQ)_perf.csv: build/csynth_%_f$(FREQ)_perf.csv
	mkdir -p ./build; \
	cd ./build; \
	perf stat -e $(PERF_EVENTS) -o synth_$*_f$(FREQ)_perf.csv -x , vivado -mode batch -source ../scripts/synth.tcl -tclargs $* $(PLAT) $(FREQ) $(SYNTH_DIRECTIVE)

build/impl_%_f$(FREQ)_perf.csv: build/synth_%_f$(FREQ)_perf.csv
	mkdir -p ./build; \
	cd ./build; \
	perf stat -e $(PERF_EVENTS) -o impl_$*_f$(FREQ)_perf.csv -x , vivado -mode batch -source ../scripts/impl.tcl -tclargs $* $(FREQ); \
	python3 ../scripts/print_results.py $*_f$(FREQ)

benchmarks: $(BUILD_TOPS:%=build/impl_%_f$(FREQ)_perf.csv)

clean:
	rm -rf ./build
