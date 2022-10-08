import os
import sys
import csv
import json
from markdownTable import markdownTable

def extract_perf_data(file):
    perf_data = {}
    with open(file, newline='') as f:
        reader = csv.reader(f, delimiter=',')
        next(reader)
        next(reader)
        for row in reader:
            perf_data[row[2]] = {'counter_value': row[0], 'counter_unit': row[1], 'counter_runtime': row[3], 'measurement_percentage': row[4], 'metric': row[5], 'metric_unit': row[6]}
    return perf_data

def get_run_time(perf_data):
    return float(perf_data['duration_time']['counter_value']) / 1000000000.0

def get_cpu_time(perf_data):
    return float(perf_data['task-clock']['counter_value']) / 1000.0

def get_cpu_load(perf_data):
    return get_cpu_time(perf_data) / get_run_time(perf_data)

def get_cache_misses_percent(perf_data):
    references = get_cache_references(perf_data)
    misses = get_cache_misses(perf_data)
    if (references and misses):
        return misses / references
    return None

def get_cache_references(perf_data):
    if ('cache-references' in perf_data):
        return float(perf_data['cache-references']['counter_value'])
    elif ('cpu_core/cache-references/' in perf_data):
        return float(perf_data['cpu_core/cache-references/']['counter_value'])
    return None

def get_cache_misses(perf_data):
    if ('cache-misses' in perf_data):
        return float(perf_data['cache-misses']['counter_value'])
    elif ('cpu_core/cache-misses/' in perf_data):
        return float(perf_data['cpu_core/cache-misses/']['counter_value'])
    return None

def get_instructions(perf_data):
    if ('instructions' in perf_data):
        return float(perf_data['instructions']['counter_value'])
    elif ('cpu_core/instructions/' in perf_data):
        return float(perf_data['cpu_core/instructions/']['counter_value'])
    return None

def get_cycles(perf_data):
    if ('cycles' in perf_data):
        return float(perf_data['cycles']['counter_value'])
    elif ('cpu_core/cycles/' in perf_data):
        return float(perf_data['cpu_core/cycles/']['counter_value'])
    return None

def get_ipc(perf_data):
    instructions = get_instructions(perf_data)
    cycles = get_cycles(perf_data)
    if (instructions and cycles):
        return instructions / cycles
    return None

def print_metric(metric, value, unit, scale=1, decimals=2):
    if value is not None:
        print(metric + ': ' + str(round(value * scale, decimals)) + ' ' + unit)

def format_metric(value, scale=1, decimals=2):
    if value is not None:
        return str(round(value * scale, decimals))
    return None

def print_perf_data(perf_data):
    print_metric('Run Time', get_run_time(perf_data), unit='s')
    print_metric('CPU Time', get_cpu_time(perf_data), unit='s')
    print_metric('CPU Load', get_cpu_load(perf_data), scale=100, unit='%')
    print_metric('LLC Cache Misses', get_cache_misses_percent(perf_data), scale=100, unit='%')
    print_metric('IPC', get_ipc(perf_data), unit='instrs/clock')

def make_table_row(stage, perf_data):
    row = {}
    row['Stage'] = stage
    row['Run Time (s)'] = format_metric(get_run_time(perf_data))
    row['CPU Time (s)'] = format_metric(get_cpu_time(perf_data))
    row['CPU Load (%)'] = format_metric(get_cpu_load(perf_data), scale=100)
    row['LLC Cache Misses (%)'] = format_metric(get_cache_misses_percent(perf_data), scale=100)
    row['IPC (instrs/clock)'] = format_metric(get_ipc(perf_data))
    return row

def make_table_row_total(data_0, data_1, data_2):
    row = {}
    row['Stage'] = 'total'
    run_time = get_run_time(data_0) + get_run_time(data_1) + get_run_time(data_2)
    cpu_time = get_cpu_time(data_0) + get_cpu_time(data_1) + get_cpu_time(data_2)
    cpu_load = cpu_time / run_time
    cache_references = get_cache_references(data_0) + get_cache_references(data_1) + get_cache_references(data_2)
    cache_misses = get_cache_misses(data_0) + get_cache_misses(data_1) + get_cache_misses(data_2)
    instructions = get_instructions(data_0) + get_instructions(data_1) + get_instructions(data_2)
    cycles = get_cycles(data_0) + get_cycles(data_1) + get_cycles(data_2)
    row['Run Time (s)'] = format_metric(run_time)
    row['CPU Time (s)'] = format_metric(cpu_time)
    row['CPU Load (%)'] = format_metric(cpu_load, scale=100)
    row['LLC Cache Misses (%)'] = format_metric(cache_misses / cache_references, scale=100)
    row['IPC (instrs/clock)'] = format_metric(instructions / cycles)
    return row

def main():
    benchmark = str(sys.argv[1])
    csynth_data = extract_perf_data('csynth_' + benchmark + '_perf.csv')
    synth_data = extract_perf_data('synth_' + benchmark + '_perf.csv')
    impl_data = extract_perf_data('impl_' + benchmark + '_perf.csv')
    table = [
        make_table_row('csynth', csynth_data),
        make_table_row('synth', synth_data),
        make_table_row('impl', impl_data),
        make_table_row_total(csynth_data, synth_data, impl_data)
    ]
    print('## ' + benchmark)
    if not os.path.exists('README.md'):
        with open('README.md', 'w') as f:
            f.write('# Benchmarks\n')
    print(markdownTable(table).getMarkdown())
    with open('README.md', 'a') as f:
        f.write('## ' + benchmark + '\n')
        f.write(markdownTable(table).getMarkdown() + '\n')
    with open(benchmark + '.json', 'w') as f:
        json.dump(table, f, indent=4)

if __name__ == "__main__":
    main()
