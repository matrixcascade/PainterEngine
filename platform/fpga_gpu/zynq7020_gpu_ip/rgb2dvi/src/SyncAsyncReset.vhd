-------------------------------------------------------------------------------
--
-- File: SyncAsyncReset.vhd
-- Author: Elod Gyorgy
-- Original Project: HDMI input on 7-series Xilinx FPGA
-- Date: 20 October 2014
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
-- This module is a reset-bridge. It takes a reset signal asynchronous to the 
-- target clock domain (OutClk) and provides a safe asynchronous or synchronous
-- reset for the OutClk domain (oRst). The signal oRst is asserted immediately 
-- as aRst arrives, but is de-asserted synchronously with the OutClk rising
-- edge. This means it can be used to safely reset any FF in the OutClk domain,
-- respecting recovery time specs for FFs.
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

entity ResetBridge is
   Generic (
      kPolarity : std_logic := '1');
   Port (
      aRst : in STD_LOGIC; -- asynchronous reset; active-high, if kPolarity=1
      OutClk : in STD_LOGIC;
      oRst : out STD_LOGIC);
end ResetBridge;

architecture Behavioral of ResetBridge is
signal aRst_int : std_logic;
attribute KEEP : string;
attribute KEEP of aRst_int: signal is "TRUE";
begin

aRst_int <= kPolarity xnor aRst; --SyncAsync uses active-high reset

SyncAsyncx: entity work.SyncAsync
   generic map (
      kResetTo => kPolarity,
      kStages => 2) --use double FF synchronizer
   port map (
      aReset => aRst_int,
      aIn => not kPolarity,
      OutClk => OutClk,
      oOut => oRst);

end Behavioral;
