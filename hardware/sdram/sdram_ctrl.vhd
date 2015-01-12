library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_ctrl is
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
        clock : in std_logic;
        reset : in std_logic;

        -- system interface
        sys_strobe : in std_logic;  -- addr, nwords and wr strobe
        sys_leave  : in std_logic;  -- stop an access
        sys_wr     : in std_logic;  -- read (0) or write (1)
        sys_addr   : in std_logic_vector(ADDR_WIDTH - 1 downto 0); -- initial address
        sys_nwords : in std_logic_vector(BUF_WIDTH - 1 downto 0); -- number of words to read/write
        sys_busy   : out std_logic; -- mem busy? 1 = yes, 0 = no
        sys_update : out std_logic;

        -- buffer
        buffer_clock  : out std_logic;
        buffer_wr     : out std_logic;
        buffer_addr   : out std_logic_vector(BUF_WIDTH - 1 downto 0);
        buffer_data_i : out std_logic_vector(DATA_WIDTH - 1 downto 0);
        buffer_data_o : in std_logic_vector(DATA_WIDTH - 1 downto 0);

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
end sdram_ctrl;

architecture behavior of sdram_ctrl is
    type state_t is (
        init,
        init_precharge,
        init_auto_refresh,
        init_load_mode_reg,
        variable_nop,
        auto_refresh,
        idle,
        setup_write,
        setup_read,

        writing0,
        writing1,

        reading0,
        reading1,
        reading2,
        reading3,
        reading4,
        reading5
    );

    constant INIT_CYCLES : integer := INIT_TIME / CLK_PERIOD;
    constant AREF_CYCLES : integer := AREF_COUNT / CLK_PERIOD;
    constant NUM_ROWS    : integer := 2 ** ROW_WIDTH;
    constant NUM_COLUMNS : integer := 2 ** COL_WIDTH;

    constant IDLE_PCHG_CYCLES : integer := (tRP / tCK) + 1;
    constant IDLE_AREF_CYCLES : integer := (tRC / tCK) + 1;
    constant IDLE_ACT_CYCLES  : integer := (tRCD / tCK) + 1;

    -- SDRAM commands
    -- CS#    RAS#    CAS#    WE#
    constant NO_OP        : std_logic_vector(3 downto 0) := "0111";
    constant PRECHARGE_OP : std_logic_vector(3 downto 0) := "0010";
    constant AUTO_REF_OP  : std_logic_vector(3 downto 0) := "0001";
    constant LOAD_MREG_OP : std_logic_vector(3 downto 0) := "0000";
    constant ACTIVATE_OP  : std_logic_vector(3 downto 0) := "0011";
    constant WRITE_OP     : std_logic_vector(3 downto 0) := "0100";
    constant READ_OP      : std_logic_vector(3 downto 0) := "0101";

    constant MODE_REGISTER : std_logic_vector(ROW_WIDTH - 1 downto 0) := (
        9 => '1',
        8 => '0',
        7 => '0',
        6 => '0',
        5 => '1',
        4 => '1',
        3 => '0',
        2 => '0',
        1 => '0',
        0 => '0',
        others => '0'
    );

    constant ADDR_R_MSB : integer := COL_WIDTH + ROW_WIDTH;
    constant ADDR_B_MSB : integer := ADDR_R_MSB + BANK_WIDTH;

    constant ZERO : std_logic_vector(31 downto 0) := (others => '0');
    constant ONES : std_logic_vector(31 downto 0) := (others => '1');
    constant ONE  : std_logic_vector(31 downto 0) := (
        0 => '1',
        others => '0'
    );

    signal current_state : state_t;
    signal back_state    : state_t;

    -- counters
    signal nop_counter : integer;
    signal nop_limit   : integer;
    signal aux_counter : integer;
    signal ref_counter : integer;

    signal addr_reg   : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal nwords_reg : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal wr_reg     : std_logic;
    signal row_addr   : std_logic_vector(ROW_WIDTH - 1 downto 0);
    signal col_addr   : std_logic_vector(COL_WIDTH - 1 downto 0);
    signal bs_addr    : std_logic_vector(BANK_WIDTH - 1 downto 0);

    signal buf_wr     : std_logic;
    signal buf_addr   : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal buf_data_i : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal buf_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal dq_sel   : std_logic;
    signal command  : std_logic_vector(3 downto 0);
    signal r_bs     : std_logic_vector(BANK_WIDTH - 1 downto 0);
    signal r_a      : std_logic_vector(ROW_WIDTH - 1 downto 0);
    signal r_dqm    : std_logic_vector(DATA_WIDTH / 8 - 1 downto 0);
    signal r_dq     : std_logic_vector(DATA_WIDTH - 1 downto 0);

    signal buf_addr_plus_one : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal r_a_plus_one      : std_logic_vector(ROW_WIDTH - 1 downto 0);

    signal nwords_reg_minus_one : std_logic_vector(BUF_WIDTH - 1 downto 0);

    signal max_column      : std_logic;
    signal read_max_column : std_logic;
    signal one_word_left   : std_logic;
    signal read_words_left : std_logic;
    signal leave_write     : std_logic;
    signal leave_read      : std_logic;
    signal r_a_col         : std_logic_vector(COL_WIDTH - 1 downto 0);
    signal w_a_col         : std_logic_vector(COL_WIDTH - 1 downto 0);
begin
    buffer_clock  <= clock;
    buffer_wr     <= buf_wr;
    buffer_addr   <= buf_addr;
    buffer_data_i <= buf_data_i;
    buf_data_o    <= buffer_data_o;

    ram_clock <= clock;
    ram_cke   <= '1';
    ram_ncs   <= command(3);
    ram_nras  <= command(2);
    ram_ncas  <= command(1);
    ram_nwe   <= command(0);
    ram_bs    <= r_bs;
    ram_a     <= r_a;
    ram_dqm   <= r_dqm;

    process (r_dq, dq_sel)
    begin
        if dq_sel = '1' then
            ram_dq <= r_dq after 2 ns;
        else
            ram_dq <= (others => 'Z') after 2 ns;
        end if;
    end process;

    row_addr <= addr_reg(ADDR_R_MSB - 1 downto COL_WIDTH);
    col_addr <= addr_reg(COL_WIDTH - 1 downto 0);
    bs_addr  <= addr_reg(ADDR_B_MSB - 1 downto ADDR_R_MSB);

    buf_addr_plus_one    <= std_logic_vector(unsigned(buf_addr) + 1);
    r_a_plus_one         <= std_logic_vector(unsigned(r_a) + 1);
    nwords_reg_minus_one <= std_logic_vector(unsigned(nwords_reg) - 1);

    w_a_col    <= std_logic_vector(unsigned(r_a(COL_WIDTH - 1 downto 0)) + 1);
    r_a_col    <= std_logic_vector(unsigned(r_a(COL_WIDTH - 1 downto 0)) - 3);
    max_column <= '1' when w_a_col = ONES(COL_WIDTH - 1 downto 0) else '0';

    one_word_left   <= '0' when unsigned(nwords_reg) > 1 else '1';
    read_max_column <= '1' when r_a_col = ONES(COL_WIDTH - 1 downto 0) else '0';

    leave_write <= max_column or sys_leave or one_word_left;
    leave_read  <= read_max_column or sys_leave or one_word_left;


    process (clock, reset)
    begin
        if reset = '1' then
            sys_update <= '0';
            sys_busy   <= '1';

            r_bs    <= (others => '0');
            r_a     <= (others => '0');
            r_dqm   <= (others => '0');
            r_dq    <= (others => '0');
            command <= NO_OP;
            dq_sel  <= '0';

            addr_reg   <= (others => '0');
            nwords_reg <= (others => '0');
            wr_reg     <= '0';
            buf_wr     <= '0';
            buf_addr   <= (others => '0');
            buf_data_i <= (others => '0');

            nop_counter <= 0;
            ref_counter <= 0;
            nop_limit   <= 0;
            aux_counter <= 0;

            current_state <= init;
            back_state    <= init;

        elsif clock'event and clock = '1' then
            case current_state is
                when init =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= (others => '0');
                    r_a     <= (others => '0');
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= NO_OP;
                    dq_sel  <= '0';

                    addr_reg   <= (others => '0');
                    nwords_reg <= (others => '0');
                    wr_reg     <= '0';

                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= 0;
                    nop_limit   <= INIT_CYCLES;
                    aux_counter <= 0;

                    current_state <= variable_nop;
                    back_state    <= init_precharge;

                when init_precharge =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= (others => '0');
                    r_a     <= (10 => '1', others => '0');
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= PRECHARGE_OP;
                    dq_sel  <= '0';

                    addr_reg   <= (others => '0');
                    nwords_reg <= (others => '0');
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= 0;
                    nop_limit   <= IDLE_PCHG_CYCLES;

                    current_state <= variable_nop;

                    if aux_counter < INIT_PCHG then
                        back_state  <= init_precharge;
                        aux_counter <= aux_counter + 1;
                    else
                        back_state  <= init_auto_refresh;
                        aux_counter <= 0;
                    end if;

                when init_auto_refresh =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= (others => '0');
                    r_a     <= (others => '0');
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= AUTO_REF_OP;
                    dq_sel  <= '0';

                    addr_reg   <= (others => '0');
                    nwords_reg <= (others => '0');
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= 0;
                    nop_limit   <= IDLE_AREF_CYCLES;

                    current_state <= variable_nop;

                    if aux_counter < INIT_AREF then
                        back_state  <= init_auto_refresh;
                        aux_counter <= aux_counter + 1;
                    else
                        back_state  <= init_load_mode_reg;
                        aux_counter <= 0;   
                    end if;

                when init_load_mode_reg =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= (others => '0');
                    r_a     <= MODE_REGISTER;
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= LOAD_MREG_OP;
                    dq_sel  <= '0';

                    addr_reg   <= (others => '0');
                    nwords_reg <= (others => '0');
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= 0;
                    nop_limit   <= 10;
                    aux_counter <= 0;

                    current_state <= variable_nop;
                    back_state    <= idle;

                when auto_refresh =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= (others => '0');
                    r_a     <= (others => '0');
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= AUTO_REF_OP;
                    dq_sel  <= '0';

                    addr_reg   <= (others => '0');
                    nwords_reg <= (others => '0');
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= 0;
                    nop_limit   <= IDLE_AREF_CYCLES;
                    aux_counter <= 0;

                    current_state <= variable_nop;
                    back_state    <= idle;

                when idle =>
                    sys_update <= '0';

                    r_bs    <= (others => '0');
                    r_a     <= (others => '0');
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= NO_OP;
                    dq_sel  <= '0';

                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= 0;
                    aux_counter <= 0;

                    back_state  <= init;

                    if ref_counter >= AREF_CYCLES then
                        current_state <= auto_refresh;
                        sys_busy      <= '1';
                        addr_reg      <= (others => '0');
                        nwords_reg    <= (others => '0');
                    elsif sys_strobe = '1' then
                        addr_reg   <= sys_addr;
                        nwords_reg <= sys_nwords;
                        sys_busy   <= '1';

                        if unsigned(sys_nwords) = 0 then
                            current_state <= idle;
                        elsif sys_wr = '1' then
                            current_state <= setup_write;
                        else
                            current_state <= setup_read;
                        end if;
                    else
                        current_state <= idle;
                        sys_busy      <= '0';
                        addr_reg      <= (others => '0');
                        nwords_reg    <= (others => '0');
                    end if;

                when setup_read =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= bs_addr;
                    r_a     <= row_addr;
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= ACTIVATE_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= IDLE_ACT_CYCLES;
                    aux_counter <= 0;

                    current_state <= variable_nop;
                    back_state    <= reading0;

                when reading0 =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= r_bs;
                    r_a(COL_WIDTH - 1 downto 0) <= col_addr;
                    r_a(ROW_WIDTH - 1 downto COL_WIDTH) <= (others => '0');
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= READ_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= 0;
                    aux_counter <= 0;

                    current_state <= reading1;
                    back_state    <= init;

                when reading1 =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= r_bs;
                    r_a     <= r_a_plus_one;
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= READ_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= 0;
                    aux_counter <= 0;

                    current_state <= reading2;
                    back_state    <= init;

                when reading2 =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= r_bs;
                    r_a     <= r_a_plus_one;
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= READ_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= 0;
                    aux_counter <= 0;

                    current_state <= reading3;
                    back_state    <= init;

                when reading3 =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= r_bs;
                    r_a     <= r_a_plus_one;
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= READ_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= 0;
                    aux_counter <= 0;

                    current_state <= reading5;
                    back_state    <= init;


                when reading5 =>
                    sys_update <= '1';
                    sys_busy   <= '1';

                    r_bs    <= r_bs;
                    r_a     <= r_a_plus_one;
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= READ_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg_minus_one;
                    wr_reg     <= '0';
                    buf_wr     <= '1';
                    buf_data_i <= ram_dq;

                    ref_counter <= ref_counter + 1;

                    if aux_counter = 0 then
                        buf_addr    <= (others => '0');
                        aux_counter <= 1;
                    else
                        buf_addr    <= buf_addr_plus_one;
                        aux_counter <= 1;
                    end if;

                    if ref_counter >= AREF_CYCLES then
                        r_a(10) <= '1';
                        current_state <= variable_nop;
                        back_state    <= auto_refresh;
                        nop_counter   <= 0;
                        nop_limit     <= IDLE_PCHG_CYCLES;
                    elsif leave_read = '1' then
                        r_a(10) <= '1';
                        current_state <= variable_nop;
                        back_state    <= idle;
                        nop_counter   <= 0;
                        nop_limit     <= IDLE_PCHG_CYCLES;
                    else
                        r_a(10) <= '0';
                        current_state <= reading5;
                        back_state    <= init;
                        nop_counter   <= 0;
                        nop_limit     <= 0;
                    end if;

                when setup_write =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= bs_addr;
                    r_a     <= row_addr;
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= ACTIVATE_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= IDLE_ACT_CYCLES;
                    aux_counter <= 0;

                    current_state <= variable_nop;
                    back_state    <= writing0;

                when writing0 =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= r_bs;
                    r_a     <= (others => '0');
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= NO_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= buf_addr_plus_one;
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= 0;
                    aux_counter <= 0;

                    current_state <= writing1;
                    back_state    <= init;

                when writing1 =>
                    sys_update <= '1';
                    sys_busy   <= '1';

                    r_bs    <= r_bs;
                    r_dqm   <= (others => '0');
                    r_dq    <= buf_data_o;
                    command <= WRITE_OP;
                    dq_sel  <= '1';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg_minus_one;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= buf_addr_plus_one;
                    buf_data_i <= (others => '0');

                    ref_counter <= ref_counter + 1;
                    aux_counter <= 1;

                    if aux_counter = 0 then
                        r_a(COL_WIDTH - 1 downto 0) <= col_addr;
                        r_a(ROW_WIDTH - 1 downto COL_WIDTH) <= (others => '0');
                    else
                        r_a <= r_a_plus_one;
                    end if;

                    if ref_counter >= AREF_CYCLES then
                        r_a(10)       <= '1';
                        current_state <= variable_nop;
                        back_state    <= auto_refresh;
                        nop_counter   <= 0;
                        nop_limit     <= IDLE_PCHG_CYCLES;
                    elsif leave_write = '1' then
                        r_a(10)       <= '1';
                        current_state <= variable_nop;
                        back_state    <= idle;
                        nop_counter   <= 0;
                        nop_limit     <= IDLE_PCHG_CYCLES;
                    else
                        r_a(10) <= '0';
                        current_state <= writing1;
                        back_state    <= init;
                        nop_counter   <= 0;
                        nop_limit     <= 0;
                    end if;
                        
                when variable_nop =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= r_bs;
                    r_a     <= r_a;
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= NO_OP;
                    dq_sel  <= '0';

                    addr_reg   <= addr_reg;
                    nwords_reg <= nwords_reg;
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= nop_counter + 1;
                    ref_counter <= ref_counter + 1;
                    nop_limit   <= nop_limit;
                    aux_counter <= aux_counter;

                    back_state  <= back_state;

                    if nop_counter < nop_limit then
                        current_state <= variable_nop;
                    else
                        current_state <= back_state;
                    end if;

                when others =>
                    sys_update <= '0';
                    sys_busy   <= '1';

                    r_bs    <= (others => '0');
                    r_a     <= (others => '0');
                    r_dqm   <= (others => '0');
                    r_dq    <= (others => '0');
                    command <= NO_OP;
                    dq_sel  <= '0';

                    addr_reg   <= (others => '0');
                    nwords_reg <= (others => '0');
                    wr_reg     <= '0';
                    buf_wr     <= '0';
                    buf_addr   <= (others => '0');
                    buf_data_i <= (others => '0');

                    nop_counter <= 0;
                    ref_counter <= 0;
                    nop_limit   <= 0;
                    aux_counter <= 0;

                    current_state <= init;
                    back_state    <= init;
            end case;
        end if;
    end process;
end behavior;
