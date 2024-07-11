
-------------------------------------------------------------------------------
--
-- File: rgb2dvi.vhd
-- Author: Elod Gyorgy
-- Original Project: HDMI input on 7-series Xilinx FPGA
-- Date: 30 October 2014
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
-- This module connects to a top level DVI 1.0 source interface comprised of three
-- TMDS data channels and one TMDS clock channel. It includes the necessary
-- clock infrastructure (optional), encoding and serialization logic.
-- On the input side it has 24-bit RGB video data bus, pixel clock and synchronization
-- signals. 
--  
-------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity rgb2dvi is
   Generic (
      kGenerateSerialClk : boolean := true;
      kClkPrimitive : string := "PLL"; -- "MMCM" or "PLL" to instantiate, if kGenerateSerialClk true
      kClkRange : natural := 1;  -- MULT_F = kClkRange*5 (choose >=120MHz=1, >=60MHz=2, >=40MHz=3)      
      kRstActiveHigh : boolean := true;  --true, if active-high; false, if active-low
      kD0Swap : boolean := false;  -- P/N Swap Options
      kD1Swap : boolean := false;
      kD2Swap : boolean := false;
      kClkSwap : boolean := false); 
   Port (
      -- DVI 1.0 TMDS video interface
      TMDS_Clk_p : out std_logic;
      TMDS_Clk_n : out std_logic;
      TMDS_Data_p : out std_logic_vector(2 downto 0);
      TMDS_Data_n : out std_logic_vector(2 downto 0);
      
      -- Auxiliary signals 
      aRst : in std_logic; --asynchronous reset; must be reset when RefClk is not within spec
      aRst_n : in std_logic; --asynchronous reset; must be reset when RefClk is not within spec
      
      -- Video in
      vid_pData : in std_logic_vector(23 downto 0);
      vid_pVDE : in std_logic;
      vid_pHSync : in std_logic;
      vid_pVSync : in std_logic;
      PixelClk : in std_logic; --pixel-clock recovered from the DVI interface
      
      SerialClk : in std_logic); -- 5x PixelClk
   
end rgb2dvi;

architecture Behavioral of rgb2dvi is
type dataOut_t is array (2 downto 0) of std_logic_vector(7 downto 0);
type dataOutRaw_t is array (2 downto 0) of std_logic_vector(9 downto 0);
signal pDataOut : dataOut_t;
signal pDataOutRaw : dataOutRaw_t;
signal pDataOutRaw_q : dataOutRaw_t;
signal pVde, pC0, pC1 : std_logic_vector(2 downto 0);
signal aRst_int, aPixelClkLckd : std_logic;
signal PixelClkIO, SerialClkIO, aRstLck, pRstLck : std_logic;
signal pClkOut : std_logic_vector(9 downto 0);
begin

ResetActiveLow: if not kRstActiveHigh generate
   aRst_int <= not aRst_n;
end generate ResetActiveLow;

ResetActiveHigh: if kRstActiveHigh generate
   aRst_int <= aRst;
end generate ResetActiveHigh;

-- Generate SerialClk internally?
ClockGenInternal: if kGenerateSerialClk generate
   ClockGenX: entity work.ClockGen
      Generic map (
         kClkRange => kClkRange,  -- MULT_F = kClkRange*5 (choose >=120MHz=1, >=60MHz=2, >=40MHz=3, >=30MHz=4, >=25MHz=5
         kClkPrimitive => kClkPrimitive) -- "MMCM" or "PLL" to instantiate, if kGenerateSerialClk true
      Port map (
         PixelClkIn => PixelClk,
         PixelClkOut => PixelClkIO,
         SerialClk => SerialClkIO,
         aRst => aRst_int,
         aLocked => aPixelClkLckd);
   --TODO revise this
   aRstLck <= not aPixelClkLckd;         
end generate ClockGenInternal;

ClockGenExternal: if not kGenerateSerialClk generate
   PixelClkIO <= PixelClk;
   SerialClkIO <= SerialClk;
   aRstLck <= aRst_int;
end generate ClockGenExternal;

-- We need a reset bridge to use the asynchronous aLocked signal to reset our circuitry
-- and decrease the chance of metastability. The signal pLockLostRst can be used as
-- asynchronous reset for any flip-flop in the PixelClk domain, since it will be de-asserted
-- synchronously.
LockLostReset: entity work.ResetBridge
   generic map (
      kPolarity => '1')
   port map (
      aRst => aRstLck,
      OutClk => PixelClk,
      oRst => pRstLck);

-- Clock needs no encoding, send a pulse
ClockSerializer: entity work.OutputSERDES
   generic map (
      kParallelWidth => 10) -- TMDS uses 1:10 serialization
   port map(
      PixelClk => PixelClkIO,
      SerialClk => SerialClkIO,
      sDataOut_p => TMDS_Clk_p,
      sDataOut_n => TMDS_Clk_n,
      --Encoded parallel data (raw)
      pDataOut => pClkOut,      
      aRst => pRstLck);

DataEncoders: for i in 0 to 2 generate
   DataEncoder: entity work.TMDS_Encoder
      port map (
         PixelClk => PixelClk,
         SerialClk => SerialClk,
         pDataOutRaw => pDataOutRaw(i),
         aRst => pRstLck,
         pDataOut => pDataOut(i),
         pC0 => pC0(i),
         pC1 => pC1(i),
         pVde => pVde(i)
      );
   DataSerializer: entity work.OutputSERDES
      generic map (
         kParallelWidth => 10) -- TMDS uses 1:10 serialization
      port map(
         PixelClk => PixelClkIO,
         SerialClk => SerialClkIO,
         sDataOut_p => TMDS_Data_p(i),
         sDataOut_n => TMDS_Data_n(i),
         --Encoded parallel data (raw)
         pDataOut => pDataOutRaw_q(i),
         aRst => pRstLck);      
end generate DataEncoders;

-- Swap each bit
D0_direct: if kD0Swap = false generate
    pDataOutRaw_q(0) <= pDataOutRaw(0);
end generate;
D0_reverse: if kD0Swap = true generate
    pDataOutRaw_q(0) <= not pDataOutRaw(0);
end generate;
D1_direct: if kD1Swap = false generate
    pDataOutRaw_q(1) <= pDataOutRaw(1);
end generate;
D1_reverse: if kD1Swap = true generate
    pDataOutRaw_q(1) <= not pDataOutRaw(1);
end generate;
D2_direct: if kD2Swap = false generate
    pDataOutRaw_q(2) <= pDataOutRaw(2);
end generate;
D2_reverse: if kD2Swap = true generate
    pDataOutRaw_q(2) <= not pDataOutRaw(2);
end generate;
Clk_direct: if kClkSwap = false generate
    pClkOut <= "1111100000";
end generate;
Clk_reverse: if kClkSwap = true generate
    pClkOut <= "0000011111";
end generate;

-- DVI Output conform DVI 1.0
-- except that it sends blank pixel during blanking
-- for some reason vid_data is packed in RBG order
pDataOut(2) <= vid_pData(23 downto 16); -- red is channel 2
pDataOut(1) <= vid_pData(7 downto 0); -- green is channel 1
pDataOut(0) <= vid_pData(15 downto 8); -- blue is channel 0
pC0(2 downto 1) <= (others => '0'); -- default is low for control signals
pC1(2 downto 1) <= (others => '0'); -- default is low for control signals
pC0(0) <= vid_pHSync; -- channel 0 carries control signals too
pC1(0) <= vid_pVSync; -- channel 0 carries control signals too
pVde <= vid_pVDE & vid_pVDE & vid_pVDE; -- all of them are either active or blanking at once

end Behavioral;