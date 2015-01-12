library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_client_fsm2 is
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
end sdram_client_fsm2;

architecture behavior of sdram_client_fsm2 is
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
    signal pixel_tmp     : std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal pixel_counter : integer;
begin
    process (clock, reset)
    begin
        if reset = '1' then
            pixel_tmp <= x"000F";
            pixel_counter <= 0;
        elsif clock'event and clock = '1' then
            --if pixel_counter > 10000000 then
            if pixel_counter > 100000000 then
                pixel_tmp <= pixel_tmp(3 downto 0) & pixel_tmp(DATA_WIDTH - 1 downto 4);
                pixel_counter <= 0;
            else
                pixel_counter <= pixel_counter + 1;
            end if;
        end if;
    end process;

    process (clock, reset)
    begin
        if reset = '1' then
            client_wr     <= '0';
            client_addr   <= (others => '1');
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
                    client_data_i <= pixel_tmp;--std_logic_vector(to_unsigned(INC, 16));
                    current_state <= init2;

                when init2 =>
                    client_wr     <= '1';
                    client_addr   <= std_logic_vector(unsigned(client_addr) + 1);
                    client_data_i <= pixel_tmp;--std_logic_vector(to_unsigned(INC, 16));

                    if unsigned(client_addr) >= 700 then
                        current_state <= init3;
                    end if;

                when init3 =>
                    client_wr     <= '0';
                    client_addr   <= (others => '0');
                    client_data_i <= (others => '0');
                    done <= '0';
                    counter <= counter + 1;

                    if busy = '0' then
                        current_state <= init4;

                        strobe   <= '1';
                        wr       <= '1';
                        addr     <= addr_reg;
                        nwords   <= std_logic_vector(to_unsigned(1, BUF_WIDTH));
                        counter  <= 0;
                    end if;

                when init4 =>
                    strobe <= '0';
                    wr     <= '0';

                    counter <= counter + 1;

                    if busy = '0' then
                        if counter > 50 then 
                            current_state <= init5;
                            counter       <= 0;
                            client_wr     <= '1';
                            client_addr   <= (others => '0');
                            client_data_i <= (others => '1');
                        end if;
                    end if;

                when init5 =>
                    client_wr     <= '0';
                    client_addr   <= (others => '0');
                    client_data_i <= (others => '0');
                    done <= '0';
                    counter <= counter + 1;

                    if busy = '0' and counter > 50 then
                        current_state <= init6;

                        strobe   <= '1';
                        wr       <= '0';
                        addr     <= addr_reg;
                        nwords   <= std_logic_vector(to_unsigned(1, BUF_WIDTH));
                        counter  <= 0;

                    end if;

                when init6 =>
                    strobe <= '0';
                    wr     <= '0';

                    if busy = '0' then
                        counter <= counter + 1;

                        if counter > 50 then 
                            done <= '0';
                            current_state <= init7;
                            counter <= 0;
                        end if;
                    end if;

                when init7 =>
                    client_wr     <= '0';
                    client_addr   <= (others => '0');
                    client_data_i <= (others => '0');

                    --if client_data_o /= std_logic_vector(to_unsigned(INC, DATA_WIDTH)) then
                        done <= '0';
                        --report "diferente!";
                        --report integer'image(to_integer(unsigned(client_data_o)));
                        --report integer'image(to_integer(unsigned(addr_reg)));
                        --assert False
                        --severity error;

                        --current_state <= init3;
                        --if unsigned(addr_reg) < 640 * 479 then
                            --addr_reg <= std_logic_vector(unsigned(addr_reg) + 1);
                        --else
                            --addr_reg <= (others => '0');
                            --report "fim da tela";
                        --end if;

                    --else
                        current_state <= init0;
                        if unsigned(addr_reg) < 640 * 480 then
                            addr_reg <= std_logic_vector(unsigned(addr_reg) + 1);
                        else
                            addr_reg <= (others => '0');
                            report "fim da tela";
                        end if;
                        done <= '1';

                        --report "igual!";
                        --report integer'image(to_integer(unsigned(client_data_o)));
                        --report integer'image(to_integer(unsigned(addr_reg)));
                        --assert False
                        --severity note;

                    --end if;


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
