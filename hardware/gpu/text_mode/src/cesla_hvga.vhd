library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity cesla_hvga is
    generic (
        RED_BITS   : integer := 4;
        GREEN_BITS : integer := 4;
        BLUE_BITS  : integer := 4
    );
    port (
        clock_vga  : in std_logic;
        clock_sys  : in std_logic;
        reset      : in std_logic;

        -- system interface
        wren   : in std_logic;
        row    : in std_logic_vector(4 downto 0);
        column : in std_logic_vector(6 downto 0);
        char_i : in std_logic_vector(15 downto 0);
       
        -- vga signals input
        vga_hs       : in std_logic;
        vga_vs       : in std_logic;
        vga_video_on : in std_logic;
        pixel_row    : in std_logic_vector(8 downto 0);
        pixel_column : in std_logic_vector(9 downto 0);

        -- vga signals output
        hs    : out std_logic;
        vs    : out std_logic;
        red   : out std_logic_vector(RED_BITS - 1 downto 0);
        green : out std_logic_vector(GREEN_BITS - 1 downto 0);
        blue  : out std_logic_vector(BLUE_BITS - 1 downto 0)
    );
end cesla_hvga;

architecture cesla_hvga of cesla_hvga is
    constant BITS_PER_PIXEL : integer := RED_BITS + GREEN_BITS + BLUE_BITS;
    constant R_LIMIT : integer := GREEN_BITS + BLUE_BITS + RED_BITS;
    constant G_LIMIT : integer := GREEN_BITS + BLUE_BITS;
    constant DELAY   : integer := 4;

    type color_table_t is array (0 to 15) of 
        std_logic_vector(BITS_PER_PIXEL - 1 downto 0);

    constant color_table : color_table_t := (
        0 => x"000",
        1 => x"00F",
        2 => x"0F0",
        3 => x"0FF",
        4 => x"F00",
        5 => x"F0F",
        6 => x"FF0",
        others => x"FFF"
    );

    signal char_row     : std_logic_vector(4 downto 0);
    signal char_column  : std_logic_vector(6 downto 0);
    signal font_row     : std_logic_vector(3 downto 0);
    signal font_column  : std_logic_vector(2 downto 0);
    signal font_cold    : std_logic_vector(2 downto 0);

    signal char_value_color : std_logic_vector(15 downto 0);

    signal char_value   : std_logic_vector(7 downto 0);
    signal char_fgcolor : std_logic_vector(3 downto 0);
    signal char_bgcolor : std_logic_vector(3 downto 0);
    signal char_pixels  : std_logic_vector(7 downto 0);

    signal fg  : std_logic;
    signal rgb : std_logic_vector(BITS_PER_PIXEL - 1 downto 0);

    signal hsv : std_logic_vector(DELAY downto 0);
    signal vsv : std_logic_vector(DELAY downto 0);
    signal von : std_logic_vector(DELAY downto 0);
begin
    char_column <= pixel_column(9 downto 3);
    char_row    <= pixel_row(8 downto 4);

    char_value  <= char_value_color(7 downto 0);

    fg  <= char_pixels(to_integer(unsigned(font_column)));
    rgb <= color_table(to_integer(unsigned(char_fgcolor))) when fg = '1' else
           color_table(to_integer(unsigned(char_bgcolor)));

    -- delay and synchronize hs & vs with red, green and blue
    process (clock_vga)
    begin
        if clock_vga'event and clock_vga = '1' then
            font_row <= pixel_row(3 downto 0);
            font_cold <= std_logic_vector(7 - unsigned(pixel_column(2 downto 0)));
            font_column <= font_cold;

            char_fgcolor <= char_value_color(15 downto 12);
            char_bgcolor <= char_value_color(11 downto 8);

            hsv(0) <= vga_hs;
            vsv(0) <= vga_vs;
            von(0) <= vga_video_on;

            hsv(1) <= hsv(0);
            hsv(2) <= hsv(1);
            vsv(1) <= vsv(0);
            vsv(2) <= vsv(1);
            von(1) <= von(0);
            von(2) <= von(1);

            hs    <= hsv(2);
            vs    <= vsv(2);
            
            if von(2) = '1' then -- von is video_on delayed
                red   <= rgb(R_LIMIT -1 downto G_LIMIT);
                green <= rgb(G_LIMIT - 1 downto BLUE_BITS);
                blue  <= rgb(BLUE_BITS - 1 downto 0);
            else
                red   <= (others => '0');
                green <= (others => '0');
                blue  <= (others => '0');
            end if;
        end if;
    end process;

    buf : entity work.char_buffer
    port map (
        clock_vga  => clock_vga,
        clock_sys  => clock_sys,
        wren_sys   => wren,
        row_sys    => row,
        column_sys => column,
        row_vga    => char_row,
        column_vga => char_column,
        char_i     => char_i,
        char_o     => char_value_color
    );

    font_rom : entity work.char_rom
    port map (
        clock  => clock_vga,
        char   => char_value(6 downto 0),
        row    => font_row,
        pixels => char_pixels
    );
end cesla_hvga;
