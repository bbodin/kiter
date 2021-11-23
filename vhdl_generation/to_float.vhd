-- temporary implementation (pass-through any incoming data)
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity to_float is
port (
    clk : in std_logic;
    rst : in std_logic;

    in_ready_0 : out std_logic;
    in_valid_0 : in std_logic;
    in_data_0 : in std_logic_vector(33 downto 0) := (others => '0');

    out_ready_0 : in std_logic;
    out_valid_0 : out std_logic;
    out_data_0 : out std_logic_vector(33 downto 0) := (others => '0')
);
end to_float;

architecture Behavioral of to_float is

begin
    in_ready_0 <= '1';
    out_valid_0 <= in_valid_0;
    out_data_0 <= in_data_0;
end Behavioral;
