library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- for 640x480 resolution
-- char_vga?
entity charT is
    port (
        pixel_row    : in std_logic_vector(8 downto 0);
        pixel_column : in std_logic_vector(9 downto 0);
        char_row     : out std_logic_vector(4 downto 0);
        char_column  : out std_logic_vector(6 downto 0);
        font_row     : out std_logic_vector(3 downto 0);
        font_column  : out std_logic_vector(2 downto 0)
    );
end charT;

architecture charT of charT is
begin
    char_column <= pixel_column(9 downto 3);
    font_column <= pixel_column(2 downto 0);

    char_row <= pixel_row(8 downto 4);
    font_row <= pixel_row(3 downto 0);
end charT;
