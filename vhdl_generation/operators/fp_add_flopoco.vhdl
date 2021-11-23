--------------------------------------------------------------------------------
--                           IntAdder_34_F400_uid4
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors: Bogdan Pasca, Florent de Dinechin (2008-2016)
--------------------------------------------------------------------------------
-- Pipeline depth: 1 cycles
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

entity IntAdder_34_F400_uid4 is
    port (clk : in std_logic;
          X : in  std_logic_vector(33 downto 0);
          Y : in  std_logic_vector(33 downto 0);
          Cin : in  std_logic;
          R : out  std_logic_vector(33 downto 0)   );
end entity;

architecture arch of IntAdder_34_F400_uid4 is
signal Cin_0, Cin_0_d1 :  std_logic;
signal X_0, X_0_d1 :  std_logic_vector(31 downto 0);
signal Y_0, Y_0_d1 :  std_logic_vector(31 downto 0);
signal S_0 :  std_logic_vector(31 downto 0);
signal R_0 :  std_logic_vector(30 downto 0);
signal Cin_1 :  std_logic;
signal X_1, X_1_d1 :  std_logic_vector(3 downto 0);
signal Y_1, Y_1_d1 :  std_logic_vector(3 downto 0);
signal S_1 :  std_logic_vector(3 downto 0);
signal R_1 :  std_logic_vector(2 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            Cin_0_d1 <=  Cin_0;
            X_0_d1 <=  X_0;
            Y_0_d1 <=  Y_0;
            X_1_d1 <=  X_1;
            Y_1_d1 <=  Y_1;
         end if;
      end process;
   Cin_0 <= Cin;
   X_0 <= '0' & X(30 downto 0);
   Y_0 <= '0' & Y(30 downto 0);
   S_0 <= X_0_d1 + Y_0_d1 + Cin_0_d1;
   R_0 <= S_0(30 downto 0);
   Cin_1 <= S_0(31);
   X_1 <= '0' & X(33 downto 31);
   Y_1 <= '0' & Y(33 downto 31);
   S_1 <= X_1_d1 + Y_1_d1 + Cin_1;
   R_1 <= S_1(2 downto 0);
   R <= R_1 & R_0 ;
end architecture;

--------------------------------------------------------------------------------
--                  RightShifterSticky24_by_max_26_F400_uid6
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors: Bogdan Pasca (2008-2011), Florent de Dinechin (2008-2019)
--------------------------------------------------------------------------------
-- Pipeline depth: 3 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: X S
-- Output signals: R Sticky

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library std;
use std.textio.all;
library work;

entity RightShifterSticky24_by_max_26_F400_uid6 is
    port (clk : in std_logic;
          X : in  std_logic_vector(23 downto 0);
          S : in  std_logic_vector(4 downto 0);
          R : out  std_logic_vector(25 downto 0);
          Sticky : out  std_logic   );
end entity;

architecture arch of RightShifterSticky24_by_max_26_F400_uid6 is
signal ps, ps_d1, ps_d2, ps_d3 :  std_logic_vector(4 downto 0);
signal Xpadded :  std_logic_vector(25 downto 0);
signal level5, level5_d1 :  std_logic_vector(25 downto 0);
signal stk4, stk4_d1 :  std_logic;
signal level4, level4_d1 :  std_logic_vector(25 downto 0);
signal stk3, stk3_d1 :  std_logic;
signal level3, level3_d1, level3_d2 :  std_logic_vector(25 downto 0);
signal stk2 :  std_logic;
signal level2, level2_d1, level2_d2 :  std_logic_vector(25 downto 0);
signal stk1, stk1_d1 :  std_logic;
signal level1, level1_d1, level1_d2, level1_d3 :  std_logic_vector(25 downto 0);
signal stk0 :  std_logic;
signal level0 :  std_logic_vector(25 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            ps_d1 <=  ps;
            ps_d2 <=  ps_d1;
            ps_d3 <=  ps_d2;
            level5_d1 <=  level5;
            stk4_d1 <=  stk4;
            level4_d1 <=  level4;
            stk3_d1 <=  stk3;
            level3_d1 <=  level3;
            level3_d2 <=  level3_d1;
            level2_d1 <=  level2;
            level2_d2 <=  level2_d1;
            stk1_d1 <=  stk1;
            level1_d1 <=  level1;
            level1_d2 <=  level1_d1;
            level1_d3 <=  level1_d2;
         end if;
      end process;
   ps<= S;
   Xpadded <= X&(1 downto 0 => '0');
   level5<= Xpadded;
   stk4 <= '1' when (level5_d1(15 downto 0)/="0000000000000000" and ps(4)='1')   else '0';
   level4 <=  level5_d1 when  ps(4)='0'    else (15 downto 0 => '0') & level5_d1(25 downto 16);
   stk3 <= '1' when (level4_d1(7 downto 0)/="00000000" and ps_d1(3)='1') or stk4_d1 ='1'   else '0';
   level3 <=  level4 when  ps(3)='0'    else (7 downto 0 => '0') & level4(25 downto 8);
   stk2 <= '1' when (level3_d2(3 downto 0)/="0000" and ps_d2(2)='1') or stk3_d1 ='1'   else '0';
   level2 <=  level3 when  ps(2)='0'    else (3 downto 0 => '0') & level3(25 downto 4);
   stk1 <= '1' when (level2_d2(1 downto 0)/="00" and ps_d2(1)='1') or stk2 ='1'   else '0';
   level1 <=  level2 when  ps(1)='0'    else (1 downto 0 => '0') & level2(25 downto 2);
   stk0 <= '1' when (level1_d3(0 downto 0)/="0" and ps_d3(0)='1') or stk1_d1 ='1'   else '0';
   level0 <=  level1 when  ps(0)='0'    else (0 downto 0 => '0') & level1(25 downto 1);
   R <= level0;
   Sticky <= stk0;
end architecture;

--------------------------------------------------------------------------------
--                           IntAdder_27_F400_uid8
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors: Bogdan Pasca, Florent de Dinechin (2008-2016)
--------------------------------------------------------------------------------
-- Pipeline depth: 1 cycles
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

entity IntAdder_27_F400_uid8 is
    port (clk : in std_logic;
          X : in  std_logic_vector(26 downto 0);
          Y : in  std_logic_vector(26 downto 0);
          Cin : in  std_logic;
          R : out  std_logic_vector(26 downto 0)   );
end entity;

architecture arch of IntAdder_27_F400_uid8 is
signal Cin_1, Cin_1_d1 :  std_logic;
signal X_1, X_1_d1, X_1_d2, X_1_d3, X_1_d4, X_1_d5 :  std_logic_vector(27 downto 0);
signal Y_1, Y_1_d1, Y_1_d2, Y_1_d3 :  std_logic_vector(27 downto 0);
signal S_1 :  std_logic_vector(27 downto 0);
signal R_1 :  std_logic_vector(26 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            Cin_1_d1 <=  Cin_1;
            X_1_d1 <=  X_1;
            X_1_d2 <=  X_1_d1;
            X_1_d3 <=  X_1_d2;
            X_1_d4 <=  X_1_d3;
            X_1_d5 <=  X_1_d4;
            Y_1_d1 <=  Y_1;
            Y_1_d2 <=  Y_1_d1;
            Y_1_d3 <=  Y_1_d2;
         end if;
      end process;
   Cin_1 <= Cin;
   X_1 <= '0' & X(26 downto 0);
   Y_1 <= '0' & Y(26 downto 0);
   S_1 <= X_1_d5 + Y_1_d3 + Cin_1_d1;
   R_1 <= S_1(26 downto 0);
   R <= R_1 ;
end architecture;

--------------------------------------------------------------------------------
--                 LZCShifter_28_to_28_counting_32_F400_uid10
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors: Florent de Dinechin, Bogdan Pasca (2007-2016)
--------------------------------------------------------------------------------
-- Pipeline depth: 4 cycles
-- Clock period (ns): 2.5
-- Target frequency (MHz): 400
-- Input signals: I
-- Output signals: Count O

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library std;
use std.textio.all;
library work;

entity LZCShifter_28_to_28_counting_32_F400_uid10 is
    port (clk : in std_logic;
          I : in  std_logic_vector(27 downto 0);
          Count : out  std_logic_vector(4 downto 0);
          O : out  std_logic_vector(27 downto 0)   );
end entity;

architecture arch of LZCShifter_28_to_28_counting_32_F400_uid10 is
signal level5, level5_d1 :  std_logic_vector(27 downto 0);
signal count4, count4_d1, count4_d2, count4_d3 :  std_logic;
signal level4, level4_d1 :  std_logic_vector(27 downto 0);
signal count3, count3_d1, count3_d2, count3_d3 :  std_logic;
signal level3, level3_d1 :  std_logic_vector(27 downto 0);
signal count2, count2_d1, count2_d2 :  std_logic;
signal level2 :  std_logic_vector(27 downto 0);
signal count1, count1_d1 :  std_logic;
signal level1, level1_d1 :  std_logic_vector(27 downto 0);
signal count0 :  std_logic;
signal level0 :  std_logic_vector(27 downto 0);
signal sCount :  std_logic_vector(4 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            level5_d1 <=  level5;
            count4_d1 <=  count4;
            count4_d2 <=  count4_d1;
            count4_d3 <=  count4_d2;
            level4_d1 <=  level4;
            count3_d1 <=  count3;
            count3_d2 <=  count3_d1;
            count3_d3 <=  count3_d2;
            level3_d1 <=  level3;
            count2_d1 <=  count2;
            count2_d2 <=  count2_d1;
            count1_d1 <=  count1;
            level1_d1 <=  level1;
         end if;
      end process;
   level5 <= I ;
   count4<= '1' when level5_d1(27 downto 12) = (27 downto 12=>'0') else '0';
   level4<= level5_d1(27 downto 0) when count4='0' else level5_d1(11 downto 0) & (15 downto 0 => '0');

   count3<= '1' when level4(27 downto 20) = (27 downto 20=>'0') else '0';
   level3<= level4_d1(27 downto 0) when count3_d1='0' else level4_d1(19 downto 0) & (7 downto 0 => '0');

   count2<= '1' when level3(27 downto 24) = (27 downto 24=>'0') else '0';
   level2<= level3_d1(27 downto 0) when count2_d1='0' else level3_d1(23 downto 0) & (3 downto 0 => '0');

   count1<= '1' when level2(27 downto 26) = (27 downto 26=>'0') else '0';
   level1<= level2(27 downto 0) when count1='0' else level2(25 downto 0) & (1 downto 0 => '0');

   count0<= '1' when level1_d1(27 downto 27) = (27 downto 27=>'0') else '0';
   level0<= level1_d1(27 downto 0) when count0='0' else level1_d1(26 downto 0) & (0 downto 0 => '0');

   O <= level0;
   sCount <= count4_d3 & count3_d3 & count2_d2 & count1_d1 & count0;
   Count <= sCount;
end architecture;

--------------------------------------------------------------------------------
--                           IntAdder_34_F400_uid12
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

entity IntAdder_34_F400_uid12 is
    port (clk : in std_logic;
          X : in  std_logic_vector(33 downto 0);
          Y : in  std_logic_vector(33 downto 0);
          Cin : in  std_logic;
          R : out  std_logic_vector(33 downto 0)   );
end entity;

architecture arch of IntAdder_34_F400_uid12 is
signal Cin_1, Cin_1_d1, Cin_1_d2 :  std_logic;
signal X_1, X_1_d1, X_1_d2 :  std_logic_vector(31 downto 0);
signal Y_1, Y_1_d1, Y_1_d2, Y_1_d3, Y_1_d4, Y_1_d5, Y_1_d6, Y_1_d7, Y_1_d8, Y_1_d9, Y_1_d10, Y_1_d11, Y_1_d12, Y_1_d13 :  std_logic_vector(31 downto 0);
signal S_1 :  std_logic_vector(31 downto 0);
signal R_1 :  std_logic_vector(30 downto 0);
signal Cin_2 :  std_logic;
signal X_2, X_2_d1, X_2_d2 :  std_logic_vector(3 downto 0);
signal Y_2, Y_2_d1, Y_2_d2, Y_2_d3, Y_2_d4, Y_2_d5, Y_2_d6, Y_2_d7, Y_2_d8, Y_2_d9, Y_2_d10, Y_2_d11, Y_2_d12, Y_2_d13 :  std_logic_vector(3 downto 0);
signal S_2 :  std_logic_vector(3 downto 0);
signal R_2 :  std_logic_vector(2 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            Cin_1_d1 <=  Cin_1;
            Cin_1_d2 <=  Cin_1_d1;
            X_1_d1 <=  X_1;
            X_1_d2 <=  X_1_d1;
            Y_1_d1 <=  Y_1;
            Y_1_d2 <=  Y_1_d1;
            Y_1_d3 <=  Y_1_d2;
            Y_1_d4 <=  Y_1_d3;
            Y_1_d5 <=  Y_1_d4;
            Y_1_d6 <=  Y_1_d5;
            Y_1_d7 <=  Y_1_d6;
            Y_1_d8 <=  Y_1_d7;
            Y_1_d9 <=  Y_1_d8;
            Y_1_d10 <=  Y_1_d9;
            Y_1_d11 <=  Y_1_d10;
            Y_1_d12 <=  Y_1_d11;
            Y_1_d13 <=  Y_1_d12;
            X_2_d1 <=  X_2;
            X_2_d2 <=  X_2_d1;
            Y_2_d1 <=  Y_2;
            Y_2_d2 <=  Y_2_d1;
            Y_2_d3 <=  Y_2_d2;
            Y_2_d4 <=  Y_2_d3;
            Y_2_d5 <=  Y_2_d4;
            Y_2_d6 <=  Y_2_d5;
            Y_2_d7 <=  Y_2_d6;
            Y_2_d8 <=  Y_2_d7;
            Y_2_d9 <=  Y_2_d8;
            Y_2_d10 <=  Y_2_d9;
            Y_2_d11 <=  Y_2_d10;
            Y_2_d12 <=  Y_2_d11;
            Y_2_d13 <=  Y_2_d12;
         end if;
      end process;
   Cin_1 <= Cin;
   X_1 <= '0' & X(30 downto 0);
   Y_1 <= '0' & Y(30 downto 0);
   S_1 <= X_1_d2 + Y_1_d13 + Cin_1_d2;
   R_1 <= S_1(30 downto 0);
   Cin_2 <= S_1(31);
   X_2 <= '0' & X(33 downto 31);
   Y_2 <= '0' & Y(33 downto 31);
   S_2 <= X_2_d2 + Y_2_d13 + Cin_2;
   R_2 <= S_2(2 downto 0);
   R <= R_2 & R_1 ;
end architecture;

--------------------------------------------------------------------------------
--                            FPAdd_8_23_F400_uid2
-- VHDL generated for Zynq7000 @ 400MHz
-- This operator is part of the Infinite Virtual Library FloPoCoLib
-- All rights reserved
-- Authors: Florent de Dinechin, Bogdan Pasca (2010-2017)
--------------------------------------------------------------------------------
-- Pipeline depth: 14 cycles
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

entity FPAdd_8_23_F400_uid2 is
    port (clk : in std_logic;
          X : in  std_logic_vector(8+23+2 downto 0);
          Y : in  std_logic_vector(8+23+2 downto 0);
          R : out  std_logic_vector(8+23+2 downto 0)   );
end entity;

architecture arch of FPAdd_8_23_F400_uid2 is
   component IntAdder_34_F400_uid4 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(33 downto 0);
             Y : in  std_logic_vector(33 downto 0);
             Cin : in  std_logic;
             R : out  std_logic_vector(33 downto 0)   );
   end component;

   component RightShifterSticky24_by_max_26_F400_uid6 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(23 downto 0);
             S : in  std_logic_vector(4 downto 0);
             R : out  std_logic_vector(25 downto 0);
             Sticky : out  std_logic   );
   end component;

   component IntAdder_27_F400_uid8 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(26 downto 0);
             Y : in  std_logic_vector(26 downto 0);
             Cin : in  std_logic;
             R : out  std_logic_vector(26 downto 0)   );
   end component;

   component LZCShifter_28_to_28_counting_32_F400_uid10 is
      port ( clk : in std_logic;
             I : in  std_logic_vector(27 downto 0);
             Count : out  std_logic_vector(4 downto 0);
             O : out  std_logic_vector(27 downto 0)   );
   end component;

   component IntAdder_34_F400_uid12 is
      port ( clk : in std_logic;
             X : in  std_logic_vector(33 downto 0);
             Y : in  std_logic_vector(33 downto 0);
             Cin : in  std_logic;
             R : out  std_logic_vector(33 downto 0)   );
   end component;

signal excExpFracX :  std_logic_vector(32 downto 0);
signal excExpFracY :  std_logic_vector(32 downto 0);
signal addCmpOp1 :  std_logic_vector(33 downto 0);
signal addCmpOp2 :  std_logic_vector(33 downto 0);
signal addCmpCin :  std_logic;
signal cmpRes, cmpRes_d1 :  std_logic_vector(33 downto 0);
signal swap :  std_logic;
signal eXmeY, eXmeY_d1, eXmeY_d2 :  std_logic_vector(7 downto 0);
signal eYmeX, eYmeX_d1, eYmeX_d2 :  std_logic_vector(7 downto 0);
signal expDiff, expDiff_d1 :  std_logic_vector(7 downto 0);
signal newX :  std_logic_vector(33 downto 0);
signal newY :  std_logic_vector(33 downto 0);
signal expX :  std_logic_vector(7 downto 0);
signal excX :  std_logic_vector(1 downto 0);
signal excY :  std_logic_vector(1 downto 0);
signal signX :  std_logic;
signal signY :  std_logic;
signal EffSub, EffSub_d1, EffSub_d2, EffSub_d3, EffSub_d4, EffSub_d5, EffSub_d6, EffSub_d7, EffSub_d8, EffSub_d9, EffSub_d10, EffSub_d11, EffSub_d12 :  std_logic;
signal sXsYExnXY :  std_logic_vector(5 downto 0);
signal sdExnXY :  std_logic_vector(3 downto 0);
signal fracY :  std_logic_vector(23 downto 0);
signal excRt, excRt_d1, excRt_d2, excRt_d3, excRt_d4, excRt_d5, excRt_d6, excRt_d7, excRt_d8, excRt_d9, excRt_d10, excRt_d11, excRt_d12 :  std_logic_vector(1 downto 0);
signal signR, signR_d1, signR_d2, signR_d3, signR_d4, signR_d5, signR_d6, signR_d7, signR_d8, signR_d9 :  std_logic;
signal shiftedOut, shiftedOut_d1 :  std_logic;
signal shiftVal :  std_logic_vector(4 downto 0);
signal shiftedFracY :  std_logic_vector(25 downto 0);
signal sticky, sticky_d1 :  std_logic;
signal fracYpad, fracYpad_d1 :  std_logic_vector(26 downto 0);
signal EffSubVector, EffSubVector_d1, EffSubVector_d2 :  std_logic_vector(26 downto 0);
signal fracYpadXorOp :  std_logic_vector(26 downto 0);
signal fracXpad :  std_logic_vector(26 downto 0);
signal cInSigAdd :  std_logic;
signal fracAddResult :  std_logic_vector(26 downto 0);
signal fracSticky :  std_logic_vector(27 downto 0);
signal nZerosNew :  std_logic_vector(4 downto 0);
signal shiftedFrac :  std_logic_vector(27 downto 0);
signal extendedExpInc, extendedExpInc_d1, extendedExpInc_d2, extendedExpInc_d3, extendedExpInc_d4, extendedExpInc_d5, extendedExpInc_d6, extendedExpInc_d7, extendedExpInc_d8, extendedExpInc_d9 :  std_logic_vector(9 downto 0);
signal updatedExp :  std_logic_vector(9 downto 0);
signal eqdiffsign, eqdiffsign_d1, eqdiffsign_d2, eqdiffsign_d3 :  std_logic;
signal expFrac :  std_logic_vector(33 downto 0);
signal stk :  std_logic;
signal rnd :  std_logic;
signal lsb :  std_logic;
signal needToRound :  std_logic;
signal zeros :  std_logic_vector(33 downto 0);
signal RoundedExpFrac :  std_logic_vector(33 downto 0);
signal upExc :  std_logic_vector(1 downto 0);
signal fracR, fracR_d1 :  std_logic_vector(22 downto 0);
signal expR, expR_d1 :  std_logic_vector(7 downto 0);
signal exExpExc :  std_logic_vector(3 downto 0);
signal excRt2, excRt2_d1 :  std_logic_vector(1 downto 0);
signal excR :  std_logic_vector(1 downto 0);
signal signR2, signR2_d1, signR2_d2, signR2_d3 :  std_logic;
signal computedR :  std_logic_vector(33 downto 0);
signal X_d1, X_d2 :  std_logic_vector(8+23+2 downto 0);
signal Y_d1, Y_d2 :  std_logic_vector(8+23+2 downto 0);
begin
   process(clk)
      begin
         if clk'event and clk = '1' then
            cmpRes_d1 <=  cmpRes;
            eXmeY_d1 <=  eXmeY;
            eXmeY_d2 <=  eXmeY_d1;
            eYmeX_d1 <=  eYmeX;
            eYmeX_d2 <=  eYmeX_d1;
            expDiff_d1 <=  expDiff;
            EffSub_d1 <=  EffSub;
            EffSub_d2 <=  EffSub_d1;
            EffSub_d3 <=  EffSub_d2;
            EffSub_d4 <=  EffSub_d3;
            EffSub_d5 <=  EffSub_d4;
            EffSub_d6 <=  EffSub_d5;
            EffSub_d7 <=  EffSub_d6;
            EffSub_d8 <=  EffSub_d7;
            EffSub_d9 <=  EffSub_d8;
            EffSub_d10 <=  EffSub_d9;
            EffSub_d11 <=  EffSub_d10;
            EffSub_d12 <=  EffSub_d11;
            excRt_d1 <=  excRt;
            excRt_d2 <=  excRt_d1;
            excRt_d3 <=  excRt_d2;
            excRt_d4 <=  excRt_d3;
            excRt_d5 <=  excRt_d4;
            excRt_d6 <=  excRt_d5;
            excRt_d7 <=  excRt_d6;
            excRt_d8 <=  excRt_d7;
            excRt_d9 <=  excRt_d8;
            excRt_d10 <=  excRt_d9;
            excRt_d11 <=  excRt_d10;
            excRt_d12 <=  excRt_d11;
            signR_d1 <=  signR;
            signR_d2 <=  signR_d1;
            signR_d3 <=  signR_d2;
            signR_d4 <=  signR_d3;
            signR_d5 <=  signR_d4;
            signR_d6 <=  signR_d5;
            signR_d7 <=  signR_d6;
            signR_d8 <=  signR_d7;
            signR_d9 <=  signR_d8;
            shiftedOut_d1 <=  shiftedOut;
            sticky_d1 <=  sticky;
            fracYpad_d1 <=  fracYpad;
            EffSubVector_d1 <=  EffSubVector;
            EffSubVector_d2 <=  EffSubVector_d1;
            extendedExpInc_d1 <=  extendedExpInc;
            extendedExpInc_d2 <=  extendedExpInc_d1;
            extendedExpInc_d3 <=  extendedExpInc_d2;
            extendedExpInc_d4 <=  extendedExpInc_d3;
            extendedExpInc_d5 <=  extendedExpInc_d4;
            extendedExpInc_d6 <=  extendedExpInc_d5;
            extendedExpInc_d7 <=  extendedExpInc_d6;
            extendedExpInc_d8 <=  extendedExpInc_d7;
            extendedExpInc_d9 <=  extendedExpInc_d8;
            eqdiffsign_d1 <=  eqdiffsign;
            eqdiffsign_d2 <=  eqdiffsign_d1;
            eqdiffsign_d3 <=  eqdiffsign_d2;
            fracR_d1 <=  fracR;
            expR_d1 <=  expR;
            excRt2_d1 <=  excRt2;
            signR2_d1 <=  signR2;
            signR2_d2 <=  signR2_d1;
            signR2_d3 <=  signR2_d2;
            X_d1 <=  X;
            X_d2 <=  X_d1;
            Y_d1 <=  Y;
            Y_d2 <=  Y_d1;
         end if;
      end process;
   excExpFracX <= X(33 downto 32) & X(30 downto 0);
   excExpFracY <= Y(33 downto 32) & Y(30 downto 0);
   addCmpOp1 <= '0'  & excExpFracX;
   addCmpOp2 <= '1'  & not excExpFracY;
   addCmpCin <= '1';
   cmpAdder: IntAdder_34_F400_uid4
      port map ( clk  => clk,
                 Cin => addCmpCin,
                 X => addCmpOp1,
                 Y => addCmpOp2,
                 R => cmpRes);
   swap <= cmpRes_d1(33);
   -- exponent difference
   eXmeY <= (X(30 downto 23)) - (Y(30 downto 23));
   eYmeX <= (Y(30 downto 23)) - (X(30 downto 23));
   expDiff <= eXmeY_d2 when swap = '0' else eYmeX_d2;
   -- input swap so that |X|>|Y|
   newX <= X_d2 when swap = '0' else Y_d2;
   newY <= Y_d2 when swap = '0' else X_d2;
   -- now we decompose the inputs into their sign, exponent, fraction
   expX<= newX(30 downto 23);
   excX<= newX(33 downto 32);
   excY<= newY(33 downto 32);
   signX<= newX(31);
   signY<= newY(31);
   EffSub <= signX xor signY;
   sXsYExnXY <= signX & signY & excX & excY;
   sdExnXY <= excX & excY;
   fracY <= "000000000000000000000000" when excY="00" else ('1' & newY(22 downto 0));
   -- Exception management logic
   with sXsYExnXY  select
   excRt <= "00" when "000000"|"010000"|"100000"|"110000",
      "01" when "000101"|"010101"|"100101"|"110101"|"000100"|"010100"|"100100"|"110100"|"000001"|"010001"|"100001"|"110001",
      "10" when "111010"|"001010"|"001000"|"011000"|"101000"|"111000"|"000010"|"010010"|"100010"|"110010"|"001001"|"011001"|"101001"|"111001"|"000110"|"010110"|"100110"|"110110",
      "11" when others;
   signR<= '0' when (sXsYExnXY="100000" or sXsYExnXY="010000") else signX;
   shiftedOut <= '1' when (expDiff > 25) else '0';
   shiftVal <= expDiff_d1(4 downto 0) when shiftedOut_d1='0' else CONV_STD_LOGIC_VECTOR(26,5);
   RightShifterComponent: RightShifterSticky24_by_max_26_F400_uid6
      port map ( clk  => clk,
                 S => shiftVal,
                 X => fracY,
                 R => shiftedFracY,
                 Sticky => sticky);
   fracYpad <= "0" & shiftedFracY;
   EffSubVector <= (26 downto 0 => EffSub);
   fracYpadXorOp <= fracYpad_d1 xor EffSubVector_d2;
   fracXpad <= "01" & (newX(22 downto 0)) & "00";
   cInSigAdd <= EffSub_d4 and not sticky; -- if we subtract and the sticky was one, some of the negated sticky bits would have absorbed this carry
   fracAdder: IntAdder_27_F400_uid8
      port map ( clk  => clk,
                 Cin => cInSigAdd,
                 X => fracXpad,
                 Y => fracYpadXorOp,
                 R => fracAddResult);
   fracSticky<= fracAddResult & sticky_d1;
   LZCAndShifter: LZCShifter_28_to_28_counting_32_F400_uid10
      port map ( clk  => clk,
                 I => fracSticky,
                 Count => nZerosNew,
                 O => shiftedFrac);
   extendedExpInc<= ("00" & expX) + '1';
   updatedExp <= extendedExpInc_d9 - ("00000" & nZerosNew);
   eqdiffsign <= '1' when nZerosNew="11111" else '0';
   expFrac<= updatedExp & shiftedFrac(26 downto 3);
   stk<= shiftedFrac(2) or shiftedFrac(1) or shiftedFrac(0);
   rnd<= shiftedFrac(3);
   lsb<= shiftedFrac(4);
   needToRound<= '1' when (rnd='1' and stk='1') or (rnd='1' and stk='0' and lsb='1')
  else '0';
   zeros  <= "0000000000000000000000000000000000";
   roundingAdder: IntAdder_34_F400_uid12
      port map ( clk  => clk,
                 Cin => needToRound,
                 X => expFrac,
                 Y => zeros,
                 R => RoundedExpFrac);
   -- possible update to exception bits
   upExc <= RoundedExpFrac(33 downto 32);
   fracR <= RoundedExpFrac(23 downto 1);
   expR <= RoundedExpFrac(31 downto 24);
   exExpExc <= upExc & excRt_d11;
   with exExpExc  select
   excRt2<= "00" when "0000"|"0100"|"1000"|"1100"|"1001"|"1101",
      "01" when "0001",
      "10" when "0010"|"0110"|"1010"|"1110"|"0101",
      "11" when others;
   excR <= "00" when (eqdiffsign_d3='1' and EffSub_d12='1'  and not(excRt_d12="11")) else excRt2_d1;
   signR2 <= '0' when (eqdiffsign='1' and EffSub_d9='1') else signR_d9;
   computedR <= excR & signR2_d3 & expR_d1 & fracR_d1;
   R <= computedR;
end architecture;
