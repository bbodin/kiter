----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date: 07/29/2021 05:07:37 PM
-- Design Name:
-- Module Name: axi_fifo_zero - Behavioral
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


entity axi_fifo_zero is
  generic (
    ram_width : natural
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
end axi_fifo_zero;

architecture rtl of axi_fifo_zero is
begin

  buffer_in_ready <= buffer_out_ready and not buffer_rst;
  buffer_out_valid <= buffer_in_valid and not buffer_rst;
  buffer_out_data <= buffer_in_data;

end architecture;
