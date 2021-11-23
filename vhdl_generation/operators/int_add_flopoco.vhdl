--------------------------------------------------------------------------------
--                           IntAdder_34_F400_uid2
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

entity IntAdder_34_F400_uid2 is
    port (clk : in std_logic;
          X : in  std_logic_vector(33 downto 0);
          Y : in  std_logic_vector(33 downto 0);
          Cin : in  std_logic := '0';
          R : out  std_logic_vector(33 downto 0)   );
end entity;

architecture arch of IntAdder_34_F400_uid2 is
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
