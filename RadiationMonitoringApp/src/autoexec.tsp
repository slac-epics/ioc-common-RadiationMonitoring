--==============================================================
--
--  Abs:  This file is Keithley Test Script Builder Lua code
-- See documentation for their 2600B series source meter documentation
-- for information on automatically starting this
-- This program configures the 2612 and 3700 series relay matrix, as configured
-- by Alan Fisher, to measure accumulated radiation dose with RadFETS. Operation is
-- similar to the Elettra DOSFET system.
-- Changing parameters and readback are done through a simple text interface,
-- implemented for EPICS streamdevice in K2612B_RadFET.proto.
--
--  Name: autoexec.tsp
--
--  Facility: LCLS-II IOC Management
--
--  Auth: 23 May 2014, Garth Brown  (gwbrown):
--  Rev:  dd-mmm-yyyy, Reviewer's Name (USERNAME)
-- --------------------------------------------------------------
--  Mod:
--        dd-mmm-yyyy, First Lastname  (USERNAME):
--          comment
--
--==============================================================

--Requires 2600 set to Node 1, 3706A set to Node 2

-- From Elettra DOSFET specs, 490uA per side for readings
SamplingI  = 0.00098
BiasV = 9
-- Hold seconds
HoldS = 1

ChBases = {}
ChActive = {}
SlotActive = {}

-- RadFET channels are mapped to pairs of lines over a pair of board banks
-- so for slot N, channels start at:
-- N1101, N1103, N1105..., N1127
-- N3101, N3103, N3105..., N3127
SlotOffset      = 10000 -- * slot # to map channels
Bank1Offset     = 01000 -- for RadFET channels 1-14
Bank2Offset     = 03000 -- for RadFET channels 15-28
ChannelInterval = 00002 -- Offset is Ch. # * ChannelInterval - 1

function ShowErrors()
	while errorqueue.count > 0 do
		print(errorqueue.next())
	end
end
function ChToBase(Ch)
	-- print("-> ChToBase " .. Ch)
	BaseAddr = 0
	-- 28 channels per slot, 1 based
	Slot = math.ceil(Ch / 28)
	BaseAddr = SlotOffset * Slot
	-- print("Slot " .. Slot .. ", BaseAddr " .. BaseAddr)

	ChInSlot = Ch - 28*(Slot - 1)
	-- print("Channel " .. ChInSlot)
	-- 14 channels per bank pair
	if ChInSlot <= 14 then
		BankInSlot = Bank1Offset
		ChInBank = ChInSlot
	else
		BankInSlot = Bank2Offset
		ChInBank = ChInSlot - 14
	end
	-- print("Bank offset " .. BankInSlot .. ", Channel " .. ChInBank .. "  of bank ")
	BaseAddr = BaseAddr + BankInSlot
	BaseAddr = BaseAddr + ChannelInterval * (ChInBank - 1) +101
	-- print("Final base address " .. BaseAddr)
	ShowErrors()
	return BaseAddr
end

function MapChToBase()
	for i=1, 168 do
		ChBases[i] = ChToBase(i)
	end
	ShowErrors()
end

function QuadToBase(quad) -- Assumes slot 1
	ChInQuad = 7 * (quad - 1) + 1
	print("->QuadToBase " .. ChInQuad .. ", starting channel " .. ChInQuad)
	return ChToBase(ChInQuad)
end

-- Offsets from top left relay of a channel, e.g. 11101
-- Measure this channel
SenseH_Ss  = 00000
SenseL_Ds  = 00101
-- Don't measure this channel
BiasGND_Ss = 00200
BiasGND_Ds = 00301
-- CCS to this channel
DriveH_S   = 01000
--BiasGND_G  = 01301
BiasGND_G  = 01101
-- Bias this channel for dose accumulation
BiasGND_S  = 01300
BiasH_G    = 01201

function ReadFirstCh(base)
	-- turn off CCS just in case, should be off already
	smua.source.output = smua.OUTPUT_OFF

	-- print("Switch first channel from bias to CCS")
	node[2].channel.open(tostring(base + BiasH_G))
	-- print("node[2].channel.open " .. base + BiasH_G)
	node[2].channel.close(tostring(base + BiasGND_G))
	-- print("node[2].channel.close " .. base + BiasGND_G)
	node[2].channel.open(tostring(base + BiasGND_S))
	-- print("node[2].channel.open " .. base + BiasGND_S)
	node[2].channel.close(tostring(base + DriveH_S))
	-- print("node[2].channel.close " .. base + DriveH_S)
	-- print("turn on CCS")
	smua.source.output = smua.OUTPUT_ON

	-- print("Switch first channel to the sense lines")
	node[2].channel.open(tostring(base + BiasGND_Ss))
	-- print("node[2].channel.open " .. base + BiasGND_Ss)
	node[2].channel.open(tostring(base + BiasGND_Ds))
	-- print("node[2].channel.open " .. base + BiasGND_Ds)
	node[2].channel.close(tostring(base + SenseL_Ds))
	-- print("node[2].channel.close " .. base + SenseL_Ds)
	node[2].channel.close(tostring(base + SenseH_Ss))
	-- print("node[2].channel.close " .. base + SenseH_Ss)

	waitcomplete()
	delay(HoldS)
	-- print("V = " .. smua.measure.v() .. ", I = " .. smua.measure.i() .. ", R = " .. smua.measure.r() .. ", P = " .. smua.measure.p())
	reading = smua.measure.v()
	waitcomplete()
	ShowErrors()
	return reading
end

function ReadNextCh(oldBase, newBase)
	-- print("disconnect sense from oldBase")
	node[2].channel.open(tostring(oldBase + SenseL_Ds))
	-- print("node[2].channel.open " .. oldBase + SenseL_Ds)
	node[2].channel.open(tostring(oldBase + SenseH_Ss))
	-- print("node[2].channel.open " .. oldBase + SenseH_Ss)

	-- print("switch newBase from bias to CCS")
	node[2].channel.open(tostring(newBase + BiasH_G))
	-- print("node[2].channel.open " .. newBase + BiasH_G)
	node[2].channel.close(tostring(newBase + BiasGND_G))
	-- print("node[2].channel.close " .. newBase + BiasGND_G)
	node[2].channel.open(tostring(newBase + BiasGND_S))
	-- print("node[2].channel.open " .. newBase + BiasGND_S)
	node[2].channel.close(tostring(newBase + DriveH_S))
	-- print("node[2].channel.close " .. newBase + DriveH_S)

	-- print("switch oldBase back to bias")
	node[2].channel.open(tostring(oldBase + DriveH_S))
	-- print("node[2].channel.open " .. oldBase + DriveH_S)
	node[2].channel.close(tostring(oldBase + BiasGND_S))
	-- print("node[2].channel.close " .. oldBase + BiasGND_S)
	node[2].channel.open(tostring(oldBase + BiasGND_G))
	-- print("node[2].channel.open " .. oldBase + BiasGND_G)
	node[2].channel.close(tostring(oldBase + BiasH_G))
	-- print("node[2].channel.close " .. oldBase + BiasH_G)

	-- print("switch oldBase sense lines back to ground")
	node[2].channel.close(tostring(oldBase + BiasGND_Ss))
	-- print("node[2].channel.close " .. oldBase + BiasGND_Ss)
	node[2].channel.close(tostring(oldBase + BiasGND_Ds))
	-- print("node[2].channel.close " .. oldBase + BiasGND_Ds)

	-- print("Switch newBase to the sense lines")
	node[2].channel.open(tostring(newBase + BiasGND_Ss))
	-- print("node[2].channel.open " .. newBase + BiasGND_Ss)
	node[2].channel.open(tostring(newBase + BiasGND_Ds))
	-- print("node[2].channel.open " .. newBase + BiasGND_Ds)
	node[2].channel.close(tostring(newBase + SenseL_Ds))
	-- print("node[2].channel.close " .. newBase + SenseL_Ds)
	node[2].channel.close(tostring(newBase + SenseH_Ss))
	-- print("node[2].channel.close " .. newBase + SenseH_Ss)

	delay(HoldS)
	--print("V = " .. smua.measure.v() .. ", I = " .. smua.measure.i() .. ", R = " .. smua.measure.r() .. ", P = " .. smua.measure.p())
	nextReading = smua.measure.v()
	waitcomplete()
	ShowErrors()
	return nextReading
end

function EndReadCycle(base)
	-- turn off CCS
	smua.source.output = smua.OUTPUT_OFF

	-- disconnect sense
	node[2].channel.open(tostring(base + SenseL_Ds))
	--print("node[2].channel.open " .. base + SenseL_Ds)
	node[2].channel.open(tostring(base + SenseH_Ss))
	--print("node[2].channel.open " .. base + SenseH_Ss)

	-- switch base back to bias
	node[2].channel.open(tostring(base + DriveH_S))
	--print("node[2].channel.open " .. base + DriveH_S)
	node[2].channel.close(tostring(base + BiasGND_S))
	--print("node[2].channel.close " .. base + BiasGND_S)
	node[2].channel.open(tostring(base + BiasGND_G))
	--print("node[2].channel.open " .. base + BiasGND_G)
	node[2].channel.close(tostring(base + BiasH_G))
	--print("node[2].channel.close " .. base + BiasH_G)

	-- switch base sense lines back to ground
	node[2].channel.close(tostring(base + BiasGND_Ss))
	--print("node[2].channel.close " .. base + BiasGND_Ss)
	node[2].channel.close(tostring(base + BiasGND_Ds))
	--print("node[2].channel.close " .. base + BiasGND_Ds)
	ShowErrors()
end

function LoadSettings()
	paramFile, paramError = io.open("/usb1/TestParams", "r")
	if paramError == nil then
		SamplingI, BiasV, HoldS = paramFile:read("*n", "*n", "*n")
		io.close(paramFile)
	else
		print(paramError)
	end
	ShowErrors()
	print("Sampling current: " .. SamplingI)
	print("Bias voltage: " .. BiasV)
	print("Sampling hold time: " .. HoldS)
end

function SaveSettings()
	-- format.asciiprecision = 8 ineffective
	paramFile, paramError = io.open("/usb1/TestParams", "w")
	if paramError == nil then
		--paramFile:write(string.format("%f %f %f", SamplingI, BiasV, HoldS))
		paramFile:write(SamplingI, ' ', BiasV, ' ', HoldS)
		io.close(paramFile)
	else
		print(paramError)
	end
	ShowErrors()
end

function SetSampleI(NewI)
	SamplingI = NewI
	SaveSettings()
end

function GetSampleI()
	print("Sampling Current: " .. SamplingI)
end

function SetBias(NewV)
	BiasV = NewV
	SaveSettings()
end

function GetBias()
	print("Bias: " .. BiasV)
end

function BiasChannel(i)
	node[2].channel.close(tostring(ChBases[i] + BiasH_G))
	-- print("node[2].channel.close " .. ChBases[i] + BiasH_G)
	node[2].channel.close(tostring(ChBases[i] + BiasGND_S))
	-- print("node[2].channel.close " .. ChBases[i] + BiasGND_S)
	node[2].channel.close(tostring(ChBases[i] + BiasGND_Ss))
	-- print("node[2].channel.close " .. ChBases[i] + BiasGND_Ss)
	node[2].channel.close(tostring(ChBases[i] + BiasGND_Ds))
	-- print("node[2].channel.close " .. ChBases[i] + BiasGND_Ds)
	ShowErrors()
end

function UnbiasChannel(i)
	node[2].channel.open(tostring(ChBases[i] + BiasH_G))
	-- print("node[2].channel.open " .. ChBases[i] + BiasH_G)
	node[2].channel.open(tostring(ChBases[i] + BiasGND_S))
	-- print("node[2].channel.open " .. ChBases[i] + BiasGND_S)
	node[2].channel.open(tostring(ChBases[i] + BiasGND_Ss))
	-- print("node[2].channel.open " .. ChBases[i] + BiasGND_Ss)
	node[2].channel.open(tostring(ChBases[i] + BiasGND_Ds))
	-- print("node[2].channel.open " .. ChBases[i] + BiasGND_Ds)
	ShowErrors()
end

function SetDelay(NewD)
	HoldS = NewD
	SaveSettings()
end

function GetDelay()
	print("Delay: " .. HoldS)
end

function FindActiveChannels()
	-- use bias and sense lines to see if the response is in the valid RadFET range
	for i = 1, 168 do
		if SlotActive[math.floor(ChBases[i]/SlotOffset)] then
			-- turn off CCS just in case, should be off already
			smua.source.output = smua.OUTPUT_OFF

			-- Bias relays are expected to be open already, but open them to be sure
			UnbiasChannel(i)

			node[2].channel.close(tostring(ChBases[i] + BiasGND_G))
			-- print("node[2].channel.close " .. ChBases[i] + BiasGND_G)
			node[2].channel.close(tostring(ChBases[i] + DriveH_S))
			-- print("node[2].channel.close " .. ChBases[i] + DriveH_S)
			-- print("turn on CCS")
			smua.source.output = smua.OUTPUT_ON

			-- print("Switch first channel to the sense lines")
			node[2].channel.close(tostring(ChBases[i] + SenseL_Ds))
			-- print("node[2].channel.close " .. ChBases[i] + SenseL_Ds)
			node[2].channel.close(tostring(ChBases[i] + SenseH_Ss))
			-- print("node[2].channel.close " .. ChBases[i] + SenseH_Ss)

			waitcomplete()
			-- print("V = " .. smua.measure.v() .. ", I = " .. smua.measure.i() .. ", R = " .. smua.measure.r() .. ", P = " .. smua.measure.p())
			loadR = smua.measure.r()
			waitcomplete()
			if loadR < 20000 then
				ChActive[i] = true
				print("Found channel " .. i .. " at base " .. ChBases[i] .. " active. R = " .. loadR)
			else
				ChActive[i] = false
				print("Found channel " .. i .. " at base " .. ChBases[i] .. " inactive. R = " .. loadR)
			end
			smua.source.output = smua.OUTPUT_OFF
			node[2].channel.open(tostring(ChBases[i] + BiasGND_G))
			node[2].channel.open(tostring(ChBases[i] + DriveH_S))
			node[2].channel.open(tostring(ChBases[i] + SenseL_Ds))
			node[2].channel.open(tostring(ChBases[i] + SenseH_Ss))
			ShowErrors()
		end
	end
end

--[[
function DisableChannel()
	print("insert contents here")
end

function EnableChannel()
	print("insert contents here")
end
]]

function ReadAll()
	Ch = 1
	while ChBases[Ch] do
		if ChActive[Ch] == true then
			break
		end
		Ch = Ch + 1
	end
	if not ChActive[Ch] then
		print("Error: No active channels found")
		exit()
	end
	ShowErrors()

	rawV = ReadFirstCh(ChBases[Ch])
	print("Sensor " .. Ch .. " " .. rawV)

	LastCh = Ch
	while ChBases[Ch] do
		Ch = Ch + 1
		-- print("Is ch. " .. Ch .. " active?")
		if ChActive[Ch] == true then
			rawV = ReadNextCh(ChBases[LastCh], ChBases[Ch])
			print("Sensor " .. Ch .. " " .. rawV)
			LastCh = Ch
		--else
			--print("No.")
		end
	end

	EndReadCycle(ChBases[LastCh])
end

function RadFETinit()
	tsplink.reset()
	tsplink.reset(2)
	-- check all 6 slots for pseudocards (no real hardware) and set to simulate
	-- avoids errors if you try to close relays that aren't installed
	for i=1, 6 do
		print("Probing slot " .. i .. "...")
		SlotActive[i] = false
		if node[2].slot[i].pseudocard then
			if node[2].slot[i].pseudocard ~= 37320 then
				node[2].slot[i].pseudocard = 37320
			end
			print("pseudocard " .. i .. ": " .. node[2].slot[i].pseudocard)
		elseif string.sub(node[2].slot[i].idn, 1, 14) == "3732,Quad 4x28" then
			print("idn " .. i .. ": " .. node[2].slot[i].idn)
			-- close relays to source meter
			node[2].channel.close(tostring(i)..'0911, '..tostring(i)..'0912, '..tostring(i)..'0913, '..tostring(i)..'0914, '..tostring(i)..'0915, '..tostring(i)..'0916, '..tostring(i)..'0917, '..tostring(i)..'0918')
			SlotActive[i] = true
		else
			print("Unknown card: " .. node[2].slot[i].idn)
		end
		ShowErrors()
	end
	print("")
	-- set up source meter outputs
	LoadSettings()
	smua.source.autorangev = smua.AUTORANGE_ON
	smua.source.func = smua.OUTPUT_DCAMPS
	smua.source.leveli = SamplingI
	smua.measure.nplc = 2 -- number of power line cycles to sample over

	smub.source.autorangei = smub.AUTORANGE_ON
	smub.source.func = smub.OUTPUT_DCVOLTS
	smub.source.output = smub.OUTPUT_ON
	smub.source.levelv = BiasV
	ShowErrors()

	-- create map of channel numbers to base address
	print("Mapping channel base addresses")
	MapChToBase()

	-- scan for which channels are connected, flag as active
	print("Probing for active channels")
	FindActiveChannels()

	-- bias channels which are active
	print("Biasing active channels")
	for i=1, 168 do
		if ChActive[i] then
			print("Biasing ch. " .. i)
			BiasChannel(i)
		end
		ShowErrors()
	end
	print("RadFET initialization complete.")
	ShowErrors()
	errorqueue.clear()
end

function BiasQuad(base)
	for i=0, 6 do
		ActiveBase = base + i * ChannelInterval
		node[2].channel.close(tostring(ActiveBase + BiasH_G))
		-- print("node[2].channel.close " .. ActiveBase + BiasH_G)
		node[2].channel.close(tostring(ActiveBase + BiasGND_S))
		-- print("node[2].channel.close " .. ActiveBase + BiasGND_S)
		node[2].channel.close(tostring(ActiveBase + BiasGND_Ss))
		-- print("node[2].channel.close " .. ActiveBase + BiasGND_Ss)
		node[2].channel.close(tostring(ActiveBase + BiasGND_Ds))
		-- print("node[2].channel.close " .. ActiveBase + BiasGND_Ds)
	end
	-- print(node[2].channel.getclose('slot1'))
	print("All channels biased, check LEDs")
end

function PCBtest(quad)
	print("Testing quad " .. quad)
	node[2].channel.open('allslots')
	smub.source.output = smub.OUTPUT_ON

	-- close connections to source meter
	node[2].channel.exclusiveclose('10911, 10912, 10913, 10914, 10915, 10916, 10917, 10918')
	-- node[2].channel.close('10911, 10912, 10913, 10914, 10915, 10916, 10917, 10918')
	-- print(node[2].channel.getclose('slot1'))

	-- calculate base address
	QuadBaseAddr = QuadToBase(quad)
	-- bias that quad
	BiasQuad(QuadBaseAddr)
	delay(1)
	-- initial read sequence
	ActiveBase = QuadBaseAddr
	print("Voltage readings:")
	print("Channel " .. (quad - 1) * 7 + 1 .. ": ")
	QuadChReading = ReadFirstCh(ActiveBase)
	-- print(node[2].channel.getclose('slot1'))
	-- print("Channel " .. (quad - 1) * 7 + 1 .. ": " .. QuadChReading)

	----XXXXX For testing state after first reading
	--exit()
	----XXXXX

	-- loop through next 6 channels of quad
	for i=1, 6 do
		-- read next sequence and print
		PrevBase = ActiveBase
		ActiveBase = QuadBaseAddr + i * ChannelInterval
		print("Channel " .. (quad - 1) * 7 + i + 1 .. ": ")
		QuadChReading = ReadNextCh(PrevBase, ActiveBase)
		-- print(node[2].channel.getclose('slot1'))
		-- print("Channel " .. (quad - 1) * 7 + i + 1 .. ": " .. QuadChReading)

	end

	-- end read cycle
	EndReadCycle(ActiveBase)
	-- print("Done test cycle, LEDs still on")
	--delay(3)
	--print("All off")
	--smub.source.output = smub.OUTPUT_OFF
	--node[2].channel.open('allslots')
end

print("RadFET software Loaded")
RadFETinit()