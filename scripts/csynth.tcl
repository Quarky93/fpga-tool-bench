set top [lindex $argv 2]
set plat [lindex $argv 3]
set freq [lindex $argv 4]
set tune [lindex $argv 5]

open_project ${top} -reset

add_files ../srcs/vhls/${top}.cpp -cflags "-I../srcs/vhls/"

set_top ${top}_top
open_solution "f${freq}" -flow_target vivado -reset
set_part ${plat}
create_clock -period ${freq}MHz -name default
if {${tune} != ""} {
	set_clock_uncertainty ${tune} default
}

config_compile -pragma_strict_mode
config_rtl -reset none
csynth_design
close_solution
close_project
exit
