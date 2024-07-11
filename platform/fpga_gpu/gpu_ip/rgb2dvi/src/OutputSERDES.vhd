-------------------------------------------------------------------------------
--
-- File: OutputSERDES.vhd
-- Author: Elod Gyorgy, Mihaita Nagy
-- Original Project: HDMI output on 7-series Xilinx FPGA
-- Date: 28 October 2014
--
-------------------------------------------------------------------------------
-- (c) 2014 Copyright Digilent Incorporated
-- All Rights Reserved
-- 
-- This program is free software; distributed under the terms of BSD 3-clause 
-- license ("Revised BSD License", "New BSD License", or "Modified BSD License")
--
-- Redistribution and use in source and binary forms, with or without modification,
-- are permitted provided that the following conditions are met:
--
-- 1. Redistributions of source code must retain the above copyright notice, this
--    list of conditions and the following disclaimer.
-- 2. Redistributions in binary form must reproduce the above copyright notice,
--    this list of conditions and the following disclaimer in the documentation
--    and/or other materials provided with the distribution.
-- 3. Neither the name(s) of the above-listed copyright holder(s) nor the names
--    of its contributors may be used to endorse or promote products derived
--    from this software without specific prior written permission.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
-- ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE 
-- FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
-- DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
-- SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
-- CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
-- OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
-- OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--
-------------------------------------------------------------------------------
--
-- Purpose:
-- This module instantiates the Xilinx 7-series primitives necessary for
-- serializing the TMDS data stream. 
--  
-------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
library UNISIM;
use UNISIM.VComponents.all;

entity OutputSERDES is
   Generic (
      kParallelWidth : natural := 10); -- number of parallel bits
   Port (
      PixelClk : in STD_LOGIC;   --TMDS clock x1 (CLKDIV)
      SerialClk : in STD_LOGIC;  --TMDS clock x5 (CLK)
      
      --Encoded serial data
      sDataOut_p : out STD_LOGIC;
      sDataOut_n : out STD_LOGIC;
      
      --Encoded parallel data (raw)
      pDataOut : in STD_LOGIC_VECTOR (kParallelWidth-1 downto 0);
      
      aRst : in STD_LOGIC);
end OutputSERDES;

architecture Behavioral of OutputSERDES is

signal sDataOut, ocascade1, ocascade2 : std_logic;
signal pDataOut_q : std_logic_vector(13 downto 0);
begin

-- Differential output buffer for TMDS I/O standard 
OutputBuffer: OBUFDS
   generic map (
      IOSTANDARD => "TMDS_33")
   port map (
      O          => sDataOut_p,
      OB         => sDataOut_n,
      I          => sDataOut);

-- Serializer, 10:1 (5:1 DDR), master-slave cascaded
SerializerMaster: OSERDESE2
   generic map (
      DATA_RATE_OQ      => "DDR",
      DATA_RATE_TQ      => "SDR",
      DATA_WIDTH        => kParallelWidth,
      TRISTATE_WIDTH    => 1,
      TBYTE_CTL         => "FALSE", 
      TBYTE_SRC         => "FALSE",
      SERDES_MODE       => "MASTER")
   port map (
      OFB               => open,             -- 1-bit output: Feedback path for data
      OQ                => sDataOut,               -- 1-bit output: Data path output
      -- SHIFTOUT1 / SHIFTOUT2: 1-bit (each) output: Data output expansion (1-bit each)
      SHIFTOUT1         => open,
      SHIFTOUT2         => open,
      TBYTEOUT          => open,   -- 1-bit output: Byte group tristate
      TFB               => open,      -- 1-bit output: 3-state control
      TQ                => open,      -- 1-bit output: 3-state control
      CLK               => SerialClk, -- 1-bit input: High speed clock
      CLKDIV            => PixelClk,  -- 1-bit input: Divided clock
      -- D1 - D8: 1-bit (each) input: Parallel data inputs (1-bit each)
      D1                => pDataOut_q(13),
      D2                => pDataOut_q(12),
      D3                => pDataOut_q(11),
      D4                => pDataOut_q(10),
      D5                => pDataOut_q(9),
      D6                => pDataOut_q(8),
      D7                => pDataOut_q(7),
      D8                => pDataOut_q(6),
      OCE               => '1',             -- 1-bit input: Output data clock enable
      RST               => aRst,             -- 1-bit input: Reset
      -- SHIFTIN1 / SHIFTIN2: 1-bit (each) input: Data input expansion (1-bit each)
      SHIFTIN1          => ocascade1,
      SHIFTIN2          => ocascade2,
      -- T1 - T4: 1-bit (each) input: Parallel 3-state inputs
      T1                => '0',
      T2                => '0',
      T3                => '0',
      T4                => '0',
      TBYTEIN           => '0',     -- 1-bit input: Byte group tristate
      TCE               => '0'              -- 1-bit input: 3-state clock enable
    );   
   
SerializerSlave: OSERDESE2
   generic map (
      DATA_RATE_OQ      => "DDR",
      DATA_RATE_TQ      => "SDR",
      DATA_WIDTH        => kParallelWidth,
      TRISTATE_WIDTH    => 1,
      TBYTE_CTL         => "FALSE", 
      TBYTE_SRC         => "FALSE",
      SERDES_MODE       => "SLAVE")
   port map (
      OFB               => open,             -- 1-bit output: Feedback path for data
      OQ                => open,               -- 1-bit output: Data path output
      -- SHIFTOUT1 / SHIFTOUT2: 1-bit (each) output: Data output expansion (1-bit each)
      SHIFTOUT1         => ocascade1,
      SHIFTOUT2         => ocascade2,
      TBYTEOUT          => open,   -- 1-bit output: Byte group tristate
      TFB               => open,             -- 1-bit output: 3-state control
      TQ                => open,               -- 1-bit output: 3-state control
      CLK               => SerialClk,             -- 1-bit input: High speed clock
      CLKDIV            => PixelClk,       -- 1-bit input: Divided clock
      -- D1 - D8: 1-bit (each) input: Parallel data inputs (1-bit each)
      D1                => '0',
      D2                => '0',
      D3                => pDataOut_q(5),
      D4                => pDataOut_q(4),
      D5                => pDataOut_q(3),
      D6                => pDataOut_q(2),
      D7                => pDataOut_q(1),
      D8                => pDataOut_q(0),
      OCE               => '1',             -- 1-bit input: Output data clock enable
      RST               => aRst,             -- 1-bit input: Reset
      -- SHIFTIN1 / SHIFTIN2: 1-bit (each) input: Data input expansion (1-bit each)
      SHIFTIN1          => '0',
      SHIFTIN2          => '0',
      -- T1 - T4: 1-bit (each) input: Parallel 3-state inputs
      T1                => '0',
      T2                => '0',
      T3                => '0',
      T4                => '0',
      TBYTEIN           => '0',     -- 1-bit input: Byte group tristate
      TCE               => '0'              -- 1-bit input: 3-state clock enable
    );      

------------------------------------------------------------- 
-- Concatenate the serdes inputs together. Keep the timesliced
-- bits together, and placing the earliest bits on the right
-- ie, if data comes in 0, 1, 2, 3, 4, 5, 6, 7, ...
-- the output will be 3210, 7654, ...
-------------------------------------------------------------   
SliceOSERDES_q: for slice_count in 0 to kParallelWidth-1 generate begin
   --DVI sends least significant bit first 
   --OSERDESE2 sends D1 bit first
   pDataOut_q(14-slice_count-1) <= pDataOut(slice_count);
end generate SliceOSERDES_q;

end Behavioral;
