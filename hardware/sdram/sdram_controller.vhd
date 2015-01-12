library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_controller is
    generic (
        CLK_PERIOD  : integer := 10;     -- in ns
        INIT_TIME   : integer := 150000; -- in ns
        INIT_PCHG   : integer := 10;     -- num of iterations
        INIT_AREF   : integer := 10;     -- num of iterations
        AREF_COUNT  : integer := 5000;   -- in ns
        BANK_WIDTH  : integer := 2;      -- in bits
        ROW_WIDTH   : integer := 12;     -- in bits
        COL_WIDTH   : integer := 8;      -- in bits
        DATA_WIDTH  : integer := 16;     -- in bits
        ADDR_WIDTH  : integer := 22;     -- in bits
        BUF_WIDTH   : integer := 12;     -- in bits

        -- sdram time parameters
        tCK  : integer := 10; -- the same as CLK_PERIOD
        tRP  : integer := 20; -- idle after precharge
        tRC  : integer := 70; -- idle after auto refresh
        tRCD : integer := 20  -- activate to r/w
    );
    port (
        clock      : in std_logic;
        reset      : in std_logic;

        -- client 0
        clock0       : in std_logic;
        strobe0      : in std_logic;
        wr0          : in std_logic;
        addr0        : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
        nwords0      : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        busy0        : out std_logic;

        buf_clock_0  : out std_logic;
        buf_wr_0     : out std_logic;
        buf_addr_0   : out std_logic_vector(BUF_WIDTH - 1 downto 0);
        buf_data_i_0 : out std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_data_o_0 : in std_logic_vector(DATA_WIDTH - 1 downto 0);

        -- client 1
        clock1       : in std_logic;
        strobe1      : in std_logic;
        wr1          : in std_logic;
        addr1        : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
        nwords1      : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        busy1        : out std_logic;

        buf_clock_1  : out std_logic;
        buf_wr_1     : out std_logic;
        buf_addr_1   : out std_logic_vector(BUF_WIDTH - 1 downto 0);
        buf_data_i_1 : out std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_data_o_1 : in std_logic_vector(DATA_WIDTH - 1 downto 0);

        -- client 2
        clock2       : in std_logic;
        strobe2      : in std_logic;
        wr2          : in std_logic;
        addr2        : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
        nwords2      : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        busy2        : out std_logic;

        buf_clock_2  : out std_logic;
        buf_wr_2     : out std_logic;
        buf_addr_2   : out std_logic_vector(BUF_WIDTH - 1 downto 0);
        buf_data_i_2 : out std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_data_o_2 : in std_logic_vector(DATA_WIDTH - 1 downto 0);


        -- sdram interface
        ram_clock  : out std_logic;
        ram_cke    : out std_logic;
        ram_ncs    : out std_logic;
        ram_nras   : out std_logic;
        ram_ncas   : out std_logic;
        ram_nwe    : out std_logic;
        ram_bs     : out std_logic_vector(BANK_WIDTH - 1 downto 0);
        ram_a      : out std_logic_vector(ROW_WIDTH - 1 downto 0);
        ram_dqm    : out std_logic_vector(DATA_WIDTH / 8 - 1 downto 0);
        ram_dq     : inout std_logic_vector(DATA_WIDTH - 1 downto 0)
    );
end sdram_controller;

architecture behavior of sdram_controller is
    -- from arbiter to sdram_ctrl
    signal sdram_arb_strobe : std_logic;
    signal sdram_arb_leave  : std_logic;
    signal sdram_arb_wr     : std_logic;
    signal sdram_arb_busy   : std_logic;
    signal sdram_arb_update : std_logic;
    signal sdram_arb_addr   : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal sdram_arb_nwords : std_logic_vector(BUF_WIDTH - 1 downto 0);

    -- from sdram_ctrl to arbiter
    signal sdram_buf_clock  : std_logic;
    signal sdram_buf_wr     : std_logic;
    signal sdram_buf_addr   : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal sdram_buf_data_i : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal sdram_buf_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);

    -- from client to arbiter
    signal arb_client_0 : std_logic;
    signal arb_wr_0     : std_logic;
    signal arb_addr_0   : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal arb_nwords_0 : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal arb_update_0 : std_logic;

    -- from client to arbiter
    signal arb_client_1 : std_logic;
    signal arb_wr_1     : std_logic;
    signal arb_addr_1   : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal arb_nwords_1 : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal arb_update_1 : std_logic;

    -- from client to arbiter
    signal arb_client_2 : std_logic;
    signal arb_wr_2     : std_logic;
    signal arb_addr_2   : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal arb_nwords_2 : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal arb_update_2 : std_logic;

    -- from arbiter to client
    signal cli_buf_clock    : std_logic;
    signal cli_buf_wr       : std_logic;
    signal cli_buf_addr     : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal cli_buf_data_i   : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal cli_buf_data_o_0 : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal cli_buf_data_o_1 : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal cli_buf_data_o_2 : std_logic_vector(DATA_WIDTH - 1 downto 0);

begin

    sdram_ctrl_u : entity work.sdram_ctrl
    generic map (
        CLK_PERIOD => CLK_PERIOD,
        INIT_TIME  => INIT_TIME,
        INIT_PCHG  => INIT_PCHG,
        INIT_AREF  => INIT_AREF,
        AREF_COUNT => AREF_COUNT,
        BANK_WIDTH => BANK_WIDTH,
        ROW_WIDTH  => ROW_WIDTH,
        COL_WIDTH  => COL_WIDTH,
        DATA_WIDTH => DATA_WIDTH,
        ADDR_WIDTH => ADDR_WIDTH,
        BUF_WIDTH  => BUF_WIDTH,
        tCK        => tCK,
        tRP        => tRP,
        tRC        => tRC,
        tRCD       => tRCD
    )
    port map (
        clock => clock,
        reset => reset,

        -- system interface
        sys_strobe => sdram_arb_strobe,
        sys_leave  => sdram_arb_leave,
        sys_wr     => sdram_arb_wr,
        sys_addr   => sdram_arb_addr,
        sys_nwords => sdram_arb_nwords,
        sys_busy   => sdram_arb_busy,
        sys_update => sdram_arb_update,

        -- buffer
        buffer_clock  => sdram_buf_clock,
        buffer_wr     => sdram_buf_wr,
        buffer_addr   => sdram_buf_addr,
        buffer_data_i => sdram_buf_data_i,
        buffer_data_o => sdram_buf_data_o,

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

    sdram_arbiter_u : entity work.sdram_arbiter
    generic map (
        ADDR_WIDTH => ADDR_WIDTH,
        DATA_WIDTH => DATA_WIDTH,
        BUF_WIDTH  => BUF_WIDTH
    )
    port map (
        clock        => clock,
        reset        => reset,

        -- sdram ctrl
        sdram_strobe => sdram_arb_strobe,
        sdram_leave  => sdram_arb_leave,
        sdram_wr     => sdram_arb_wr,
        sdram_addr   => sdram_arb_addr,
        sdram_nwords => sdram_arb_nwords,
        sdram_busy   => sdram_arb_busy,
        sdram_update => sdram_arb_update,

        -- clients
        client_0     => arb_client_0,
        wr_0         => arb_wr_0,
        addr_0       => arb_addr_0,
        nwords_0     => arb_nwords_0,
        update_0     => arb_update_0,

        client_1     => arb_client_1,
        wr_1         => arb_wr_1,
        addr_1       => arb_addr_1,
        nwords_1     => arb_nwords_1,
        update_1     => arb_update_1,

        client_2     => arb_client_2,
        wr_2         => arb_wr_2,
        addr_2       => arb_addr_2,
        nwords_2     => arb_nwords_2,
        update_2     => arb_update_2,

        -- buffer
        -- from sdram
        buf_sdram_clock  => sdram_buf_clock,
        buf_sdram_wr     => sdram_buf_wr,
        buf_sdram_addr   => sdram_buf_addr,
        buf_sdram_data_i => sdram_buf_data_i,
        buf_sdram_data_o => sdram_buf_data_o,

        -- to client
        buf_clock    => cli_buf_clock,
        buf_wr       => cli_buf_wr,
        buf_addr     => cli_buf_addr,
        buf_data_i   => cli_buf_data_i,

        buf_data_o_0 => cli_buf_data_o_0,
        buf_data_o_1 => cli_buf_data_o_1,
        buf_data_o_2 => cli_buf_data_o_2
    );

    client0_u : entity work.sdram_client_state
    generic map (
        ADDR_WIDTH => ADDR_WIDTH,
        DATA_WIDTH => DATA_WIDTH,
        BUF_WIDTH  => BUF_WIDTH
    )
    port map (
        -- client
        clock => clock0, 
        reset => reset,

        sys_strobe   => strobe0,
        sys_wr       => wr0,
        sys_addr     => addr0,
        sys_nwords   => nwords0,

        -- arbiter
        arb_wr       => arb_wr_0,
        arb_addr     => arb_addr_0,
        arb_nwords   => arb_nwords_0,

        client       => arb_client_0,
        update       => arb_update_0,
        mem_busy     => busy0,

        -- from arbiter
        sdram_clock  => cli_buf_clock,
        sdram_wr     => cli_buf_wr,
        sdram_addr   => cli_buf_addr,
        sdram_data_i => cli_buf_data_i,
        sdram_data_o => cli_buf_data_o_0,

        -- to client
        buf_clock    => buf_clock_0,
        buf_wr       => buf_wr_0,
        buf_addr     => buf_addr_0,
        buf_data_i   => buf_data_i_0,
        buf_data_o   => buf_data_o_0
    );

    client1_u : entity work.sdram_client_state
    generic map (
        ADDR_WIDTH => ADDR_WIDTH,
        DATA_WIDTH => DATA_WIDTH,
        BUF_WIDTH  => BUF_WIDTH
    )
    port map (
        -- client
        clock => clock1, 
        reset => reset,

        sys_strobe   => strobe1,
        sys_wr       => wr1,
        sys_addr     => addr1,
        sys_nwords   => nwords1,

        -- arbiter
        arb_wr       => arb_wr_1,
        arb_addr     => arb_addr_1,
        arb_nwords   => arb_nwords_1,

        client       => arb_client_1,
        update       => arb_update_1,
        mem_busy     => busy1,

        -- from arbiter
        sdram_clock  => cli_buf_clock,
        sdram_wr     => cli_buf_wr,
        sdram_addr   => cli_buf_addr,
        sdram_data_i => cli_buf_data_i,
        sdram_data_o => cli_buf_data_o_1,

        -- to client
        buf_clock    => buf_clock_1,
        buf_wr       => buf_wr_1,
        buf_addr     => buf_addr_1,
        buf_data_i   => buf_data_i_1,
        buf_data_o   => buf_data_o_1
    );

    client2_u : entity work.sdram_client_state
    generic map (
        ADDR_WIDTH => ADDR_WIDTH,
        DATA_WIDTH => DATA_WIDTH,
        BUF_WIDTH  => BUF_WIDTH
    )
    port map (
        -- client
        clock => clock2, 
        reset => reset,

        sys_strobe   => strobe2,
        sys_wr       => wr2,
        sys_addr     => addr2,
        sys_nwords   => nwords2,

        -- arbiter
        arb_wr       => arb_wr_2,
        arb_addr     => arb_addr_2,
        arb_nwords   => arb_nwords_2,

        client       => arb_client_2,
        update       => arb_update_2,
        mem_busy     => busy2,

        -- from arbiter
        sdram_clock  => cli_buf_clock,
        sdram_wr     => cli_buf_wr,
        sdram_addr   => cli_buf_addr,
        sdram_data_i => cli_buf_data_i,
        sdram_data_o => cli_buf_data_o_2,

        -- to client
        buf_clock    => buf_clock_2,
        buf_wr       => buf_wr_2,
        buf_addr     => buf_addr_2,
        buf_data_i   => buf_data_i_2,
        buf_data_o   => buf_data_o_2
    );

end behavior;
