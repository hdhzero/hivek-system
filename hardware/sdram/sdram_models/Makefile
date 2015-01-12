all:
	ghdl -i mt48lc8m16a2.vhd
	ghdl -i mt48lc8m16a2_tb.vhd
	ghdl -m mt48lc8m16a2_tb
	./mt48lc8m16a2_tb --stop-time=700us --wave=m.ghw

clean:
	rm -rf *.o *.cf mt48lc8m16a2_tb
