--------------------------------------------------------------------------------
--                         Compressor_23_3_F400_uid15
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors:
--------------------------------------------------------------------------------
-- combinatorial
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X1 X0
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library std;
use std.textio.all;
library work;

entity Compressor_23_3_F400_uid15 is
    port (X1 : in  std_logic_vector(1 downto 0);
          X0 : in  std_logic_vector(2 downto 0);
          R : out  std_logic_vector(2 downto 0)   );
end entity;

architecture arch of Compressor_23_3_F400_uid15 is
signal X :  std_logic_vector(4 downto 0);
signal R0 :  std_logic_vector(2 downto 0);
begin
   X <= X1 & X0 ;

   with X  select  R0 <=
      "000" when "00000",
      "001" when "00001" | "00010" | "00100",
      "010" when "00011" | "00101" | "00110" | "01000" | "10000",
      "011" when "00111" | "01001" | "01010" | "01100" | "10001" | "10010" | "10100",
      "100" when "01011" | "01101" | "01110" | "10011" | "10101" | "10110" | "11000",
      "101" when "01111" | "10111" | "11001" | "11010" | "11100",
      "110" when "11011" | "11101" | "11110",
      "111" when "11111",
      "---" when others;
   R <= R0;
end architecture;

--------------------------------------------------------------------------------
--                         Compressor_3_2_F400_uid23
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors:
--------------------------------------------------------------------------------
-- combinatorial
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X0
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library std;
use std.textio.all;
library work;

entity Compressor_3_2_F400_uid23 is
    port (X0 : in  std_logic_vector(2 downto 0);
          R : out  std_logic_vector(1 downto 0)   );
end entity;

architecture arch of Compressor_3_2_F400_uid23 is
signal X :  std_logic_vector(2 downto 0);
signal R0 :  std_logic_vector(1 downto 0);
begin
   X <= X0 ;

   with X  select  R0 <=
      "00" when "000",
      "01" when "001" | "010" | "100",
      "10" when "011" | "101" | "110",
      "11" when "111",
      "--" when others;
   R <= R0;
end architecture;

--------------------------------------------------------------------------------
--                          DSPBlock_17x24_F400_uid6
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors:
--------------------------------------------------------------------------------
-- Pipeline depth: 0 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X Y
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library std;
use std.textio.all;
library work;

entity DSPBlock_17x24_F400_uid6 is
    port (clk : in std_logic;
          X : in  std_logic_vector(16 downto 0);
          Y : in  std_logic_vector(23 downto 0);
          R : out  std_logic_vector(40 downto 0)   );
end entity;

architecture arch of DSPBlock_17x24_F400_uid6 is
signal Mint :  std_logic_vector(40 downto 0);
signal M :  std_logic_vector(40 downto 0);
signal Rtmp :  std_logic_vector(40 downto 0);
begin
   Mint <= std_logic_vector(unsigned(X) * unsigned(Y)); -- multiplier
   M <= Mint(40 downto 0);
   Rtmp <= M;
   R <= Rtmp;
end architecture;

--------------------------------------------------------------------------------
--                          DSPBlock_15x24_F400_uid8
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors:
--------------------------------------------------------------------------------
-- Pipeline depth: 0 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X Y
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library std;
use std.textio.all;
library work;

entity DSPBlock_15x24_F400_uid8 is
    port (clk : in std_logic;
          X : in  std_logic_vector(14 downto 0);
          Y : in  std_logic_vector(23 downto 0);
          R : out  std_logic_vector(38 downto 0)   );
end entity;

architecture arch of DSPBlock_15x24_F400_uid8 is
signal Mint :  std_logic_vector(38 downto 0);
signal M :  std_logic_vector(38 downto 0);
signal Rtmp :  std_logic_vector(38 downto 0);
begin
   Mint <= std_logic_vector(unsigned(X) * unsigned(Y)); -- multiplier
   M <= Mint(38 downto 0);
   Rtmp <= M;
   R <= Rtmp;
end architecture;

--------------------------------------------------------------------------------
--                          DSPBlock_17x8_F400_uid10
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors:
--------------------------------------------------------------------------------
-- Pipeline depth: 0 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X Y
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library std;
use std.textio.all;
library work;

entity DSPBlock_17x8_F400_uid10 is
    port (clk : in std_logic;
          X : in  std_logic_vector(16 downto 0);
          Y : in  std_logic_vector(7 downto 0);
          R : out  std_logic_vector(24 downto 0)   );
end entity;

architecture arch of DSPBlock_17x8_F400_uid10 is
signal Mint :  std_logic_vector(24 downto 0);
signal M :  std_logic_vector(24 downto 0);
signal Rtmp :  std_logic_vector(24 downto 0);
begin
   Mint <= std_logic_vector(unsigned(X) * unsigned(Y)); -- multiplier
   M <= Mint(24 downto 0);
   Rtmp <= M;
   R <= Rtmp;
end architecture;

--------------------------------------------------------------------------------
--                          DSPBlock_15x8_F400_uid12
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors:
--------------------------------------------------------------------------------
-- Pipeline depth: 0 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X Y
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library std;
use std.textio.all;
library work;

entity DSPBlock_15x8_F400_uid12 is
    port (clk : in std_logic;
          X : in  std_logic_vector(14 downto 0);
          Y : in  std_logic_vector(7 downto 0);
          R : out  std_logic_vector(22 downto 0)   );
end entity;

architecture arch of DSPBlock_15x8_F400_uid12 is
signal Mint :  std_logic_vector(22 downto 0);
signal M :  std_logic_vector(22 downto 0);
signal Rtmp :  std_logic_vector(22 downto 0);
begin
   Mint <= std_logic_vector(unsigned(X) * unsigned(Y)); -- multiplier
   M <= Mint(22 downto 0);
   Rtmp <= M;
   R <= Rtmp;
end architecture;

--------------------------------------------------------------------------------
--                           IntAdder_46_F400_uid59
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors: Bogdan Pasca, Florent de Dinechin (2008-2016)
--------------------------------------------------------------------------------
-- Pipeline depth: 2 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X Y Cin
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library std;
use std.textio.all;
library work;

entity IntAdder_46_F400_uid59 is
    port (clk : in std_logic;
          X : in  std_logic_vector(45 downto 0);
          Y : in  std_logic_vector(45 downto 0);
          Cin : in  std_logic;
          R : out  std_logic_vector(45 downto 0)   );
end entity;

architecture arch of IntAdder_46_F400_uid59 is
signal Cin_0, Cin_0_d1 :  std_logic;
signal X_0, X_0_d1 :  std_logic_vector(7 downto 0);
signal Y_0, Y_0_d1 :  std_logic_vector(7 downto 0);
signal S_0 :  std_logic_vector(7 downto 0);
signal R_0, R_0_d1 :  std_logic_vector(6 downto 0);
signal Cin_1, Cin_1_d1 :  std_logic;
signal X_1, X_1_d1, X_1_d2 :  std_logic_vector(31 downto 0);
signal Y_1, Y_1_d1, Y_1_d2 :  std_logic_vector(31 downto 0);
signal S_1 :  std_logic_vector(31 downto 0);
signal R_1 :  std_logic_vector(30 downto 0);
signal Cin_2 :  std_logic;
signal X_2, X_2_d1, X_2_d2 :  std_logic_vector(8 downto 0);
signal Y_2, Y_2_d1, Y_2_d2 :  std_logic_vector(8 downto 0);
signal S_2 :  std_logic_vector(8 downto 0);
signal R_2 :  std_logic_vector(7 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            Cin_0_d1 <=  Cin_0;
            X_0_d1 <=  X_0;
            Y_0_d1 <=  Y_0;
            R_0_d1 <=  R_0;
            Cin_1_d1 <=  Cin_1;
            X_1_d1 <=  X_1;
            X_1_d2 <=  X_1_d1;
            Y_1_d1 <=  Y_1;
            Y_1_d2 <=  Y_1_d1;
            X_2_d1 <=  X_2;
            X_2_d2 <=  X_2_d1;
            Y_2_d1 <=  Y_2;
            Y_2_d2 <=  Y_2_d1;
         end if;
      end process;
   Cin_0 <= Cin;
   X_0 <= '0' & X(6 downto 0);
   Y_0 <= '0' & Y(6 downto 0);
   S_0 <= X_0_d1 + Y_0_d1 + Cin_0_d1;
   R_0 <= S_0(6 downto 0);
   Cin_1 <= S_0(7);
   X_1 <= '0' & X(37 downto 7);
   Y_1 <= '0' & Y(37 downto 7);
   S_1 <= X_1_d2 + Y_1_d2 + Cin_1_d1;
   R_1 <= S_1(30 downto 0);
   Cin_2 <= S_1(31);
   X_2 <= '0' & X(45 downto 38);
   Y_2 <= '0' & Y(45 downto 38);
   S_2 <= X_2_d2 + Y_2_d2 + Cin_2;
   R_2 <= S_2(7 downto 0);
   R <= R_2 & R_1 & R_0_d1 ;
end architecture;

--------------------------------------------------------------------------------
--                          IntMultiplier_F400_uid2
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors: Martin Kumm, Florent de Dinechin, Kinga Illyes, Bogdan Popa, Bogdan Pasca, 2012
--------------------------------------------------------------------------------
-- Pipeline depth: 2 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X Y
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library std;
use std.textio.all;
library work;

entity IntMultiplier_F400_uid2 is
    port (clk : in std_logic;
          X : in  std_logic_vector(31 downto 0);
          Y : in  std_logic_vector(31 downto 0);
          R : out  std_logic_vector(63 downto 0) );
end entity;

architecture arch of IntMultiplier_F400_uid2 is
   component DSPBlock_17x24_F400_uid6 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(16 downto 0);
             Y : in  std_logic_vector(23 downto 0);
             R : out  std_logic_vector(40 downto 0)   );
   end component;

   component DSPBlock_15x24_F400_uid8 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(14 downto 0);
             Y : in  std_logic_vector(23 downto 0);
             R : out  std_logic_vector(38 downto 0)   );
   end component;

   component DSPBlock_17x8_F400_uid10 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(16 downto 0);
             Y : in  std_logic_vector(7 downto 0);
             R : out  std_logic_vector(24 downto 0)   );
   end component;

   component DSPBlock_15x8_F400_uid12 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(14 downto 0);
             Y : in  std_logic_vector(7 downto 0);
             R : out  std_logic_vector(22 downto 0)   );
   end component;

   component Compressor_23_3_F400_uid15 is
      port ( X1 : in  std_logic_vector(1 downto 0);
             X0 : in  std_logic_vector(2 downto 0);
             R : out  std_logic_vector(2 downto 0)   );
   end component;

   component Compressor_3_2_F400_uid23 is
      port ( X0 : in  std_logic_vector(2 downto 0);
             R : out  std_logic_vector(1 downto 0)   );
   end component;

   component IntAdder_46_F400_uid59 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(45 downto 0);
             Y : in  std_logic_vector(45 downto 0);
             Cin : in  std_logic;
             R : out  std_logic_vector(45 downto 0)   );
   end component;

signal XX_m3 :  std_logic_vector(31 downto 0);
signal YY_m3 :  std_logic_vector(31 downto 0);
signal tile_0_X :  std_logic_vector(16 downto 0);
signal tile_0_Y :  std_logic_vector(23 downto 0);
signal tile_0_output :  std_logic_vector(40 downto 0);
signal tile_0_filtered_output :  std_logic_vector(40 downto 0);
signal bh4_w0_0 :  std_logic;
signal bh4_w1_0 :  std_logic;
signal bh4_w2_0 :  std_logic;
signal bh4_w3_0 :  std_logic;
signal bh4_w4_0 :  std_logic;
signal bh4_w5_0 :  std_logic;
signal bh4_w6_0 :  std_logic;
signal bh4_w7_0 :  std_logic;
signal bh4_w8_0 :  std_logic;
signal bh4_w9_0 :  std_logic;
signal bh4_w10_0 :  std_logic;
signal bh4_w11_0 :  std_logic;
signal bh4_w12_0 :  std_logic;
signal bh4_w13_0 :  std_logic;
signal bh4_w14_0 :  std_logic;
signal bh4_w15_0 :  std_logic;
signal bh4_w16_0 :  std_logic;
signal bh4_w17_0 :  std_logic;
signal bh4_w18_0 :  std_logic;
signal bh4_w19_0 :  std_logic;
signal bh4_w20_0 :  std_logic;
signal bh4_w21_0 :  std_logic;
signal bh4_w22_0 :  std_logic;
signal bh4_w23_0 :  std_logic;
signal bh4_w24_0 :  std_logic;
signal bh4_w25_0 :  std_logic;
signal bh4_w26_0 :  std_logic;
signal bh4_w27_0 :  std_logic;
signal bh4_w28_0 :  std_logic;
signal bh4_w29_0 :  std_logic;
signal bh4_w30_0 :  std_logic;
signal bh4_w31_0 :  std_logic;
signal bh4_w32_0 :  std_logic;
signal bh4_w33_0 :  std_logic;
signal bh4_w34_0 :  std_logic;
signal bh4_w35_0 :  std_logic;
signal bh4_w36_0 :  std_logic;
signal bh4_w37_0 :  std_logic;
signal bh4_w38_0 :  std_logic;
signal bh4_w39_0 :  std_logic;
signal bh4_w40_0 :  std_logic;
signal tile_1_X :  std_logic_vector(14 downto 0);
signal tile_1_Y :  std_logic_vector(23 downto 0);
signal tile_1_output :  std_logic_vector(38 downto 0);
signal tile_1_filtered_output :  std_logic_vector(38 downto 0);
signal bh4_w17_1 :  std_logic;
signal bh4_w18_1 :  std_logic;
signal bh4_w19_1 :  std_logic;
signal bh4_w20_1 :  std_logic;
signal bh4_w21_1 :  std_logic;
signal bh4_w22_1 :  std_logic;
signal bh4_w23_1 :  std_logic;
signal bh4_w24_1 :  std_logic;
signal bh4_w25_1 :  std_logic;
signal bh4_w26_1 :  std_logic;
signal bh4_w27_1 :  std_logic;
signal bh4_w28_1 :  std_logic;
signal bh4_w29_1 :  std_logic;
signal bh4_w30_1 :  std_logic;
signal bh4_w31_1 :  std_logic;
signal bh4_w32_1 :  std_logic;
signal bh4_w33_1 :  std_logic;
signal bh4_w34_1 :  std_logic;
signal bh4_w35_1 :  std_logic;
signal bh4_w36_1 :  std_logic;
signal bh4_w37_1 :  std_logic;
signal bh4_w38_1 :  std_logic;
signal bh4_w39_1 :  std_logic;
signal bh4_w40_1 :  std_logic;
signal bh4_w41_0 :  std_logic;
signal bh4_w42_0 :  std_logic;
signal bh4_w43_0 :  std_logic;
signal bh4_w44_0 :  std_logic;
signal bh4_w45_0 :  std_logic;
signal bh4_w46_0 :  std_logic;
signal bh4_w47_0 :  std_logic;
signal bh4_w48_0 :  std_logic;
signal bh4_w49_0 :  std_logic;
signal bh4_w50_0 :  std_logic;
signal bh4_w51_0 :  std_logic;
signal bh4_w52_0 :  std_logic;
signal bh4_w53_0 :  std_logic;
signal bh4_w54_0 :  std_logic;
signal bh4_w55_0 :  std_logic;
signal tile_2_X :  std_logic_vector(16 downto 0);
signal tile_2_Y :  std_logic_vector(7 downto 0);
signal tile_2_output :  std_logic_vector(24 downto 0);
signal tile_2_filtered_output :  std_logic_vector(24 downto 0);
signal bh4_w24_2 :  std_logic;
signal bh4_w25_2 :  std_logic;
signal bh4_w26_2 :  std_logic;
signal bh4_w27_2 :  std_logic;
signal bh4_w28_2 :  std_logic;
signal bh4_w29_2 :  std_logic;
signal bh4_w30_2 :  std_logic;
signal bh4_w31_2 :  std_logic;
signal bh4_w32_2 :  std_logic;
signal bh4_w33_2 :  std_logic;
signal bh4_w34_2 :  std_logic;
signal bh4_w35_2 :  std_logic;
signal bh4_w36_2 :  std_logic;
signal bh4_w37_2 :  std_logic;
signal bh4_w38_2 :  std_logic;
signal bh4_w39_2 :  std_logic;
signal bh4_w40_2 :  std_logic;
signal bh4_w41_1 :  std_logic;
signal bh4_w42_1 :  std_logic;
signal bh4_w43_1 :  std_logic;
signal bh4_w44_1 :  std_logic;
signal bh4_w45_1 :  std_logic;
signal bh4_w46_1 :  std_logic;
signal bh4_w47_1 :  std_logic;
signal bh4_w48_1 :  std_logic;
signal tile_3_X :  std_logic_vector(14 downto 0);
signal tile_3_Y :  std_logic_vector(7 downto 0);
signal tile_3_output :  std_logic_vector(22 downto 0);
signal tile_3_filtered_output :  std_logic_vector(22 downto 0);
signal bh4_w41_2 :  std_logic;
signal bh4_w42_2 :  std_logic;
signal bh4_w43_2 :  std_logic;
signal bh4_w44_2 :  std_logic;
signal bh4_w45_2 :  std_logic;
signal bh4_w46_2 :  std_logic;
signal bh4_w47_2 :  std_logic;
signal bh4_w48_2 :  std_logic;
signal bh4_w49_1 :  std_logic;
signal bh4_w50_1 :  std_logic;
signal bh4_w51_1 :  std_logic;
signal bh4_w52_1 :  std_logic;
signal bh4_w53_1 :  std_logic;
signal bh4_w54_1 :  std_logic;
signal bh4_w55_1 :  std_logic;
signal bh4_w56_0 :  std_logic;
signal bh4_w57_0 :  std_logic;
signal bh4_w58_0 :  std_logic;
signal bh4_w59_0 :  std_logic;
signal bh4_w60_0 :  std_logic;
signal bh4_w61_0 :  std_logic;
signal bh4_w62_0 :  std_logic;
signal bh4_w63_0 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid16_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid16_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid16_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid16_Out0_copy17 :  std_logic_vector(2 downto 0);
signal bh4_w17_2 :  std_logic;
signal bh4_w18_2 :  std_logic;
signal bh4_w19_2 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid18_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid18_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid18_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid18_Out0_copy19 :  std_logic_vector(2 downto 0);
signal bh4_w19_3 :  std_logic;
signal bh4_w20_2 :  std_logic;
signal bh4_w21_2 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid20_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid20_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid20_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid20_Out0_copy21 :  std_logic_vector(2 downto 0);
signal bh4_w21_3 :  std_logic;
signal bh4_w22_2 :  std_logic;
signal bh4_w23_2 :  std_logic;
signal Compressor_3_2_F400_uid23_bh4_uid24_In0 :  std_logic_vector(2 downto 0);
signal Compressor_3_2_F400_uid23_bh4_uid24_Out0 :  std_logic_vector(1 downto 0);
signal Compressor_3_2_F400_uid23_bh4_uid24_Out0_copy25 :  std_logic_vector(1 downto 0);
signal bh4_w23_3 :  std_logic;
signal bh4_w24_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid26_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid26_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid26_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid26_Out0_copy27 :  std_logic_vector(2 downto 0);
signal bh4_w24_4 :  std_logic;
signal bh4_w25_3 :  std_logic;
signal bh4_w26_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid28_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid28_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid28_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid28_Out0_copy29 :  std_logic_vector(2 downto 0);
signal bh4_w26_4 :  std_logic;
signal bh4_w27_3 :  std_logic;
signal bh4_w28_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid30_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid30_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid30_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid30_Out0_copy31 :  std_logic_vector(2 downto 0);
signal bh4_w28_4 :  std_logic;
signal bh4_w29_3 :  std_logic;
signal bh4_w30_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid32_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid32_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid32_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid32_Out0_copy33 :  std_logic_vector(2 downto 0);
signal bh4_w30_4 :  std_logic;
signal bh4_w31_3 :  std_logic;
signal bh4_w32_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid34_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid34_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid34_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid34_Out0_copy35 :  std_logic_vector(2 downto 0);
signal bh4_w32_4 :  std_logic;
signal bh4_w33_3 :  std_logic;
signal bh4_w34_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid36_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid36_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid36_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid36_Out0_copy37 :  std_logic_vector(2 downto 0);
signal bh4_w34_4 :  std_logic;
signal bh4_w35_3 :  std_logic;
signal bh4_w36_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid38_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid38_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid38_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid38_Out0_copy39 :  std_logic_vector(2 downto 0);
signal bh4_w36_4 :  std_logic;
signal bh4_w37_3 :  std_logic;
signal bh4_w38_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid40_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid40_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid40_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid40_Out0_copy41 :  std_logic_vector(2 downto 0);
signal bh4_w38_4 :  std_logic;
signal bh4_w39_3 :  std_logic;
signal bh4_w40_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid42_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid42_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid42_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid42_Out0_copy43 :  std_logic_vector(2 downto 0);
signal bh4_w40_4 :  std_logic;
signal bh4_w41_3 :  std_logic;
signal bh4_w42_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid44_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid44_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid44_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid44_Out0_copy45 :  std_logic_vector(2 downto 0);
signal bh4_w42_4 :  std_logic;
signal bh4_w43_3 :  std_logic;
signal bh4_w44_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid46_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid46_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid46_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid46_Out0_copy47 :  std_logic_vector(2 downto 0);
signal bh4_w44_4 :  std_logic;
signal bh4_w45_3 :  std_logic;
signal bh4_w46_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid48_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid48_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid48_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid48_Out0_copy49 :  std_logic_vector(2 downto 0);
signal bh4_w46_4 :  std_logic;
signal bh4_w47_3 :  std_logic;
signal bh4_w48_3 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid50_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid50_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid50_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid50_Out0_copy51 :  std_logic_vector(2 downto 0);
signal bh4_w48_4 :  std_logic;
signal bh4_w49_2 :  std_logic;
signal bh4_w50_2 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid52_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid52_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid52_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid52_Out0_copy53 :  std_logic_vector(2 downto 0);
signal bh4_w50_3 :  std_logic;
signal bh4_w51_2 :  std_logic;
signal bh4_w52_2 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid54_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid54_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid54_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid54_Out0_copy55 :  std_logic_vector(2 downto 0);
signal bh4_w52_3 :  std_logic;
signal bh4_w53_2 :  std_logic;
signal bh4_w54_2 :  std_logic;
signal Compressor_23_3_F400_uid15_bh4_uid56_In0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid56_In1 :  std_logic_vector(1 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid56_Out0 :  std_logic_vector(2 downto 0);
signal Compressor_23_3_F400_uid15_bh4_uid56_Out0_copy57 :  std_logic_vector(2 downto 0);
signal bh4_w54_3 :  std_logic;
signal bh4_w55_2 :  std_logic;
signal bh4_w56_1 :  std_logic;
signal tmp_bitheapResult_bh4_18, tmp_bitheapResult_bh4_18_d1, tmp_bitheapResult_bh4_18_d2 :  std_logic_vector(18 downto 0);
signal bitheapFinalAdd_bh4_In0 :  std_logic_vector(45 downto 0);
signal bitheapFinalAdd_bh4_In1 :  std_logic_vector(45 downto 0);
signal bitheapFinalAdd_bh4_Cin :  std_logic;
signal bitheapFinalAdd_bh4_Out :  std_logic_vector(45 downto 0);
signal bitheapResult_bh4 :  std_logic_vector(63 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            tmp_bitheapResult_bh4_18_d1 <=  tmp_bitheapResult_bh4_18;
            tmp_bitheapResult_bh4_18_d2 <=  tmp_bitheapResult_bh4_18_d1;
         end if;
      end process;
   XX_m3 <= X ;
   YY_m3 <= Y ;
   tile_0_X <= X(16 downto 0);
   tile_0_Y <= Y(23 downto 0);
   tile_0_mult: DSPBlock_17x24_F400_uid6
      port map ( clk  => clk,
                 X => tile_0_X,
                 Y => tile_0_Y,
                 R => tile_0_output);

tile_0_filtered_output <= tile_0_output(40 downto 0);
   bh4_w0_0 <= tile_0_filtered_output(0);
   bh4_w1_0 <= tile_0_filtered_output(1);
   bh4_w2_0 <= tile_0_filtered_output(2);
   bh4_w3_0 <= tile_0_filtered_output(3);
   bh4_w4_0 <= tile_0_filtered_output(4);
   bh4_w5_0 <= tile_0_filtered_output(5);
   bh4_w6_0 <= tile_0_filtered_output(6);
   bh4_w7_0 <= tile_0_filtered_output(7);
   bh4_w8_0 <= tile_0_filtered_output(8);
   bh4_w9_0 <= tile_0_filtered_output(9);
   bh4_w10_0 <= tile_0_filtered_output(10);
   bh4_w11_0 <= tile_0_filtered_output(11);
   bh4_w12_0 <= tile_0_filtered_output(12);
   bh4_w13_0 <= tile_0_filtered_output(13);
   bh4_w14_0 <= tile_0_filtered_output(14);
   bh4_w15_0 <= tile_0_filtered_output(15);
   bh4_w16_0 <= tile_0_filtered_output(16);
   bh4_w17_0 <= tile_0_filtered_output(17);
   bh4_w18_0 <= tile_0_filtered_output(18);
   bh4_w19_0 <= tile_0_filtered_output(19);
   bh4_w20_0 <= tile_0_filtered_output(20);
   bh4_w21_0 <= tile_0_filtered_output(21);
   bh4_w22_0 <= tile_0_filtered_output(22);
   bh4_w23_0 <= tile_0_filtered_output(23);
   bh4_w24_0 <= tile_0_filtered_output(24);
   bh4_w25_0 <= tile_0_filtered_output(25);
   bh4_w26_0 <= tile_0_filtered_output(26);
   bh4_w27_0 <= tile_0_filtered_output(27);
   bh4_w28_0 <= tile_0_filtered_output(28);
   bh4_w29_0 <= tile_0_filtered_output(29);
   bh4_w30_0 <= tile_0_filtered_output(30);
   bh4_w31_0 <= tile_0_filtered_output(31);
   bh4_w32_0 <= tile_0_filtered_output(32);
   bh4_w33_0 <= tile_0_filtered_output(33);
   bh4_w34_0 <= tile_0_filtered_output(34);
   bh4_w35_0 <= tile_0_filtered_output(35);
   bh4_w36_0 <= tile_0_filtered_output(36);
   bh4_w37_0 <= tile_0_filtered_output(37);
   bh4_w38_0 <= tile_0_filtered_output(38);
   bh4_w39_0 <= tile_0_filtered_output(39);
   bh4_w40_0 <= tile_0_filtered_output(40);
   tile_1_X <= X(31 downto 17);
   tile_1_Y <= Y(23 downto 0);
   tile_1_mult: DSPBlock_15x24_F400_uid8
      port map ( clk  => clk,
                 X => tile_1_X,
                 Y => tile_1_Y,
                 R => tile_1_output);

tile_1_filtered_output <= tile_1_output(38 downto 0);
   bh4_w17_1 <= tile_1_filtered_output(0);
   bh4_w18_1 <= tile_1_filtered_output(1);
   bh4_w19_1 <= tile_1_filtered_output(2);
   bh4_w20_1 <= tile_1_filtered_output(3);
   bh4_w21_1 <= tile_1_filtered_output(4);
   bh4_w22_1 <= tile_1_filtered_output(5);
   bh4_w23_1 <= tile_1_filtered_output(6);
   bh4_w24_1 <= tile_1_filtered_output(7);
   bh4_w25_1 <= tile_1_filtered_output(8);
   bh4_w26_1 <= tile_1_filtered_output(9);
   bh4_w27_1 <= tile_1_filtered_output(10);
   bh4_w28_1 <= tile_1_filtered_output(11);
   bh4_w29_1 <= tile_1_filtered_output(12);
   bh4_w30_1 <= tile_1_filtered_output(13);
   bh4_w31_1 <= tile_1_filtered_output(14);
   bh4_w32_1 <= tile_1_filtered_output(15);
   bh4_w33_1 <= tile_1_filtered_output(16);
   bh4_w34_1 <= tile_1_filtered_output(17);
   bh4_w35_1 <= tile_1_filtered_output(18);
   bh4_w36_1 <= tile_1_filtered_output(19);
   bh4_w37_1 <= tile_1_filtered_output(20);
   bh4_w38_1 <= tile_1_filtered_output(21);
   bh4_w39_1 <= tile_1_filtered_output(22);
   bh4_w40_1 <= tile_1_filtered_output(23);
   bh4_w41_0 <= tile_1_filtered_output(24);
   bh4_w42_0 <= tile_1_filtered_output(25);
   bh4_w43_0 <= tile_1_filtered_output(26);
   bh4_w44_0 <= tile_1_filtered_output(27);
   bh4_w45_0 <= tile_1_filtered_output(28);
   bh4_w46_0 <= tile_1_filtered_output(29);
   bh4_w47_0 <= tile_1_filtered_output(30);
   bh4_w48_0 <= tile_1_filtered_output(31);
   bh4_w49_0 <= tile_1_filtered_output(32);
   bh4_w50_0 <= tile_1_filtered_output(33);
   bh4_w51_0 <= tile_1_filtered_output(34);
   bh4_w52_0 <= tile_1_filtered_output(35);
   bh4_w53_0 <= tile_1_filtered_output(36);
   bh4_w54_0 <= tile_1_filtered_output(37);
   bh4_w55_0 <= tile_1_filtered_output(38);
   tile_2_X <= X(16 downto 0);
   tile_2_Y <= Y(31 downto 24);
   tile_2_mult: DSPBlock_17x8_F400_uid10
      port map ( clk  => clk,
                 X => tile_2_X,
                 Y => tile_2_Y,
                 R => tile_2_output);

tile_2_filtered_output <= tile_2_output(24 downto 0);
   bh4_w24_2 <= tile_2_filtered_output(0);
   bh4_w25_2 <= tile_2_filtered_output(1);
   bh4_w26_2 <= tile_2_filtered_output(2);
   bh4_w27_2 <= tile_2_filtered_output(3);
   bh4_w28_2 <= tile_2_filtered_output(4);
   bh4_w29_2 <= tile_2_filtered_output(5);
   bh4_w30_2 <= tile_2_filtered_output(6);
   bh4_w31_2 <= tile_2_filtered_output(7);
   bh4_w32_2 <= tile_2_filtered_output(8);
   bh4_w33_2 <= tile_2_filtered_output(9);
   bh4_w34_2 <= tile_2_filtered_output(10);
   bh4_w35_2 <= tile_2_filtered_output(11);
   bh4_w36_2 <= tile_2_filtered_output(12);
   bh4_w37_2 <= tile_2_filtered_output(13);
   bh4_w38_2 <= tile_2_filtered_output(14);
   bh4_w39_2 <= tile_2_filtered_output(15);
   bh4_w40_2 <= tile_2_filtered_output(16);
   bh4_w41_1 <= tile_2_filtered_output(17);
   bh4_w42_1 <= tile_2_filtered_output(18);
   bh4_w43_1 <= tile_2_filtered_output(19);
   bh4_w44_1 <= tile_2_filtered_output(20);
   bh4_w45_1 <= tile_2_filtered_output(21);
   bh4_w46_1 <= tile_2_filtered_output(22);
   bh4_w47_1 <= tile_2_filtered_output(23);
   bh4_w48_1 <= tile_2_filtered_output(24);
   tile_3_X <= X(31 downto 17);
   tile_3_Y <= Y(31 downto 24);
   tile_3_mult: DSPBlock_15x8_F400_uid12
      port map ( clk  => clk,
                 X => tile_3_X,
                 Y => tile_3_Y,
                 R => tile_3_output);

tile_3_filtered_output <= tile_3_output(22 downto 0);
   bh4_w41_2 <= tile_3_filtered_output(0);
   bh4_w42_2 <= tile_3_filtered_output(1);
   bh4_w43_2 <= tile_3_filtered_output(2);
   bh4_w44_2 <= tile_3_filtered_output(3);
   bh4_w45_2 <= tile_3_filtered_output(4);
   bh4_w46_2 <= tile_3_filtered_output(5);
   bh4_w47_2 <= tile_3_filtered_output(6);
   bh4_w48_2 <= tile_3_filtered_output(7);
   bh4_w49_1 <= tile_3_filtered_output(8);
   bh4_w50_1 <= tile_3_filtered_output(9);
   bh4_w51_1 <= tile_3_filtered_output(10);
   bh4_w52_1 <= tile_3_filtered_output(11);
   bh4_w53_1 <= tile_3_filtered_output(12);
   bh4_w54_1 <= tile_3_filtered_output(13);
   bh4_w55_1 <= tile_3_filtered_output(14);
   bh4_w56_0 <= tile_3_filtered_output(15);
   bh4_w57_0 <= tile_3_filtered_output(16);
   bh4_w58_0 <= tile_3_filtered_output(17);
   bh4_w59_0 <= tile_3_filtered_output(18);
   bh4_w60_0 <= tile_3_filtered_output(19);
   bh4_w61_0 <= tile_3_filtered_output(20);
   bh4_w62_0 <= tile_3_filtered_output(21);
   bh4_w63_0 <= tile_3_filtered_output(22);

   -- Adding the constant bits
      -- All the constant bits are zero, nothing to add


   Compressor_23_3_F400_uid15_bh4_uid16_In0 <= "" & bh4_w17_0 & bh4_w17_1 & "0";
   Compressor_23_3_F400_uid15_bh4_uid16_In1 <= "" & bh4_w18_0 & bh4_w18_1;
   Compressor_23_3_F400_uid15_uid16: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid16_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid16_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid16_Out0_copy17);
   Compressor_23_3_F400_uid15_bh4_uid16_Out0 <= Compressor_23_3_F400_uid15_bh4_uid16_Out0_copy17; -- output copy to hold a pipeline register if needed

   bh4_w17_2 <= Compressor_23_3_F400_uid15_bh4_uid16_Out0(0);
   bh4_w18_2 <= Compressor_23_3_F400_uid15_bh4_uid16_Out0(1);
   bh4_w19_2 <= Compressor_23_3_F400_uid15_bh4_uid16_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid18_In0 <= "" & bh4_w19_0 & bh4_w19_1 & "0";
   Compressor_23_3_F400_uid15_bh4_uid18_In1 <= "" & bh4_w20_0 & bh4_w20_1;
   Compressor_23_3_F400_uid15_uid18: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid18_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid18_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid18_Out0_copy19);
   Compressor_23_3_F400_uid15_bh4_uid18_Out0 <= Compressor_23_3_F400_uid15_bh4_uid18_Out0_copy19; -- output copy to hold a pipeline register if needed

   bh4_w19_3 <= Compressor_23_3_F400_uid15_bh4_uid18_Out0(0);
   bh4_w20_2 <= Compressor_23_3_F400_uid15_bh4_uid18_Out0(1);
   bh4_w21_2 <= Compressor_23_3_F400_uid15_bh4_uid18_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid20_In0 <= "" & bh4_w21_0 & bh4_w21_1 & "0";
   Compressor_23_3_F400_uid15_bh4_uid20_In1 <= "" & bh4_w22_0 & bh4_w22_1;
   Compressor_23_3_F400_uid15_uid20: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid20_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid20_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid20_Out0_copy21);
   Compressor_23_3_F400_uid15_bh4_uid20_Out0 <= Compressor_23_3_F400_uid15_bh4_uid20_Out0_copy21; -- output copy to hold a pipeline register if needed

   bh4_w21_3 <= Compressor_23_3_F400_uid15_bh4_uid20_Out0(0);
   bh4_w22_2 <= Compressor_23_3_F400_uid15_bh4_uid20_Out0(1);
   bh4_w23_2 <= Compressor_23_3_F400_uid15_bh4_uid20_Out0(2);

   Compressor_3_2_F400_uid23_bh4_uid24_In0 <= "" & bh4_w23_0 & bh4_w23_1 & "0";
   Compressor_3_2_F400_uid23_uid24: Compressor_3_2_F400_uid23
      port map ( X0 => Compressor_3_2_F400_uid23_bh4_uid24_In0,
                 R => Compressor_3_2_F400_uid23_bh4_uid24_Out0_copy25);
   Compressor_3_2_F400_uid23_bh4_uid24_Out0 <= Compressor_3_2_F400_uid23_bh4_uid24_Out0_copy25; -- output copy to hold a pipeline register if needed

   bh4_w23_3 <= Compressor_3_2_F400_uid23_bh4_uid24_Out0(0);
   bh4_w24_3 <= Compressor_3_2_F400_uid23_bh4_uid24_Out0(1);

   Compressor_23_3_F400_uid15_bh4_uid26_In0 <= "" & bh4_w24_0 & bh4_w24_1 & bh4_w24_2;
   Compressor_23_3_F400_uid15_bh4_uid26_In1 <= "" & bh4_w25_0 & bh4_w25_1;
   Compressor_23_3_F400_uid15_uid26: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid26_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid26_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid26_Out0_copy27);
   Compressor_23_3_F400_uid15_bh4_uid26_Out0 <= Compressor_23_3_F400_uid15_bh4_uid26_Out0_copy27; -- output copy to hold a pipeline register if needed

   bh4_w24_4 <= Compressor_23_3_F400_uid15_bh4_uid26_Out0(0);
   bh4_w25_3 <= Compressor_23_3_F400_uid15_bh4_uid26_Out0(1);
   bh4_w26_3 <= Compressor_23_3_F400_uid15_bh4_uid26_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid28_In0 <= "" & bh4_w26_0 & bh4_w26_1 & bh4_w26_2;
   Compressor_23_3_F400_uid15_bh4_uid28_In1 <= "" & bh4_w27_0 & bh4_w27_1;
   Compressor_23_3_F400_uid15_uid28: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid28_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid28_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid28_Out0_copy29);
   Compressor_23_3_F400_uid15_bh4_uid28_Out0 <= Compressor_23_3_F400_uid15_bh4_uid28_Out0_copy29; -- output copy to hold a pipeline register if needed

   bh4_w26_4 <= Compressor_23_3_F400_uid15_bh4_uid28_Out0(0);
   bh4_w27_3 <= Compressor_23_3_F400_uid15_bh4_uid28_Out0(1);
   bh4_w28_3 <= Compressor_23_3_F400_uid15_bh4_uid28_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid30_In0 <= "" & bh4_w28_0 & bh4_w28_1 & bh4_w28_2;
   Compressor_23_3_F400_uid15_bh4_uid30_In1 <= "" & bh4_w29_0 & bh4_w29_1;
   Compressor_23_3_F400_uid15_uid30: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid30_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid30_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid30_Out0_copy31);
   Compressor_23_3_F400_uid15_bh4_uid30_Out0 <= Compressor_23_3_F400_uid15_bh4_uid30_Out0_copy31; -- output copy to hold a pipeline register if needed

   bh4_w28_4 <= Compressor_23_3_F400_uid15_bh4_uid30_Out0(0);
   bh4_w29_3 <= Compressor_23_3_F400_uid15_bh4_uid30_Out0(1);
   bh4_w30_3 <= Compressor_23_3_F400_uid15_bh4_uid30_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid32_In0 <= "" & bh4_w30_0 & bh4_w30_1 & bh4_w30_2;
   Compressor_23_3_F400_uid15_bh4_uid32_In1 <= "" & bh4_w31_0 & bh4_w31_1;
   Compressor_23_3_F400_uid15_uid32: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid32_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid32_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid32_Out0_copy33);
   Compressor_23_3_F400_uid15_bh4_uid32_Out0 <= Compressor_23_3_F400_uid15_bh4_uid32_Out0_copy33; -- output copy to hold a pipeline register if needed

   bh4_w30_4 <= Compressor_23_3_F400_uid15_bh4_uid32_Out0(0);
   bh4_w31_3 <= Compressor_23_3_F400_uid15_bh4_uid32_Out0(1);
   bh4_w32_3 <= Compressor_23_3_F400_uid15_bh4_uid32_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid34_In0 <= "" & bh4_w32_0 & bh4_w32_1 & bh4_w32_2;
   Compressor_23_3_F400_uid15_bh4_uid34_In1 <= "" & bh4_w33_0 & bh4_w33_1;
   Compressor_23_3_F400_uid15_uid34: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid34_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid34_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid34_Out0_copy35);
   Compressor_23_3_F400_uid15_bh4_uid34_Out0 <= Compressor_23_3_F400_uid15_bh4_uid34_Out0_copy35; -- output copy to hold a pipeline register if needed

   bh4_w32_4 <= Compressor_23_3_F400_uid15_bh4_uid34_Out0(0);
   bh4_w33_3 <= Compressor_23_3_F400_uid15_bh4_uid34_Out0(1);
   bh4_w34_3 <= Compressor_23_3_F400_uid15_bh4_uid34_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid36_In0 <= "" & bh4_w34_0 & bh4_w34_1 & bh4_w34_2;
   Compressor_23_3_F400_uid15_bh4_uid36_In1 <= "" & bh4_w35_0 & bh4_w35_1;
   Compressor_23_3_F400_uid15_uid36: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid36_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid36_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid36_Out0_copy37);
   Compressor_23_3_F400_uid15_bh4_uid36_Out0 <= Compressor_23_3_F400_uid15_bh4_uid36_Out0_copy37; -- output copy to hold a pipeline register if needed

   bh4_w34_4 <= Compressor_23_3_F400_uid15_bh4_uid36_Out0(0);
   bh4_w35_3 <= Compressor_23_3_F400_uid15_bh4_uid36_Out0(1);
   bh4_w36_3 <= Compressor_23_3_F400_uid15_bh4_uid36_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid38_In0 <= "" & bh4_w36_0 & bh4_w36_1 & bh4_w36_2;
   Compressor_23_3_F400_uid15_bh4_uid38_In1 <= "" & bh4_w37_0 & bh4_w37_1;
   Compressor_23_3_F400_uid15_uid38: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid38_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid38_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid38_Out0_copy39);
   Compressor_23_3_F400_uid15_bh4_uid38_Out0 <= Compressor_23_3_F400_uid15_bh4_uid38_Out0_copy39; -- output copy to hold a pipeline register if needed

   bh4_w36_4 <= Compressor_23_3_F400_uid15_bh4_uid38_Out0(0);
   bh4_w37_3 <= Compressor_23_3_F400_uid15_bh4_uid38_Out0(1);
   bh4_w38_3 <= Compressor_23_3_F400_uid15_bh4_uid38_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid40_In0 <= "" & bh4_w38_0 & bh4_w38_1 & bh4_w38_2;
   Compressor_23_3_F400_uid15_bh4_uid40_In1 <= "" & bh4_w39_0 & bh4_w39_1;
   Compressor_23_3_F400_uid15_uid40: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid40_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid40_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid40_Out0_copy41);
   Compressor_23_3_F400_uid15_bh4_uid40_Out0 <= Compressor_23_3_F400_uid15_bh4_uid40_Out0_copy41; -- output copy to hold a pipeline register if needed

   bh4_w38_4 <= Compressor_23_3_F400_uid15_bh4_uid40_Out0(0);
   bh4_w39_3 <= Compressor_23_3_F400_uid15_bh4_uid40_Out0(1);
   bh4_w40_3 <= Compressor_23_3_F400_uid15_bh4_uid40_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid42_In0 <= "" & bh4_w40_0 & bh4_w40_1 & bh4_w40_2;
   Compressor_23_3_F400_uid15_bh4_uid42_In1 <= "" & bh4_w41_0 & bh4_w41_1;
   Compressor_23_3_F400_uid15_uid42: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid42_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid42_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid42_Out0_copy43);
   Compressor_23_3_F400_uid15_bh4_uid42_Out0 <= Compressor_23_3_F400_uid15_bh4_uid42_Out0_copy43; -- output copy to hold a pipeline register if needed

   bh4_w40_4 <= Compressor_23_3_F400_uid15_bh4_uid42_Out0(0);
   bh4_w41_3 <= Compressor_23_3_F400_uid15_bh4_uid42_Out0(1);
   bh4_w42_3 <= Compressor_23_3_F400_uid15_bh4_uid42_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid44_In0 <= "" & bh4_w42_0 & bh4_w42_1 & bh4_w42_2;
   Compressor_23_3_F400_uid15_bh4_uid44_In1 <= "" & bh4_w43_0 & bh4_w43_1;
   Compressor_23_3_F400_uid15_uid44: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid44_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid44_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid44_Out0_copy45);
   Compressor_23_3_F400_uid15_bh4_uid44_Out0 <= Compressor_23_3_F400_uid15_bh4_uid44_Out0_copy45; -- output copy to hold a pipeline register if needed

   bh4_w42_4 <= Compressor_23_3_F400_uid15_bh4_uid44_Out0(0);
   bh4_w43_3 <= Compressor_23_3_F400_uid15_bh4_uid44_Out0(1);
   bh4_w44_3 <= Compressor_23_3_F400_uid15_bh4_uid44_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid46_In0 <= "" & bh4_w44_0 & bh4_w44_1 & bh4_w44_2;
   Compressor_23_3_F400_uid15_bh4_uid46_In1 <= "" & bh4_w45_0 & bh4_w45_1;
   Compressor_23_3_F400_uid15_uid46: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid46_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid46_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid46_Out0_copy47);
   Compressor_23_3_F400_uid15_bh4_uid46_Out0 <= Compressor_23_3_F400_uid15_bh4_uid46_Out0_copy47; -- output copy to hold a pipeline register if needed

   bh4_w44_4 <= Compressor_23_3_F400_uid15_bh4_uid46_Out0(0);
   bh4_w45_3 <= Compressor_23_3_F400_uid15_bh4_uid46_Out0(1);
   bh4_w46_3 <= Compressor_23_3_F400_uid15_bh4_uid46_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid48_In0 <= "" & bh4_w46_0 & bh4_w46_1 & bh4_w46_2;
   Compressor_23_3_F400_uid15_bh4_uid48_In1 <= "" & bh4_w47_0 & bh4_w47_1;
   Compressor_23_3_F400_uid15_uid48: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid48_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid48_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid48_Out0_copy49);
   Compressor_23_3_F400_uid15_bh4_uid48_Out0 <= Compressor_23_3_F400_uid15_bh4_uid48_Out0_copy49; -- output copy to hold a pipeline register if needed

   bh4_w46_4 <= Compressor_23_3_F400_uid15_bh4_uid48_Out0(0);
   bh4_w47_3 <= Compressor_23_3_F400_uid15_bh4_uid48_Out0(1);
   bh4_w48_3 <= Compressor_23_3_F400_uid15_bh4_uid48_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid50_In0 <= "" & bh4_w48_0 & bh4_w48_1 & bh4_w48_2;
   Compressor_23_3_F400_uid15_bh4_uid50_In1 <= "" & bh4_w49_0 & bh4_w49_1;
   Compressor_23_3_F400_uid15_uid50: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid50_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid50_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid50_Out0_copy51);
   Compressor_23_3_F400_uid15_bh4_uid50_Out0 <= Compressor_23_3_F400_uid15_bh4_uid50_Out0_copy51; -- output copy to hold a pipeline register if needed

   bh4_w48_4 <= Compressor_23_3_F400_uid15_bh4_uid50_Out0(0);
   bh4_w49_2 <= Compressor_23_3_F400_uid15_bh4_uid50_Out0(1);
   bh4_w50_2 <= Compressor_23_3_F400_uid15_bh4_uid50_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid52_In0 <= "" & bh4_w50_0 & bh4_w50_1 & "0";
   Compressor_23_3_F400_uid15_bh4_uid52_In1 <= "" & bh4_w51_0 & bh4_w51_1;
   Compressor_23_3_F400_uid15_uid52: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid52_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid52_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid52_Out0_copy53);
   Compressor_23_3_F400_uid15_bh4_uid52_Out0 <= Compressor_23_3_F400_uid15_bh4_uid52_Out0_copy53; -- output copy to hold a pipeline register if needed

   bh4_w50_3 <= Compressor_23_3_F400_uid15_bh4_uid52_Out0(0);
   bh4_w51_2 <= Compressor_23_3_F400_uid15_bh4_uid52_Out0(1);
   bh4_w52_2 <= Compressor_23_3_F400_uid15_bh4_uid52_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid54_In0 <= "" & bh4_w52_0 & bh4_w52_1 & "0";
   Compressor_23_3_F400_uid15_bh4_uid54_In1 <= "" & bh4_w53_0 & bh4_w53_1;
   Compressor_23_3_F400_uid15_uid54: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid54_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid54_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid54_Out0_copy55);
   Compressor_23_3_F400_uid15_bh4_uid54_Out0 <= Compressor_23_3_F400_uid15_bh4_uid54_Out0_copy55; -- output copy to hold a pipeline register if needed

   bh4_w52_3 <= Compressor_23_3_F400_uid15_bh4_uid54_Out0(0);
   bh4_w53_2 <= Compressor_23_3_F400_uid15_bh4_uid54_Out0(1);
   bh4_w54_2 <= Compressor_23_3_F400_uid15_bh4_uid54_Out0(2);

   Compressor_23_3_F400_uid15_bh4_uid56_In0 <= "" & bh4_w54_0 & bh4_w54_1 & "0";
   Compressor_23_3_F400_uid15_bh4_uid56_In1 <= "" & bh4_w55_0 & bh4_w55_1;
   Compressor_23_3_F400_uid15_uid56: Compressor_23_3_F400_uid15
      port map ( X0 => Compressor_23_3_F400_uid15_bh4_uid56_In0,
                 X1 => Compressor_23_3_F400_uid15_bh4_uid56_In1,
                 R => Compressor_23_3_F400_uid15_bh4_uid56_Out0_copy57);
   Compressor_23_3_F400_uid15_bh4_uid56_Out0 <= Compressor_23_3_F400_uid15_bh4_uid56_Out0_copy57; -- output copy to hold a pipeline register if needed

   bh4_w54_3 <= Compressor_23_3_F400_uid15_bh4_uid56_Out0(0);
   bh4_w55_2 <= Compressor_23_3_F400_uid15_bh4_uid56_Out0(1);
   bh4_w56_1 <= Compressor_23_3_F400_uid15_bh4_uid56_Out0(2);
   tmp_bitheapResult_bh4_18 <= bh4_w18_2 & bh4_w17_2 & bh4_w16_0 & bh4_w15_0 & bh4_w14_0 & bh4_w13_0 & bh4_w12_0 & bh4_w11_0 & bh4_w10_0 & bh4_w9_0 & bh4_w8_0 & bh4_w7_0 & bh4_w6_0 & bh4_w5_0 & bh4_w4_0 & bh4_w3_0 & bh4_w2_0 & bh4_w1_0 & bh4_w0_0;

   bitheapFinalAdd_bh4_In0 <= "0" & bh4_w63_0 & bh4_w62_0 & bh4_w61_0 & bh4_w60_0 & bh4_w59_0 & bh4_w58_0 & bh4_w57_0 & bh4_w56_0 & bh4_w55_2 & bh4_w54_3 & bh4_w53_2 & bh4_w52_3 & bh4_w51_2 & bh4_w50_3 & bh4_w49_2 & bh4_w48_4 & bh4_w47_2 & bh4_w46_4 & bh4_w45_2 & bh4_w44_4 & bh4_w43_2 & bh4_w42_4 & bh4_w41_2 & bh4_w40_4 & bh4_w39_2 & bh4_w38_4 & bh4_w37_2 & bh4_w36_4 & bh4_w35_2 & bh4_w34_4 & bh4_w33_2 & bh4_w32_4 & bh4_w31_2 & bh4_w30_4 & bh4_w29_2 & bh4_w28_4 & bh4_w27_2 & bh4_w26_4 & bh4_w25_2 & bh4_w24_4 & bh4_w23_3 & bh4_w22_2 & bh4_w21_3 & bh4_w20_2 & bh4_w19_3;
   bitheapFinalAdd_bh4_In1 <= "0" & "0" & "0" & "0" & "0" & "0" & "0" & "0" & bh4_w56_1 & "0" & bh4_w54_2 & "0" & bh4_w52_2 & "0" & bh4_w50_2 & "0" & bh4_w48_3 & bh4_w47_3 & bh4_w46_3 & bh4_w45_3 & bh4_w44_3 & bh4_w43_3 & bh4_w42_3 & bh4_w41_3 & bh4_w40_3 & bh4_w39_3 & bh4_w38_3 & bh4_w37_3 & bh4_w36_3 & bh4_w35_3 & bh4_w34_3 & bh4_w33_3 & bh4_w32_3 & bh4_w31_3 & bh4_w30_3 & bh4_w29_3 & bh4_w28_3 & bh4_w27_3 & bh4_w26_3 & bh4_w25_3 & bh4_w24_3 & bh4_w23_2 & "0" & bh4_w21_2 & "0" & bh4_w19_2;
   bitheapFinalAdd_bh4_Cin <= '0';

   bitheapFinalAdd_bh4: IntAdder_46_F400_uid59
      port map ( clk  => clk,
                 Cin => bitheapFinalAdd_bh4_Cin,
                 X => bitheapFinalAdd_bh4_In0,
                 Y => bitheapFinalAdd_bh4_In1,
                 R => bitheapFinalAdd_bh4_Out);
   bitheapResult_bh4 <= bitheapFinalAdd_bh4_Out(44 downto 0) & tmp_bitheapResult_bh4_18_d2;
   R <= bitheapResult_bh4(63 downto 0);
end architecture;
