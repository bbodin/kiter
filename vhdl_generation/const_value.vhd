library ieee;
use ieee.std_logic_1164.all;

entity const_value is
    generic ( value : std_logic_vector (33 downto 0) );
    port ( clk : in std_logic;
           rst : in std_logic;
           out_ready_0 : in std_logic;
           out_valid_0 : out std_logic;
           out_data_0  : out std_logic_vector (33 downto 0) := (others => '0') );
end const_value;

architecture Behavioral of const_value is
begin

out_valid_0 <= '1';
out_data_0 <= value;

end Behavioral;
