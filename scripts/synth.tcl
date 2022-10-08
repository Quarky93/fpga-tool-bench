set top [lindex $argv 0]
set plat [lindex $argv 1]
set freq [lindex $argv 2]
set directive [lindex $argv 3]

create_project -in_memory -part ${plat}
read_verilog [ glob ./${top}/f${freq}/syn/verilog/*.v ]

set period [expr 1.0/${freq} * 1000]
exec echo create_clock -period ${period} \[get_ports ap_clk\] > ${top}_f${freq}.xdc
read_xdc ./${top}_f${freq}.xdc

synth_design -top ${top}_top -directive ${directive} -mode out_of_context
write_checkpoint -force ./synth_${top}_f${freq}.dcp
