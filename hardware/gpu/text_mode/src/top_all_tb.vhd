library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity top_all_tb is
end top_all_tb;

architecture top_all_tb of top_all_tb is
    signal clock_50 : std_logic;
    signal key      : std_logic_vector(2 downto 0);
    signal sw       : std_logic_vector(9 downto 0);
    signal ledg     : std_logic_vector(9 downto 0);
    signal vga_hs   : std_logic;
    signal vga_vs   : std_logic;
    signal vga_r    : std_logic_vector(3 downto 0);
    signal vga_g    : std_logic_vector(3 downto 0);
    signal vga_b    : std_logic_vector(3 downto 0);
begin
    dut : entity work.top_all
    port map (clock_50, key, sw, ledg, vga_hs, vga_vs, vga_r, vga_g, vga_b);

    process
    begin
        clock_50 <= '0';
        wait for 20 ns;
        clock_50 <= '1';
        wait for 20 ns;
    end process;

    process
    begin
        key <= "110";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        for i in 0 to 100 loop
            wait until clock_50'event and clock_50 = '1';
        end loop;

        key <= "110";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        key <= "111";
        sw  <= "0000000000";
        wait until clock_50'event and clock_50 = '1';

        wait;
    end process;
end top_all_tb;
