library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity top_all is
    port (
        clock_50 : in std_logic;
        key      : in std_logic_vector(2 downto 0);
        sw       : in std_logic_vector(9 downto 0);
        ledg     : out std_logic_vector(9 downto 0);
        vga_hs   : out std_logic;
        vga_vs   : out std_logic;
        vga_r    : out std_logic_vector(3 downto 0);
        vga_g    : out std_logic_vector(3 downto 0);
        vga_b    : out std_logic_vector(3 downto 0)
    );
end top_all;

architecture top_all of top_all is
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
    signal vga_hss       : std_logic;
    signal vga_vss       : std_logic;
    signal vga_video_on : std_logic;
    signal pixel_row    : std_logic_vector(8 downto 0);
    signal pixel_column : std_logic_vector(9 downto 0);

    -- vga signals output
    signal hs    : std_logic;
    signal vs    : std_logic;
    signal red   : std_logic_vector(RED_BITS - 1 downto 0);
    signal green : std_logic_vector(GREEN_BITS - 1 downto 0);
    signal blue  : std_logic_vector(BLUE_BITS - 1 downto 0);

    signal pixel_x : std_logic_vector(10 downto 0);
    signal pixel_y : std_logic_vector(10 downto 0);
    signal state : integer;
    constant CVALUE : integer := 7000000;
begin
    reset <= not key(0);
    wren  <= sw(1);

    process (clock_50, reset)
    begin
        if reset = '1' then
            clock_vga <= '0';
        elsif clock_50'event and clock_50 = '1' then
            clock_vga <= not clock_vga;
        end if;

        if reset = '1' then
            state <= 0;
            char_i <= x"513F";
            row <= (others => '0');
            column <= (others => '0');
        elsif clock_50'event and clock_50 = '1' then
            if state >= CVALUE and sw(2) = '1' then
                state <= 0;

                if unsigned(column) < 79 then
                    column <= std_logic_vector(unsigned(column) + 1);
                else
                    column <= (others => '0');

                    if unsigned(row) < 29 then
                        row <= std_logic_vector(unsigned(row) + 1);
                    else
                        row <= (others => '0');
                    end if;
                end if;

                char_i <= std_logic_vector(unsigned(char_i) + 1);
            else
                state <= state + 1;
            end if;
        end if;
    end process;

    pixel_column <= pixel_x(9 downto 0);
    pixel_row <= pixel_y(8 downto 0);

    C0 : entity work.vga_controller
    port map (
        clock_vga,
        reset,
        vga_hss,
        vga_vss,
        vga_video_on,
        open,
        open,
        pixel_x,
        pixel_y,
        open
    );
 
    dut : entity work.cesla_hvga
    generic map (RED_BITS, GREEN_BITS, BLUE_BITS)
    port map (
        clock_vga, 
        clock_vga, 
        reset, 
        wren, 
        row, 
        column, 
        char_i, 
        vga_hss,
        vga_vss, 
        vga_video_on, 
        pixel_row, 
        pixel_column, 
        vga_hs, 
        vga_vs, 
        vga_r, 
        vga_g, 
        vga_b
    );
      
end top_all;
