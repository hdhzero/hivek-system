sdram_controller:
	ghdl -i sdram_models/mt48lc8m16a2.vhd
	ghdl -i dual_memory_bram.vhd
	ghdl -i sdram_client_state.vhd
	ghdl -i sdram_client_fsm.vhd
	ghdl -i sdram_arbiter.vhd
	ghdl -i sdram_ctrl.vhd
	ghdl -i sdram_controller.vhd
	ghdl -i sdram_controller_tb.vhd
	ghdl -m sdram_controller_tb
	./sdram_controller_tb --stop-time=700us --wave=sdram_controller_tb.ghw

sdram_ctrl:
	ghdl -i sdram_ctrl.vhd
	ghdl -i sdram_models/mt48lc8m16a2.vhd
	ghdl -i dual_memory_bram.vhd
	ghdl -i sdram_ctrl_tb.vhd
	ghdl -m sdram_ctrl_tb
	./sdram_ctrl_tb --stop-time=700us --wave=m.ghw


sdram_arbiter:
	ghdl -i sdram_client_state.vhd
	ghdl -i sdram_arbiter.vhd
	ghdl -i sdram_arbiter_tb.vhd
	ghdl -m sdram_arbiter_tb
	./sdram_arbiter_tb --stop-time=700us --wave=arb.ghw

sdram_client_state:
	ghdl -i sdram_client_state.vhd
	ghdl -i sdram_client_state_tb.vhd
	ghdl -m sdram_client_state_tb
	./sdram_client_state_tb --stop-time=1us --wave=sdram_client_state_tb.ghw

vga_sdram:
	ghdl -i sdram_models/mt48lc8m16a2.vhd
	ghdl -i dual_memory_bram.vhd
	ghdl -i sdram_client_state.vhd
	ghdl -i sdram_client_fsm.vhd
	ghdl -i sdram_client_fsm2.vhd
	ghdl -i sdram_arbiter.vhd
	ghdl -i sdram_ctrl.vhd
	ghdl -i sdram_controller.vhd
	ghdl -i vga_sdram.vhd
	ghdl -i vga_sdram_tb.vhd
	ghdl -m vga_sdram_tb
	./vga_sdram_tb --stop-time=1700us --wave=vga_sdram_tb.ghw #--assert-level=error


clean:
	rm -rf *.o *.cf sdram_ctrl_tb sdram_ctrl sdram_arbiter_tb sdram_controller_tb vga_sdram vga_sdram_tb sdram_client_state_tb
