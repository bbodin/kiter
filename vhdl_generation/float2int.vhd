-- This code comes from https://github.com/stark-dev/dlx-processor
-- The code is under MIT licence (a copy of this licence is available in the LICENCE.md file)
-- Original authors:
--   Claudio Poidomani - Electronics Engineering
--   Mauro Guerrera - Computer Engineering
-- The project is part of Microelectronic Systems course, 
-- held by Professor Mariagrazia Graziano in the first year
--  of Embedded Systems Master Degree at Politecnico di Torino.



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;


-- rounding mode: 00 --> minus infinity
--                01 --> plus infinity
--                10 --> Zero
--                11 --> Nearest (default IEEE)

ENTITY float2int IS -- WOLOLO F2I
  PORT( DATA_IN       : IN std_logic_vector(31 downto 0);
        ROUNDING_MODE : IN std_logic_vector(1 downto 0);
        DATA_OUT      : OUT std_logic_vector(31 downto 0);
        OVERFLOW      : OUT std_logic;
        INVALID       : OUT std_logic
  );
END ENTITY;


ARCHITECTURE Behavioral OF float2int IS

  constant bias : integer := 127;

  signal real_exp : integer;

  signal sign_bit : std_logic;
  signal exponent : std_logic_vector(7 downto 0);
  signal mantissa : std_logic_vector(22 downto 0);
  signal mantissa_ext : std_logic_vector(23 downto 0);

  signal partial_result : std_logic_vector(31 downto 0);
  signal rounded_result : std_logic_vector(31 downto 0);
  signal final_result   : std_logic_vector(31 downto 0);
  -- signal discarded_bits : std_logic_vector(23 downto 0);

  signal valid : std_logic;
  signal ovfl  : std_logic; --overflow due to high exponent
  signal v     : std_logic; --overflow due to rounding

  signal r : std_logic;
  signal s : std_logic;

function or_reduce(datain : std_logic_vector) return std_logic is
  variable res : std_logic := '0';
begin
  for i in datain'range loop
    res := res or datain(i);
  end loop;
  return res;
end or_reduce;

BEGIN

  sign_bit <= DATA_IN(31);
  exponent <= DATA_IN(30 downto 23);
  mantissa <= DATA_IN(22 downto 0);

  mantissa_ext <= '1' & mantissa;

  DATA_OUT <= final_result;

  real_exp <= to_integer(signed('0' & exponent)-bias);

  conv_proc : process(DATA_IN, real_exp, mantissa, mantissa_ext)
    variable shifted_value  : std_logic_vector(54 downto 0);
    --variable shifted_value_step1  : std_logic_vector(31 downto 0);
    --variable shifted_value_step2  : std_logic_vector(54 downto 0);
  begin
  
  
  --shifted_value_step1 := ("0000000000000000000000000000000" & mantissa_ext);
  --shifted_value_step2 := shifted_value_step1 sll real_exp;
  
    -- shifted_value := std_logic_vector(unsigned("0000000000000000000000000000000" & mantissa_ext) sll real_exp);
    
    
    case real_exp is
      when 31 to 127  => partial_result <= "01111111111111111111111111111111";
                         valid <= '1';
                         ovfl <= '1';
                         r <= '0';
                         s <= '0';
      when 128        => partial_result <= "10000000000000000000000000000000";
                         valid <= '0';
                         ovfl <= '0';
                         r <= '0';
                         s <= '0';
      when -127       => partial_result <= "00000000000000000000000000000000";
                         valid <= '1';
                         ovfl <= '0';
                         r <= '0';
                         s <= or_reduce(mantissa);
      when -126 to -2 => partial_result <= "00000000000000000000000000000000";
                         valid <= '1';
                         ovfl <= '0';
                         r <= '0';
                         s <= '1';
      when -1         => partial_result <= "00000000000000000000000000000000";
                         valid <= '1';
                         ovfl <= '0';
                         r <= '1';
                         s <= or_reduce(mantissa);
      when 0 to 30    => shifted_value := std_logic_vector(unsigned("0000000000000000000000000000000" & mantissa_ext) sll real_exp);
                         partial_result <= shifted_value(54 downto 23);
                         valid <= '1';
                         ovfl <= '0';
                         r <= shifted_value(22);
                         s <= or_reduce(shifted_value(21 downto 0));
      when others     => partial_result <= "00000000000000000000000000000000";
                         valid <= '0';
                         ovfl <= '0';
                         r <= '0';
                         s <= '0';
    end case;
  end process;

rounding_proc : process(sign_bit, r, s, partial_result, ROUNDING_MODE, ovfl, valid)
begin
  rounded_result <= partial_result;
  v <= '0';
  if ovfl = '0' and valid = '1' then
    case ROUNDING_MODE is
      when "00"  => if sign_bit = '1' then
                      if r = '1' or s = '1' then
                        rounded_result <= std_logic_vector(unsigned(partial_result)+1);
                      end if;
                    end if;
      when "01"  => if sign_bit = '0' then
                      if r = '1' or s = '1' then
                        rounded_result <= std_logic_vector(unsigned(partial_result)+1);
                      end if;
                    end if;
      when "10"  => rounded_result <= partial_result;
      when "11"  => if (r='1' and partial_result(0)='1') or (r='1' and s = '1') then
                      rounded_result <= std_logic_vector(unsigned(partial_result)+1);
                    end if;
      when others => rounded_result <= partial_result;
    end case;
    if rounded_result(31) = '1' then
      v <= '1';
    end if;
  end if;
end process;

final_res_proc : process(rounded_result, sign_bit, ovfl, valid)
begin
  if ovfl = '1' and sign_bit = '1' then
    final_result <= not(rounded_result);
  elsif valid = '0' then
    final_result <= rounded_result;
  elsif sign_bit = '1' then
    final_result <= std_logic_vector(unsigned(not(rounded_result)) + 1);
  else
    final_result <= rounded_result;
  end if;
end process;

OVERFLOW <= ovfl or v;
INVALID <= not(valid);

END ARCHITECTURE;