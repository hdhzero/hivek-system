library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_controller_tb is
end sdram_controller_tb;

architecture behavior of sdram_controller_tb is
    constant CLK_PERIOD  : integer := 10;     -- in ns
    constant INIT_TIME   : integer := 150000; -- in ns
    constant INIT_PCHG   : integer := 10;     -- num of iterations
    constant INIT_AREF   : integer := 10;     -- num of iterations
    constant BANK_WIDTH  : integer := 2;      -- in bits
    constant ROW_WIDTH   : integer := 12;     -- in bits
    constant COL_WIDTH   : integer := 8;      -- in bits
    constant DATA_WIDTH  : integer := 16;     -- in bits
    constant ADDR_WIDTH  : integer := 22;     -- in bits
    constant BUF_WIDTH   : integer := 12;     -- in bits

    signal clock_100MHz : std_logic;
    signal clock_50MHz  : std_logic;
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

    -- sdram interface
    signal ram_clock  : std_logic;
    signal ram_cke    : std_logic;
    signal ram_ncs, rram_ncs   : std_logic;
    signal ram_nras, rram_nras : std_logic;
    signal ram_ncas, rram_ncas : std_logic;
    signal ram_nwe, rram_nwe   : std_logic;
    signal ram_bs, rram_bs     : std_logic_vector(BANK_WIDTH - 1 downto 0);
    signal ram_a, rram_a       : std_logic_vector(ROW_WIDTH - 1 downto 0);
    signal ram_dqm, rram_dqm   : std_logic_vector(DATA_WIDTH / 8 - 1 downto 0);
    signal ram_dq, rram_dq     : std_logic_vector(DATA_WIDTH - 1 downto 0);

    -- just used for test client_fsm
    signal done0 : std_logic;
    signal done1 : std_logic;
    signal done2 : std_logic;

    -- just used for count bandwidth
    signal bw_counter : integer;
begin

    process (clock_100MHz, reset)
    begin
        if reset = '1' then
            bw_counter <= 0;
        elsif clock_100MHz'event and clock_100MHz = '1' then
            if ram_dq /= "XXXXXXXXXXXXXXXX" and ram_dq /= "ZZZZZZZZZZZZZZZZ" then
                bw_counter <= bw_counter + 1;
            end if;
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
        ram_clock  => ram_clock,
        ram_cke    => ram_cke,
        ram_ncs    => ram_ncs,
        ram_nras   => ram_nras,
        ram_ncas   => ram_ncas,
        ram_nwe    => ram_nwe,
        ram_bs     => ram_bs,
        ram_a      => ram_a,
        ram_dqm    => ram_dqm,
        ram_dq     => ram_dq
    );

    -- sdram
    rram_dq   <= ram_dq after 2 ns;
    rram_a    <= ram_a after 2 ns;
    rram_bs   <= ram_bs after 2 ns;
    rram_ncs  <= ram_ncs after 2 ns;
    rram_nras <= ram_nras after 2 ns;
    rram_ncas <= ram_ncas after 2 ns;
    rram_nwe  <= ram_nwe after 2 ns;
    rram_dqm  <= ram_dqm after 2 ns;

    sdram_u : entity work.mt48lc8m16a2
    port map (
        ram_dq, 
        rram_a, 
        rram_bs, 
        ram_clock, 
        ram_cke, 
        rram_ncs, 
        rram_nras, 
        rram_ncas, 
        rram_nwe, 
        rram_dqm
    );

    client_0_u : entity work.sdram_client_fsm
    generic map (
        ADDR_WIDTH,
        DATA_WIDTH,
        BUF_WIDTH,
        2
    )
    port map (
        clock      => clock_50MHz,
        reset      => reset,
        busy       => busy0,
        strobe     => strobe0,
        wr         => wr0,
        addr       => addr0,
        nwords     => nwords0,
        done       => done0,
        buf_clock  => buf_clock_0,
        buf_wr     => buf_wr_0,
        buf_addr   => buf_addr_0,
        buf_data_i => buf_data_i_0,
        buf_data_o => buf_data_o_0
    );

    client_1_u : entity work.sdram_client_fsm
    generic map (
        ADDR_WIDTH,
        DATA_WIDTH,
        BUF_WIDTH,
        3
    )
    port map (
        clock      => clock_50MHz,
        reset      => reset,
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

    client_2_u : entity work.sdram_client_fsm
    generic map (
        ADDR_WIDTH,
        DATA_WIDTH,
        BUF_WIDTH,
        5
    )
    port map (
        clock      => clock_50MHz,
        reset      => reset,
        busy       => busy2,
        strobe     => strobe2,
        wr         => wr2,
        addr       => addr2,
        nwords     => nwords2,
        done       => done2,
        buf_clock  => buf_clock_2,
        buf_wr     => buf_wr_2,
        buf_addr   => buf_addr_2,
        buf_data_i => buf_data_i_2,
        buf_data_o => buf_data_o_2
    );

    process
    begin
        clock_100MHz <= '0';
        wait for 5 ns;
        clock_100MHz <= '1';
        wait for 5 ns;
    end process;

    process
    begin
        clock_50MHz <= '0';
        wait for 10 ns;
        clock_50MHz <= '1';
        wait for 10 ns;
    end process;

    process
    begin
        reset <= '1';
        wait for 50 ns;
        reset <= '0';
        wait;
    end process;

end behavior;
