library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity vga_sdram is
    generic (
        -- VGA Info
        HD : integer := 640; -- Horizontal display area
        HF : integer := 16;  -- Horizontal front porch
        HB : integer := 48;  -- Horizontal back porch
        HR : integer := 96;  -- Horizontal retrace
        VD : integer := 480; -- Vertical display area
        VF : integer := 10;  -- Vertical front porch
        VB : integer := 31;  -- Vertical back porch
        VR : integer := 2;   -- Vertical retrace

        -- bits per pixels
        RED_WIDTH   : integer := 4;
        GREEN_WIDTH : integer := 4;
        BLUE_WIDTH  : integer := 4;

        ADDR_WIDTH : integer := 22;
        DATA_WIDTH : integer := 16;
        BUF_WIDTH  : integer := 12
    );
    port (
        clock : in std_logic;
        reset : in std_logic;

        -- VGA interface
        hsync : out std_logic;
        vsync : out std_logic;
        red   : out std_logic_vector(RED_WIDTH - 1 downto 0);
        green : out std_logic_vector(GREEN_WIDTH - 1 downto 0);
        blue  : out std_logic_vector(BLUE_WIDTH - 1 downto 0);

        -- sdram interface
        mem_busy   : in std_logic;
        mem_strobe : out std_logic;
        mem_wr     : out std_logic;
        mem_addr   : out std_logic_vector(ADDR_WIDTH - 1 downto 0);
        mem_nwords : out std_logic_vector(BUF_WIDTH - 1 downto 0);

        buf_clock  : in std_logic;
        buf_wr     : in std_logic;
        buf_addr   : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        buf_data_i : in std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_data_o : out std_logic_vector(DATA_WIDTH - 1 downto 0)
    );
end vga_sdram;

architecture behavior of vga_sdram is
    type state_t is (
        idle,
        fetch_pixels,
        wait_sdram_free
    );

    constant GREEN_ACC : integer := BLUE_WIDTH + GREEN_WIDTH;
    constant RED_ACC   : integer := GREEN_ACC  + RED_WIDTH;
 
    signal NUM_PIXELS : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal current_state : state_t;

    signal vcounter : integer;
    signal hcounter : integer;

    signal sdram_fetch_addr : std_logic_vector(ADDR_WIDTH - 1 downto 0);

    signal buf_wr_delay     : std_logic;
    signal buf_addr_delay   : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal buf_data_i_delay : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal vga_fetch_addr : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal vga_cache_addr : std_logic_vector(BUF_WIDTH - 1 downto 0);

    signal new_vga_cache_addr : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal new_vga_fetch_addr : std_logic_vector(BUF_WIDTH - 1 downto 0);

    signal vga_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal cache_base : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal fetch_base : std_logic_vector(BUF_WIDTH - 1 downto 0);

    signal hsync_delay     : std_logic;
    signal hsync_delay2    : std_logic;
    signal vsync_delay     : std_logic;
    signal vsync_delay2    : std_logic;
    signal video_on        : std_logic;
    signal video_on_delay  : std_logic;
    signal video_on_delay2 : std_logic;

    signal fetch_new_pixels : std_logic;
begin
    new_vga_cache_addr <= std_logic_vector(unsigned(buf_addr) + unsigned(cache_base));
    NUM_PIXELS         <= std_logic_vector(to_unsigned(HD, BUF_WIDTH));

    process (clock)
    begin
        if clock'event and clock = '1' then
            if video_on = '0' then
                red   <= (others => '0');
                green <= (others => '0');
                blue  <= (others => '0');
            else
                red   <= vga_data_o(RED_ACC - 1 downto GREEN_ACC);
                green <= vga_data_o(GREEN_ACC - 1 downto BLUE_WIDTH);
                blue  <= vga_data_o(BLUE_WIDTH - 1 downto 0);
            end if;
        end if;
    end process;

    process (buf_clock)
    begin
        if buf_clock'event and buf_clock = '1' then
            buf_wr_delay     <= buf_wr;
            vga_cache_addr   <= new_vga_cache_addr;
            buf_data_i_delay <= buf_data_i;
        end if;
    end process;

    process (clock, reset)
    begin
        if reset = '1' then
            current_state <= idle;
            mem_strobe    <= '0';
            mem_wr        <= '0';
            mem_addr      <= (others => '0');
            mem_nwords    <= (others => '0');
        elsif clock'event and clock = '1' then
            case current_state is
                when idle =>
                    if fetch_new_pixels = '1' then
                        current_state <= fetch_pixels;
                    end if;

                when fetch_pixels =>
                    if mem_busy = '0' then
                        current_state <= wait_sdram_free;
                        mem_strobe    <= '1';
                        mem_wr        <= '0';
                        mem_addr      <= sdram_fetch_addr;
                        mem_nwords    <= NUM_PIXELS;
                    end if;

                when wait_sdram_free =>
                    mem_strobe <= '0';

                    if mem_busy = '0' then
                        current_state <= idle;
                    end if;

                when others =>
                    current_state <= idle;
                    mem_strobe    <= '0';
            end case;
        end if;
    end process;

    -- VGA process
    process (clock, reset)
    begin
        if reset = '1' then
            hcounter         <= 0;
            vcounter         <= 0;
            hsync            <= '1';
            vsync            <= '1';
            hsync_delay      <= '1';
            hsync_delay2     <= '1';
            vsync_delay      <= '1';
            vsync_delay2     <= '1';
            fetch_base       <= (others => '0');
            cache_base       <= std_logic_vector(to_unsigned(1024, BUF_WIDTH));
            sdram_fetch_addr <= std_logic_vector(to_unsigned(640, ADDR_WIDTH));
            fetch_new_pixels <= '0';
        elsif clock'event and clock = '1' then
            vsync <= vsync_delay2;
            hsync <= hsync_delay2;

            vsync_delay2 <= vsync_delay;
            hsync_delay2 <= hsync_delay;

            video_on        <= video_on_delay2;
            video_on_delay2 <= video_on_delay;

            fetch_new_pixels <= '0';

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
                hsync_delay <= '0';
            else
                hsync_delay <= '1';
            end if;
            
            if vcounter >= VD + VF and vcounter <= VD + VF + VR - 1 then
                vsync_delay <= '0';
            else
                vsync_delay <= '1';
            end if;

            if hcounter < HD and vcounter < VD then
                video_on_delay <= '1';
            else
                video_on_delay <= '0';
            end if;

            if hcounter = HD - 1 and vcounter < VD then
                cache_base <= fetch_base;
                fetch_base <= cache_base;
                fetch_new_pixels <= '1';

                if unsigned(sdram_fetch_addr) < HD * (VD - 1) then
                    sdram_fetch_addr <= std_logic_vector(unsigned(sdram_fetch_addr) + HD);
                else
                    sdram_fetch_addr <= (others => '0');
                end if;
            end if;

            vga_fetch_addr <= std_logic_vector(to_unsigned(hcounter, BUF_WIDTH) + unsigned(fetch_base));
        end if;
    end process;

    dual_memory_u : entity work.dual_memory_bram
    generic map (
        DATA_WIDTH => DATA_WIDTH,
        ADDR_WIDTH => BUF_WIDTH
    )   
    port map (
        a_clock  => buf_clock,
        a_wren   => buf_wr_delay,
        a_addr   => vga_cache_addr, 
        a_data_i => buf_data_i_delay,
        a_data_o => buf_data_o,

        b_clock  => clock, 
        b_wren   => '0',
        b_addr   => vga_fetch_addr,
        b_data_i => (others => '0'),
        b_data_o => vga_data_o
    );
end behavior;
