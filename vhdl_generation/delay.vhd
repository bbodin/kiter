-- Starts a countdown once new input is received from 'axi_merger' that
-- generates a 'trigger_store' signal when the result from the FloPoCo operator
-- is expected. This is used in conjunction with the 'store_send' component.
library ieee;
use ieee.std_logic_1164.all;

entity delay is
  generic ( operator_lifespan : integer); -- NOTE fifo buffer implementation
                                          -- uses natural type but, according
                                          -- to https://www.ics.uci.edu/~jmoorkan/vhdlref/generics.html,
                                          -- generics tend to work better with
                                          -- integer type
  port ( clk           : in std_logic;
         reset         : in std_logic;
         in_ready      : out std_logic;
         in_valid      : in std_logic;
         can_store     : in std_logic;
         count         : out integer := operator_lifespan; -- just for debugging
         trigger_store : out std_logic := '0' );
end delay;

architecture Behavioral of delay is
  signal is_ready         : std_logic;
  signal is_trigger_store : std_logic;
  signal local_counter    : integer := 0;

begin

  dec_counter: process (clk, reset)  -- check values on falling edge
  begin
    -- Start the counter
    if  falling_edge(clk) then
      if (local_counter = 0) AND (is_trigger_store = '0') AND (is_ready = '1') AND (in_valid = '1') then
          local_counter <= operator_lifespan;
          is_trigger_store <= '0';
          is_ready <= '0';
      -- Dec the counter and set is_trigger_store
      elsif (local_counter > 0) then
          local_counter <= local_counter - 1;
          if local_counter = 1 then
            is_trigger_store <= '1';
          else
            is_trigger_store <= '0';
          end if;
          is_ready <= '0';
      else
          is_trigger_store <= '0';
          if (local_counter = 0) AND (is_trigger_store = '0') AND (can_store = '1') then
            is_ready <= '1';
          else
            is_ready <= '0';
          end if;
      end if;
    end if;

    if reset = '1' then
      is_ready <= '0';
      is_trigger_store <= '0';
      local_counter <= 0;
    end if;

    if rising_edge(clk) then
      in_ready <= is_ready;
      trigger_store <= is_trigger_store;
      count <= local_counter;
    end if;

  end process dec_counter;

end Behavioral;
