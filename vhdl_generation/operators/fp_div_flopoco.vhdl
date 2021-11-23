--------------------------------------------------------------------------------
--                          selFunction7_4_F400_uid4
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved 
-- Authors: Florent de Dinechin, Bogdan Pasca (2007-2018)
--------------------------------------------------------------------------------
-- combinatorial
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X
-- Output signals: Y

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library std;
use std.textio.all;
library work;

entity selFunction7_4_F400_uid4 is
    port (X : in  std_logic_vector(6 downto 0);
          Y : out  std_logic_vector(3 downto 0)   );
end entity;

architecture arch of selFunction7_4_F400_uid4 is
signal Y0 :  std_logic_vector(3 downto 0);
attribute ram_extract: string;
attribute ram_style: string;
attribute ram_extract of Y0: signal is "yes";
attribute ram_style of Y0: signal is "distributed";
begin
   with X  select  Y0 <= 
      "0000" when "0000000",
      "0000" when "0000001",
      "0000" when "0000010",
      "0000" when "0000011",
      "0001" when "0000100",
      "0001" when "0000101",
      "0001" when "0000110",
      "0001" when "0000111",
      "0001" when "0001000",
      "0001" when "0001001",
      "0001" when "0001010",
      "0001" when "0001011",
      "0010" when "0001100",
      "0010" when "0001101",
      "0010" when "0001110",
      "0010" when "0001111",
      "0011" when "0010000",
      "0011" when "0010001",
      "0010" when "0010010",
      "0010" when "0010011",
      "0011" when "0010100",
      "0011" when "0010101",
      "0011" when "0010110",
      "0011" when "0010111",
      "0100" when "0011000",
      "0100" when "0011001",
      "0011" when "0011010",
      "0011" when "0011011",
      "0101" when "0011100",
      "0100" when "0011101",
      "0100" when "0011110",
      "0100" when "0011111",
      "0101" when "0100000",
      "0101" when "0100001",
      "0101" when "0100010",
      "0100" when "0100011",
      "0110" when "0100100",
      "0110" when "0100101",
      "0101" when "0100110",
      "0101" when "0100111",
      "0111" when "0101000",
      "0110" when "0101001",
      "0110" when "0101010",
      "0101" when "0101011",
      "0111" when "0101100",
      "0111" when "0101101",
      "0110" when "0101110",
      "0110" when "0101111",
      "0111" when "0110000",
      "0111" when "0110001",
      "0111" when "0110010",
      "0110" when "0110011",
      "0111" when "0110100",
      "0111" when "0110101",
      "0111" when "0110110",
      "0111" when "0110111",
      "0111" when "0111000",
      "0111" when "0111001",
      "0111" when "0111010",
      "0111" when "0111011",
      "0111" when "0111100",
      "0111" when "0111101",
      "0111" when "0111110",
      "0111" when "0111111",
      "1001" when "1000000",
      "1001" when "1000001",
      "1001" when "1000010",
      "1001" when "1000011",
      "1001" when "1000100",
      "1001" when "1000101",
      "1001" when "1000110",
      "1001" when "1000111",
      "1001" when "1001000",
      "1001" when "1001001",
      "1001" when "1001010",
      "1001" when "1001011",
      "1001" when "1001100",
      "1001" when "1001101",
      "1001" when "1001110",
      "1001" when "1001111",
      "1001" when "1010000",
      "1001" when "1010001",
      "1010" when "1010010",
      "1010" when "1010011",
      "1001" when "1010100",
      "1010" when "1010101",
      "1010" when "1010110",
      "1010" when "1010111",
      "1010" when "1011000",
      "1010" when "1011001",
      "1011" when "1011010",
      "1011" when "1011011",
      "1011" when "1011100",
      "1011" when "1011101",
      "1011" when "1011110",
      "1011" when "1011111",
      "1011" when "1100000",
      "1011" when "1100001",
      "1100" when "1100010",
      "1100" when "1100011",
      "1100" when "1100100",
      "1100" when "1100101",
      "1100" when "1100110",
      "1100" when "1100111",
      "1100" when "1101000",
      "1101" when "1101001",
      "1101" when "1101010",
      "1101" when "1101011",
      "1101" when "1101100",
      "1101" when "1101101",
      "1101" when "1101110",
      "1101" when "1101111",
      "1110" when "1110000",
      "1110" when "1110001",
      "1110" when "1110010",
      "1110" when "1110011",
      "1110" when "1110100",
      "1110" when "1110101",
      "1110" when "1110110",
      "1110" when "1110111",
      "1111" when "1111000",
      "1111" when "1111001",
      "1111" when "1111010",
      "1111" when "1111011",
      "1111" when "1111100",
      "1111" when "1111101",
      "1111" when "1111110",
      "1111" when "1111111",
      "----" when others;
   Y <= Y0;
end architecture;

--------------------------------------------------------------------------------
--                            FPDiv_8_23_F400_uid2
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved 
-- Authors: Maxime Christ, Florent de Dinechin (2015)
--------------------------------------------------------------------------------
-- Pipeline depth: 29 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X Y
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library std;
use std.textio.all;
library work;

entity FPDiv_8_23_F400_uid2 is
    port (clk : in std_logic;
          X : in  std_logic_vector(8+23+2 downto 0);
          Y : in  std_logic_vector(8+23+2 downto 0);
          R : out  std_logic_vector(8+23+2 downto 0)   );
end entity;

architecture arch of FPDiv_8_23_F400_uid2 is
   component selFunction7_4_F400_uid4 is
      port ( X : in  std_logic_vector(6 downto 0);
             Y : out  std_logic_vector(3 downto 0)   );
   end component;

signal fX :  std_logic_vector(23 downto 0);
signal fY :  std_logic_vector(23 downto 0);
signal expR0, expR0_d1, expR0_d2, expR0_d3, expR0_d4, expR0_d5, expR0_d6, expR0_d7, expR0_d8, expR0_d9, expR0_d10, expR0_d11, expR0_d12, expR0_d13, expR0_d14, expR0_d15, expR0_d16, expR0_d17, expR0_d18, expR0_d19, expR0_d20, expR0_d21, expR0_d22, expR0_d23, expR0_d24, expR0_d25, expR0_d26, expR0_d27, expR0_d28, expR0_d29 :  std_logic_vector(9 downto 0);
signal sR, sR_d1, sR_d2, sR_d3, sR_d4, sR_d5, sR_d6, sR_d7, sR_d8, sR_d9, sR_d10, sR_d11, sR_d12, sR_d13, sR_d14, sR_d15, sR_d16, sR_d17, sR_d18, sR_d19, sR_d20, sR_d21, sR_d22, sR_d23, sR_d24, sR_d25, sR_d26, sR_d27, sR_d28, sR_d29 :  std_logic;
signal exnXY :  std_logic_vector(3 downto 0);
signal exnR0, exnR0_d1, exnR0_d2, exnR0_d3, exnR0_d4, exnR0_d5, exnR0_d6, exnR0_d7, exnR0_d8, exnR0_d9, exnR0_d10, exnR0_d11, exnR0_d12, exnR0_d13, exnR0_d14, exnR0_d15, exnR0_d16, exnR0_d17, exnR0_d18, exnR0_d19, exnR0_d20, exnR0_d21, exnR0_d22, exnR0_d23, exnR0_d24, exnR0_d25, exnR0_d26, exnR0_d27, exnR0_d28, exnR0_d29 :  std_logic_vector(1 downto 0);
signal prescaledfY, prescaledfY_d1, prescaledfY_d2, prescaledfY_d3, prescaledfY_d4, prescaledfY_d5, prescaledfY_d6, prescaledfY_d7, prescaledfY_d8, prescaledfY_d9, prescaledfY_d10, prescaledfY_d11, prescaledfY_d12, prescaledfY_d13, prescaledfY_d14, prescaledfY_d15, prescaledfY_d16, prescaledfY_d17, prescaledfY_d18, prescaledfY_d19, prescaledfY_d20, prescaledfY_d21, prescaledfY_d22, prescaledfY_d23, prescaledfY_d24, prescaledfY_d25 :  std_logic_vector(25 downto 0);
signal prescaledfX :  std_logic_vector(26 downto 0);
signal w9 :  std_logic_vector(28 downto 0);
signal sel9 :  std_logic_vector(6 downto 0);
signal q9, q9_d1, q9_d2 :  std_logic_vector(3 downto 0);
signal q9_copy5, q9_copy5_d1 :  std_logic_vector(3 downto 0);
signal w9pad, w9pad_d1, w9pad_d2 :  std_logic_vector(29 downto 0);
signal w8fulla, w8fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec8, fYdec8_d1, fYdec8_d2 :  std_logic_vector(29 downto 0);
signal w8full :  std_logic_vector(29 downto 0);
signal w8 :  std_logic_vector(28 downto 0);
signal sel8 :  std_logic_vector(6 downto 0);
signal q8, q8_d1, q8_d2 :  std_logic_vector(3 downto 0);
signal q8_copy6, q8_copy6_d1 :  std_logic_vector(3 downto 0);
signal w8pad, w8pad_d1, w8pad_d2 :  std_logic_vector(29 downto 0);
signal w7fulla, w7fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec7, fYdec7_d1, fYdec7_d2 :  std_logic_vector(29 downto 0);
signal w7full :  std_logic_vector(29 downto 0);
signal w7 :  std_logic_vector(28 downto 0);
signal sel7 :  std_logic_vector(6 downto 0);
signal q7, q7_d1, q7_d2 :  std_logic_vector(3 downto 0);
signal q7_copy7, q7_copy7_d1 :  std_logic_vector(3 downto 0);
signal w7pad, w7pad_d1, w7pad_d2 :  std_logic_vector(29 downto 0);
signal w6fulla, w6fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec6, fYdec6_d1, fYdec6_d2 :  std_logic_vector(29 downto 0);
signal w6full :  std_logic_vector(29 downto 0);
signal w6 :  std_logic_vector(28 downto 0);
signal sel6 :  std_logic_vector(6 downto 0);
signal q6, q6_d1, q6_d2 :  std_logic_vector(3 downto 0);
signal q6_copy8, q6_copy8_d1 :  std_logic_vector(3 downto 0);
signal w6pad, w6pad_d1, w6pad_d2 :  std_logic_vector(29 downto 0);
signal w5fulla, w5fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec5, fYdec5_d1, fYdec5_d2 :  std_logic_vector(29 downto 0);
signal w5full :  std_logic_vector(29 downto 0);
signal w5 :  std_logic_vector(28 downto 0);
signal sel5 :  std_logic_vector(6 downto 0);
signal q5, q5_d1, q5_d2 :  std_logic_vector(3 downto 0);
signal q5_copy9, q5_copy9_d1 :  std_logic_vector(3 downto 0);
signal w5pad, w5pad_d1, w5pad_d2 :  std_logic_vector(29 downto 0);
signal w4fulla, w4fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec4, fYdec4_d1, fYdec4_d2 :  std_logic_vector(29 downto 0);
signal w4full :  std_logic_vector(29 downto 0);
signal w4 :  std_logic_vector(28 downto 0);
signal sel4 :  std_logic_vector(6 downto 0);
signal q4, q4_d1, q4_d2, q4_d3 :  std_logic_vector(3 downto 0);
signal q4_copy10 :  std_logic_vector(3 downto 0);
signal w4pad, w4pad_d1, w4pad_d2 :  std_logic_vector(29 downto 0);
signal w3fulla, w3fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec3, fYdec3_d1, fYdec3_d2 :  std_logic_vector(29 downto 0);
signal w3full :  std_logic_vector(29 downto 0);
signal w3 :  std_logic_vector(28 downto 0);
signal sel3 :  std_logic_vector(6 downto 0);
signal q3, q3_d1, q3_d2, q3_d3 :  std_logic_vector(3 downto 0);
signal q3_copy11 :  std_logic_vector(3 downto 0);
signal w3pad, w3pad_d1, w3pad_d2 :  std_logic_vector(29 downto 0);
signal w2fulla, w2fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec2, fYdec2_d1, fYdec2_d2 :  std_logic_vector(29 downto 0);
signal w2full :  std_logic_vector(29 downto 0);
signal w2 :  std_logic_vector(28 downto 0);
signal sel2 :  std_logic_vector(6 downto 0);
signal q2, q2_d1, q2_d2, q2_d3 :  std_logic_vector(3 downto 0);
signal q2_copy12 :  std_logic_vector(3 downto 0);
signal w2pad, w2pad_d1, w2pad_d2 :  std_logic_vector(29 downto 0);
signal w1fulla, w1fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec1, fYdec1_d1, fYdec1_d2 :  std_logic_vector(29 downto 0);
signal w1full :  std_logic_vector(29 downto 0);
signal w1 :  std_logic_vector(28 downto 0);
signal sel1 :  std_logic_vector(6 downto 0);
signal q1, q1_d1, q1_d2 :  std_logic_vector(3 downto 0);
signal q1_copy13 :  std_logic_vector(3 downto 0);
signal w1pad, w1pad_d1 :  std_logic_vector(29 downto 0);
signal w0fulla, w0fulla_d1 :  std_logic_vector(29 downto 0);
signal fYdec0, fYdec0_d1 :  std_logic_vector(29 downto 0);
signal w0full :  std_logic_vector(29 downto 0);
signal w0, w0_d1 :  std_logic_vector(28 downto 0);
signal q0 :  std_logic_vector(3 downto 0);
signal qP9, qP9_d1, qP9_d2, qP9_d3, qP9_d4, qP9_d5, qP9_d6, qP9_d7, qP9_d8, qP9_d9, qP9_d10, qP9_d11, qP9_d12, qP9_d13, qP9_d14, qP9_d15, qP9_d16, qP9_d17, qP9_d18, qP9_d19, qP9_d20, qP9_d21, qP9_d22, qP9_d23, qP9_d24, qP9_d25, qP9_d26 :  std_logic_vector(2 downto 0);
signal qM9, qM9_d1, qM9_d2, qM9_d3, qM9_d4, qM9_d5, qM9_d6, qM9_d7, qM9_d8, qM9_d9, qM9_d10, qM9_d11, qM9_d12, qM9_d13, qM9_d14, qM9_d15, qM9_d16, qM9_d17, qM9_d18, qM9_d19, qM9_d20, qM9_d21, qM9_d22, qM9_d23, qM9_d24, qM9_d25, qM9_d26 :  std_logic_vector(2 downto 0);
signal qP8, qP8_d1, qP8_d2, qP8_d3, qP8_d4, qP8_d5, qP8_d6, qP8_d7, qP8_d8, qP8_d9, qP8_d10, qP8_d11, qP8_d12, qP8_d13, qP8_d14, qP8_d15, qP8_d16, qP8_d17, qP8_d18, qP8_d19, qP8_d20, qP8_d21, qP8_d22, qP8_d23 :  std_logic_vector(2 downto 0);
signal qM8, qM8_d1, qM8_d2, qM8_d3, qM8_d4, qM8_d5, qM8_d6, qM8_d7, qM8_d8, qM8_d9, qM8_d10, qM8_d11, qM8_d12, qM8_d13, qM8_d14, qM8_d15, qM8_d16, qM8_d17, qM8_d18, qM8_d19, qM8_d20, qM8_d21, qM8_d22, qM8_d23 :  std_logic_vector(2 downto 0);
signal qP7, qP7_d1, qP7_d2, qP7_d3, qP7_d4, qP7_d5, qP7_d6, qP7_d7, qP7_d8, qP7_d9, qP7_d10, qP7_d11, qP7_d12, qP7_d13, qP7_d14, qP7_d15, qP7_d16, qP7_d17, qP7_d18, qP7_d19, qP7_d20 :  std_logic_vector(2 downto 0);
signal qM7, qM7_d1, qM7_d2, qM7_d3, qM7_d4, qM7_d5, qM7_d6, qM7_d7, qM7_d8, qM7_d9, qM7_d10, qM7_d11, qM7_d12, qM7_d13, qM7_d14, qM7_d15, qM7_d16, qM7_d17, qM7_d18, qM7_d19, qM7_d20 :  std_logic_vector(2 downto 0);
signal qP6, qP6_d1, qP6_d2, qP6_d3, qP6_d4, qP6_d5, qP6_d6, qP6_d7, qP6_d8, qP6_d9, qP6_d10, qP6_d11, qP6_d12, qP6_d13, qP6_d14, qP6_d15, qP6_d16, qP6_d17 :  std_logic_vector(2 downto 0);
signal qM6, qM6_d1, qM6_d2, qM6_d3, qM6_d4, qM6_d5, qM6_d6, qM6_d7, qM6_d8, qM6_d9, qM6_d10, qM6_d11, qM6_d12, qM6_d13, qM6_d14, qM6_d15, qM6_d16, qM6_d17 :  std_logic_vector(2 downto 0);
signal qP5, qP5_d1, qP5_d2, qP5_d3, qP5_d4, qP5_d5, qP5_d6, qP5_d7, qP5_d8, qP5_d9, qP5_d10, qP5_d11, qP5_d12, qP5_d13, qP5_d14 :  std_logic_vector(2 downto 0);
signal qM5, qM5_d1, qM5_d2, qM5_d3, qM5_d4, qM5_d5, qM5_d6, qM5_d7, qM5_d8, qM5_d9, qM5_d10, qM5_d11, qM5_d12, qM5_d13, qM5_d14 :  std_logic_vector(2 downto 0);
signal qP4, qP4_d1, qP4_d2, qP4_d3, qP4_d4, qP4_d5, qP4_d6, qP4_d7, qP4_d8, qP4_d9, qP4_d10, qP4_d11, qP4_d12 :  std_logic_vector(2 downto 0);
signal qM4, qM4_d1, qM4_d2, qM4_d3, qM4_d4, qM4_d5, qM4_d6, qM4_d7, qM4_d8, qM4_d9, qM4_d10, qM4_d11, qM4_d12 :  std_logic_vector(2 downto 0);
signal qP3, qP3_d1, qP3_d2, qP3_d3, qP3_d4, qP3_d5, qP3_d6, qP3_d7, qP3_d8, qP3_d9 :  std_logic_vector(2 downto 0);
signal qM3, qM3_d1, qM3_d2, qM3_d3, qM3_d4, qM3_d5, qM3_d6, qM3_d7, qM3_d8, qM3_d9 :  std_logic_vector(2 downto 0);
signal qP2, qP2_d1, qP2_d2, qP2_d3, qP2_d4, qP2_d5, qP2_d6 :  std_logic_vector(2 downto 0);
signal qM2, qM2_d1, qM2_d2, qM2_d3, qM2_d4, qM2_d5, qM2_d6 :  std_logic_vector(2 downto 0);
signal qP1, qP1_d1, qP1_d2, qP1_d3 :  std_logic_vector(2 downto 0);
signal qM1, qM1_d1, qM1_d2, qM1_d3 :  std_logic_vector(2 downto 0);
signal qP0 :  std_logic_vector(2 downto 0);
signal qM0 :  std_logic_vector(2 downto 0);
signal qP, qP_d1 :  std_logic_vector(29 downto 0);
signal qM, qM_d1 :  std_logic_vector(29 downto 0);
signal fR0 :  std_logic_vector(29 downto 0);
signal fR, fR_d1 :  std_logic_vector(28 downto 0);
signal fRn1, fRn1_d1 :  std_logic_vector(26 downto 0);
signal round, round_d1 :  std_logic;
signal expR1 :  std_logic_vector(9 downto 0);
signal expfrac :  std_logic_vector(32 downto 0);
signal expfracR :  std_logic_vector(32 downto 0);
signal exnR :  std_logic_vector(1 downto 0);
signal exnRfinal :  std_logic_vector(1 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            expR0_d1 <=  expR0;
            expR0_d2 <=  expR0_d1;
            expR0_d3 <=  expR0_d2;
            expR0_d4 <=  expR0_d3;
            expR0_d5 <=  expR0_d4;
            expR0_d6 <=  expR0_d5;
            expR0_d7 <=  expR0_d6;
            expR0_d8 <=  expR0_d7;
            expR0_d9 <=  expR0_d8;
            expR0_d10 <=  expR0_d9;
            expR0_d11 <=  expR0_d10;
            expR0_d12 <=  expR0_d11;
            expR0_d13 <=  expR0_d12;
            expR0_d14 <=  expR0_d13;
            expR0_d15 <=  expR0_d14;
            expR0_d16 <=  expR0_d15;
            expR0_d17 <=  expR0_d16;
            expR0_d18 <=  expR0_d17;
            expR0_d19 <=  expR0_d18;
            expR0_d20 <=  expR0_d19;
            expR0_d21 <=  expR0_d20;
            expR0_d22 <=  expR0_d21;
            expR0_d23 <=  expR0_d22;
            expR0_d24 <=  expR0_d23;
            expR0_d25 <=  expR0_d24;
            expR0_d26 <=  expR0_d25;
            expR0_d27 <=  expR0_d26;
            expR0_d28 <=  expR0_d27;
            expR0_d29 <=  expR0_d28;
            sR_d1 <=  sR;
            sR_d2 <=  sR_d1;
            sR_d3 <=  sR_d2;
            sR_d4 <=  sR_d3;
            sR_d5 <=  sR_d4;
            sR_d6 <=  sR_d5;
            sR_d7 <=  sR_d6;
            sR_d8 <=  sR_d7;
            sR_d9 <=  sR_d8;
            sR_d10 <=  sR_d9;
            sR_d11 <=  sR_d10;
            sR_d12 <=  sR_d11;
            sR_d13 <=  sR_d12;
            sR_d14 <=  sR_d13;
            sR_d15 <=  sR_d14;
            sR_d16 <=  sR_d15;
            sR_d17 <=  sR_d16;
            sR_d18 <=  sR_d17;
            sR_d19 <=  sR_d18;
            sR_d20 <=  sR_d19;
            sR_d21 <=  sR_d20;
            sR_d22 <=  sR_d21;
            sR_d23 <=  sR_d22;
            sR_d24 <=  sR_d23;
            sR_d25 <=  sR_d24;
            sR_d26 <=  sR_d25;
            sR_d27 <=  sR_d26;
            sR_d28 <=  sR_d27;
            sR_d29 <=  sR_d28;
            exnR0_d1 <=  exnR0;
            exnR0_d2 <=  exnR0_d1;
            exnR0_d3 <=  exnR0_d2;
            exnR0_d4 <=  exnR0_d3;
            exnR0_d5 <=  exnR0_d4;
            exnR0_d6 <=  exnR0_d5;
            exnR0_d7 <=  exnR0_d6;
            exnR0_d8 <=  exnR0_d7;
            exnR0_d9 <=  exnR0_d8;
            exnR0_d10 <=  exnR0_d9;
            exnR0_d11 <=  exnR0_d10;
            exnR0_d12 <=  exnR0_d11;
            exnR0_d13 <=  exnR0_d12;
            exnR0_d14 <=  exnR0_d13;
            exnR0_d15 <=  exnR0_d14;
            exnR0_d16 <=  exnR0_d15;
            exnR0_d17 <=  exnR0_d16;
            exnR0_d18 <=  exnR0_d17;
            exnR0_d19 <=  exnR0_d18;
            exnR0_d20 <=  exnR0_d19;
            exnR0_d21 <=  exnR0_d20;
            exnR0_d22 <=  exnR0_d21;
            exnR0_d23 <=  exnR0_d22;
            exnR0_d24 <=  exnR0_d23;
            exnR0_d25 <=  exnR0_d24;
            exnR0_d26 <=  exnR0_d25;
            exnR0_d27 <=  exnR0_d26;
            exnR0_d28 <=  exnR0_d27;
            exnR0_d29 <=  exnR0_d28;
            prescaledfY_d1 <=  prescaledfY;
            prescaledfY_d2 <=  prescaledfY_d1;
            prescaledfY_d3 <=  prescaledfY_d2;
            prescaledfY_d4 <=  prescaledfY_d3;
            prescaledfY_d5 <=  prescaledfY_d4;
            prescaledfY_d6 <=  prescaledfY_d5;
            prescaledfY_d7 <=  prescaledfY_d6;
            prescaledfY_d8 <=  prescaledfY_d7;
            prescaledfY_d9 <=  prescaledfY_d8;
            prescaledfY_d10 <=  prescaledfY_d9;
            prescaledfY_d11 <=  prescaledfY_d10;
            prescaledfY_d12 <=  prescaledfY_d11;
            prescaledfY_d13 <=  prescaledfY_d12;
            prescaledfY_d14 <=  prescaledfY_d13;
            prescaledfY_d15 <=  prescaledfY_d14;
            prescaledfY_d16 <=  prescaledfY_d15;
            prescaledfY_d17 <=  prescaledfY_d16;
            prescaledfY_d18 <=  prescaledfY_d17;
            prescaledfY_d19 <=  prescaledfY_d18;
            prescaledfY_d20 <=  prescaledfY_d19;
            prescaledfY_d21 <=  prescaledfY_d20;
            prescaledfY_d22 <=  prescaledfY_d21;
            prescaledfY_d23 <=  prescaledfY_d22;
            prescaledfY_d24 <=  prescaledfY_d23;
            prescaledfY_d25 <=  prescaledfY_d24;
            q9_d1 <=  q9;
            q9_d2 <=  q9_d1;
            q9_copy5_d1 <=  q9_copy5;
            w9pad_d1 <=  w9pad;
            w9pad_d2 <=  w9pad_d1;
            w8fulla_d1 <=  w8fulla;
            fYdec8_d1 <=  fYdec8;
            fYdec8_d2 <=  fYdec8_d1;
            q8_d1 <=  q8;
            q8_d2 <=  q8_d1;
            q8_copy6_d1 <=  q8_copy6;
            w8pad_d1 <=  w8pad;
            w8pad_d2 <=  w8pad_d1;
            w7fulla_d1 <=  w7fulla;
            fYdec7_d1 <=  fYdec7;
            fYdec7_d2 <=  fYdec7_d1;
            q7_d1 <=  q7;
            q7_d2 <=  q7_d1;
            q7_copy7_d1 <=  q7_copy7;
            w7pad_d1 <=  w7pad;
            w7pad_d2 <=  w7pad_d1;
            w6fulla_d1 <=  w6fulla;
            fYdec6_d1 <=  fYdec6;
            fYdec6_d2 <=  fYdec6_d1;
            q6_d1 <=  q6;
            q6_d2 <=  q6_d1;
            q6_copy8_d1 <=  q6_copy8;
            w6pad_d1 <=  w6pad;
            w6pad_d2 <=  w6pad_d1;
            w5fulla_d1 <=  w5fulla;
            fYdec5_d1 <=  fYdec5;
            fYdec5_d2 <=  fYdec5_d1;
            q5_d1 <=  q5;
            q5_d2 <=  q5_d1;
            q5_copy9_d1 <=  q5_copy9;
            w5pad_d1 <=  w5pad;
            w5pad_d2 <=  w5pad_d1;
            w4fulla_d1 <=  w4fulla;
            fYdec4_d1 <=  fYdec4;
            fYdec4_d2 <=  fYdec4_d1;
            q4_d1 <=  q4;
            q4_d2 <=  q4_d1;
            q4_d3 <=  q4_d2;
            w4pad_d1 <=  w4pad;
            w4pad_d2 <=  w4pad_d1;
            w3fulla_d1 <=  w3fulla;
            fYdec3_d1 <=  fYdec3;
            fYdec3_d2 <=  fYdec3_d1;
            q3_d1 <=  q3;
            q3_d2 <=  q3_d1;
            q3_d3 <=  q3_d2;
            w3pad_d1 <=  w3pad;
            w3pad_d2 <=  w3pad_d1;
            w2fulla_d1 <=  w2fulla;
            fYdec2_d1 <=  fYdec2;
            fYdec2_d2 <=  fYdec2_d1;
            q2_d1 <=  q2;
            q2_d2 <=  q2_d1;
            q2_d3 <=  q2_d2;
            w2pad_d1 <=  w2pad;
            w2pad_d2 <=  w2pad_d1;
            w1fulla_d1 <=  w1fulla;
            fYdec1_d1 <=  fYdec1;
            fYdec1_d2 <=  fYdec1_d1;
            q1_d1 <=  q1;
            q1_d2 <=  q1_d1;
            w1pad_d1 <=  w1pad;
            w0fulla_d1 <=  w0fulla;
            fYdec0_d1 <=  fYdec0;
            w0_d1 <=  w0;
            qP9_d1 <=  qP9;
            qP9_d2 <=  qP9_d1;
            qP9_d3 <=  qP9_d2;
            qP9_d4 <=  qP9_d3;
            qP9_d5 <=  qP9_d4;
            qP9_d6 <=  qP9_d5;
            qP9_d7 <=  qP9_d6;
            qP9_d8 <=  qP9_d7;
            qP9_d9 <=  qP9_d8;
            qP9_d10 <=  qP9_d9;
            qP9_d11 <=  qP9_d10;
            qP9_d12 <=  qP9_d11;
            qP9_d13 <=  qP9_d12;
            qP9_d14 <=  qP9_d13;
            qP9_d15 <=  qP9_d14;
            qP9_d16 <=  qP9_d15;
            qP9_d17 <=  qP9_d16;
            qP9_d18 <=  qP9_d17;
            qP9_d19 <=  qP9_d18;
            qP9_d20 <=  qP9_d19;
            qP9_d21 <=  qP9_d20;
            qP9_d22 <=  qP9_d21;
            qP9_d23 <=  qP9_d22;
            qP9_d24 <=  qP9_d23;
            qP9_d25 <=  qP9_d24;
            qP9_d26 <=  qP9_d25;
            qM9_d1 <=  qM9;
            qM9_d2 <=  qM9_d1;
            qM9_d3 <=  qM9_d2;
            qM9_d4 <=  qM9_d3;
            qM9_d5 <=  qM9_d4;
            qM9_d6 <=  qM9_d5;
            qM9_d7 <=  qM9_d6;
            qM9_d8 <=  qM9_d7;
            qM9_d9 <=  qM9_d8;
            qM9_d10 <=  qM9_d9;
            qM9_d11 <=  qM9_d10;
            qM9_d12 <=  qM9_d11;
            qM9_d13 <=  qM9_d12;
            qM9_d14 <=  qM9_d13;
            qM9_d15 <=  qM9_d14;
            qM9_d16 <=  qM9_d15;
            qM9_d17 <=  qM9_d16;
            qM9_d18 <=  qM9_d17;
            qM9_d19 <=  qM9_d18;
            qM9_d20 <=  qM9_d19;
            qM9_d21 <=  qM9_d20;
            qM9_d22 <=  qM9_d21;
            qM9_d23 <=  qM9_d22;
            qM9_d24 <=  qM9_d23;
            qM9_d25 <=  qM9_d24;
            qM9_d26 <=  qM9_d25;
            qP8_d1 <=  qP8;
            qP8_d2 <=  qP8_d1;
            qP8_d3 <=  qP8_d2;
            qP8_d4 <=  qP8_d3;
            qP8_d5 <=  qP8_d4;
            qP8_d6 <=  qP8_d5;
            qP8_d7 <=  qP8_d6;
            qP8_d8 <=  qP8_d7;
            qP8_d9 <=  qP8_d8;
            qP8_d10 <=  qP8_d9;
            qP8_d11 <=  qP8_d10;
            qP8_d12 <=  qP8_d11;
            qP8_d13 <=  qP8_d12;
            qP8_d14 <=  qP8_d13;
            qP8_d15 <=  qP8_d14;
            qP8_d16 <=  qP8_d15;
            qP8_d17 <=  qP8_d16;
            qP8_d18 <=  qP8_d17;
            qP8_d19 <=  qP8_d18;
            qP8_d20 <=  qP8_d19;
            qP8_d21 <=  qP8_d20;
            qP8_d22 <=  qP8_d21;
            qP8_d23 <=  qP8_d22;
            qM8_d1 <=  qM8;
            qM8_d2 <=  qM8_d1;
            qM8_d3 <=  qM8_d2;
            qM8_d4 <=  qM8_d3;
            qM8_d5 <=  qM8_d4;
            qM8_d6 <=  qM8_d5;
            qM8_d7 <=  qM8_d6;
            qM8_d8 <=  qM8_d7;
            qM8_d9 <=  qM8_d8;
            qM8_d10 <=  qM8_d9;
            qM8_d11 <=  qM8_d10;
            qM8_d12 <=  qM8_d11;
            qM8_d13 <=  qM8_d12;
            qM8_d14 <=  qM8_d13;
            qM8_d15 <=  qM8_d14;
            qM8_d16 <=  qM8_d15;
            qM8_d17 <=  qM8_d16;
            qM8_d18 <=  qM8_d17;
            qM8_d19 <=  qM8_d18;
            qM8_d20 <=  qM8_d19;
            qM8_d21 <=  qM8_d20;
            qM8_d22 <=  qM8_d21;
            qM8_d23 <=  qM8_d22;
            qP7_d1 <=  qP7;
            qP7_d2 <=  qP7_d1;
            qP7_d3 <=  qP7_d2;
            qP7_d4 <=  qP7_d3;
            qP7_d5 <=  qP7_d4;
            qP7_d6 <=  qP7_d5;
            qP7_d7 <=  qP7_d6;
            qP7_d8 <=  qP7_d7;
            qP7_d9 <=  qP7_d8;
            qP7_d10 <=  qP7_d9;
            qP7_d11 <=  qP7_d10;
            qP7_d12 <=  qP7_d11;
            qP7_d13 <=  qP7_d12;
            qP7_d14 <=  qP7_d13;
            qP7_d15 <=  qP7_d14;
            qP7_d16 <=  qP7_d15;
            qP7_d17 <=  qP7_d16;
            qP7_d18 <=  qP7_d17;
            qP7_d19 <=  qP7_d18;
            qP7_d20 <=  qP7_d19;
            qM7_d1 <=  qM7;
            qM7_d2 <=  qM7_d1;
            qM7_d3 <=  qM7_d2;
            qM7_d4 <=  qM7_d3;
            qM7_d5 <=  qM7_d4;
            qM7_d6 <=  qM7_d5;
            qM7_d7 <=  qM7_d6;
            qM7_d8 <=  qM7_d7;
            qM7_d9 <=  qM7_d8;
            qM7_d10 <=  qM7_d9;
            qM7_d11 <=  qM7_d10;
            qM7_d12 <=  qM7_d11;
            qM7_d13 <=  qM7_d12;
            qM7_d14 <=  qM7_d13;
            qM7_d15 <=  qM7_d14;
            qM7_d16 <=  qM7_d15;
            qM7_d17 <=  qM7_d16;
            qM7_d18 <=  qM7_d17;
            qM7_d19 <=  qM7_d18;
            qM7_d20 <=  qM7_d19;
            qP6_d1 <=  qP6;
            qP6_d2 <=  qP6_d1;
            qP6_d3 <=  qP6_d2;
            qP6_d4 <=  qP6_d3;
            qP6_d5 <=  qP6_d4;
            qP6_d6 <=  qP6_d5;
            qP6_d7 <=  qP6_d6;
            qP6_d8 <=  qP6_d7;
            qP6_d9 <=  qP6_d8;
            qP6_d10 <=  qP6_d9;
            qP6_d11 <=  qP6_d10;
            qP6_d12 <=  qP6_d11;
            qP6_d13 <=  qP6_d12;
            qP6_d14 <=  qP6_d13;
            qP6_d15 <=  qP6_d14;
            qP6_d16 <=  qP6_d15;
            qP6_d17 <=  qP6_d16;
            qM6_d1 <=  qM6;
            qM6_d2 <=  qM6_d1;
            qM6_d3 <=  qM6_d2;
            qM6_d4 <=  qM6_d3;
            qM6_d5 <=  qM6_d4;
            qM6_d6 <=  qM6_d5;
            qM6_d7 <=  qM6_d6;
            qM6_d8 <=  qM6_d7;
            qM6_d9 <=  qM6_d8;
            qM6_d10 <=  qM6_d9;
            qM6_d11 <=  qM6_d10;
            qM6_d12 <=  qM6_d11;
            qM6_d13 <=  qM6_d12;
            qM6_d14 <=  qM6_d13;
            qM6_d15 <=  qM6_d14;
            qM6_d16 <=  qM6_d15;
            qM6_d17 <=  qM6_d16;
            qP5_d1 <=  qP5;
            qP5_d2 <=  qP5_d1;
            qP5_d3 <=  qP5_d2;
            qP5_d4 <=  qP5_d3;
            qP5_d5 <=  qP5_d4;
            qP5_d6 <=  qP5_d5;
            qP5_d7 <=  qP5_d6;
            qP5_d8 <=  qP5_d7;
            qP5_d9 <=  qP5_d8;
            qP5_d10 <=  qP5_d9;
            qP5_d11 <=  qP5_d10;
            qP5_d12 <=  qP5_d11;
            qP5_d13 <=  qP5_d12;
            qP5_d14 <=  qP5_d13;
            qM5_d1 <=  qM5;
            qM5_d2 <=  qM5_d1;
            qM5_d3 <=  qM5_d2;
            qM5_d4 <=  qM5_d3;
            qM5_d5 <=  qM5_d4;
            qM5_d6 <=  qM5_d5;
            qM5_d7 <=  qM5_d6;
            qM5_d8 <=  qM5_d7;
            qM5_d9 <=  qM5_d8;
            qM5_d10 <=  qM5_d9;
            qM5_d11 <=  qM5_d10;
            qM5_d12 <=  qM5_d11;
            qM5_d13 <=  qM5_d12;
            qM5_d14 <=  qM5_d13;
            qP4_d1 <=  qP4;
            qP4_d2 <=  qP4_d1;
            qP4_d3 <=  qP4_d2;
            qP4_d4 <=  qP4_d3;
            qP4_d5 <=  qP4_d4;
            qP4_d6 <=  qP4_d5;
            qP4_d7 <=  qP4_d6;
            qP4_d8 <=  qP4_d7;
            qP4_d9 <=  qP4_d8;
            qP4_d10 <=  qP4_d9;
            qP4_d11 <=  qP4_d10;
            qP4_d12 <=  qP4_d11;
            qM4_d1 <=  qM4;
            qM4_d2 <=  qM4_d1;
            qM4_d3 <=  qM4_d2;
            qM4_d4 <=  qM4_d3;
            qM4_d5 <=  qM4_d4;
            qM4_d6 <=  qM4_d5;
            qM4_d7 <=  qM4_d6;
            qM4_d8 <=  qM4_d7;
            qM4_d9 <=  qM4_d8;
            qM4_d10 <=  qM4_d9;
            qM4_d11 <=  qM4_d10;
            qM4_d12 <=  qM4_d11;
            qP3_d1 <=  qP3;
            qP3_d2 <=  qP3_d1;
            qP3_d3 <=  qP3_d2;
            qP3_d4 <=  qP3_d3;
            qP3_d5 <=  qP3_d4;
            qP3_d6 <=  qP3_d5;
            qP3_d7 <=  qP3_d6;
            qP3_d8 <=  qP3_d7;
            qP3_d9 <=  qP3_d8;
            qM3_d1 <=  qM3;
            qM3_d2 <=  qM3_d1;
            qM3_d3 <=  qM3_d2;
            qM3_d4 <=  qM3_d3;
            qM3_d5 <=  qM3_d4;
            qM3_d6 <=  qM3_d5;
            qM3_d7 <=  qM3_d6;
            qM3_d8 <=  qM3_d7;
            qM3_d9 <=  qM3_d8;
            qP2_d1 <=  qP2;
            qP2_d2 <=  qP2_d1;
            qP2_d3 <=  qP2_d2;
            qP2_d4 <=  qP2_d3;
            qP2_d5 <=  qP2_d4;
            qP2_d6 <=  qP2_d5;
            qM2_d1 <=  qM2;
            qM2_d2 <=  qM2_d1;
            qM2_d3 <=  qM2_d2;
            qM2_d4 <=  qM2_d3;
            qM2_d5 <=  qM2_d4;
            qM2_d6 <=  qM2_d5;
            qP1_d1 <=  qP1;
            qP1_d2 <=  qP1_d1;
            qP1_d3 <=  qP1_d2;
            qM1_d1 <=  qM1;
            qM1_d2 <=  qM1_d1;
            qM1_d3 <=  qM1_d2;
            qP_d1 <=  qP;
            qM_d1 <=  qM;
            fR_d1 <=  fR;
            fRn1_d1 <=  fRn1;
            round_d1 <=  round;
         end if;
      end process;
   fX <= "1" & X(22 downto 0);
   fY <= "1" & Y(22 downto 0);
   -- exponent difference, sign and exception combination computed early, to have less bits to pipeline
   expR0 <= ("00" & X(30 downto 23)) - ("00" & Y(30 downto 23));
   sR <= X(31) xor Y(31);
   -- early exception handling 
   exnXY <= X(33 downto 32) & Y(33 downto 32);
   with exnXY  select 
      exnR0 <= 
         "01"	 when "0101",										-- normal
         "00"	 when "0001" | "0010" | "0110", -- zero
         "10"	 when "0100" | "1000" | "1001", -- overflow
         "11"	 when others;										-- NaN
    -- Prescaling
   with fY (22 downto 21)  select 
      prescaledfY <= 
         ("0" & fY & "0") + (fY & "00") when "00",
         ("00" & fY) + (fY & "00") when "01",
         fY &"00" when others;
   with fY (22 downto 21)  select 
      prescaledfX <= 
         ("00" & fX & "0") + ("0" & fX & "00") when "00",
         ("000" & fX) + ("0" & fX & "00") when "01",
         "0" & fX &"00" when others;
   w9 <=  "00" & prescaledfX;
   sel9 <= w9(28 downto 24) & prescaledfY(23 downto 22);
   SelFunctionTable9: selFunction7_4_F400_uid4
      port map ( X => sel9,
                 Y => q9_copy5);
   q9 <= q9_copy5_d1; -- output copy to hold a pipeline register if needed
   w9pad <= w9 & '0';
   with q9_d1(1 downto 0)  select  
   w8fulla <= 
      w9pad_d2 - ("0000" & prescaledfY_d2)			when "01",
      w9pad_d2 + ("0000" & prescaledfY_d2)			when "11",
      w9pad_d2 + ("000" & prescaledfY_d2 & "0")		when "10",
      w9pad_d2							when others;
   with q9(3 downto 1)  select  
   fYdec8 <= 
      ("00" & prescaledfY_d1 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d1 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q9_d2(3)  select 
   w8full <= 
      w8fulla_d1 - fYdec8_d2			when '0',
      w8fulla_d1 + fYdec8_d2			when others;
   w8 <= w8full(26 downto 0) & "00";
   sel8 <= w8(28 downto 24) & prescaledfY_d3(23 downto 22);
   SelFunctionTable8: selFunction7_4_F400_uid4
      port map ( X => sel8,
                 Y => q8_copy6);
   q8 <= q8_copy6_d1; -- output copy to hold a pipeline register if needed
   w8pad <= w8 & '0';
   with q8_d1(1 downto 0)  select  
   w7fulla <= 
      w8pad_d2 - ("0000" & prescaledfY_d5)			when "01",
      w8pad_d2 + ("0000" & prescaledfY_d5)			when "11",
      w8pad_d2 + ("000" & prescaledfY_d5 & "0")		when "10",
      w8pad_d2							when others;
   with q8(3 downto 1)  select  
   fYdec7 <= 
      ("00" & prescaledfY_d4 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d4 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q8_d2(3)  select 
   w7full <= 
      w7fulla_d1 - fYdec7_d2			when '0',
      w7fulla_d1 + fYdec7_d2			when others;
   w7 <= w7full(26 downto 0) & "00";
   sel7 <= w7(28 downto 24) & prescaledfY_d6(23 downto 22);
   SelFunctionTable7: selFunction7_4_F400_uid4
      port map ( X => sel7,
                 Y => q7_copy7);
   q7 <= q7_copy7_d1; -- output copy to hold a pipeline register if needed
   w7pad <= w7 & '0';
   with q7_d1(1 downto 0)  select  
   w6fulla <= 
      w7pad_d2 - ("0000" & prescaledfY_d8)			when "01",
      w7pad_d2 + ("0000" & prescaledfY_d8)			when "11",
      w7pad_d2 + ("000" & prescaledfY_d8 & "0")		when "10",
      w7pad_d2							when others;
   with q7(3 downto 1)  select  
   fYdec6 <= 
      ("00" & prescaledfY_d7 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d7 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q7_d2(3)  select 
   w6full <= 
      w6fulla_d1 - fYdec6_d2			when '0',
      w6fulla_d1 + fYdec6_d2			when others;
   w6 <= w6full(26 downto 0) & "00";
   sel6 <= w6(28 downto 24) & prescaledfY_d9(23 downto 22);
   SelFunctionTable6: selFunction7_4_F400_uid4
      port map ( X => sel6,
                 Y => q6_copy8);
   q6 <= q6_copy8_d1; -- output copy to hold a pipeline register if needed
   w6pad <= w6 & '0';
   with q6_d1(1 downto 0)  select  
   w5fulla <= 
      w6pad_d2 - ("0000" & prescaledfY_d11)			when "01",
      w6pad_d2 + ("0000" & prescaledfY_d11)			when "11",
      w6pad_d2 + ("000" & prescaledfY_d11 & "0")		when "10",
      w6pad_d2							when others;
   with q6(3 downto 1)  select  
   fYdec5 <= 
      ("00" & prescaledfY_d10 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d10 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q6_d2(3)  select 
   w5full <= 
      w5fulla_d1 - fYdec5_d2			when '0',
      w5fulla_d1 + fYdec5_d2			when others;
   w5 <= w5full(26 downto 0) & "00";
   sel5 <= w5(28 downto 24) & prescaledfY_d12(23 downto 22);
   SelFunctionTable5: selFunction7_4_F400_uid4
      port map ( X => sel5,
                 Y => q5_copy9);
   q5 <= q5_copy9_d1; -- output copy to hold a pipeline register if needed
   w5pad <= w5 & '0';
   with q5_d1(1 downto 0)  select  
   w4fulla <= 
      w5pad_d2 - ("0000" & prescaledfY_d14)			when "01",
      w5pad_d2 + ("0000" & prescaledfY_d14)			when "11",
      w5pad_d2 + ("000" & prescaledfY_d14 & "0")		when "10",
      w5pad_d2							when others;
   with q5(3 downto 1)  select  
   fYdec4 <= 
      ("00" & prescaledfY_d13 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d13 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q5_d2(3)  select 
   w4full <= 
      w4fulla_d1 - fYdec4_d2			when '0',
      w4fulla_d1 + fYdec4_d2			when others;
   w4 <= w4full(26 downto 0) & "00";
   sel4 <= w4(28 downto 24) & prescaledfY_d15(23 downto 22);
   SelFunctionTable4: selFunction7_4_F400_uid4
      port map ( X => sel4,
                 Y => q4_copy10);
   q4 <= q4_copy10; -- output copy to hold a pipeline register if needed
   w4pad <= w4 & '0';
   with q4_d2(1 downto 0)  select  
   w3fulla <= 
      w4pad_d2 - ("0000" & prescaledfY_d17)			when "01",
      w4pad_d2 + ("0000" & prescaledfY_d17)			when "11",
      w4pad_d2 + ("000" & prescaledfY_d17 & "0")		when "10",
      w4pad_d2							when others;
   with q4_d1(3 downto 1)  select  
   fYdec3 <= 
      ("00" & prescaledfY_d16 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d16 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q4_d3(3)  select 
   w3full <= 
      w3fulla_d1 - fYdec3_d2			when '0',
      w3fulla_d1 + fYdec3_d2			when others;
   w3 <= w3full(26 downto 0) & "00";
   sel3 <= w3(28 downto 24) & prescaledfY_d18(23 downto 22);
   SelFunctionTable3: selFunction7_4_F400_uid4
      port map ( X => sel3,
                 Y => q3_copy11);
   q3 <= q3_copy11; -- output copy to hold a pipeline register if needed
   w3pad <= w3 & '0';
   with q3_d2(1 downto 0)  select  
   w2fulla <= 
      w3pad_d2 - ("0000" & prescaledfY_d20)			when "01",
      w3pad_d2 + ("0000" & prescaledfY_d20)			when "11",
      w3pad_d2 + ("000" & prescaledfY_d20 & "0")		when "10",
      w3pad_d2							when others;
   with q3_d1(3 downto 1)  select  
   fYdec2 <= 
      ("00" & prescaledfY_d19 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d19 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q3_d3(3)  select 
   w2full <= 
      w2fulla_d1 - fYdec2_d2			when '0',
      w2fulla_d1 + fYdec2_d2			when others;
   w2 <= w2full(26 downto 0) & "00";
   sel2 <= w2(28 downto 24) & prescaledfY_d21(23 downto 22);
   SelFunctionTable2: selFunction7_4_F400_uid4
      port map ( X => sel2,
                 Y => q2_copy12);
   q2 <= q2_copy12; -- output copy to hold a pipeline register if needed
   w2pad <= w2 & '0';
   with q2_d2(1 downto 0)  select  
   w1fulla <= 
      w2pad_d2 - ("0000" & prescaledfY_d23)			when "01",
      w2pad_d2 + ("0000" & prescaledfY_d23)			when "11",
      w2pad_d2 + ("000" & prescaledfY_d23 & "0")		when "10",
      w2pad_d2							when others;
   with q2_d1(3 downto 1)  select  
   fYdec1 <= 
      ("00" & prescaledfY_d22 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d22 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q2_d3(3)  select 
   w1full <= 
      w1fulla_d1 - fYdec1_d2			when '0',
      w1fulla_d1 + fYdec1_d2			when others;
   w1 <= w1full(26 downto 0) & "00";
   sel1 <= w1(28 downto 24) & prescaledfY_d24(23 downto 22);
   SelFunctionTable1: selFunction7_4_F400_uid4
      port map ( X => sel1,
                 Y => q1_copy13);
   q1 <= q1_copy13; -- output copy to hold a pipeline register if needed
   w1pad <= w1 & '0';
   with q1_d1(1 downto 0)  select  
   w0fulla <= 
      w1pad_d1 - ("0000" & prescaledfY_d25)			when "01",
      w1pad_d1 + ("0000" & prescaledfY_d25)			when "11",
      w1pad_d1 + ("000" & prescaledfY_d25 & "0")		when "10",
      w1pad_d1							when others;
   with q1_d1(3 downto 1)  select  
   fYdec0 <= 
      ("00" & prescaledfY_d25 & "00")			when "001" | "010" | "110"| "101",
      ("0" & prescaledfY_d25 & "000")			when "011"| "100",
      (29 downto 0 => '0')when others;
   with q1_d2(3)  select 
   w0full <= 
      w0fulla_d1 - fYdec0_d1			when '0',
      w0fulla_d1 + fYdec0_d1			when others;
   w0 <= w0full(26 downto 0) & "00";
   q0(3 downto 0) <= "0000" when  w0_d1 = (28 downto 0 => '0')
                else w0_d1(28) & "010";
   qP9 <=      q9(2 downto 0);
   qM9 <=      q9(3) & "00";
   qP8 <=      q8(2 downto 0);
   qM8 <=      q8(3) & "00";
   qP7 <=      q7(2 downto 0);
   qM7 <=      q7(3) & "00";
   qP6 <=      q6(2 downto 0);
   qM6 <=      q6(3) & "00";
   qP5 <=      q5(2 downto 0);
   qM5 <=      q5(3) & "00";
   qP4 <=      q4(2 downto 0);
   qM4 <=      q4(3) & "00";
   qP3 <=      q3(2 downto 0);
   qM3 <=      q3(3) & "00";
   qP2 <=      q2(2 downto 0);
   qM2 <=      q2(3) & "00";
   qP1 <=      q1(2 downto 0);
   qM1 <=      q1(3) & "00";
   qP0 <= q0(2 downto 0);
   qM0 <= q0(3)  & "00";
   qP <= qP9_d26 & qP8_d23 & qP7_d20 & qP6_d17 & qP5_d14 & qP4_d12 & qP3_d9 & qP2_d6 & qP1_d3 & qP0;
   qM <= qM9_d26(1 downto 0) & qM8_d23 & qM7_d20 & qM6_d17 & qM5_d14 & qM4_d12 & qM3_d9 & qM2_d6 & qM1_d3 & qM0 & "0";
   fR0 <= qP_d1 - qM_d1;
   fR <= fR0(29 downto 2) & (fR0(0) or fR0(1)); 
   -- normalisation
   with fR(27)  select 
      fRn1 <= fR(27 downto 2) & (fR(0) or fR(1)) when '1',
              fR(26 downto 0)          when others;
   round <= fRn1(2) and (fRn1(0) or fRn1(1) or fRn1(3)); -- fRn1(0) is the sticky bit
   expR1 <= expR0_d29 + ("000" & (6 downto 1 => '1') & fR_d1(27)); -- add back bias
   -- final rounding
   expfrac <= expR1 & fRn1_d1(25 downto 3) ;
   expfracR <= expfrac + ((32 downto 1 => '0') & round_d1);
   exnR <=      "00"  when expfracR(32) = '1'   -- underflow
           else "10"  when  expfracR(32 downto 31) =  "01" -- overflow
           else "01";      -- 00, normal case
   with exnR0_d29  select 
      exnRfinal <= 
         exnR   when "01", -- normal
         exnR0_d29  when others;
   R <= exnRfinal & sR_d29 & expfracR(30 downto 0);
end architecture;

