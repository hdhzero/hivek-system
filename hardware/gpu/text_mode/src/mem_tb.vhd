library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity mem_tb is
end mem_tb;

architecture mem_tb of mem_tb is
    constant ADDR_WIDTH : integer := 8;
    constant DATA_WIDTH : integer := 16;

    signal a_clock  : std_logic;
    signal a_wren   : std_logic;
    signal a_addr   : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal a_data_i : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal a_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal b_clock  : std_logic;
    signal b_wren   : std_logic;
    signal b_addr   : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal b_data_i : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal b_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal clock : std_logic;
begin
    a_clock <= clock;
    b_clock <= clock;

    process
    begin
        clock <= '0';
        wait for 10 ns;
        clock <= '1';
        wait for 10 ns;
    end process;

    process
    begin
        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '1'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        a_wren <= '0'; b_wren <= '0';
        a_addr <= x"00"; b_addr <= x"00";
        a_data_i <= x"0000"; b_data_i <= x"0000";
        wait until a_clock'event and a_clock = '1';

        wait;
    end process;

    dut : entity work.dual_memory_bram
    generic map (16, 8)
    port map (
        a_clock, a_wren, a_addr, a_data_i, a_data_o,
        b_clock, b_wren, b_addr, b_data_i, b_data_o
    );
        
end mem_tb;
