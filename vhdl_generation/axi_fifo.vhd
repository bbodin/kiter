-- I did some research and found this tutorial looking very simular to Uros buffer
--  https://vhdlwhiz.com/axi-fifo/

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;




entity axi_fifo is
  generic (
    ram_width : natural;
    ram_depth : natural;
    ram_init  : natural := 0
    );

  port (
    buffer_clk : in std_logic;
    buffer_rst : in std_logic;

    -- axi input interface
    buffer_in_ready : out std_logic;
    buffer_in_valid : in std_logic;
    buffer_in_data : in std_logic_vector(ram_width - 1 downto 0);

    -- axi output interface
    buffer_out_ready : in std_logic;
    buffer_out_valid : out std_logic;
    buffer_out_data : out std_logic_vector(ram_width - 1 downto 0)
    );
end axi_fifo;

architecture simulation of axi_fifo is


begin

  assert (ram_depth >= 0) report "Buffer size must be positive or zero" severity error;
  assert (ram_init >= 0) report "Initial marking must be positive or zero" severity error;
  assert (ram_depth >= ram_init) report "Initial marking of buffer overflow" severity error;

  gen_fifo_zero: if (ram_depth = 0) generate

    -- instance
    DUT : entity work.axi_fifo_zero
      generic map ( ram_width => ram_width
                    )
      port map ( buffer_clk        => buffer_clk,
                 buffer_rst      => buffer_rst,
                 buffer_in_ready => buffer_in_ready,
                 buffer_in_valid => buffer_in_valid,
                 buffer_in_data  => buffer_in_data,
                 buffer_out_ready  => buffer_out_ready,
                 buffer_out_valid  => buffer_out_valid,
                 buffer_out_data => buffer_out_data);

  end generate gen_fifo_zero;


  gen_fifo_one: if (ram_depth = 1) generate

    -- instance
    DUT : entity work.axi_fifo_one
      generic map ( ram_width => ram_width ,
                    ram_init => ram_init
                    )
      port map ( buffer_clk        => buffer_clk,
                 buffer_rst      => buffer_rst,
                 buffer_in_ready => buffer_in_ready,
                 buffer_in_valid => buffer_in_valid,
                 buffer_in_data  => buffer_in_data,
                 buffer_out_ready  => buffer_out_ready,
                 buffer_out_valid  => buffer_out_valid,
                 buffer_out_data => buffer_out_data);

  end generate gen_fifo_one;


  gen_fifo_n: if (ram_depth > 1) generate

    -- instance
    DUT : entity work.axi_fifo_n
      generic map ( ram_width => ram_width ,
                    ram_depth => ram_depth ,
                    ram_init => ram_init
                    )
      port map ( buffer_clk        => buffer_clk,
                 buffer_rst      => buffer_rst,
                 buffer_in_ready => buffer_in_ready,
                 buffer_in_valid => buffer_in_valid,
                 buffer_in_data  => buffer_in_data,
                 buffer_out_ready  => buffer_out_ready,
                 buffer_out_valid  => buffer_out_valid,
                 buffer_out_data => buffer_out_data);

  end generate gen_fifo_n;


end architecture;
