all:
	ghdl -i dual_memory_bram.vhd
	ghdl -i vga_controller.vhd
	ghdl -i char_rom.vhd
	ghdl -i char_buffer.vhd
	ghdl -i cesla_hvga.vhd
	ghdl -i top_all.vhd
	ghdl -i top_all_tb.vhd
	ghdl -m top_all_tb

mem:
	ghdl -i dual_memory_bram.vhd mem_tb.vhd
	ghdl -m mem_tb
	./mem_tb --stop-time=1us --wave=mem.ghw

run:
	./top_all_tb --stop-time=100us --wave=waves.ghw

clean:
	rm -rf *.o *.cf
