library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity char_buffer_tb is
end char_buffer_tb;

architecture char_buffer_tb of char_buffer_tb is
    signal clock  : std_logic;
    signal reset  : std_logic;
    signal wr     : std_logic;
    signal row    : std_logic_vector(4 downto 0);
    signal column : std_logic_vector(6 downto 0);
    signal char_i : std_logic_vector(7 downto 0);
    signal char_o : std_logic_vector(7 downto 0);
begin
    process
    begin
        clock <= '0';
        wait for 10 ns;
        clock <= '1';
        wait for 10 ns;
    end process;

    process
    begin
        reset  <= '1';
        wr     <= '0';
        row    <= "00000";
        column <= "0000000";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "01100";
        column <= "0101010";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '1';
        row    <= "10110";
        column <= "0111100";
        char_i <= x"40";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "10001";
        column <= "0001100";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "11100";
        column <= "0011000";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "11001";
        column <= "0110110";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "10011";
        column <= "1011100";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "00011";
        column <= "0011110";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "01101";
        column <= "0011111";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "00110";
        column <= "1111111";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "01010";
        column <= "0011110";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "00111";
        column <= "1011000";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "00000";
        column <= "0000000";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "00000";
        column <= "0000000";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "00000";
        column <= "0000000";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        reset  <= '0';
        wr     <= '0';
        row    <= "00000";
        column <= "0000000";
        char_i <= x"00";
        wait until clock'event and clock = '1';

        wait;
    end process;

    buf : entity work.char_buffer
    port map (clock, reset, wr, row, column, char_i, char_o);
end char_buffer_tb;
