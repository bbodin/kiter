library ieee;
use ieee.std_logic_1164.all;

entity const_value_$N_OUTPUTS is
    generic ( value : std_logic_vector (33 downto 0) );
    port ( clk : in std_logic;
           rst : in std_logic;
           $PORT_LIST );
end const_value_$N_OUTPUTS;

architecture Behavioral of const_value_$N_OUTPUTS is
begin

$PORT_CONNECTIONS

end Behavioral;
