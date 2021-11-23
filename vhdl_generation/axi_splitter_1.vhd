-- Takes in one input (following AXI conventions), sending to one output
-- (filler component).
library ieee;
use ieee.std_logic_1164.all;

entity axi_splitter_1 is
  generic (bit_width : natural);
  port ( clk        : in std_logic;
         rst      : in std_logic;

         in_ready_0 : out std_logic;
         in_valid_0 : in std_logic;
         in_data_0  : in std_logic_vector (bit_width-1 downto 0);

         out_ready_0 : in std_logic;
         out_valid_0 : out std_logic;
         out_data_0 : out std_logic_vector (bit_width-1 downto 0) );
end axi_splitter_1;

architecture Behavioral of axi_splitter_1 is
  signal temp_data_0   : std_logic_vector (bit_width-1 downto 0);
  signal is_in_ready_0 : std_logic := '1';
  signal is_stored_0   : std_logic := '0';
  signal is_sent       : std_logic := '0';

begin

  store_data : process(clk) -- store data when available in temporary storage
  begin
    if falling_edge(clk) then
      -- clear previous data (considered sent after 1 cycle where out valid and ready = 1)
      if (is_stored_0 = '1' AND out_ready_0 = '1') then
        is_stored_0 <= '0';
        is_in_ready_0 <= '1';
      else
        -- store data if input available and temp storage not used
        if (is_stored_0 = '0' AND
            in_valid_0 = '1' AND is_in_ready_0 = '1') then
          is_in_ready_0 <= '0';
          is_stored_0 <= '1';
          temp_data_0(bit_width-1 downto 0) <= in_data_0;
        end if;
      end if;
      -- for resetting component
      if (rst = '1') then
        is_stored_0 <= '0';
        is_in_ready_0 <= '1';
        is_sent <= '0';
      end if;
    end if;
  end process store_data;

  write_data : process(clk) -- write stored data to output ports
  begin
    if rising_edge(clk) then
      if (is_stored_0 = '1') then -- only write when both data ready to send
        out_data_0 <= temp_data_0(bit_width-1 downto 0);
      end if;
      in_ready_0 <= is_in_ready_0;
      out_valid_0 <= is_stored_0;
    end if;
  end process write_data;

end Behavioral;
