library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_ctrl_tb is
end sdram_ctrl_tb;

architecture behavior of sdram_ctrl_tb is
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

    signal clock : std_logic;
    signal clock_50MHz : std_logic;
    signal reset : std_logic;

    -- system interface
    signal sys_strobe : std_logic;  -- addr, nwords and wr strobe
    signal sys_leave  : std_logic;
    signal sys_wr     : std_logic;  -- read (0) or write (1)
    signal sys_addr   : std_logic_vector(ADDR_WIDTH - 1 downto 0); -- initial address
    signal sys_nwords : std_logic_vector(11 downto 0); -- number of words to read/write
    signal sys_busy   : std_logic; -- mem busy? 1 = yes, 0 = no
    signal sys_update : std_logic;

    -- buffer
    signal buffer_clock  : std_logic;
    signal buffer_wr     : std_logic;
    signal buffer_addr   : std_logic_vector(11 downto 0);
    signal buffer_data_i : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal buffer_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal client_wr     : std_logic;
    signal client_addr   : std_logic_vector(11 downto 0);
    signal client_data_i : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal client_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal client_nwords : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal client_addrs  : std_logic_vector(ADDR_WIDTH - 1 downto 0); -- initial address


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

begin
    process
    begin
        clock <= '0';
        wait for 5 ns;
        clock <= '1';
        wait for 5 ns;
    end process;

    process
    begin
        clock_50MHz <= '0';
        wait for 10 ns;
        clock_50MHz <= '1';
        wait for 10 ns;
    end process;

    process (clock, reset)
    begin
        if reset = '1' then
            client_nwords <= "000000000111"; --(others => '0');
            --client_nwords <= "000100000001"; --(others => '0');
            client_addrs <= (others => '0');
        elsif clock'event and clock = '1' then
            if sys_strobe = '1' then
                client_nwords <= sys_nwords;
                client_addrs  <= sys_addr;
            elsif sys_update = '1' then
            client_nwords <= std_logic_vector(unsigned(client_nwords) - 1);
            client_addrs <= std_logic_vector(unsigned(client_addrs) + 1);
            end if;
        end if;
    end process;

    process
    begin
        reset <= '1';
        sys_strobe <= '0';
        sys_leave <= '0';
        sys_wr <= '0';

        client_wr <= '0';
        client_addr   <= (others => '0');
        client_data_i <= (others => '0');
        wait until clock_50MHz'event and clock_50MHz = '1';

        client_wr <= '1';
        client_addr   <= (others => '0');
        client_data_i <= (others => '1');
        wait until clock_50MHz'event and clock_50MHz = '1';

        for i in 0 to 700 loop
            client_wr     <= '1';
            client_addr   <= std_logic_vector(unsigned(client_addr) + 1);
            client_data_i <= std_logic_vector(unsigned(client_data_i) + 2);
            wait until clock_50MHz'event and clock_50MHz = '1';
        end loop;

        wait for 50 ns;
        client_wr <= '0';
        client_addr   <= (others => '0');
        client_data_i <= (others => '0');

        reset <= '0';

        wait for 170 us;
        wait until clock'event and clock = '1';

        for i in 65280 to 65920 loop
            sys_wr     <= '1';
            sys_leave  <= '0';
            sys_strobe <= '1';
            sys_addr   <= std_logic_vector(to_unsigned(i, 22));
            sys_nwords <= std_logic_vector(to_unsigned(1, 12));
            wait until clock'event and clock = '1';

            sys_wr     <= '0';
            sys_leave  <= '0';
            sys_strobe <= '0';
            sys_addr   <= "0000000000000000000000";
            sys_nwords <= "000000000000";
            wait until clock'event and clock = '1';
            wait until sys_busy = '0';

            sys_wr     <= '0';
            sys_leave  <= '0';
            sys_strobe <= '1';
            sys_addr   <= std_logic_vector(to_unsigned(i, 22));
            sys_nwords <= std_logic_vector(to_unsigned(1, 12));
            wait until clock'event and clock = '1';

            sys_wr     <= '0';
            sys_leave  <= '0';
            sys_strobe <= '0';
            sys_addr   <= "0000000000000000000000";
            sys_nwords <= "000000000000";
            wait until clock'event and clock = '1';
            wait until sys_busy = '0';
        end loop;

        wait;
    end process;

    sdram_ctrl_u : entity work.sdram_ctrl
    port map (
        clock => clock,
        reset => reset,

        -- system interface
        sys_strobe => sys_strobe,
        sys_leave  => sys_leave,
        sys_wr     => sys_wr,
        sys_addr   => sys_addr,
        sys_nwords => sys_nwords,
        sys_busy   => sys_busy,
        sys_update => sys_update,

        -- buffer
        buffer_clock  => buffer_clock,
        buffer_wr     => buffer_wr,
        buffer_addr   => buffer_addr,
        buffer_data_i => buffer_data_i,
        buffer_data_o => buffer_data_o,

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

    dual_mem_u : entity work.dual_memory_bram
    generic map (16, 12)
    port map (
        buffer_clock, 
        buffer_wr, buffer_addr, buffer_data_i, buffer_data_o,
        clock_50MHz,
        client_wr, client_addr, client_data_i, client_data_o
    );

    rram_dq   <= ram_dq after 2 ns;
    rram_a    <= ram_a after 2 ns;
    rram_bs   <= ram_bs after 2 ns;
    rram_ncs  <= ram_ncs after 2 ns;
    rram_nras <= ram_nras after 2 ns;
    rram_ncas <= ram_ncas after 2 ns;
    rram_nwe  <= ram_nwe after 2 ns;
    rram_dqm  <= ram_dqm after 2 ns;
end behavior;
