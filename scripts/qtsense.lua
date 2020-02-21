--------------------------------------
------------ Do not edit! ------------
--------------------------------------
if QtSense then return end
QtSense = true

local OnConnect = {}
local OnEventCheck = {}
local Msg, DebugMsg, CustomMsg
--------------------------------------
--------------------------------------


--------------------------------------
------- Change these values if -------
------ you want to connect to a ------
--------- different IP/Port. ---------
--------------------------------------
local Server = {
	IP = '127.0.0.1',
	Port = 27970
}
--------------------------------------
--------------------------------------
--------------------------------------


function OnEventCheck:Seasons()
	local currentSeason = math.floor(df.global.cur_year_tick/100800)
	if self.lastSeason ~= currentSeason then
		CustomMsg("SEASON", currentSeason)
		self.lastSeason = currentSeason
	end
end

function OnEventCheck:Weather()
	local diffWeathers = {}

	for x=0, #df.global.current_weather-1 do
		for y=0, #df.global.current_weather[x]-1 do
			diffWeathers[df.global.current_weather[x][y] or 0] = true
		end
	end
end


local Eventful=require('plugins.eventful')
Eventful.onWorkshopFillSidebarMenu.QtSense =
function(...)
	DebugMsg("onWorkshopFillSidebarMenu", ...)
end

Eventful.onBuildingCreatedDestroyed.QtSense =
function(...)
	DebugMsg("onBuildingCreatedDestroyed", ...)
end
Eventful.enableEvent(Eventful.eventType.BUILDING,1)

Eventful.onConstructionCreatedDestroyed.QtSense =
function(...)
	DebugMsg("onConstructionCreatedDestroyed", ...)
end
Eventful.enableEvent(Eventful.eventType.CONSTRUCTION,1)

Eventful.onJobInitiated.QtSense =
function(job)
	local jobUser = dfhack.job.getWorker(job)
	if jobUser and dfhack.units.isVisible(jobUser) then
		CustomMsg("JOBSTART", dfhack.job.getName(job))
	end
end
Eventful.enableEvent(Eventful.eventType.JOB_INITIATED,1)

--[[
Eventful.onJobCompleted.QtSense =
function(job)
	CustomMsg("JOBCOMPLETED", dfhack.job.getName(job))
end
Eventful.enableEvent(Eventful.eventType.JOB_COMPLETED,1)
]]

Eventful.onUnitDeath.QtSense =
function(...)
	DebugMsg("onUnitDeath", ...)
end
Eventful.enableEvent(Eventful.eventType.UNIT_DEATH,1)

Eventful.onItemCreated.QtSense =
function(...)
	DebugMsg("onItemCreated", ...)
end
Eventful.enableEvent(Eventful.eventType.ITEM_CREATED,1)

Eventful.onSyndrome.QtSense =
function(...)
	DebugMsg("onSyndrome", ...)
end
Eventful.enableEvent(Eventful.eventType.SYNDROME,1)

Eventful.onInvasion.QtSense =
function(...)
	DebugMsg("onInvasion", ...)
end
Eventful.enableEvent(Eventful.eventType.INVASION,1)

--[[
Eventful.onInventoryChange.QtSense =
function(...)
	DebugMsg("onInventoryChange", ...)
end
Eventful.enableEvent(Eventful.eventType.INVENTORY_CHANGE,1)
]]

Eventful.onReport.QtSense =
function(...)
	DebugMsg("onReport", ...)
end
Eventful.enableEvent(Eventful.eventType.REPORT,1)

Eventful.onUnitAttack.QtSense =
function(...)
	DebugMsg("onUnitAttack", ...)
end
Eventful.enableEvent(Eventful.eventType.UNIT_ATTACK,1)

Eventful.onUnload.QtSense =
function(...)
	DebugMsg("onUnload")
end
Eventful.enableEvent(Eventful.eventType.UNLOAD,1)

Eventful.onInteraction.QtSense =
function(...)
	DebugMsg("onInteraction", ...)
end
Eventful.enableEvent(Eventful.eventType.INTERACTION,1)

--------------------------------------
------------ Do not edit! ------------
--------------------------------------
-- Unless you know what you're doing.
--------------------------------------
local clsocket = require("plugins.luasocket")
local socket = nil
local serverVersion = nil

function SocketClose()
	if socket ~= nil then
		print("QtSense: Connection lost.")
		pcall(socket.close, socket)
		socket = nil
		serverVersion = nil
	end
end

function SocketSend(data)
	if socket == nil then return false end
	local rval, err = pcall(socket.send, socket, data)
	if not rval then
		SocketClose()
		return false
	end
	return true
end

-- *l = Read Line
-- *a = Read Everything
-- number = Read N bytes

function SocketRead(mode)
	if socket == nil then return false end
	local rval, data = pcall(socket.receive, socket, mode)
	if not rval then
		SocketClose()
		return false
	end
	return true, data
end

function SocketReadTimeout(mode, Time)
	local status, data = SocketRead(mode)
	if status == false then return false end
	if data ~= nil then return true, data end

	local t0 = os.clock()
	while os.clock() - t0 <= Time do
		status, data = SocketRead(mode)
		if status == false then
			return false
		end
		if data ~= nil then
			return true, data
		end
	end
	return true, nil
end

function BusyWait(Time)
	local t0 = os.clock()
	while os.clock() - t0 <= Time do end
end

local EventCheckStorage = {}

function TryConnect(ip, port)
	SocketClose()

	local status, sck = pcall(clsocket.tcp.connect, clsocket.tcp, ip, port)
	if not status then
		return false
	end
	socket = sck

	print("QtSense: Connecting...")

	local statusA, rdA = SocketReadTimeout("*l", 2)
	local statusB, rdB = SocketReadTimeout("*l", 1)

	print(statusA, rdA)
	print(statusB, rdB)
	if	(not statusA) or
		(not statusB) or
		(rdB ~= "READY")
	then
		SocketClose()
		return false
	end

	serverVersion = string.match(rdA or "", "^QtSense%s*(.-)%s*$")
	if serverVersion == nil then
		SocketClose()
		return false
	end

	if not string.match(serverVersion, "^1.0.(.*)$") then
		print("QtSense: Version not supported: " .. serverVersion)
		SocketClose()
		return false
	end

	print("QtSense: Connected (v. " .. serverVersion .. ")")
	for k,v in ipairs(OnConnect) do
		v()
	end

	return true
end

local PacketManager = {
	{
		Name = "PongManager", -- Optional Field
		Match = "PONG",
		Function = function(data, me)
			-- Pong Received. Do nothing.
		end
	}
}

function CheckForEvents()
	for k,v in pairs(OnEventCheck) do
		EventCheckStorage[v] = EventCheckStorage[v] or {}
		v(EventCheckStorage[v])
	end
end

function ServerReader()
	if socket == nil then return end
	local readdata = nil
	while true do
		readdata = SocketRead("*l")
		if type(readdata) ~= "string" then break end
		for k,v in ipairs(PacketManager) do
			if string.match(readdata, v.Match) then
				v.Function(readdata, v)
			end
		end
	end
end

function EscapedMessage(...)
	local args = {...}

	local FinalArgs = {}
	for k,v in ipairs(args) do
		if type(v) ~= "string" then
			local s, v = pcall(tostring, v)
			if not s then v = "{object}" end
		end

		FinalArgs[k] = string.gsub(v, "<>&\n\r", {
			["&"] = "&1",
			["|"] = "&2",
			["\n"] = "&3",
			["\r"] = "&4"
		})
	end

	return table.concat(FinalArgs, "|")
end

Msg = function(m)
	SocketSend(
		EscapedMessage("MSG", m)
		.. "\n"
	)
end

DebugMsg = function(...)
	SocketSend(
		EscapedMessage("DBG", ...)
		.. "\n"
	)
end

CustomMsg = function(...)
	SocketSend(
		EscapedMessage("CMG", ...)
		.. "\n"
	)
end

function Ping()
	SocketSend("PING\n")
end


function AutoUpdate()
	dfhack.timeout(1500, 'frames', AutoUpdate)
	if socket == nil then return end
	ServerReader()
	Ping()
end
AutoUpdate()

function AutoEventCheck()
	dfhack.timeout(30, 'frames', AutoEventCheck)
	if socket == nil then return end
	CheckForEvents()
end

function AutoTryConnect()
	dfhack.timeout(1000, 'frames', AutoTryConnect)
	if socket ~= nil then return end
	TryConnect(Server.IP, Server.Port)
end
AutoTryConnect()
