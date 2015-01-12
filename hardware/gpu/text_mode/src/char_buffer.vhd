library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity char_buffer is
    port (
        clock_vga  : in std_logic;
        clock_sys  : in std_logic;
        wren_sys   : in std_logic;
        row_sys    : in std_logic_vector(4 downto 0);
        column_sys : in std_logic_vector(6 downto 0);
        row_vga    : in std_logic_vector(4 downto 0);
        column_vga : in std_logic_vector(6 downto 0);
        char_i     : in std_logic_vector(15 downto 0);
        char_o     : out std_logic_vector(15 downto 0)
    );
end char_buffer;

architecture char_buffer of char_buffer is
    signal address_vga : std_logic_vector(11 downto 0);
    signal row_4_vga   : std_logic_vector(11 downto 0);
    signal row_6_vga   : std_logic_vector(11 downto 0);
    signal times_vga   : std_logic_vector(11 downto 0);
    signal columnh_vga : std_logic_vector(11 downto 0);

    signal address_sys : std_logic_vector(11 downto 0);
    signal row_4_sys   : std_logic_vector(11 downto 0);
    signal row_6_sys   : std_logic_vector(11 downto 0);
    signal times_sys   : std_logic_vector(11 downto 0);
    signal columnh_sys : std_logic_vector(11 downto 0);
begin
    -- row * 80 = row * (64 + 16) = (row << 4) + (row << 6);
    row_4_vga <= "000" & row_vga & "0000";
    row_6_vga <= "0" & row_vga & "000000";
    times_vga <= std_logic_vector(unsigned(row_4_vga) + unsigned(row_6_vga));

    -- conversion std to unsigned
    columnh_vga <= "00000" & column_vga;

    -- row * n_columns + column
    -- row * 80 + column
    address_vga <= std_logic_vector(unsigned(times_vga) + unsigned(columnh_vga));

    -- row * 80 = row * (64 + 16) = (row << 4) + (row << 6);
    row_4_sys <= "000" & row_sys & "0000";
    row_6_sys <= "0" & row_sys & "000000";
    times_sys <= std_logic_vector(unsigned(row_4_sys) + unsigned(row_6_sys));

    -- conversion std to unsigned
    columnh_sys <= "00000" & column_sys;

    -- row * n_columns + column
    -- row * 80 + column
    address_sys <= std_logic_vector(unsigned(times_sys) + unsigned(columnh_sys));

    char_buf : entity work.dual_memory_bram
    generic map (
        DATA_WIDTH => 16,
        ADDR_WIDTH => 12
    )
    port map (
        a_clock  => clock_sys,
        a_wren   => wren_sys,
        a_addr   => address_sys,
        a_data_i => char_i,
        a_data_o => open,

        b_clock  => clock_vga,
        b_wren   => '0',
        b_addr   => address_vga,
        b_data_i => (others => '0'),
        b_data_o => char_o
    );
end char_buffer;
