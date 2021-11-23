----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date: 29.08.2021
-- Design Name:
-- Module Name: axi_int2float - Behavioral
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


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- top-level entity declaration
entity axi_int2float is
  port (
    clk : in std_logic;
    rst : in std_logic;

    op_in_ready_0 : out std_logic;
    op_in_valid_0 : in std_logic;
    op_in_data_0 : in std_logic_vector(33 downto 0) := (others => '0');

    op_out_ready_0 : in std_logic;
    op_out_valid_0 : out std_logic;
    op_out_data_0 : out std_logic_vector(33 downto 0) := (others => '0')
    );
end axi_int2float;

architecture connections of axi_int2float is

  component int2float is
    PORT( DATA_IN       : IN std_logic_vector(31 downto 0);
          ROUNDING_MODE : IN std_logic_vector(1 downto 0);
          DATA_OUT      : OUT std_logic_vector(31 downto 0)
          );
  end component;

-- internal signals
  signal rounding :  std_logic_vector(1 downto 0);
begin
  rounding <= "00";

  op_in_ready_0 <= op_out_ready_0;
  op_out_valid_0 <= op_in_valid_0;
  op_out_data_0 (33 downto 32) <= "01";
  i2f : int2float port map ( DATA_IN => op_in_data_0 (31 downto 0) ,
                             DATA_OUT => op_out_data_0 (31 downto 0),
                             ROUNDING_MODE => rounding );

end connections;
