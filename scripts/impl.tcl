set top [lindex $argv 0]
set freq [lindex $argv 1]
# set place_directive [lindex $argv 2]
# set route_directive [lindex $argv 3]

open_checkpoint ./synth_${top}_f${freq}.dcp
opt_design
place_design
phys_opt_design
route_design
report_utilization -file ./impl_${top}_f${freq}_utilization.rpt
report_timing_summary -file ./impl_${top}_f${freq}_timing.rpt
write_checkpoint -force ./impl_${top}_f${freq}.dcp
