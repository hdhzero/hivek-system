library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_arbiter is
    generic (
        ADDR_WIDTH  : integer := 22;     -- in bits
        DATA_WIDTH  : integer := 16;     -- in bits
        BUF_WIDTH   : integer := 12      -- in bits
    );
    port (
        clock        : in std_logic;
        reset        : in std_logic;

        -- sdram ctrl
        sdram_strobe : out std_logic;
        sdram_leave  : out std_logic;
        sdram_wr     : out std_logic;
        sdram_addr   : out std_logic_vector(ADDR_WIDTH - 1 downto 0);
        sdram_nwords : out std_logic_vector(BUF_WIDTH - 1 downto 0);
        sdram_busy   : in std_logic;
        sdram_update : in std_logic;

        -- clients
        client_0     : out std_logic;
        wr_0         : in std_logic;
        addr_0       : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
        nwords_0     : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        update_0     : out std_logic;

        client_1     : out std_logic;
        wr_1         : in std_logic;
        addr_1       : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
        nwords_1     : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        update_1     : out std_logic;

        client_2     : out std_logic;
        wr_2         : in std_logic;
        addr_2       : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
        nwords_2     : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        update_2     : out std_logic;

        -- buffer
        -- from sdram
        buf_sdram_clock  : in std_logic;
        buf_sdram_wr     : in std_logic;
        buf_sdram_addr   : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        buf_sdram_data_i : in std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_sdram_data_o : out std_logic_vector(DATA_WIDTH - 1 downto 0);

        -- to client
        buf_clock    : out std_logic;
        buf_wr       : out std_logic;
        buf_addr     : out std_logic_vector(BUF_WIDTH - 1 downto 0);
        buf_data_i   : out std_logic_vector(DATA_WIDTH - 1 downto 0);

        buf_data_o_0 : in std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_data_o_1 : in std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_data_o_2 : in std_logic_vector(DATA_WIDTH - 1 downto 0)
    );
end sdram_arbiter;

architecture behavior of sdram_arbiter is
    type state_t is (
        idle,
        priority_access,
        priority_wait_sdram_free,
        normal_access,
        normal_wait_sdram_free
    );

    signal current_state : state_t;

    constant ZERO : std_logic_vector(31 downto 0) := (others => '0');
    constant NUM_CLIENTS : integer := 3;

    signal client_reg  : integer;
    signal round_robin : integer;
    signal counter     : integer;

    signal irq : std_logic_vector(NUM_CLIENTS - 1 downto 0);

begin
    irq(0) <= '1' when nwords_0 /= ZERO(BUF_WIDTH - 1 downto 0) else '0';
    irq(1) <= '1' when nwords_1 /= ZERO(BUF_WIDTH - 1 downto 0) else '0';
    irq(2) <= '1' when nwords_2 /= ZERO(BUF_WIDTH - 1 downto 0) else '0';

    client_0 <= '1' when client_reg = 0 else '0';
    client_1 <= '1' when client_reg = 1 else '0';
    client_2 <= '1' when client_reg = 2 else '0';

    update_0 <= sdram_update;
    update_1 <= sdram_update;
    update_2 <= sdram_update;

    -- buffer <--> sdram
    buf_clock  <= buf_sdram_clock;
    buf_wr     <= buf_sdram_wr;
    buf_addr   <= buf_sdram_addr;
    buf_data_i <= buf_sdram_data_i;

    buf_sdram_data_o <= buf_data_o_0 when client_reg = 0 else
                        buf_data_o_1 when client_reg = 1 else
                        buf_data_o_2; 

    sdram_wr <= wr_0 when client_reg = 0 else
                wr_1 when client_reg = 1 else
                wr_2;

    sdram_addr <= addr_0 when client_reg = 0 else
                  addr_1 when client_reg = 1 else
                  addr_2;

    sdram_nwords <= nwords_0 when client_reg = 0 else
                    nwords_1 when client_reg = 1 else
                    nwords_2;

    process (clock, reset)
    begin
        if reset = '1' then
            current_state <= idle;
            client_reg    <= 0;
            sdram_strobe  <= '0';
            sdram_leave   <= '0';
            round_robin   <= 1;
            counter       <= 0;
        elsif clock'event and clock = '1' then
            case current_state is
                when idle =>
                    sdram_leave  <= '0';
                    sdram_strobe <= '0';

                    if irq(0) = '1' then
                        current_state <= priority_access;
                        client_reg    <= 0;
                    elsif irq(round_robin) = '1' then
                        current_state <= normal_access;
                        client_reg    <= round_robin;

                        if round_robin = NUM_CLIENTS - 1 then
                            round_robin <= 1;
                        else
                            round_robin <= round_robin + 1;
                        end if;
                    else
                        if round_robin = NUM_CLIENTS - 1 then
                            round_robin <= 1;
                        else
                            round_robin <= round_robin + 1;
                        end if;
                    end if;

                when priority_access =>
                    counter <= 0;

                    if sdram_busy = '0' then
                        sdram_strobe  <= '1';
                        current_state <= priority_wait_sdram_free;
                    end if;

                when priority_wait_sdram_free =>
                    sdram_strobe <= '0';
                    counter      <= counter + 1;

                    if sdram_busy = '0' and counter > 7 then
                        current_state <= idle;
                        counter <= 0;
                    end if;

                when normal_access =>
                    counter <= 0;

                    if sdram_busy = '0' then
                        sdram_strobe  <= '1';
                        current_state <= normal_wait_sdram_free;
                    end if;

                when normal_wait_sdram_free =>
                    sdram_strobe <= '0';
                    counter      <= counter + 1;

                    if sdram_busy = '0' and counter > 7 then
                        current_state <= idle;
                        sdram_leave   <= '0';
                        counter <= 0;
                    else
                        if irq(0) = '1' then
                            sdram_leave <= '1';
                        else
                            sdram_leave <= '0';
                        end if;
                    end if;
                        
                when others =>
                    current_state <= idle;
                    sdram_leave   <= '0';
                    sdram_strobe  <= '0';
                    client_reg    <= 0;
                    round_robin   <= 1;
            end case;
        end if;
    end process;
end behavior;
