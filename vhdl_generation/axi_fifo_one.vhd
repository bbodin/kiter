----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date: 07/29/2021 05:08:17 PM
-- Design Name:
-- Module Name: axi_fifo_one - Behavioral
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


entity axi_fifo_one is
  generic (
    ram_width : natural;
    ram_init  : natural := 0
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
end axi_fifo_one;

architecture rtl of axi_fifo_one is

  -- single value ram
  signal present: std_logic := '0';
  signal present_mask : std_logic_vector(buffer_in_data'range) := (others => '0');
  signal ram : std_logic_vector(buffer_in_data'range) := (others => '0');

  -- local values for output
  signal buffer_in_ready_local :  std_logic;
  signal buffer_out_valid_local : std_logic;

begin


  -- copy internal signals to output

  buffer_in_ready <= not buffer_rst and  buffer_in_ready_local;
  buffer_out_valid <= not buffer_rst and buffer_out_valid_local;

  buffer_in_ready_local <= (not present);
  buffer_out_valid_local <= present or buffer_in_valid;
  buffer_out_data <= (ram and present_mask) or (buffer_in_data and not present_mask) ;

  present_mask <= (others => present);


  proc_main : process(buffer_rst, buffer_clk)
  begin
    if buffer_rst = '1' then
      if ram_init = 0 then
        present <= '0';
      else
        present <= '1';
        ram <= (others => '0');
      end if;
    else
      if rising_edge(buffer_clk) then


        if buffer_in_ready_local = '1' and buffer_in_valid = '1' and buffer_out_ready = '0' and present = '0' then
          ram <= buffer_in_data;
          present <= '1';
        elsif buffer_out_ready = '1' and buffer_out_valid_local = '1' and present = '1' then
          present <= '0';
        end if;


      end if;
    end if;
  end process;


end architecture;
