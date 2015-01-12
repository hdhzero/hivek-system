library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_client_fsm is
    generic (
        ADDR_WIDTH : integer := 22;
        DATA_WIDTH : integer := 16;
        BUF_WIDTH  : integer := 12;
        INC        : integer := 2
    );
    port (
        clock      : in std_logic;
        reset      : in std_logic;
        busy       : in std_logic;

        strobe     : out std_logic;
        wr         : out std_logic;
        addr       : out std_logic_vector(ADDR_WIDTH - 1 downto 0);
        nwords     : out std_logic_vector(BUF_WIDTH - 1 downto 0);
        done       : out std_logic;

        buf_clock  : in std_logic;
        buf_wr     : in std_logic;
        buf_addr   : in std_logic_vector(BUF_WIDTH - 1 downto 0);
        buf_data_i : in std_logic_vector(DATA_WIDTH - 1 downto 0);
        buf_data_o : out std_logic_vector(DATA_WIDTH - 1 downto 0)
    );
end sdram_client_fsm;

architecture behavior of sdram_client_fsm is
    type state_t is (
        init0, init1, init2, init3, init4, init5, init6, init7
    );

    signal current_state : state_t;

    signal client_wr     : std_logic;
    signal client_addr   : std_logic_vector(BUF_WIDTH - 1 downto 0);
    signal client_data_i : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal client_data_o : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal counter       : integer;
    signal addr_reg      : std_logic_vector(ADDR_WIDTH - 1 downto 0);
begin
    process (clock, reset)
    begin
        if reset = '1' then
            client_wr     <= '0';
            client_addr   <= (others => '0');
            client_data_i <= (others => '0');
            current_state <= init0;
            strobe <= '0';
            wr     <= '0';
            addr   <= (others => '0');
            nwords <= (others => '0');
            done   <= '0';
            counter <= 0;
            addr_reg <= (others => '0');
        elsif clock'event and clock = '1' then
            case current_state is
                when init0 =>
                    client_wr     <= '0';
                    client_addr   <= (others => '0');
                    client_data_i <= (others => '0');
                    current_state <= init1;

                when init1 =>
                    client_wr     <= '1';
                    client_addr   <= (others => '0');
                    client_data_i <= (others => '0');
                    current_state <= init2;

                when init2 =>
                    client_wr     <= '1';
                    client_addr   <= std_logic_vector(unsigned(client_addr) + 1);
                    client_data_i <= std_logic_vector(to_unsigned(INC, 16));

                    if unsigned(client_addr) >= 700 then
                        current_state <= init3;
                    end if;

                when init3 =>
                    client_wr     <= '0';
                    client_addr   <= (others => '0');
                    client_data_i <= (others => '0');
                    done <= '0';
                    counter <= counter + 1;

                    if busy = '0' and counter > 50 then
                        current_state <= init4;

                        strobe   <= '1';
                        wr       <= '1';
                        addr     <= addr_reg;
                        --addr     <= std_logic_vector(to_unsigned(640 * 320 + 220, ADDR_WIDTH));
                        nwords   <= std_logic_vector(to_unsigned(640, BUF_WIDTH));
                        counter  <= 0;

                        if unsigned(addr_reg) < 640 * 479 then
                            addr_reg <= std_logic_vector(unsigned(addr_reg) + 640);
                        else
                            addr_reg <= (others => '0');
                        end if;
                    end if;

                when init4 =>
                    strobe <= '0';
                    wr     <= '0';

                    if busy = '0' then
                        counter <= counter + 1;

                        if counter > 50 then 
                            done <= '1';
                            current_state <= init2;
                            counter <= 0;
                        end if;
                    end if;

                when others =>
                    client_wr     <= '0';
                    client_addr   <= (others => '0');
                    client_data_i <= (others => '0');
                    current_state <= init0;
                    strobe <= '0';
                    wr     <= '0';
                    addr   <= (others => '0');
                    nwords <= (others => '0');
                    done   <= '0';
            end case;
        end if;    
    end process;

    dual_mem_u : entity work.dual_memory_bram
    generic map (16, 12)
    port map (
        buf_clock, 
        buf_wr, buf_addr, buf_data_i, buf_data_o,
        clock,
        client_wr, client_addr, client_data_i, client_data_o
    );

end behavior;
