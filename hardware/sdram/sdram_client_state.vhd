library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_client_state is
    generic (
        ADDR_WIDTH  : integer := 22;     -- in bits
        DATA_WIDTH  : integer := 16;     -- in bits
        BUF_WIDTH   : integer := 12      -- in bits
    );
    port (
        -- client
        clock        : in std_logic;
        reset        : in std_logic;

        sys_strobe   : in std_logic;
        sys_wr       : in std_logic;
        sys_addr     : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
        sys_nwords   : in std_logic_vector(BUF_WIDTH - 1 downto 0);

        -- arbiter
        arb_wr       : out std_logic;
        arb_addr     : out std_logic_vector(ADDR_WIDTH - 1 downto 0);
        arb_nwords   : out std_logic_vector(BUF_WIDTH - 1 downto 0);

        client       : in std_logic;
        update       : in std_logic;
        mem_busy     : out std_logic;

        -- from sdram
        sdram_clock  : in std_logic;
        sdram_wr     : in std_logic;
        sdram_addr   : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        sdram_data_i : in std_logic_vector(DATA_WIDTH - 1 downto 0);
        sdram_data_o : out std_logic_vector(DATA_WIDTH - 1 downto 0);

        -- to client
        buf_clock    : out std_logic;
        buf_wr       : out std_logic;
        buf_addr     : out std_logic_vector(BUF_WIDTH - 1 downto 0);
        buf_data_i   : out std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_data_o   : in std_logic_vector(DATA_WIDTH - 1 downto 0)
    );
end sdram_client_state;

architecture behavior of sdram_client_state is
    constant ZERO : std_logic_vector(31 downto 0) := (others => '0');

    signal nwords_reg : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal nwords_tmp : std_logic_vector(BUF_WIDTH - 1 downto 0);

    signal addr_reg   : std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal addr_tmp   : std_logic_vector(ADDR_WIDTH - 1 downto 0);

    signal wr_reg     : std_logic;
    signal wr_tmp     : std_logic;

    signal strobe_irq : std_logic;
    signal strobe_ack : std_logic;

    signal equal_zero : std_logic;

    signal update_delay : std_logic;
    signal client_delay : std_logic;

    signal buf_addr_reg : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal buf_addr_aux : std_logic_vector(BUF_WIDTH - 1 downto 0);
begin
    mem_busy   <= '1' when equal_zero = '0' or strobe_irq = '1' or sys_strobe = '1' else '0';
    equal_zero <= '1' when nwords_reg = ZERO(BUF_WIDTH - 1 downto 0) else
                  '0';

    buf_clock    <= sdram_clock;
    buf_wr       <= '1' when sdram_wr = '1' and client = '1' else '0';
    buf_addr     <= std_logic_vector(unsigned(sdram_addr) + unsigned(buf_addr_reg));
    buf_data_i   <= sdram_data_i;
    sdram_data_o <= buf_data_o;

    arb_wr     <= wr_reg;
    arb_addr   <= addr_reg;
    arb_nwords <= nwords_reg;

    process (clock, reset)
    begin
        if reset = '1' then
            nwords_tmp <= (others => '0');
            addr_tmp   <= (others => '0');
            wr_tmp     <= '0';
            strobe_irq <= '0';
        elsif clock'event and clock = '1' then
            if sys_strobe = '1' then
                nwords_tmp <= sys_nwords;
                addr_tmp   <= sys_addr;
                wr_tmp     <= sys_wr;
            end if;

            if sys_strobe = '1' then
                strobe_irq <= '1';
            elsif strobe_ack = '1' then
                strobe_irq <= '0';
            end if;
        end if;
    end process;

    process (sdram_clock, reset)
    begin
        if reset = '1' then
            nwords_reg <= (others => '0');
            addr_reg   <= (others => '0');
            wr_reg     <= '0';
            strobe_ack <= '0';
            update_delay <= '0';
            client_delay <= '0';
            buf_addr_reg <= (others => '0');
            buf_addr_aux <= (others => '0');
        elsif sdram_clock'event and sdram_clock = '1' then
            update_delay <= update;
            client_delay <= client;

            if update = '1' and client = '1' then
                addr_reg     <= std_logic_vector(unsigned(addr_reg) + 1);
                buf_addr_aux <= std_logic_vector(unsigned(buf_addr_aux) + 1);
            elsif update_delay = '1' and client_delay = '1' then
                buf_addr_reg <= buf_addr_aux; 
            elsif strobe_irq = '1' then
                addr_reg     <= addr_tmp;
                buf_addr_aux <= (others => '0');
                buf_addr_reg <= (others => '0');
            end if;

            if update = '1' and client = '1' then
                nwords_reg <= std_logic_vector(unsigned(nwords_reg) - 1);
            elsif strobe_irq = '1' then
                nwords_reg <= nwords_tmp;
                wr_reg     <= wr_tmp;
            end if;

            if strobe_irq = '1' then
                strobe_ack <= '1';
            else
                strobe_ack <= '0';
            end if;
        end if;
    end process;
end behavior;
