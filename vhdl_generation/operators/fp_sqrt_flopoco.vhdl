--------------------------------------------------------------------------------
--                                FPSqrt_8_23
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors:
--------------------------------------------------------------------------------
-- Pipeline depth: 22 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X
-- Output signals: R

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library std;
use std.textio.all;
library work;

entity FPSqrt_8_23 is
    port (clk : in std_logic;
          X : in  std_logic_vector(8+23+2 downto 0);
          R : out  std_logic_vector(8+23+2 downto 0)   );
end entity;

architecture arch of FPSqrt_8_23 is
signal fracX :  std_logic_vector(22 downto 0);
signal eRn0 :  std_logic_vector(7 downto 0);
signal xsX :  std_logic_vector(2 downto 0);
signal eRn1, eRn1_d1, eRn1_d2, eRn1_d3, eRn1_d4, eRn1_d5, eRn1_d6, eRn1_d7, eRn1_d8, eRn1_d9, eRn1_d10, eRn1_d11, eRn1_d12, eRn1_d13, eRn1_d14, eRn1_d15, eRn1_d16, eRn1_d17, eRn1_d18, eRn1_d19, eRn1_d20, eRn1_d21, eRn1_d22 :  std_logic_vector(7 downto 0);
signal w26 :  std_logic_vector(26 downto 0);
signal d25 :  std_logic;
signal x25 :  std_logic_vector(27 downto 0);
signal ds25 :  std_logic_vector(3 downto 0);
signal xh25 :  std_logic_vector(3 downto 0);
signal wh25 :  std_logic_vector(3 downto 0);
signal w25 :  std_logic_vector(26 downto 0);
signal s25 :  std_logic_vector(0 downto 0);
signal d24, d24_d1 :  std_logic;
signal x24, x24_d1 :  std_logic_vector(27 downto 0);
signal ds24, ds24_d1 :  std_logic_vector(4 downto 0);
signal xh24, xh24_d1 :  std_logic_vector(4 downto 0);
signal wh24 :  std_logic_vector(4 downto 0);
signal w24 :  std_logic_vector(26 downto 0);
signal s24, s24_d1 :  std_logic_vector(1 downto 0);
signal d23, d23_d1 :  std_logic;
signal x23, x23_d1 :  std_logic_vector(27 downto 0);
signal ds23, ds23_d1 :  std_logic_vector(5 downto 0);
signal xh23, xh23_d1 :  std_logic_vector(5 downto 0);
signal wh23 :  std_logic_vector(5 downto 0);
signal w23 :  std_logic_vector(26 downto 0);
signal s23, s23_d1 :  std_logic_vector(2 downto 0);
signal d22 :  std_logic;
signal x22 :  std_logic_vector(27 downto 0);
signal ds22 :  std_logic_vector(6 downto 0);
signal xh22 :  std_logic_vector(6 downto 0);
signal wh22 :  std_logic_vector(6 downto 0);
signal w22 :  std_logic_vector(26 downto 0);
signal s22 :  std_logic_vector(3 downto 0);
signal d21, d21_d1 :  std_logic;
signal x21, x21_d1 :  std_logic_vector(27 downto 0);
signal ds21, ds21_d1 :  std_logic_vector(7 downto 0);
signal xh21, xh21_d1 :  std_logic_vector(7 downto 0);
signal wh21 :  std_logic_vector(7 downto 0);
signal w21 :  std_logic_vector(26 downto 0);
signal s21, s21_d1 :  std_logic_vector(4 downto 0);
signal d20, d20_d1 :  std_logic;
signal x20, x20_d1 :  std_logic_vector(27 downto 0);
signal ds20, ds20_d1 :  std_logic_vector(8 downto 0);
signal xh20, xh20_d1 :  std_logic_vector(8 downto 0);
signal wh20 :  std_logic_vector(8 downto 0);
signal w20 :  std_logic_vector(26 downto 0);
signal s20, s20_d1 :  std_logic_vector(5 downto 0);
signal d19, d19_d1 :  std_logic;
signal x19, x19_d1 :  std_logic_vector(27 downto 0);
signal ds19, ds19_d1 :  std_logic_vector(9 downto 0);
signal xh19, xh19_d1 :  std_logic_vector(9 downto 0);
signal wh19 :  std_logic_vector(9 downto 0);
signal w19 :  std_logic_vector(26 downto 0);
signal s19, s19_d1 :  std_logic_vector(6 downto 0);
signal d18, d18_d1 :  std_logic;
signal x18, x18_d1 :  std_logic_vector(27 downto 0);
signal ds18, ds18_d1 :  std_logic_vector(10 downto 0);
signal xh18, xh18_d1 :  std_logic_vector(10 downto 0);
signal wh18 :  std_logic_vector(10 downto 0);
signal w18 :  std_logic_vector(26 downto 0);
signal s18, s18_d1 :  std_logic_vector(7 downto 0);
signal d17 :  std_logic;
signal x17 :  std_logic_vector(27 downto 0);
signal ds17 :  std_logic_vector(11 downto 0);
signal xh17 :  std_logic_vector(11 downto 0);
signal wh17 :  std_logic_vector(11 downto 0);
signal w17 :  std_logic_vector(26 downto 0);
signal s17 :  std_logic_vector(8 downto 0);
signal d16, d16_d1 :  std_logic;
signal x16, x16_d1 :  std_logic_vector(27 downto 0);
signal ds16, ds16_d1 :  std_logic_vector(12 downto 0);
signal xh16, xh16_d1 :  std_logic_vector(12 downto 0);
signal wh16 :  std_logic_vector(12 downto 0);
signal w16 :  std_logic_vector(26 downto 0);
signal s16, s16_d1 :  std_logic_vector(9 downto 0);
signal d15, d15_d1 :  std_logic;
signal x15, x15_d1 :  std_logic_vector(27 downto 0);
signal ds15, ds15_d1 :  std_logic_vector(13 downto 0);
signal xh15, xh15_d1 :  std_logic_vector(13 downto 0);
signal wh15 :  std_logic_vector(13 downto 0);
signal w15 :  std_logic_vector(26 downto 0);
signal s15, s15_d1 :  std_logic_vector(10 downto 0);
signal d14, d14_d1 :  std_logic;
signal x14, x14_d1 :  std_logic_vector(27 downto 0);
signal ds14, ds14_d1 :  std_logic_vector(14 downto 0);
signal xh14, xh14_d1 :  std_logic_vector(14 downto 0);
signal wh14 :  std_logic_vector(14 downto 0);
signal w14 :  std_logic_vector(26 downto 0);
signal s14, s14_d1 :  std_logic_vector(11 downto 0);
signal d13, d13_d1 :  std_logic;
signal x13, x13_d1 :  std_logic_vector(27 downto 0);
signal ds13, ds13_d1 :  std_logic_vector(15 downto 0);
signal xh13, xh13_d1 :  std_logic_vector(15 downto 0);
signal wh13 :  std_logic_vector(15 downto 0);
signal w13 :  std_logic_vector(26 downto 0);
signal s13, s13_d1 :  std_logic_vector(12 downto 0);
signal d12, d12_d1 :  std_logic;
signal x12, x12_d1 :  std_logic_vector(27 downto 0);
signal ds12, ds12_d1 :  std_logic_vector(16 downto 0);
signal xh12, xh12_d1 :  std_logic_vector(16 downto 0);
signal wh12 :  std_logic_vector(16 downto 0);
signal w12 :  std_logic_vector(26 downto 0);
signal s12, s12_d1 :  std_logic_vector(13 downto 0);
signal d11 :  std_logic;
signal x11 :  std_logic_vector(27 downto 0);
signal ds11 :  std_logic_vector(17 downto 0);
signal xh11 :  std_logic_vector(17 downto 0);
signal wh11 :  std_logic_vector(17 downto 0);
signal w11 :  std_logic_vector(26 downto 0);
signal s11 :  std_logic_vector(14 downto 0);
signal d10, d10_d1 :  std_logic;
signal x10, x10_d1 :  std_logic_vector(27 downto 0);
signal ds10, ds10_d1 :  std_logic_vector(18 downto 0);
signal xh10, xh10_d1 :  std_logic_vector(18 downto 0);
signal wh10 :  std_logic_vector(18 downto 0);
signal w10 :  std_logic_vector(26 downto 0);
signal s10, s10_d1 :  std_logic_vector(15 downto 0);
signal d9, d9_d1 :  std_logic;
signal x9, x9_d1 :  std_logic_vector(27 downto 0);
signal ds9, ds9_d1 :  std_logic_vector(19 downto 0);
signal xh9, xh9_d1 :  std_logic_vector(19 downto 0);
signal wh9 :  std_logic_vector(19 downto 0);
signal w9 :  std_logic_vector(26 downto 0);
signal s9, s9_d1 :  std_logic_vector(16 downto 0);
signal d8, d8_d1 :  std_logic;
signal x8, x8_d1 :  std_logic_vector(27 downto 0);
signal ds8, ds8_d1 :  std_logic_vector(20 downto 0);
signal xh8, xh8_d1 :  std_logic_vector(20 downto 0);
signal wh8 :  std_logic_vector(20 downto 0);
signal w8 :  std_logic_vector(26 downto 0);
signal s8, s8_d1 :  std_logic_vector(17 downto 0);
signal d7, d7_d1 :  std_logic;
signal x7, x7_d1 :  std_logic_vector(27 downto 0);
signal ds7, ds7_d1 :  std_logic_vector(21 downto 0);
signal xh7, xh7_d1 :  std_logic_vector(21 downto 0);
signal wh7 :  std_logic_vector(21 downto 0);
signal w7 :  std_logic_vector(26 downto 0);
signal s7, s7_d1 :  std_logic_vector(18 downto 0);
signal d6, d6_d1 :  std_logic;
signal x6, x6_d1 :  std_logic_vector(27 downto 0);
signal ds6, ds6_d1 :  std_logic_vector(22 downto 0);
signal xh6, xh6_d1 :  std_logic_vector(22 downto 0);
signal wh6 :  std_logic_vector(22 downto 0);
signal w6 :  std_logic_vector(26 downto 0);
signal s6, s6_d1 :  std_logic_vector(19 downto 0);
signal d5, d5_d1 :  std_logic;
signal x5, x5_d1 :  std_logic_vector(27 downto 0);
signal ds5, ds5_d1 :  std_logic_vector(23 downto 0);
signal xh5, xh5_d1 :  std_logic_vector(23 downto 0);
signal wh5 :  std_logic_vector(23 downto 0);
signal w5 :  std_logic_vector(26 downto 0);
signal s5, s5_d1 :  std_logic_vector(20 downto 0);
signal d4, d4_d1 :  std_logic;
signal x4, x4_d1 :  std_logic_vector(27 downto 0);
signal ds4, ds4_d1 :  std_logic_vector(24 downto 0);
signal xh4, xh4_d1 :  std_logic_vector(24 downto 0);
signal wh4 :  std_logic_vector(24 downto 0);
signal w4 :  std_logic_vector(26 downto 0);
signal s4, s4_d1 :  std_logic_vector(21 downto 0);
signal d3, d3_d1 :  std_logic;
signal x3, x3_d1 :  std_logic_vector(27 downto 0);
signal ds3, ds3_d1 :  std_logic_vector(25 downto 0);
signal xh3, xh3_d1 :  std_logic_vector(25 downto 0);
signal wh3 :  std_logic_vector(25 downto 0);
signal w3 :  std_logic_vector(26 downto 0);
signal s3, s3_d1 :  std_logic_vector(22 downto 0);
signal d2, d2_d1 :  std_logic;
signal x2, x2_d1 :  std_logic_vector(27 downto 0);
signal ds2, ds2_d1 :  std_logic_vector(26 downto 0);
signal xh2, xh2_d1 :  std_logic_vector(26 downto 0);
signal wh2 :  std_logic_vector(26 downto 0);
signal w2 :  std_logic_vector(26 downto 0);
signal s2, s2_d1 :  std_logic_vector(23 downto 0);
signal d1, d1_d1 :  std_logic;
signal x1 :  std_logic_vector(27 downto 0);
signal ds1, ds1_d1 :  std_logic_vector(27 downto 0);
signal xh1, xh1_d1 :  std_logic_vector(27 downto 0);
signal wh1 :  std_logic_vector(27 downto 0);
signal w1 :  std_logic_vector(26 downto 0);
signal s1, s1_d1 :  std_logic_vector(24 downto 0);
signal d0 :  std_logic;
signal fR :  std_logic_vector(26 downto 0);
signal fRn1, fRn1_d1 :  std_logic_vector(24 downto 0);
signal round, round_d1 :  std_logic;
signal fRn2 :  std_logic_vector(22 downto 0);
signal Rn2 :  std_logic_vector(30 downto 0);
signal xsR, xsR_d1, xsR_d2, xsR_d3, xsR_d4, xsR_d5, xsR_d6, xsR_d7, xsR_d8, xsR_d9, xsR_d10, xsR_d11, xsR_d12, xsR_d13, xsR_d14, xsR_d15, xsR_d16, xsR_d17, xsR_d18, xsR_d19, xsR_d20, xsR_d21, xsR_d22 :  std_logic_vector(2 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            eRn1_d1 <=  eRn1;
            eRn1_d2 <=  eRn1_d1;
            eRn1_d3 <=  eRn1_d2;
            eRn1_d4 <=  eRn1_d3;
            eRn1_d5 <=  eRn1_d4;
            eRn1_d6 <=  eRn1_d5;
            eRn1_d7 <=  eRn1_d6;
            eRn1_d8 <=  eRn1_d7;
            eRn1_d9 <=  eRn1_d8;
            eRn1_d10 <=  eRn1_d9;
            eRn1_d11 <=  eRn1_d10;
            eRn1_d12 <=  eRn1_d11;
            eRn1_d13 <=  eRn1_d12;
            eRn1_d14 <=  eRn1_d13;
            eRn1_d15 <=  eRn1_d14;
            eRn1_d16 <=  eRn1_d15;
            eRn1_d17 <=  eRn1_d16;
            eRn1_d18 <=  eRn1_d17;
            eRn1_d19 <=  eRn1_d18;
            eRn1_d20 <=  eRn1_d19;
            eRn1_d21 <=  eRn1_d20;
            eRn1_d22 <=  eRn1_d21;
            d24_d1 <=  d24;
            x24_d1 <=  x24;
            ds24_d1 <=  ds24;
            xh24_d1 <=  xh24;
            s24_d1 <=  s24;
            d23_d1 <=  d23;
            x23_d1 <=  x23;
            ds23_d1 <=  ds23;
            xh23_d1 <=  xh23;
            s23_d1 <=  s23;
            d21_d1 <=  d21;
            x21_d1 <=  x21;
            ds21_d1 <=  ds21;
            xh21_d1 <=  xh21;
            s21_d1 <=  s21;
            d20_d1 <=  d20;
            x20_d1 <=  x20;
            ds20_d1 <=  ds20;
            xh20_d1 <=  xh20;
            s20_d1 <=  s20;
            d19_d1 <=  d19;
            x19_d1 <=  x19;
            ds19_d1 <=  ds19;
            xh19_d1 <=  xh19;
            s19_d1 <=  s19;
            d18_d1 <=  d18;
            x18_d1 <=  x18;
            ds18_d1 <=  ds18;
            xh18_d1 <=  xh18;
            s18_d1 <=  s18;
            d16_d1 <=  d16;
            x16_d1 <=  x16;
            ds16_d1 <=  ds16;
            xh16_d1 <=  xh16;
            s16_d1 <=  s16;
            d15_d1 <=  d15;
            x15_d1 <=  x15;
            ds15_d1 <=  ds15;
            xh15_d1 <=  xh15;
            s15_d1 <=  s15;
            d14_d1 <=  d14;
            x14_d1 <=  x14;
            ds14_d1 <=  ds14;
            xh14_d1 <=  xh14;
            s14_d1 <=  s14;
            d13_d1 <=  d13;
            x13_d1 <=  x13;
            ds13_d1 <=  ds13;
            xh13_d1 <=  xh13;
            s13_d1 <=  s13;
            d12_d1 <=  d12;
            x12_d1 <=  x12;
            ds12_d1 <=  ds12;
            xh12_d1 <=  xh12;
            s12_d1 <=  s12;
            d10_d1 <=  d10;
            x10_d1 <=  x10;
            ds10_d1 <=  ds10;
            xh10_d1 <=  xh10;
            s10_d1 <=  s10;
            d9_d1 <=  d9;
            x9_d1 <=  x9;
            ds9_d1 <=  ds9;
            xh9_d1 <=  xh9;
            s9_d1 <=  s9;
            d8_d1 <=  d8;
            x8_d1 <=  x8;
            ds8_d1 <=  ds8;
            xh8_d1 <=  xh8;
            s8_d1 <=  s8;
            d7_d1 <=  d7;
            x7_d1 <=  x7;
            ds7_d1 <=  ds7;
            xh7_d1 <=  xh7;
            s7_d1 <=  s7;
            d6_d1 <=  d6;
            x6_d1 <=  x6;
            ds6_d1 <=  ds6;
            xh6_d1 <=  xh6;
            s6_d1 <=  s6;
            d5_d1 <=  d5;
            x5_d1 <=  x5;
            ds5_d1 <=  ds5;
            xh5_d1 <=  xh5;
            s5_d1 <=  s5;
            d4_d1 <=  d4;
            x4_d1 <=  x4;
            ds4_d1 <=  ds4;
            xh4_d1 <=  xh4;
            s4_d1 <=  s4;
            d3_d1 <=  d3;
            x3_d1 <=  x3;
            ds3_d1 <=  ds3;
            xh3_d1 <=  xh3;
            s3_d1 <=  s3;
            d2_d1 <=  d2;
            x2_d1 <=  x2;
            ds2_d1 <=  ds2;
            xh2_d1 <=  xh2;
            s2_d1 <=  s2;
            d1_d1 <=  d1;
            ds1_d1 <=  ds1;
            xh1_d1 <=  xh1;
            s1_d1 <=  s1;
            fRn1_d1 <=  fRn1;
            round_d1 <=  round;
            xsR_d1 <=  xsR;
            xsR_d2 <=  xsR_d1;
            xsR_d3 <=  xsR_d2;
            xsR_d4 <=  xsR_d3;
            xsR_d5 <=  xsR_d4;
            xsR_d6 <=  xsR_d5;
            xsR_d7 <=  xsR_d6;
            xsR_d8 <=  xsR_d7;
            xsR_d9 <=  xsR_d8;
            xsR_d10 <=  xsR_d9;
            xsR_d11 <=  xsR_d10;
            xsR_d12 <=  xsR_d11;
            xsR_d13 <=  xsR_d12;
            xsR_d14 <=  xsR_d13;
            xsR_d15 <=  xsR_d14;
            xsR_d16 <=  xsR_d15;
            xsR_d17 <=  xsR_d16;
            xsR_d18 <=  xsR_d17;
            xsR_d19 <=  xsR_d18;
            xsR_d20 <=  xsR_d19;
            xsR_d21 <=  xsR_d20;
            xsR_d22 <=  xsR_d21;
         end if;
      end process;
   fracX <= X(22 downto 0); -- fraction
   eRn0 <= "0" & X(30 downto 24); -- exponent
   xsX <= X(33 downto 31); -- exception and sign
   eRn1 <= eRn0 + ("00" & (5 downto 0 => '1')) + X(23);
   w26 <= "111" & fracX & "0" when X(23) = '0' else
          "1101" & fracX;
   -- Step 25
   d25 <= w26(26);
   x25 <= w26 & "0";
   ds25 <=  "0" &  (not d25) & d25 & "1";
   xh25 <= x25(27 downto 24);
   with d25  select
      wh25 <= xh25 - ds25 when '0',
            xh25 + ds25 when others;
   w25 <= wh25(2 downto 0) & x25(23 downto 0);
   s25 <= "" & (not d25) ;
   -- Step 24
   d24 <= w25(26);
   x24 <= w25 & "0";
   ds24 <=  "0" & s25 &  (not d24) & d24 & "1";
   xh24 <= x24(27 downto 23);
   with d24_d1  select
      wh24 <= xh24_d1 - ds24_d1 when '0',
            xh24_d1 + ds24_d1 when others;
   w24 <= wh24(3 downto 0) & x24_d1(22 downto 0);
   s24 <= s25 & not d24;
   -- Step 23
   d23 <= w24(26);
   x23 <= w24 & "0";
   ds23 <=  "0" & s24_d1 &  (not d23) & d23 & "1";
   xh23 <= x23(27 downto 22);
   with d23_d1  select
      wh23 <= xh23_d1 - ds23_d1 when '0',
            xh23_d1 + ds23_d1 when others;
   w23 <= wh23(4 downto 0) & x23_d1(21 downto 0);
   s23 <= s24_d1 & not d23;
   -- Step 22
   d22 <= w23(26);
   x22 <= w23 & "0";
   ds22 <=  "0" & s23_d1 &  (not d22) & d22 & "1";
   xh22 <= x22(27 downto 21);
   with d22  select
      wh22 <= xh22 - ds22 when '0',
            xh22 + ds22 when others;
   w22 <= wh22(5 downto 0) & x22(20 downto 0);
   s22 <= s23_d1 & not d22;
   -- Step 21
   d21 <= w22(26);
   x21 <= w22 & "0";
   ds21 <=  "0" & s22 &  (not d21) & d21 & "1";
   xh21 <= x21(27 downto 20);
   with d21_d1  select
      wh21 <= xh21_d1 - ds21_d1 when '0',
            xh21_d1 + ds21_d1 when others;
   w21 <= wh21(6 downto 0) & x21_d1(19 downto 0);
   s21 <= s22 & not d21;
   -- Step 20
   d20 <= w21(26);
   x20 <= w21 & "0";
   ds20 <=  "0" & s21_d1 &  (not d20) & d20 & "1";
   xh20 <= x20(27 downto 19);
   with d20_d1  select
      wh20 <= xh20_d1 - ds20_d1 when '0',
            xh20_d1 + ds20_d1 when others;
   w20 <= wh20(7 downto 0) & x20_d1(18 downto 0);
   s20 <= s21_d1 & not d20;
   -- Step 19
   d19 <= w20(26);
   x19 <= w20 & "0";
   ds19 <=  "0" & s20_d1 &  (not d19) & d19 & "1";
   xh19 <= x19(27 downto 18);
   with d19_d1  select
      wh19 <= xh19_d1 - ds19_d1 when '0',
            xh19_d1 + ds19_d1 when others;
   w19 <= wh19(8 downto 0) & x19_d1(17 downto 0);
   s19 <= s20_d1 & not d19;
   -- Step 18
   d18 <= w19(26);
   x18 <= w19 & "0";
   ds18 <=  "0" & s19_d1 &  (not d18) & d18 & "1";
   xh18 <= x18(27 downto 17);
   with d18_d1  select
      wh18 <= xh18_d1 - ds18_d1 when '0',
            xh18_d1 + ds18_d1 when others;
   w18 <= wh18(9 downto 0) & x18_d1(16 downto 0);
   s18 <= s19_d1 & not d18;
   -- Step 17
   d17 <= w18(26);
   x17 <= w18 & "0";
   ds17 <=  "0" & s18_d1 &  (not d17) & d17 & "1";
   xh17 <= x17(27 downto 16);
   with d17  select
      wh17 <= xh17 - ds17 when '0',
            xh17 + ds17 when others;
   w17 <= wh17(10 downto 0) & x17(15 downto 0);
   s17 <= s18_d1 & not d17;
   -- Step 16
   d16 <= w17(26);
   x16 <= w17 & "0";
   ds16 <=  "0" & s17 &  (not d16) & d16 & "1";
   xh16 <= x16(27 downto 15);
   with d16_d1  select
      wh16 <= xh16_d1 - ds16_d1 when '0',
            xh16_d1 + ds16_d1 when others;
   w16 <= wh16(11 downto 0) & x16_d1(14 downto 0);
   s16 <= s17 & not d16;
   -- Step 15
   d15 <= w16(26);
   x15 <= w16 & "0";
   ds15 <=  "0" & s16_d1 &  (not d15) & d15 & "1";
   xh15 <= x15(27 downto 14);
   with d15_d1  select
      wh15 <= xh15_d1 - ds15_d1 when '0',
            xh15_d1 + ds15_d1 when others;
   w15 <= wh15(12 downto 0) & x15_d1(13 downto 0);
   s15 <= s16_d1 & not d15;
   -- Step 14
   d14 <= w15(26);
   x14 <= w15 & "0";
   ds14 <=  "0" & s15_d1 &  (not d14) & d14 & "1";
   xh14 <= x14(27 downto 13);
   with d14_d1  select
      wh14 <= xh14_d1 - ds14_d1 when '0',
            xh14_d1 + ds14_d1 when others;
   w14 <= wh14(13 downto 0) & x14_d1(12 downto 0);
   s14 <= s15_d1 & not d14;
   -- Step 13
   d13 <= w14(26);
   x13 <= w14 & "0";
   ds13 <=  "0" & s14_d1 &  (not d13) & d13 & "1";
   xh13 <= x13(27 downto 12);
   with d13_d1  select
      wh13 <= xh13_d1 - ds13_d1 when '0',
            xh13_d1 + ds13_d1 when others;
   w13 <= wh13(14 downto 0) & x13_d1(11 downto 0);
   s13 <= s14_d1 & not d13;
   -- Step 12
   d12 <= w13(26);
   x12 <= w13 & "0";
   ds12 <=  "0" & s13_d1 &  (not d12) & d12 & "1";
   xh12 <= x12(27 downto 11);
   with d12_d1  select
      wh12 <= xh12_d1 - ds12_d1 when '0',
            xh12_d1 + ds12_d1 when others;
   w12 <= wh12(15 downto 0) & x12_d1(10 downto 0);
   s12 <= s13_d1 & not d12;
   -- Step 11
   d11 <= w12(26);
   x11 <= w12 & "0";
   ds11 <=  "0" & s12_d1 &  (not d11) & d11 & "1";
   xh11 <= x11(27 downto 10);
   with d11  select
      wh11 <= xh11 - ds11 when '0',
            xh11 + ds11 when others;
   w11 <= wh11(16 downto 0) & x11(9 downto 0);
   s11 <= s12_d1 & not d11;
   -- Step 10
   d10 <= w11(26);
   x10 <= w11 & "0";
   ds10 <=  "0" & s11 &  (not d10) & d10 & "1";
   xh10 <= x10(27 downto 9);
   with d10_d1  select
      wh10 <= xh10_d1 - ds10_d1 when '0',
            xh10_d1 + ds10_d1 when others;
   w10 <= wh10(17 downto 0) & x10_d1(8 downto 0);
   s10 <= s11 & not d10;
   -- Step 9
   d9 <= w10(26);
   x9 <= w10 & "0";
   ds9 <=  "0" & s10_d1 &  (not d9) & d9 & "1";
   xh9 <= x9(27 downto 8);
   with d9_d1  select
      wh9 <= xh9_d1 - ds9_d1 when '0',
            xh9_d1 + ds9_d1 when others;
   w9 <= wh9(18 downto 0) & x9_d1(7 downto 0);
   s9 <= s10_d1 & not d9;
   -- Step 8
   d8 <= w9(26);
   x8 <= w9 & "0";
   ds8 <=  "0" & s9_d1 &  (not d8) & d8 & "1";
   xh8 <= x8(27 downto 7);
   with d8_d1  select
      wh8 <= xh8_d1 - ds8_d1 when '0',
            xh8_d1 + ds8_d1 when others;
   w8 <= wh8(19 downto 0) & x8_d1(6 downto 0);
   s8 <= s9_d1 & not d8;
   -- Step 7
   d7 <= w8(26);
   x7 <= w8 & "0";
   ds7 <=  "0" & s8_d1 &  (not d7) & d7 & "1";
   xh7 <= x7(27 downto 6);
   with d7_d1  select
      wh7 <= xh7_d1 - ds7_d1 when '0',
            xh7_d1 + ds7_d1 when others;
   w7 <= wh7(20 downto 0) & x7_d1(5 downto 0);
   s7 <= s8_d1 & not d7;
   -- Step 6
   d6 <= w7(26);
   x6 <= w7 & "0";
   ds6 <=  "0" & s7_d1 &  (not d6) & d6 & "1";
   xh6 <= x6(27 downto 5);
   with d6_d1  select
      wh6 <= xh6_d1 - ds6_d1 when '0',
            xh6_d1 + ds6_d1 when others;
   w6 <= wh6(21 downto 0) & x6_d1(4 downto 0);
   s6 <= s7_d1 & not d6;
   -- Step 5
   d5 <= w6(26);
   x5 <= w6 & "0";
   ds5 <=  "0" & s6_d1 &  (not d5) & d5 & "1";
   xh5 <= x5(27 downto 4);
   with d5_d1  select
      wh5 <= xh5_d1 - ds5_d1 when '0',
            xh5_d1 + ds5_d1 when others;
   w5 <= wh5(22 downto 0) & x5_d1(3 downto 0);
   s5 <= s6_d1 & not d5;
   -- Step 4
   d4 <= w5(26);
   x4 <= w5 & "0";
   ds4 <=  "0" & s5_d1 &  (not d4) & d4 & "1";
   xh4 <= x4(27 downto 3);
   with d4_d1  select
      wh4 <= xh4_d1 - ds4_d1 when '0',
            xh4_d1 + ds4_d1 when others;
   w4 <= wh4(23 downto 0) & x4_d1(2 downto 0);
   s4 <= s5_d1 & not d4;
   -- Step 3
   d3 <= w4(26);
   x3 <= w4 & "0";
   ds3 <=  "0" & s4_d1 &  (not d3) & d3 & "1";
   xh3 <= x3(27 downto 2);
   with d3_d1  select
      wh3 <= xh3_d1 - ds3_d1 when '0',
            xh3_d1 + ds3_d1 when others;
   w3 <= wh3(24 downto 0) & x3_d1(1 downto 0);
   s3 <= s4_d1 & not d3;
   -- Step 2
   d2 <= w3(26);
   x2 <= w3 & "0";
   ds2 <=  "0" & s3_d1 &  (not d2) & d2 & "1";
   xh2 <= x2(27 downto 1);
   with d2_d1  select
      wh2 <= xh2_d1 - ds2_d1 when '0',
            xh2_d1 + ds2_d1 when others;
   w2 <= wh2(25 downto 0) & x2_d1(0 downto 0);
   s2 <= s3_d1 & not d2;
   -- Step 1
   d1 <= w2(26);
   x1 <= w2 & "0";
   ds1 <=  "0" & s2_d1 &  (not d1) & d1 & "1";
   xh1 <= x1(27 downto 0);
   with d1_d1  select
      wh1 <= xh1_d1 - ds1_d1 when '0',
            xh1_d1 + ds1_d1 when others;
   w1 <= wh1(26 downto 0);
   s1 <= s2_d1 & not d1;
   d0 <= w1(26) ;
   fR <= s1_d1 & not d0 & '1';
   -- normalisation of the result, removing leading 1
   with fR(26)  select
      fRn1 <= fR(25 downto 2) & (fR(1) or fR(0)) when '1',
              fR(24 downto 0)                    when others;
   round <= fRn1(1) and (fRn1(2) or fRn1(0)) ; -- round  and (lsb or sticky) : that's RN, tie to even
   fRn2 <= fRn1_d1(24 downto 2) + ((22 downto 1 => '0') & round_d1); -- rounding sqrt never changes exponents
   Rn2 <= eRn1_d22 & fRn2;
   -- sign and exception processing
   with xsX  select
      xsR <= "010"  when "010",  -- normal case
             "100"  when "100",  -- +infty
             "000"  when "000",  -- +0
             "001"  when "001",  -- the infamous sqrt(-0)=-0
             "110"  when others; -- return NaN
   R <= xsR_d22 & Rn2;
end architecture;
