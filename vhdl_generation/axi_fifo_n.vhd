----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date: 07/29/2021 05:08:17 PM
-- Design Name:
-- Module Name: axi_fifo_n - Behavioral
-- Project Name:
-- Target Devices:
-- Tool Versions:
-- Description:
--
-- Dependencies:
--
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;


entity axi_fifo_n is
  generic (
    ram_width : natural;
    ram_init  : natural := 0;
    ram_depth : natural := 1
    );
  port (
    buffer_clk : in std_logic;
    buffer_rst : in std_logic;

    -- axi input interface
    buffer_in_ready : out std_logic;
    buffer_in_valid : in std_logic;
    buffer_in_data : in std_logic_vector(ram_width - 1 downto 0);

    -- axi output interface
    buffer_out_ready : in std_logic;
    buffer_out_valid : out std_logic;
    buffer_out_data : out std_logic_vector(ram_width - 1 downto 0)
    );
end axi_fifo_n;

architecture rtl of axi_fifo_n is

  -- the fifo is full when the ram contains ram_depth elements (extra element for circular buffer)
  type ram_type is array (0 to ram_depth) of std_logic_vector(buffer_in_data'range);
  signal ram : ram_type := (others => (others => '0'));


  -- newest element at head, oldest element at tail
  subtype index_type is natural range ram_type'range;
  signal head : index_type;
  signal tail : index_type;

  -- local values for outputs
  signal in_ready_local :  std_logic;
  signal out_valid_local : std_logic;
  signal out_data_local :   std_logic_vector(ram_width - 1 downto 0);

  -- increment or wrap the index if this transaction is valid
  function next_index(
    index : index_type;
    ready : std_logic;
    valid : std_logic) return index_type is
  begin
    if ready = '1' and valid = '1' then
      if index = index_type'high then
        return index_type'low;
      else
        return index + 1;
      end if;
    end if;

    return index;
  end function;

begin


  -- Local values affect outputs




  proc_tail_head_count_inready_outvalid : process(buffer_rst, buffer_clk)
    variable next_tail : index_type;
    variable next_head : index_type;
    variable next_count : index_type;
  begin


    if buffer_rst = '1' then
      tail <= index_type'low;
      head <= index_type'low + ram_init;
      in_ready_local <= '0';
      out_valid_local <= '0';

      buffer_in_ready <=   '0';
      buffer_out_valid <=   '0';


      buffer_out_data <= (others => '0');
      ram <= (others => (others => '0'));
    else

      if falling_edge(buffer_clk) then

        next_head := next_index(head, in_ready_local, buffer_in_valid);
        next_tail := next_index(tail, buffer_out_ready, out_valid_local);

        if next_head < next_tail then
          next_count := next_head - next_tail + ram_depth + 1;
        else
          next_count := next_head - next_tail;
        end if;


        -- At this point, next_* is validated, this is going to be the future
        -- Given what we know from the future, we can make decision


        if next_count < ram_depth then
          in_ready_local <= '1';
        else
          in_ready_local <= '0';
        end if;

        if next_count > 0 then
          out_valid_local <= '1';
        else
          out_valid_local <= '0';
        end if;


        tail <= next_tail;
        head <= next_head;
        ram(head) <= buffer_in_data;

        -- We will have a delay in the output if we have tail and head at the same location
        if head = next_tail then
          out_data_local <= buffer_in_data;
        else
          out_data_local <= ram(next_tail);
        end if;
      end if;
      if rising_edge(buffer_clk) then

        buffer_in_ready <=  in_ready_local;
        buffer_out_valid <=  out_valid_local;
        buffer_out_data <=  out_data_local;

      end if;
    end if;
  end process;




end architecture;
