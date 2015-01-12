library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity top_tb is
end top_tb;

architecture top_tb of top_tb is
    constant RED_BITS   : integer := 4;
    constant GREEN_BITS : integer := 4;
    constant BLUE_BITS  : integer := 4;

    signal clock_vga  : std_logic;
    signal clock_sys  : std_logic;
    signal reset      : std_logic;

    -- system interface
    signal wren   : std_logic;
    signal row    : std_logic_vector(4 downto 0);
    signal column : std_logic_vector(6 downto 0);
    signal char_i : std_logic_vector(15 downto 0);
       
    -- vga signals input
    signal vga_hs       : std_logic;
    signal vga_vs       : std_logic;
    signal vga_video_on : std_logic;
    signal pixel_row    : std_logic_vector(8 downto 0);
    signal pixel_column : std_logic_vector(9 downto 0);

    -- vga signals output
    signal hs    : std_logic;
    signal vs    : std_logic;
    signal red   : std_logic_vector(RED_BITS - 1 downto 0);
    signal green : std_logic_vector(GREEN_BITS - 1 downto 0);
    signal blue  : std_logic_vector(BLUE_BITS - 1 downto 0);
begin
    process
    begin
        clock_vga <= '0';
        clock_sys <= '0';
        wait for 40 ns;

        clock_vga <= '1';
        clock_sys <= '1';
        wait for 40 ns;
    end process;

    process
    begin
        reset <= '1';
        wren  <= '0';
        row   <= "00000";
        column <= "0000000";
        char_i <= x"0000";
        vga_hs <= '0';
        vga_vs <= '0';
        pixel_row <= "000000000";
        pixel_column <= "0000000000";
        wait until clock_vga'event and clock_vga = '1';

        reset <= '0';
        wren  <= '0';
        row   <= "00000";
        column <= "0000000";
        char_i <= x"0000";
        vga_hs <= '0';
        vga_vs <= '0';
        pixel_row <= "000000000";
        pixel_column <= "0000000000";
        wait until clock_vga'event and clock_vga = '1';

        reset <= '0';
        wren  <= '1';
        row   <= "00000";
        column <= "0000000";
        char_i <= x"FF70";
        vga_hs <= '0';
        vga_vs <= '0';
        pixel_row <= "000000000";
        pixel_column <= "0000000000";
        wait until clock_vga'event and clock_vga = '1';

        reset <= '0';
        wren  <= '1';
        row   <= "00000";
        column <= "0000000";
        char_i <= x"0172";
        vga_hs <= '0';
        vga_vs <= '0';
        pixel_row <= "000000000";
        pixel_column <= "0000000000";
        wait until clock_vga'event and clock_vga = '1';

        reset <= '0';
        wren  <= '0';
        row   <= "00000";
        column <= "0000000";
        char_i <= x"0000";
        vga_hs <= '0';
        vga_vs <= '0';
        pixel_row <= "000000000";
        pixel_column <= "0000000000";
        wait until clock_vga'event and clock_vga = '1';

        reset <= '0';
        wren  <= '0';
        row   <= "00000";
        column <= "0000000";
        char_i <= x"0000";
        vga_hs <= '0';
        vga_vs <= '0';
        pixel_row <= "000000000";
        pixel_column <= "0000000000";
        wait until clock_vga'event and clock_vga = '1';

        reset <= '0';
        wren  <= '0';
        row   <= "00000";
        column <= "0000000";
        char_i <= x"0000";
        vga_hs <= '0';
        vga_vs <= '0';
        pixel_row <= "000000000";
        pixel_column <= "0000000000";
        wait until clock_vga'event and clock_vga = '1';

        reset <= '0';
        wren  <= '0';
        row   <= "00000";
        column <= "0000000";
        char_i <= x"0000";
        vga_hs <= '0';
        vga_vs <= '0';
        pixel_row <= "000000000";
        pixel_column <= "0000000000";
        wait until clock_vga'event and clock_vga = '1';

        wait;
    end process;

    dut : entity work.top
    generic map (RED_BITS, GREEN_BITS, BLUE_BITS)
    port map (clock_vga, clock_sys, reset, wren, row, column, char_i, vga_hs,
        vga_vs, vga_video_on, pixel_row, pixel_column, hs, vs, red, green, blue
    );
end top_tb;

