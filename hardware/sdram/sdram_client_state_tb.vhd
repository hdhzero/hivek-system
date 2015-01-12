library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_client_state_tb is
end sdram_client_state_tb;

architecture behavior of sdram_client_state_tb is
    constant ADDR_WIDTH  : integer := 22;     -- in bits
    constant DATA_WIDTH  : integer := 16;     -- in bits
    constant BUF_WIDTH   : integer := 12;     -- in bits

    -- client
    signal clock        : std_logic;
    signal reset        : std_logic;

    signal sys_strobe   : std_logic;
    signal sys_wr       : std_logic;
    signal sys_addr     : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal sys_nwords   : std_logic_vector(BUF_WIDTH - 1 downto 0);

    -- arbiter
    signal arb_wr       : std_logic;
    signal arb_addr     : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal arb_nwords   : std_logic_vector(BUF_WIDTH - 1 downto 0);

    signal client       : std_logic;
    signal update       : std_logic;
    signal mem_busy     : std_logic;

    -- from sdram
    signal sdram_clock  : std_logic;
    signal sdram_wr     : std_logic;
    signal sdram_addr   : std_logic_vector(11 downto 0);
    signal sdram_data_i : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal sdram_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);

    -- to client
    signal buf_clock    : std_logic;
    signal buf_wr       : std_logic;
    signal buf_addr     : std_logic_vector(11 downto 0);
    signal buf_data_i   : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal buf_data_o   : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal test_clock : std_logic;
begin
    process
    begin
        reset <= '1';
        wait for 70 ns;
        reset <= '0';
        wait;
    end process;

    process
    begin
        clock <= '0';
        wait for 10 ns;
        clock <= '1';
        wait for 10 ns;
    end process;

    process
    begin
        test_clock <= '0';
        wait for 5 ns;
        test_clock <= '1';
        wait for 5 ns;
    end process;

    sdram_clock <= test_clock;

    --process
    --begin
        --sdram_clock <= '0';
        --wait for 5 ns;
        --sdram_clock <= '1';
        --wait for 5 ns;
    --end process;


    process
    begin
        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '0';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until clock'event and clock = '1';
        wait until clock'event and clock = '1';
        wait until clock'event and clock = '1';
        wait until clock'event and clock = '1';
        wait until clock'event and clock = '1';
        wait until clock'event and clock = '1';

        -- strobe
        sys_strobe <= '1';
        sys_wr     <= '1';
        sys_addr   <= "0000000000000000000001";
        sys_nwords <= "000000000111";
        client     <= '0';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until clock'event and clock = '1';

        -- nop
        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '1';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until test_clock'event and test_clock = '1';

        -- update without being the client
        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '1';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until test_clock'event and test_clock = '1';

        -- update being the client
        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '1';
        update     <= '1';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until test_clock'event and test_clock = '1';

        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '0';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until test_clock'event and test_clock = '1';

        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '0';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until clock'event and clock = '1';

        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '0';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until clock'event and clock = '1';

        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '0';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until clock'event and clock = '1';

        sys_strobe <= '0';
        sys_wr     <= '0';
        sys_addr   <= "0000000000000000000000";
        sys_nwords <= "000000000000";
        client     <= '0';
        update     <= '0';

        sdram_wr   <= '0';
        sdram_addr <= "000000000000";
        sdram_data_i <= x"0000";
        buf_data_o   <= x"0000";
        wait until clock'event and clock = '1';

        wait;
    end process;

    sdram_client_state_u : entity work.sdram_client_state
    generic map (
        ADDR_WIDTH => ADDR_WIDTH,     -- in bits
        DATA_WIDTH => DATA_WIDTH,     -- in bits
        BUF_WIDTH  => BUF_WIDTH       -- in bits
    )
    port map (
        -- client
        clock        => clock,
        reset        => reset,

        sys_strobe   => sys_strobe,
        sys_wr       => sys_wr,
        sys_addr     => sys_addr,
        sys_nwords   => sys_nwords,

        -- arbiter
        arb_wr       => arb_wr,
        arb_addr     => arb_addr,
        arb_nwords   => arb_nwords,

        client       => client,
        update       => update,
        mem_busy     => mem_busy,

        -- from sdram
        sdram_clock  => sdram_clock,
        sdram_wr     => sdram_wr,
        sdram_addr   => sdram_addr,
        sdram_data_i => sdram_data_i,
        sdram_data_o => sdram_data_o,

        -- to client
        buf_clock    => buf_clock,
        buf_wr       => buf_wr,
        buf_addr     => buf_addr,
        buf_data_i   => buf_data_i,
        buf_data_o   => buf_data_o
    );
end behavior;
