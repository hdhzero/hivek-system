library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_board is
    port (
        CLOCK_50MHz : in std_logic;
        KEY         : in std_logic_vector(11 downto 0);
        DISP0_D     : out std_logic_vector(7 downto 0);

        VGA_HS      : out std_logic;
        VGA_VS      : out std_logic;
        VGA_R       : out std_logic_vector(3 downto 0);
        VGA_G       : out std_logic_vector(3 downto 0);
        VGA_B       : out std_logic_vector(3 downto 0);

        SDRAM_A     : out std_logic_vector(12 downto 0);
        SDRAM_BA    : out std_logic_vector(1 downto 0);
        SDRAM_D     : inout std_logic_vector(15 downto 0);
        SDRAM_DQM   : out std_logic_vector(1 downto 0);
        SDRAM_CLK   : out std_logic;
        SDRAM_CKE   : out std_logic;
        SDRAM_CAS_N : out std_logic;
        SDRAM_CS_N  : out std_logic;
        SDRAM_RAS_N : out std_logic;
        SDRAM_WE_N  : out std_logic
    );
end sdram_board;

architecture behavior of sdram_board is
    type state_t is (
        init
    );

    component mypll is
	port (
		areset : in std_logic  := '0';
		inclk0 : in std_logic  := '0';
		c0     : out std_logic ;
		locked : out std_logic 
	);
    end component;

    signal clock, light : std_logic;
    signal counter : integer;
    signal ram_a : std_logic_vector(11 downto 0);
    signal current_state : state_t;

    constant CLK_PERIOD  : integer := 10;     -- in ns
    constant INIT_TIME   : integer := 150000; -- in ns
    constant INIT_PCHG   : integer := 10;     -- num of iterations
    constant INIT_AREF   : integer := 10;     -- num of iterations
    constant BANK_WIDTH  : integer := 2;      -- in bits
    constant ROW_WIDTH   : integer := 13;     -- in bits
    constant COL_WIDTH   : integer := 10;      -- in bits
    constant DATA_WIDTH  : integer := 16;     -- in bits
    constant ADDR_WIDTH  : integer := 22;     -- in bits
    constant BUF_WIDTH   : integer := 12;     -- in bits

    signal clock_100MHz : std_logic;
    signal clock_25MHz  : std_logic;
    signal reset        : std_logic;

    -- client 0
    signal clock0       : std_logic;
    signal strobe0      : std_logic;
    signal wr0          : std_logic;
    signal addr0        : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal nwords0      : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal busy0        : std_logic;

    signal buf_clock_0  : std_logic;
    signal buf_wr_0     : std_logic;
    signal buf_addr_0   : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal buf_data_i_0 : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal buf_data_o_0 : std_logic_vector(DATA_WIDTH - 1 downto 0);

    -- client 1
    signal clock1       : std_logic;
    signal strobe1      : std_logic;
    signal wr1          : std_logic;
    signal addr1        : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal nwords1      : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal busy1        : std_logic;

    signal buf_clock_1  : std_logic;
    signal buf_wr_1     : std_logic;
    signal buf_addr_1   : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal buf_data_i_1 : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal buf_data_o_1 : std_logic_vector(DATA_WIDTH - 1 downto 0);

    -- client 2
    signal clock2       : std_logic;
    signal strobe2      : std_logic;
    signal wr2          : std_logic;
    signal addr2        : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal nwords2      : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal busy2        : std_logic;

    signal buf_clock_2  : std_logic;
    signal buf_wr_2     : std_logic;
    signal buf_addr_2   : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal buf_data_i_2 : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal buf_data_o_2 : std_logic_vector(DATA_WIDTH - 1 downto 0);


    -- just used for test client_fsm
    signal done0 : std_logic;
    signal done1 : std_logic;
    signal done2 : std_logic;

    -- just used for count bandwidth
    signal bw_counter : integer;

    -- VGA
    signal hsync : std_logic;
    signal vsync : std_logic;
    signal red   : std_logic_vector(3 downto 0);
    signal green : std_logic_vector(3 downto 0);
    signal blue  : std_logic_vector(3 downto 0);

begin

    mypll_u : mypll
    port map (reset, CLOCK_50MHz, clock_100MHz, open);
	
    reset      <= KEY(0);
    SDRAM_A    <= "0" & ram_a;
    DISP0_D(0) <= light;
    DISP0_D(1) <= KEY(1);
    DISP0_D(2) <= done0;
    DISP0_D(3) <= done1;
    DISP0_D(4) <= done2;
    DISP0_D(5) <= buf_wr_1;

    process (clock_50MHz)
    begin
        if clock_50MHz'event and clock_50MHz = '1' then
            clock_25MHz <= not clock_25MHz;
        end if;
    end process;


    sdram_controller_u : entity work.sdram_controller
    port map (
        clock => clock_100MHz,
        reset => reset,

        -- client 0
        clock0  => clock_50MHz,
        strobe0 => strobe0,
        wr0     => wr0,
        addr0   => addr0,  
        nwords0 => nwords0,
        busy0   => busy0,

        buf_clock_0  => buf_clock_0,
        buf_wr_0     => buf_wr_0, 
        buf_addr_0   => buf_addr_0,
        buf_data_i_0 => buf_data_i_0,
        buf_data_o_0 => buf_data_o_0,

        -- client 1
        clock1  => clock_50MHz,
        strobe1 => strobe1,
        wr1     => wr1,
        addr1   => addr1,  
        nwords1 => nwords1,
        busy1   => busy1,

        buf_clock_1  => buf_clock_1,
        buf_wr_1     => buf_wr_1, 
        buf_addr_1   => buf_addr_1,
        buf_data_i_1 => buf_data_i_1,
        buf_data_o_1 => buf_data_o_1,

        -- client 2
        clock2  => clock_50MHz,
        strobe2 => strobe2,
        wr2     => wr2,
        addr2   => addr2,  
        nwords2 => nwords2,
        busy2   => busy2,

        buf_clock_2  => buf_clock_2,
        buf_wr_2     => buf_wr_2, 
        buf_addr_2   => buf_addr_2,
        buf_data_i_2 => buf_data_i_2,
        buf_data_o_2 => buf_data_o_2,

        -- sdram interface
        ram_clock  => SDRAM_CLK,
        ram_cke    => SDRAM_CKE,
        ram_ncs    => SDRAM_CS_N,
        ram_nras   => SDRAM_RAS_N,
        ram_ncas   => SDRAM_CAS_N,
        ram_nwe    => SDRAM_WE_N,
        ram_bs     => SDRAM_BA,
        ram_a      => ram_a,
        ram_dqm    => SDRAM_DQM,
        ram_dq     => SDRAM_D
    );

    client_1_u : entity work.sdram_client_fsm2
    generic map (
        ADDR_WIDTH,
        DATA_WIDTH,
        BUF_WIDTH,
        240
    )
    port map (
        clock      => clock_50MHz,
        reset      => KEY(2),--reset,
        busy       => busy1,
        strobe     => strobe1,
        wr         => wr1,
        addr       => addr1,
        nwords     => nwords1,
        done       => done1,
        buf_clock  => buf_clock_1,
        buf_wr     => buf_wr_1,
        buf_addr   => buf_addr_1,
        buf_data_i => buf_data_i_1,
        buf_data_o => buf_data_o_1
    );

    strobe2 <= '0';
    done2 <= '0';
    --client_2_u : entity work.sdram_client_fsm2
    --generic map (
        --ADDR_WIDTH,
        --DATA_WIDTH,
        --BUF_WIDTH,
        --15
    --)
    --port map (
        --clock      => clock_50MHz,
        --reset      => KEY(3), --reset,
        --busy       => busy2,
        --strobe     => strobe2,
        --wr         => wr2,
        --addr       => addr2,
        --nwords     => nwords2,
        --done       => done2,
        --buf_clock  => buf_clock_2,
        --buf_wr     => buf_wr_2,
        --buf_addr   => buf_addr_2,
        --buf_data_i => buf_data_i_2,
        --buf_data_o => buf_data_o_2
    --);

    vga_sdram_u : entity work.vga_sdram
    generic map (
        ADDR_WIDTH => ADDR_WIDTH,
        DATA_WIDTH => DATA_WIDTH,
        BUF_WIDTH  => BUF_WIDTH
    )
    port map (
        clock_25MHz,
        KEY(1),--reset, 

        -- VGA interface
        hsync => VGA_HS, --hsync, 
        vsync => VGA_VS, --vsync, 
        red   => VGA_R, --red,   
        green => VGA_G, --green, 
        blue  => VGA_B, --blue,

        -- sdram interface
        mem_busy   => busy0,   
        mem_strobe => strobe0,
        mem_wr     => wr0,
        mem_addr   => addr0,
        mem_nwords => nwords0,

        buf_clock  => buf_clock_0,  
        buf_wr     => buf_wr_0,
        buf_addr   => buf_addr_0,
        buf_data_i => buf_data_i_0,
        buf_data_o => buf_data_o_0
    );

end behavior;
