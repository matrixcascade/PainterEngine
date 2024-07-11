-------------------------------------------------------------------------------
--
-- File: SyncAsync.vhd
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
-- This module synchronizes the asynchronous signal (aIn) with the OutClk clock
-- domain and provides it on oOut. The number of FFs in the synchronizer chain
-- can be configured with kStages. The reset value for oOut can be configured
-- with kResetTo. The asynchronous reset (aReset) is always active-high.
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

entity SyncAsync is
   Generic (
      kResetTo : std_logic := '0'; --value when reset and upon init
      kStages : natural := 2); --double sync by default
   Port (
      aReset : in STD_LOGIC; -- active-high asynchronous reset
      aIn : in STD_LOGIC;
      OutClk : in STD_LOGIC;
      oOut : out STD_LOGIC);
end SyncAsync;

architecture Behavioral of SyncAsync is
signal oSyncStages : std_logic_vector(kStages-1 downto 0) := (others => kResetTo);
attribute ASYNC_REG : string;
attribute ASYNC_REG of oSyncStages: signal is "TRUE";
begin

Sync: process (OutClk, aReset)
begin
   if (aReset = '1') then
      oSyncStages <= (others => kResetTo);
   elsif Rising_Edge(OutClk) then
      oSyncStages <= oSyncStages(oSyncStages'high-1 downto 0) & aIn;
   end if;
end process Sync;
oOut <= oSyncStages(oSyncStages'high);

end Behavioral;
