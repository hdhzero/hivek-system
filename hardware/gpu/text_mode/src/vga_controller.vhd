library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity vga_controller is
    generic (
        HD : integer := 640; -- Horizontal display area
        HF : integer := 16;  -- Horizontal front porch
        HB : integer := 48;  -- Horizontal back porch
        HR : integer := 96;  -- Horizontal retrace
        VD : integer := 480; -- Vertical display area
        VF : integer := 10;  -- Vertical front porch
        VB : integer := 31;  -- Vertical back porch
        VR : integer := 2    -- Vertical retrace
    );
    port (
        clock       : in std_logic;
        reset       : in std_logic;
        hsync       : out std_logic;
        vsync       : out std_logic;
        video_on    : out std_logic;
        vtrace      : out std_logic;
        pixel_clk   : out std_logic;
        pixel_x     : out std_logic_vector(10 downto 0);
        pixel_y     : out std_logic_vector(10 downto 0);
        linear_addr : out std_logic_vector(31 downto 0)
    );
end vga_controller;

architecture behavior of vga_controller is
    signal vcounter : integer;
    signal hcounter : integer;
begin
    pixel_clk <= clock;

    process (clock, reset)
    begin
        if reset = '1' then
            hcounter <= 0;
            vcounter <= 0;
            hsync    <= '1';
            vsync    <= '1';
        elsif clock'event and clock = '1' then
            if hcounter = HD + HF + HB + HR - 1 then
                hcounter <= 0;
                
                if vcounter = VD + VF + VB + VR - 1 then
                    vcounter <= 0;
                else
                    vcounter <= vcounter + 1;
                end if;    
            else
                hcounter <= hcounter + 1;
            end if;
            
            if hcounter >= HD + HF and hcounter <= HD + HF + HR - 1 then
                hsync <= '0';
            else
                hsync <= '1';
            end if;
            
            if vcounter >= VD + VF and vcounter <= VD + VF + VR - 1 then
                vsync <= '0';
            else
                vsync <= '1';
            end if;

            if hcounter < HD and vcounter < VD then
                video_on <= '1';
            else
                video_on <= '0';
            end if;

            if vcounter < VD then
                vtrace <= '0';
            else
                vtrace <= '1';
            end if;

            pixel_x <= std_logic_vector(to_unsigned(hcounter, 11));
            pixel_y <= std_logic_vector(to_unsigned(vcounter, 11));
            linear_addr <= std_logic_vector(to_unsigned((vcounter * HD + hcounter), 32));
        end if;
    end process;
end behavior;

