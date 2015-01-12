library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity mt48lc8m16a2_tb is
end mt48lc8m16a2_tb;

architecture behavior of mt48lc8m16a2_tb is
    signal Dq    : STD_LOGIC_VECTOR (15 DOWNTO 0);
    signal Addr  : STD_LOGIC_VECTOR (11 DOWNTO 0);
    signal Ba    : STD_LOGIC_VECTOR (1 downto 0) := "00"; 
    signal Clk   : STD_LOGIC := '0'; 
    signal Cke   : STD_LOGIC := '0'; 
    signal Cs_n  : STD_LOGIC := '1'; 
    signal Ras_n : STD_LOGIC := '0'; 
    signal Cas_n : STD_LOGIC := '0'; 
    signal We_n  : STD_LOGIC := '0'; 
    signal Dqm   : STD_LOGIC_VECTOR (1 DOWNTO 0) := "00";

    constant NO_OP        : std_logic_vector(3 downto 0) := "0111";
    constant PRECHARGE_OP : std_logic_vector(3 downto 0) := "0010";
    constant AUTO_REF_OP  : std_logic_vector(3 downto 0) := "0001";
    constant LOAD_MREG_OP : std_logic_vector(3 downto 0) := "0000";
    constant ACTIVATE_OP  : std_logic_vector(3 downto 0) := "0011";
    constant WRITE_OP     : std_logic_vector(3 downto 0) := "0100";
    constant READ_OP      : std_logic_vector(3 downto 0) := "0101";

    -- in ns
    constant tRC  : integer := 70;
    constant tRCD : integer := 20;
    constant tREF : integer := 15000;
    constant tWR  : integer := 2;

    constant hold_time : time := 2 ns;

begin
    process
    begin
        clk <= '1';
        wait for 5 ns;
        clk <= '0';
        wait for 5 ns;
    end process;

    cke <= '1';

    process
    begin
        addr  <= "111111111111" after hold_time;
        ba    <= "00" after hold_time;
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        dqm   <= "00";
        dq    <= (others => 'Z');

        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait for 150 us;

        for i in 0 to 10 loop
            addr  <= "010000000000" after hold_time;
            ba    <= "00" after hold_time; 
            cs_n  <= PRECHARGE_OP(3) after hold_time;
            ras_n <= PRECHARGE_OP(2) after hold_time;
            cas_n <= PRECHARGE_OP(1) after hold_time;
            we_n  <= PRECHARGE_OP(0) after hold_time;
            wait until clk'event and clk = '1';

            for j in 0 to 5 loop
                addr  <= "000000000000" after hold_time;
                ba    <= "00" after hold_time;
                cs_n  <= NO_OP(3) after hold_time;
                ras_n <= NO_OP(2) after hold_time;
                cas_n <= NO_OP(1) after hold_time;
                we_n  <= NO_OP(0) after hold_time;
                wait until clk'event and clk = '1';
            end loop;
        end loop;

        for i in 0 to 10 loop
            addr  <= "000000000000" after hold_time;
            ba    <= "00" after hold_time;
            cs_n  <= AUTO_REF_OP(3) after hold_time;
            ras_n <= AUTO_REF_OP(2) after hold_time;
            cas_n <= AUTO_REF_OP(1) after hold_time;
            we_n  <= AUTO_REF_OP(0) after hold_time;
            wait until clk'event and clk = '1';

            for j in 0 to 5 loop
                addr  <= "000000000000" after hold_time;
                ba    <= "00" after hold_time;
                cs_n  <= NO_OP(3) after hold_time;
                ras_n <= NO_OP(2) after hold_time;
                cas_n <= NO_OP(1) after hold_time;
                we_n  <= NO_OP(0) after hold_time;
                wait until clk'event and clk = '1';
            end loop;
        end loop;

        -- load mode register
        addr <= "001000110000" after hold_time;
        ba   <= "00" after hold_time;
        cs_n  <= LOAD_MREG_OP(3) after hold_time;
        ras_n <= LOAD_MREG_OP(2) after hold_time;
        cas_n <= LOAD_MREG_OP(1) after hold_time;
        we_n  <= LOAD_MREG_OP(0) after hold_time;
        wait until clk'event and clk = '1';

         -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

        -- activate
        addr <= "000100000000" after hold_time;
        ba   <= "00" after hold_time;
        cs_n  <= ACTIVATE_OP(3) after hold_time;
        ras_n <= ACTIVATE_OP(2) after hold_time;
        cas_n <= ACTIVATE_OP(1) after hold_time;
        we_n  <= ACTIVATE_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

        -- activate to r/w := round_up(tRCD / clock_freq)
        -- tCK = clock_freq
        -- tRCD = 20 ns ==> write on third clock, two nops between
        -- page 12
        -- page 36: some timing parameters
        -- write
        addr <= "010000000000" after hold_time;
        ba   <= "00" after hold_time;
        dqm  <= "00" after 1 ns;
        dq   <= "1111111111111111" after 1 ns;
        cs_n  <= WRITE_OP(3) after hold_time;
        ras_n <= WRITE_OP(2) after hold_time;
        cas_n <= WRITE_OP(1) after hold_time;
        we_n  <= WRITE_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        --addr <= "000000000001" after hold_time;
        --ba   <= "00" after hold_time;
        --dqm  <= "00" after 1 ns;
        --dq   <= "0000000000000010" after 1 ns;
        --cs_n  <= WRITE_OP(3) after hold_time;
        --ras_n <= WRITE_OP(2) after hold_time;
        --cas_n <= WRITE_OP(1) after hold_time;
        --we_n  <= WRITE_OP(0) after hold_time;
        --wait until clk'event and clk = '1';

        --addr <= "010000000010" after hold_time;
        --ba   <= "00" after hold_time;
        --dqm  <= "00" after 1 ns;
        --dq   <= "0000000000000100" after 1 ns;
        --cs_n  <= WRITE_OP(3) after hold_time;
        --ras_n <= WRITE_OP(2) after hold_time;
        --cas_n <= WRITE_OP(1) after hold_time;
        --we_n  <= WRITE_OP(0) after hold_time;
        --wait until clk'event and clk = '1';

        -- tRP = time before issue any command after a precharge
        -- tRP = 20 ns ==> tRP / tCK = 20 / 10 = 2 clocks + 1 (for safety)
        -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        dq   <= (others => 'Z') after hold_time;
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

         -- activate
        addr <= "000100000000" after hold_time;
        ba   <= "00" after hold_time;
        cs_n  <= ACTIVATE_OP(3) after hold_time;
        ras_n <= ACTIVATE_OP(2) after hold_time;
        cas_n <= ACTIVATE_OP(1) after hold_time;
        we_n  <= ACTIVATE_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

        -- read
        addr <= "010000000000" after hold_time;
        ba   <= "00" after hold_time;
        dqm  <= "00";
        cs_n  <= READ_OP(3) after hold_time;
        ras_n <= READ_OP(2) after hold_time;
        cas_n <= READ_OP(1) after hold_time;
        we_n  <= READ_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        --addr <= "000000000001" after hold_time;
        --ba   <= "00" after hold_time;
        --dqm  <= "00";
        --cs_n  <= READ_OP(3) after hold_time;
        --ras_n <= READ_OP(2) after hold_time;
        --cas_n <= READ_OP(1) after hold_time;
        --we_n  <= READ_OP(0) after hold_time;
        --wait until clk'event and clk = '1';

        --addr <= "010000000010" after hold_time;
        --ba   <= "00" after hold_time;
        --dqm  <= "00";
        --cs_n  <= READ_OP(3) after hold_time;
        --ras_n <= READ_OP(2) after hold_time;
        --cas_n <= READ_OP(1) after hold_time;
        --we_n  <= READ_OP(0) after hold_time;
        --wait until clk'event and clk = '1';

        -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        dq   <= (others => 'Z');
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

        -- auto refresh 
        -- page 11: tRC is min time between two refreshes
        -- tRC = 70 ns / tCK = 70 / 10 = 7
        addr  <= "000000000000" after hold_time;
        ba    <= "00" after hold_time;
        cs_n  <= AUTO_REF_OP(3) after hold_time;
        ras_n <= AUTO_REF_OP(2) after hold_time;
        cas_n <= AUTO_REF_OP(1) after hold_time;
        we_n  <= AUTO_REF_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        dq   <= (others => 'Z');
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

        -- auto refresh 
        addr  <= "000000000000" after hold_time;
        ba    <= "00" after hold_time;
        cs_n  <= AUTO_REF_OP(3) after hold_time;
        ras_n <= AUTO_REF_OP(2) after hold_time;
        cas_n <= AUTO_REF_OP(1) after hold_time;
        we_n  <= AUTO_REF_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        dq   <= (others => 'Z');
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

        -- activate
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        cs_n  <= ACTIVATE_OP(3) after hold_time;
        ras_n <= ACTIVATE_OP(2) after hold_time;
        cas_n <= ACTIVATE_OP(1) after hold_time;
        we_n  <= ACTIVATE_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

        -- activate to r/w := round_up(tRCD / clock_freq)
        -- tCK = clock_freq
        -- tRCD = 20 ns ==> write on third clock, two nops between
        -- page 12
        -- page 36: some timing parameters
        -- write
        addr <= "010000000000" after hold_time;
        ba   <= "00" after hold_time;
        dqm  <= "00" after 1 ns;
        dq   <= "0000000000000001" after 1 ns;
        cs_n  <= WRITE_OP(3) after hold_time;
        ras_n <= WRITE_OP(2) after hold_time;
        cas_n <= WRITE_OP(1) after hold_time;
        we_n  <= WRITE_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        addr <= "000000000001" after hold_time;
        ba   <= "00" after hold_time;
        dqm  <= "00" after 1 ns;
        dq   <= "0000000000000010" after 1 ns;
        cs_n  <= WRITE_OP(3) after hold_time;
        ras_n <= WRITE_OP(2) after hold_time;
        cas_n <= WRITE_OP(1) after hold_time;
        we_n  <= WRITE_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        addr <= "010000000010" after hold_time;
        ba   <= "00" after hold_time;
        dqm  <= "00" after 1 ns;
        dq   <= "0000000000000100" after 1 ns;
        cs_n  <= WRITE_OP(3) after hold_time;
        ras_n <= WRITE_OP(2) after hold_time;
        cas_n <= WRITE_OP(1) after hold_time;
        we_n  <= WRITE_OP(0) after hold_time;
        wait until clk'event and clk = '1';

        -- tRP = time before issue any command after a precharge
        -- tRP = 20 ns ==> tRP / tCK = 20 / 10 = 2 clocks + 1 (for safety)
        -- idle
        addr <= "000000000000" after hold_time;
        ba   <= "00" after hold_time;
        dq   <= (others => 'Z') after hold_time;
        cs_n  <= NO_OP(3) after hold_time;
        ras_n <= NO_OP(2) after hold_time;
        cas_n <= NO_OP(1) after hold_time;
        we_n  <= NO_OP(0) after hold_time;
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';
        wait until clk'event and clk = '1';

        wait;
                
    end process;

    dut : entity work.mt48lc8m16a2
    port map (dq, addr, ba, clk, cke, cs_n, ras_n, cas_n, we_n, dqm);
end behavior;
