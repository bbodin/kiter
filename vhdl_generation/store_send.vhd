-- Stores result from FloPoCo operator until output buffer is ready to receive
-- data. Storage is triggered by the 'delay' component. Note that only one
-- result can be stored at a time --- we expect the output buffer to be ready
-- to receive before the FloPoCo operator produces the following result.
library ieee;
use ieee.std_logic_1164.all;

entity store_send is
  generic ( data_bit_width : integer);
  Port ( clk       : in std_logic;
         reset     : in std_logic;
         out_ready : in std_logic;
         in_data   : in std_logic_vector (data_bit_width-1 downto 0);
         out_valid : out std_logic;
         out_data  : out std_logic_vector (data_bit_width-1 downto 0);
         can_store : out std_logic;
         trigger_store : in std_logic );
end store_send;

architecture Behavioral of store_send is
  signal local_storage : std_logic_vector (data_bit_width-1 downto 0);
  signal local_set     : std_logic;
  signal is_out_valid  : std_logic;

begin

  results_storage : process (reset, clk)
  begin
    if falling_edge(clk) then
      -- Can only save
      if (trigger_store = '1' and local_set = '0') then
        is_out_valid <= '1';
        local_storage <= in_data;
        local_set     <= '1';
      elsif (trigger_store = '1' and local_set = '1') then
        -- can't store if local store already set; do nothing
        local_set <= local_set;
        is_out_valid <= is_out_valid;
      -- Output buffer ready with data locally stored
      elsif (trigger_store = '0' and local_set = '1' and out_ready = '1') then
        local_set <= '0';
        is_out_valid <= '0';
      -- I am valid waiting for future ready
      elsif (trigger_store = '0' and local_set = '1' and out_ready = '0') then
        is_out_valid <= '1';
      else
        is_out_valid <= '0';
      end if;
      if reset = '1' then
        local_set <= '0';
        is_out_valid <= '0';
      end if;
    end if;

    if rising_edge(clk) then
      out_valid <= is_out_valid;
      can_store <= NOT(local_set);
      out_data <= local_storage;
    end if;
  end process results_storage;

end Behavioral;
