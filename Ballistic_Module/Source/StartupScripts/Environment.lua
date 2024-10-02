--!native
--!optimize 2

-- Alright guys considering you enjoy pretending like you did something by spreading our init script around, just know that we don't obfuscate it because we don't give a flying fuck.

if not game:IsLoaded() then game["Loaded"]:Wait() end

local VirtualInputManager = game:GetService("VirtualInputManager")
local UserInputService = game:GetService("UserInputService")
local InsertService = game:GetService("InsertService")
local RunService = game:GetService("RunService")
local Players = game:GetService("Players")

local ExploitInfo = table.freeze({
    ["Name"] = "%EXPLOIT_NAME%",
})

local ClientInfo = {
	["IsWindowFocused"] = true
}

local RobloxEnvironment = table.freeze({
	["print"] = print, ["warn"] = warn, ["error"] = error, ["assert"] = assert, ["collectgarbage"] = collectgarbage, ["require"] = require,
	["select"] = select, ["tonumber"] = tonumber, ["tostring"] = tostring, ["type"] = type, ["xpcall"] = xpcall,
	["pairs"] = pairs, ["next"] = next, ["ipairs"] = ipairs, ["newproxy"] = newproxy, ["rawequal"] = rawequal, ["rawget"] = rawget,
	["rawset"] = rawset, ["rawlen"] = rawlen, ["gcinfo"] = gcinfo,

	["coroutine"] = {
		["create"] = coroutine["create"], ["resume"] = coroutine["resume"], ["running"] = coroutine["running"],
		["status"] = coroutine["status"], ["wrap"] = coroutine["wrap"], ["yield"] = coroutine["yield"],
	},

	["bit32"] = {
		["arshift"] = bit32["arshift"], ["band"] = bit32["band"], ["bnot"] = bit32["bnot"], ["bor"] = bit32["bor"], ["btest"] = bit32["btest"],
		["extract"] = bit32["extract"], ["lshift"] = bit32["lshift"], ["replace"] = bit32["replace"], ["rshift"] = bit32["rshift"], ["xor"] = bit32["xor"],
	},

	["math"] = {
		["abs"] = math["abs"], ["acos"] = math["acos"], ["asin"] = math["asin"], ["atan"] = math["atan"], ["atan2"] = math["atan2"], ["ceil"] = math["ceil"],
		["cos"] = math["cos"], ["cosh"] = math["cosh"], ["deg"] = math["deg"], ["exp"] = math["exp"], ["floor"] = math["floor"], ["fmod"] = math["fmod"],
		["frexp"] = math["frexp"], ["ldexp"] = math["ldexp"], ["log"] = math["log"], ["log10"] = math["log10"], ["max"] = math["max"], ["min"] = math["min"],
		["modf"] = math["modf"], ["pow"] = math["pow"], ["rad"] = math["rad"], ["random"] = math["random"], ["randomseed"] = math["randomseed"],
		["sin"] = math["sin"], ["sinh"] = math["sinh"], ["sqrt"] = math["sqrt"], ["tan"] = math["tan"], ["tanh"] = math["tanh"]
	},

	["string"] = {
		["byte"] = string["byte"], ["char"] = string["char"], ["find"] = string["find"], ["format"] = string["format"], ["gmatch"] = string["gmatch"],
		["gsub"] = string["gsub"], ["len"] = string["len"], ["lower"] = string["lower"], ["match"] = string["match"], ["pack"] = string["pack"],
		["packsize"] = string["packsize"], ["rep"] = string["rep"], ["reverse"] = string["reverse"], ["sub"] = string["sub"],
		["unpack"] = string["unpack"], ["upper"] = string["upper"],
	},

	["table"] = {
		["clone"] = table.clone, ["concat"] = table.concat, ["insert"] = table.insert, ["pack"] = table.pack, ["remove"] = table.remove, ["sort"] = table.sort,
		["unpack"] = table.unpack,
	},

	["utf8"] = {
		["char"] = utf8["char"], ["charpattern"] = utf8["charpattern"], ["codepoint"] = utf8["codepoint"], ["codes"] = utf8["codes"],
		["len"] = utf8["len"], ["nfdnormalize"] = utf8["nfdnormalize"], ["nfcnormalize"] = utf8["nfcnormalize"],
	},

	["os"] = {
		["clock"] = os["clock"], ["date"] = os["date"], ["difftime"] = os["difftime"], ["time"] = os["time"],
	},

	["delay"] = delay, ["elapsedTime"] = elapsedTime, ["spawn"] = spawn, ["tick"] = tick, ["time"] = time, ["typeof"] = typeof,
	["UserSettings"] = UserSettings, ["version"] = version, ["wait"] = wait, ["_VERSION"] = _VERSION,

	["task"] = {
		["defer"] = task["defer"], ["delay"] = task["delay"], ["spawn"] = task["spawn"], ["wait"] = task["wait"], ["cancel"] = task["cancel"]
	},

	["debug"] = {
		["traceback"] = debug["traceback"], ["profilebegin"] = debug["profilebegin"], ["profileend"] = debug["profileend"],
	},

	["game"] = game, ["workspace"] = workspace, ["Game"] = game, ["Workspace"] = workspace,

	["getmetatable"] = getmetatable, ["setmetatable"] = setmetatable
})

-- Returns local asset.
getgenv()["getobjects"] = newcclosure(function(Asset)
    return { InsertService:LoadLocalAsset(Asset) }
end)

getgenv()["get_objects"] = getobjects
getgenv()["GetObjects"] = getobjects

-- Returns the script responsible for the currently running function.
getgenv()["getcallingscript"] = (function() return getgenv(0)["script"] end)
getgenv()["get_calling_script"] = getcallingscript
getgenv()["GetCallingScript"] = getcallingscript

-- Generates a new closure using the bytecode of script.
getgenv()["getscriptclosure"] = (function(script)
	return function()
		return getrenv()["table"].clone(getrenv().require(script))
	end
end)

getgenv()["get_script_closure"] = getscriptclosure
getgenv()["GetScriptClosure"] = getscriptclosure

getgenv()["getscriptfunction"] = getscriptclosure
getgenv()["get_script_function"] = getscriptclosure
getgenv()["GetScriptFunction"] = getscriptclosure

local function RobloxConsoleHandler(...)
	warn("Disabled \"rconsole\" library.")
end

getgenv()["rconsoleclear"] = newcclosure(RobloxConsoleHandler)
getgenv()["consoleclear"] = rconsoleclear
getgenv()["console_clear"] = rconsoleclear
getgenv()["ConsoleClear"] = rconsoleclear

getgenv()["rconsolecreate"] = newcclosure(RobloxConsoleHandler)
getgenv()["consolecreate"] = rconsolecreate
getgenv()["console_create"] = rconsolecreate
getgenv()["ConsoleCreate"] = rconsolecreate

getgenv()["rconsoledestroy"] = newcclosure(RobloxConsoleHandler)
getgenv()["consoledestroy"] = rconsoledestroy
getgenv()["console_destroy"] = rconsoledestroy
getgenv()["ConsoleDestroy"] = rconsoledestroy

getgenv()["rconsoleinput"] = newcclosure(RobloxConsoleHandler)
getgenv()["consoleinput"] = rconsoleinput
getgenv()["console_input"] = rconsoleinput
getgenv()["ConsoleInput"] = rconsoleinput

getgenv()["rconsoleprint"] = newcclosure(RobloxConsoleHandler)
getgenv()["consoleprint"] = rconsoleprint
getgenv()["console_print"] = rconsoleprint
getgenv()["ConsolePrint"] = rconsoleprint

getgenv()["rconsolesettitle"] = newcclosure(RobloxConsoleHandler)
getgenv()["rconsolename"] = rconsolesettitle
getgenv()["consolesettitle"] = rconsolesettitle
getgenv()["console_set_title"] = rconsolesettitle
getgenv()["ConsoleSetTitle"] = rconsolesettitle

-- Returns the global environment of the game client. It can be used to access the global functions that LocalScripts and ModuleScripts use.
getgenv()["getrenv"] = newcclosure(function() return RobloxEnvironment end)

-- Returns whether the game's window is in focus. Must be true for other input functions to work.
getgenv()["isrbxactive"] = newcclosure(function()
    return ClientInfo["IsWindowFocused"]
end)

getgenv()["is_rbx_active"] = isrbxactive
getgenv()["IsRbxActive"] = isrbxactive

getgenv()["isgameactive"] = isrbxactive
getgenv()["is_game_active"] = isrbxactive
getgenv()["IsGameActive"] = isrbxactive

UserInputService["WindowFocused"]:Connect(function()
    ClientInfo["IsWindowFocused"] = true
end)

UserInputService["WindowFocusReleased"]:Connect(function()
    ClientInfo["IsWindowFocused"] = false
end)

-- Dispatches a left mouse button click.
getgenv()["mouse1click"] = newcclosure(function()
    VirtualInputManager:SendMouseButtonEvent(0, 0, 0, true, game, 1)
    VirtualInputManager:SendMouseButtonEvent(0, 0, 0, false, game, 1)
end)

getgenv()["Mouse1Click"] = mouse1click

-- Dispatches a left mouse button press.
getgenv()["mouse1press"] = newcclosure(function()
    VirtualInputManager:SendMouseButtonEvent(0, 0, 0, true, game, 1)
end)

getgenv()["Mouse1Press"] = mouse1press

-- Dispatches a left mouse button release.
getgenv()["mouse1release"] = newcclosure(function()
    VirtualInputManager:SendMouseButtonEvent(0, 0, 0, false, game, 1)
end)

getgenv()["Mouse1Release"] = mouse1release

-- Dispatches a right mouse button click.
getgenv()["mouse2click"] = newcclosure(function()
    VirtualInputManager:SendMouseButtonEvent(0, 0, 1, true, game, 1)
    VirtualInputManager:SendMouseButtonEvent(0, 0, 1, false, game, 1)
end)

getgenv()["Mouse2Click"] = mouse2click

-- Dispatches a right mouse button press.
getgenv()["mouse2press"] = newcclosure(function()
    VirtualInputManager:SendMouseButtonEvent(0, 0, 1, true, game, 1)
end)

getgenv()["Mouse2Press"] = mouse2press

-- Dispatches a right mouse button release.
getgenv()["mouse2release"] = newcclosure(function()
    VirtualInputManager:SendMouseButtonEvent(0, 0, 1, false, game, 1)
end)

getgenv()["Mouse2Release"] = mouse2release

-- Moves the mouse cursor to the specified absolute position.
getgenv()["mousemoveabs"] = newcclosure(function(x, y)
    VirtualInputManager:SendMouseMoveEvent(x, y, game)
end)

getgenv()["MouseMoveAbs"] = mousemoveabs

-- Adjusts the mouse cursor by the specified relative amount.
getgenv()["mousemoverel"] = newcclosure(function(x, y)
    local MouseLocation = UserInputService:GetMouseLocation()
    VirtualInputManager:SendMouseMoveEvent(MouseLocation.X + x, MouseLocation.Y + y, game)
end)

getgenv()["MouseMoveRel"] = mousemoverel

-- Dispatches a mouse scroll by the specified number of pixels.
getgenv()["mousescroll"] = newcclosure(function(Pixels)
    VirtualInputManager:SendMouseWheelEvent(0, 0, Pixels > 0, game)
end)

getgenv()["MouseScroll"] = mousescroll

--[[
	Dispatches a click or hover event to the given ClickDetector. When absent, `distance` defaults to zero, and `event` defaults to "MouseClick".
	Possible input events include 'MouseClick', 'RightMouseClick', 'MouseHoverEnter', and 'MouseHoverLeave'.
]]
getgenv()["fireclickdetector"] = newcclosure(function(Target)
	assert(typeof(Target) == "Instance", "invalid argument #1 to 'fireclickdetector' (Instance expected, got " .. type(Target) .. ") ", 2)
	local ClickDetector = Target:FindFirstChild("ClickDetector") or Target
	local PreviousParent = ClickDetector["Parent"]

	local NewPart = Instance.new("Part", getrenv()["workspace"])
	do
		NewPart["Transparency"] = 1
		NewPart["Size"] = Vector3.new(30, 30, 30)
		NewPart["Anchored"] = true
		NewPart["CanCollide"] = false
		getrenv()["task"].delay(15, function()
			if NewPart:IsDescendantOf(getrenv()["game"]) then
				NewPart:Destroy()
			end
		end)
		ClickDetector["Parent"] = NewPart
		ClickDetector["MaxActivationDistance"] = math.huge
	end

	local VirtualUser = getrenv()["game"]:FindService("VirtualUser") or getrenv()["game"]:GetService("VirtualUser")

	local HeartbeatConnection = RunService["Heartbeat"]:Connect(function()
		local CurrentCamera = getrenv()["workspace"]["CurrentCamera"] or getrenv()["workspace"]["Camera"]
		NewPart["CFrame"] = CurrentCamera["CFrame"] * CFrame.new(0, 0, -20) * CFrame.new(CurrentCamera["CFrame"]["LookVector"]['X'], CurrentCamera["CFrame"]["LookVector"]['Y'], CurrentCamera["CFrame"]["LookVector"]['Z'])
		VirtualUser:ClickButton1(Vector2.new(20, 20), CurrentCamera["CFrame"])
	end)

	ClickDetector["MouseClick"]:Once(function()
		HeartbeatConnection:Disconnect()
		ClickDetector["Parent"] = PreviousParent
		NewPart:Destroy()
	end)
end)

getgenv()["fire_click_detector"] = fireclickdetector
getgenv()["FireClickDetector"] = fireclickdetector

-- Dispatches a ProximityPrompt.
getgenv()["fireproximityprompt"] = (function(ProximityPrompt, Amount, Skip)
	assert(typeof(ProximityPrompt) == "Instance", "invalid argument #1 to 'fireproximityprompt' (Instance expected, got " .. typeof(ProximityPrompt) .. ") ", 2)
	assert(ProximityPrompt:IsA("ProximityPrompt"), "invalid argument #1 to 'fireproximityprompt' (ProximityPrompt expected, got " .. ProximityPrompt["ClassName"] .. ") ", 2)

	Amount = Amount or 1
	Skip = Skip or false

	assert(type(Amount) == "number", "invalid argument #2 to 'fireproximityprompt' (number expected, got " .. type(Amount) .. ") ", 2)
	assert(type(Skip) == "boolean", "invalid argument #2 to 'fireproximityprompt' (boolean expected, got " .. type(Amount) .. ") ", 2)

	local OldHoldDuration = ProximityPrompt.HoldDuration
	local OldMaxDistance = ProximityPrompt.MaxActivationDistance

	ProximityPrompt["MaxActivationDistance"] = 9e9
	ProximityPrompt:InputHoldBegin()

	for i = 1, Amount or 1 do
		if Skip then
			ProximityPrompt["HoldDuration"] = 0
		else
			getrenv()["task"].wait(ProximityPrompt["HoldDuration"] + 0.01)
		end
	end

	ProximityPrompt:InputHoldEnd()
	ProximityPrompt["MaxActivationDistance"] = OldMaxDistance
	ProximityPrompt["HoldDuration"] = OldHoldDuration
end)

getgenv()["fire_proximity_prompt"] = fireproximityprompt
getgenv()["FireProximityPrompt"] = fireproximityprompt

-- Creates a list of Connection objects for the functions connected to `signal`.
getgenv()["getconnections"] = newcclosure(function(Event)
    assert(typeof(Event) == "RBXScriptSignal", "Argument must be a RBXScriptSignal")

    local Connections = {}

    local Connection = Event:Connect(function() end)
    
    local ConnectionInfo = {
        ["Enabled"] = true,
        ["ForeignState"] = false,
        ["LuaConnection"] = true,
        ["Function"] = function() end,
        ["Thread"] = getrenv()["coroutine"].create(function() end),
        ["Fire"] = function() Connection:Fire() end,
        ["Defer"] = function() task.defer(Connection["Fire"], Connection) end,
        ["Disconnect"] = function() Connection:Disconnect() end,
        ["Disable"] = function() ConnectionInfo["Enabled"] = false end,
        ["Enable"] = function() ConnectionInfo["Enabled"] = true end,
    }

    getrenv()["table"].insert(Connections, ConnectionInfo)

    Connection:Disconnect()
    return Connections
end)

getgenv()["get_connections"] = getconnections
getgenv()["GetConnections"] = getconnections

local _isscriptable = clonefunction(isscriptable)
local _setscriptable = clonefunction(setscriptable)
local ScriptableCache = {}

--[[
	Returns the value of a hidden property of `object`, which cannot be indexed normally.
	If the property is hidden, the second return value will be `true`. Otherwise, it will be `false`.
]]
getgenv()["gethiddenproperty"] = newcclosure(function(Inst, Idx) 
	assert(typeof(Inst) == "Instance", "invalid argument #1 to 'gethiddenproperty' [Instance expected]", 2);
	local Was = _isscriptable(Inst, Idx);
	_setscriptable(Inst, Idx, true)

	local Value = Inst[Idx]
	_setscriptable(Inst, Idx, Was)

	return Value, not Was
end)

getgenv()["get_hidden_property"] = gethiddenproperty
getgenv()["GetHiddenProperty"] = gethiddenproperty

-- Sets the value of a hidden property of `object`, which cannot be set normally. Returns whether the property was hidden.
getgenv()["sethiddenproperty"] = (function(Inst, Idx, Value) 
	assert(typeof(Inst) == "Instance", "invalid argument #1 to 'sethiddenproperty' [Instance expected]", 2);
	local Was = _isscriptable(Inst, Idx);
	_setscriptable(Inst, Idx, true)

	Inst[Idx] = Value

	_setscriptable(Inst, Idx, Was)

	return not Was
end)

getgenv()["set_hidden_property"] = sethiddenproperty
getgenv()["SetHiddenProperty"] = sethiddenproperty

getgenv()["isscriptable"] = newcclosure(function(Inst: Instance, Property: string)
    local InstanceCache = ScriptableCache[Inst]
    if InstanceCache then
        local Value = InstanceCache[Property]
        if Value ~= nil then
            return Value
        end
    end
    return _isscriptable(Inst, Property)
end)

getgenv()["is_scriptable"] = isscriptable
getgenv()["IsScriptable"] = isscriptable

getgenv()["setscriptable"] = newcclosure(function(Inst: Instance, Property: string, Scriptable: boolean)
    local WasScriptable = _isscriptable(Inst, Property)
    if ScriptableCache[Inst] == nil then
        ScriptableCache[Inst] = {}
    end
    ScriptableCache[Inst][Property] = Scriptable
    return WasScriptable
end)

getgenv()["set_scriptable"] = setscriptable
getgenv()["SetScriptable"] = setscriptable

local NewIndex; NewIndex = hookmetamethod(game, "__newindex", function(t, k, v)
    if checkcaller() then
        local Cache = ScriptableCache[t]
        if Cache and Cache[k] ~= nil and _isscriptable(t, k) ~= Cache[k] then
            _setscriptable(t, k, Cache[k])
            local s, r = pcall(function()
                NewIndex(t, k, v)
            end)
            _setscriptable(t, k, not Cache[k])
            if not s then
                error(r)
            end
            return
        end
    end
    NewIndex(t, k, v)
end)

getgenv()["setrbxclipboard"] = setclipboard
getgenv()["set_rbx_clipboard"] = setclipboard
getgenv()["SetRbxClipboard"] = setclipboard

-- Compresses `data` using LZ4 compression.
getgenv()["lz4compress"] = newcclosure(function(Data)
    local Out, i, DataLen = {}, 1, #Data

    while i <= DataLen do
        local BestLen, BestDist = 0, 0

        for Dist = 1, math.min(i - 1, 65535) do
            local MatchStart, Len = i - Dist, 0

            while i + Len <= DataLen and Data:sub(MatchStart + Len, MatchStart + Len) == Data:sub(i + Len, i + Len) do
                Len += 1
                if Len == 65535 then break end
            end

            if Len > BestLen then BestLen, BestDist = Len, Dist end
        end

        if BestLen >= 4 then
            getrenv()["table"].insert(Out, getrenv()["string"].char(0) .. getrenv()["string"].pack(">I2I2", BestDist - 1, BestLen - 4))
            i += BestLen
        else
            local LitStart = i

            while i <= DataLen and (i - LitStart < 15 or i == DataLen) do i += 1 end
            getrenv()["table"].insert(Out, getrenv()["string"].char(i - LitStart) .. Data:sub(LitStart, i - 1))
        end
    end
    return getrenv()["table"].concat(Out)
end)

getgenv()["lz4_compress"] = lz4compress
getgenv()["Lz4Compress"] = lz4compress

-- Decompresses `data` using LZ4 compression, with the decompressed size specified by `size`.
getgenv()["lz4decompress"] = newcclosure(function(Data, Size)
    local Out, i, DataLen = {}, 1, #Data

    while i <= DataLen and #getrenv()["table"].concat(Out) < Size do
        local Token = Data:byte(i)
        i = i + 1

        if Token == 0 then
            local Dist, Len = getrenv()["string"].unpack(">I2I2", Data:sub(i, i + 3))

            i = i + 4
            Dist = Dist + 1
            Len = Len + 4

            local Start = #getrenv()["table"].concat(Out) - Dist + 1
            local Match = getrenv()["table"].concat(Out):sub(Start, Start + Len - 1)

            while #Match < Len do
                Match = Match .. Match
            end

            getrenv()["table"].insert(Out, Match:sub(1, Len))
        else
            getrenv()["table"].insert(Out, Data:sub(i, i + Token - 1))
            i = i + Token
        end
    end

    return getrenv()["table"].concat(Out):sub(1, Size)
end)

getgenv()["lz4_decompress"] = lz4decompress
getgenv()["Lz4Decompress"] = lz4decompress

local function MessageBoxHandler(...)
	warn("Disabled function \"messagebox\".")
end

getgenv()["messagebox"] = newcclosure(MessageBoxHandler)
getgenv()["message_box"] = newcclosure(MessageBoxHandler)
getgenv()["MessageBox"] = newcclosure(MessageBoxHandler)

local function QueueOnTeleportHandler(...)
	warn("Disabled function \"queue_on_teleport\".")
end

getgenv()["queueonteleport"] = newcclosure(QueueOnTeleportHandler)
getgenv()["queue_on_teleport"] = newcclosure(QueueOnTeleportHandler)
getgenv()["QueueOnTeleport"] = newcclosure(QueueOnTeleportHandler)

local CurrentFps, _Task = nil, nil

-- Sets the in-game FPS cap to `fps`. If `fps` is 0, the FPS cap is disabled.
getgenv()["setfpscap"] = newcclosure(function(NewCap)
    if _Task then
        getrenv()["task"].cancel(_Task)
        _Task = nil
    end

    if NewCap and NewCap > 0 and NewCap < 10000 then
        CurrentFps = NewCap
        local Interval = 1 / NewCap

        _Task = getrenv()["task"].spawn(function()
            while true do
                local Start = getrenv()["os"].clock()
                RunService["Heartbeat"]:Wait()
                while getrenv()["os"].clock() - Start < Interval do end
            end
        end)
    else 
        CurrentFps = nil
    end
end)

getgenv()["set_fps_cap"] = setfpscap
getgenv()["SetFpsCap"] = setfpscap

-- Returns the in-game FPS cap.
getgenv()["getfpscap"] = newcclosure(function()
	return getrenv()["workspace"]:GetRealPhysicsFPS()
end)

getgenv()["get_fps_cap"] = getfpscap
getgenv()["GetFpsCap"] = getfpscap

-- Returns a list of ModuleScripts that have been loaded. If `excludeCore` is true, CoreScript-related modules will not be included in the list.
getgenv()["getloadedmodules"] = newcclosure(function()
    local LoadedModules = {}
    for _, v in pairs(getrenv()["game"]:GetDescendants()) do
        if typeof(v) == "Instance" and v:IsA("ModuleScript") then getrenv()["table"].insert(LoadedModules, v) end
    end
    return LoadedModules
end)

getgenv()["get_loaded_modules"] = getloadedmodules
getgenv()["GetLoadedModules"] = getloadedmodules

-- Returns a list of scripts that are currently running.
getgenv()["getrunningscripts"] = newcclosure(function()
    local RunningScripts = {}
    for _, v in ipairs(Players["LocalPlayer"]:GetDescendants()) do
        if v:IsA("LocalScript") or v:IsA("ModuleScript") then
            RunningScripts[#RunningScripts + 1] = v
        end
    end
    return RunningScripts
end)

getgenv()["get_running_scripts"] = getrunningscripts
getgenv()["GetRunningScripts"] = getrunningscripts

getgenv()["getscripts"] = getrunningscripts
getgenv()["get_scripts"] = getrunningscripts
getgenv()["GetScripts"] = getrunningscripts

-- Returns a SHA384 hash of the script's bytecode. This is useful for detecting changes to a script's source code.
getgenv()["getscripthash"] = newcclosure(function(Inst)
    assert(typeof(Inst) == "Instance", "invalid argument #1 to 'getscripthash' (Instance expected, got " .. typeof(Inst) .. ") ", 2)
	assert(Inst:IsA("LuaSourceContainer"), "invalid argument #1 to 'getscripthash' (LuaSourceContainer expected, got " .. Inst["ClassName"] .. ") ", 2)
	return Inst:GetHash()
end)

-- Returns the global environment of the given script. It can be used to access variables and functions that are not defined as local.
getgenv()["getsenv"] = newcclosure(function(script)
    assert(typeof(script) == "Instance", "invalid argument #1 to 'getsenv' [ModuleScript or LocalScript expected]", 2);
	assert((script:IsA("LocalScript") or script:IsA("ModuleScript")), "invalid argument #1 to 'getsenv' [ModuleScript or LocalScript expected]", 2)
	if (script:IsA("LocalScript") == true) then 
		for _, v in getreg() do
			if (type(v) == "function") then
				if getfenv(v)["script"] then
					if getfenv(v)["script"] == script then
						return getfenv(v)
					end
				end
			end
		end
	else
		local Reg = getreg()
		local Senv = {}

		if #Reg == 0 then
			return require(script)
		end

		for _, v in next, Reg do
			if type(v) == "function" and islclosure(v) then
				local Fenv = getfenv(v)
				local Raw = rawget(Fenv, "script")
				if Raw and Raw == script then
					for i, k in next, Fenv do
						if i ~= "script" then
							rawset(Senv, i, k)
						end
					end
				end
			end
		end
		return Senv
	end
end)

local Drawings = {}
local drawings = {}
local camera = game.Workspace.CurrentCamera
local drawingUI = Instance.new("ScreenGui")
local coreGui = game:GetService("CoreGui")
drawingUI.Name = "Drawing"
drawingUI.IgnoreGuiInset = true
drawingUI.DisplayOrder = 0x7fffffff
drawingUI.Parent = coreGui

local drawingIndex = 0
local uiStrokes = table.create(0)
local baseDrawingObj = setmetatable({
	Visible = true,
	ZIndex = 0,
	Transparency = 1,
	Color = Color3.new(),
	Remove = function(self)
		setmetatable(self, nil)
	end
}, {
	__add = function(t1, t2)
		local result = table.clone(t1)

		for index, value in t2 do
			result[index] = value
		end
		return result
	end
})
local drawingFontsEnum = {
	[0] = Font.fromEnum(Enum.Font.Roboto),
	[1] = Font.fromEnum(Enum.Font.Legacy),
	[2] = Font.fromEnum(Enum.Font.SourceSans),
	[3] = Font.fromEnum(Enum.Font.RobotoMono),
}
-- function
local function getFontFromIndex(fontIndex: number): Font
	return drawingFontsEnum[fontIndex]
end

local function convertTransparency(transparency: number): number
	return math.clamp(1 - transparency, 0, 1)
end
-- main
local DrawingLib = {}
DrawingLib.Fonts = {
	["UI"] = 0,
	["System"] = 1,
	["Flex"] = 2,
	["Monospace"] = 3
}

function DrawingLib.new(drawingType)
	drawingIndex += 1
if drawingType == "Line" then
    local lineObj = {
        From = Vector2.zero,
        To = Vector2.zero,
        Thickness = 1
    }

    for key, value in pairs(baseDrawingObj) do
        lineObj[key] = value
    end

    local lineFrame = Instance.new("Frame")
    lineFrame.Name = drawingIndex
    lineFrame.AnchorPoint = Vector2.one * 0.5
    lineFrame.BorderSizePixel = 0

    lineFrame.BackgroundColor3 = lineObj.Color
    lineFrame.Visible = lineObj.Visible
    lineFrame.ZIndex = lineObj.ZIndex
    lineFrame.BackgroundTransparency = convertTransparency(lineObj.Transparency)
    lineFrame.Size = UDim2.new()

    lineFrame.Parent = drawingUI
    local bs = table.create(0) -- Cache table
    table.insert(drawings, bs)

    local function updateLine()
        local direction = lineObj.To - lineObj.From
        local center = (lineObj.To + lineObj.From) / 2
        local distance = direction.Magnitude
        local theta = math.deg(math.atan2(direction.Y, direction.X))

        lineFrame.Position = UDim2.fromOffset(center.X, center.Y)
        lineFrame.Rotation = theta
        lineFrame.Size = UDim2.fromOffset(distance, lineObj.Thickness)
    end

    return setmetatable(bs, {
        __newindex = function(_, index, value)
            if lineObj[index] == nil then return end

            lineObj[index] = value
            if index == "From" or index == "To" or index == "Thickness" then
                updateLine()
            elseif index == "Visible" then
                lineFrame.Visible = value
            elseif index == "ZIndex" then
                lineFrame.ZIndex = value
            elseif index == "Transparency" then
                lineFrame.BackgroundTransparency = convertTransparency(value)
            elseif index == "Color" then
                lineFrame.BackgroundColor3 = value
            end
        end,
        __index = function(self, index)
            if index == "Remove" or index == "Destroy" then
                return function()
                    lineFrame:Destroy()
                    lineObj.Remove(self)
                    for k in pairs(bs) do
                        bs[k] = nil
                    end
                end
            end
            return lineObj[index]
        end
    })
	elseif drawingType == "Text" then
    local textObj = {
        Text = "",
        Font = DrawingLib.Fonts.UI,
        Size = 0,
        Position = Vector2.zero,
        Center = false,
        Outline = false,
        OutlineColor = Color3.new()
    }

    for key, value in pairs(baseDrawingObj) do
        textObj[key] = value
    end

    local textLabel = Instance.new("TextLabel")
    local uiStroke = Instance.new("UIStroke")
    
    textLabel.Name = drawingIndex
    textLabel.AnchorPoint = Vector2.one * 0.5
    textLabel.BorderSizePixel = 0
    textLabel.BackgroundTransparency = 1

    textLabel.Visible = textObj.Visible
    textLabel.TextColor3 = textObj.Color
    textLabel.TextTransparency = convertTransparency(textObj.Transparency)
    textLabel.ZIndex = textObj.ZIndex
    textLabel.FontFace = getFontFromIndex(textObj.Font)
    textLabel.TextSize = textObj.Size

    -- Function to update textLabel size and position
    local function updateTextLabel()
        local textBounds = textLabel.TextBounds
        local offset = textBounds / 2
        textLabel.Size = UDim2.fromOffset(textBounds.X, textBounds.Y)
        textLabel.Position = UDim2.fromOffset(textObj.Position.X + (textObj.Center and 0 or offset.X), textObj.Position.Y + offset.Y)
    end

    -- Connect to TextBounds property change
    textLabel:GetPropertyChangedSignal("TextBounds"):Connect(updateTextLabel)

    uiStroke.Thickness = 1
    uiStroke.Enabled = textObj.Outline
    uiStroke.Color = textObj.OutlineColor

    textLabel.Parent = drawingUI
    uiStroke.Parent = textLabel
    
    local bs = table.create(0) -- Cache table
    table.insert(drawings, bs)

    return setmetatable(bs, {
        __newindex = function(_, index, value)
            if textObj[index] == nil then return end

            textObj[index] = value
            if index == "Text" then
                textLabel.Text = value
            elseif index == "Font" then
                textLabel.FontFace = getFontFromIndex(math.clamp(value, 0, 3))
            elseif index == "Size" then
                textLabel.TextSize = value
            elseif index == "Position" then
                updateTextLabel()
            elseif index == "Center" then
                local position = value and (camera.ViewportSize / 2) or textObj.Position
                textLabel.Position = UDim2.fromOffset(position.X, position.Y)
            elseif index == "Outline" then
                uiStroke.Enabled = value
            elseif index == "OutlineColor" then
                uiStroke.Color = value
            elseif index == "Visible" then
                textLabel.Visible = value
            elseif index == "ZIndex" then
                textLabel.ZIndex = value
            elseif index == "Transparency" then
                local transparency = convertTransparency(value)
                textLabel.TextTransparency = transparency
                uiStroke.Transparency = transparency
            elseif index == "Color" then
                textLabel.TextColor3 = value
            end
        end,
        __index = function(self, index)
            if index == "Remove" or index == "Destroy" then
                return function()
                    textLabel:Destroy()
                    textObj.Remove(self)
                    for k in pairs(bs) do
                        bs[k] = nil
                    end
                end
            elseif index == "TextBounds" then
                return textLabel.TextBounds
            end
            return textObj[index]
        end
    })
elseif drawingType == "Circle" then
    local circleObj = {
        Radius = 150,
        Position = Vector2.zero,
        Thickness = 0.7,
        Filled = false
    }

    for key, value in pairs(baseDrawingObj) do
        circleObj[key] = value
    end

    local circleFrame = Instance.new("Frame")
    local uiCorner = Instance.new("UICorner")
    local uiStroke = Instance.new("UIStroke")
    
    circleFrame.Name = drawingIndex
    circleFrame.AnchorPoint = Vector2.one * 0.5
    circleFrame.BorderSizePixel = 0
    circleFrame.BackgroundTransparency = circleObj.Filled and convertTransparency(circleObj.Transparency) or 1
    circleFrame.BackgroundColor3 = circleObj.Color
    circleFrame.Visible = circleObj.Visible
    circleFrame.ZIndex = circleObj.ZIndex
    circleFrame.Size = UDim2.fromOffset(circleObj.Radius * 2, circleObj.Radius * 2)

    uiCorner.CornerRadius = UDim.new(1, 0)
    uiStroke.Thickness = circleObj.Thickness
    uiStroke.Enabled = not circleObj.Filled
    uiStroke.ApplyStrokeMode = Enum.ApplyStrokeMode.Border

    circleFrame.Parent = drawingUI
    uiCorner.Parent = circleFrame
    uiStroke.Parent = circleFrame

    local bs = table.create(0)
    table.insert(drawings, bs)

    return setmetatable(bs, {
        __newindex = function(_, index, value)
            if circleObj[index] == nil then return end

            circleObj[index] = value
            if index == "Radius" then
                circleFrame.Size = UDim2.fromOffset(value * 2, value * 2)
            elseif index == "Position" then
                circleFrame.Position = UDim2.fromOffset(value.X, value.Y)
            elseif index == "Thickness" then
                uiStroke.Thickness = math.clamp(value, 0.6, math.huge)
            elseif index == "Filled" then
                circleFrame.BackgroundTransparency = value and convertTransparency(circleObj.Transparency) or 1
                uiStroke.Enabled = not value
            elseif index == "Visible" then
                circleFrame.Visible = value
            elseif index == "ZIndex" then
                circleFrame.ZIndex = value
            elseif index == "Transparency" then
                local transparency = convertTransparency(value)
                circleFrame.BackgroundTransparency = circleObj.Filled and transparency or 1
                uiStroke.Transparency = transparency
            elseif index == "Color" then
                circleFrame.BackgroundColor3 = value
                uiStroke.Color = value
            end
        end,
        __index = function(self, index)
            if index == "Remove" or index == "Destroy" then
                return function()
                    circleFrame:Destroy()
                    circleObj.Remove(self)
                    for k in pairs(bs) do
                        bs[k] = nil
                    end
                end
            end
            return circleObj[index]
        end
    })

elseif drawingType == "Square" then
    local squareObj = {
        Size = Vector2.zero,
        Position = Vector2.zero,
        Thickness = 0.7,
        Filled = false
    }

    for key, value in pairs(baseDrawingObj) do
        squareObj[key] = value
    end

    local squareFrame = Instance.new("Frame")
    local uiStroke = Instance.new("UIStroke")

    squareFrame.Name = drawingIndex
    squareFrame.BorderSizePixel = 0
    squareFrame.BackgroundTransparency = squareObj.Filled and convertTransparency(squareObj.Transparency) or 1
    squareFrame.BackgroundColor3 = squareObj.Color
    squareFrame.Visible = squareObj.Visible
    squareFrame.ZIndex = squareObj.ZIndex
    squareFrame.Size = UDim2.fromOffset(squareObj.Size.X, squareObj.Size.Y)

    uiStroke.Thickness = squareObj.Thickness
    uiStroke.Enabled = not squareObj.Filled
    uiStroke.LineJoinMode = Enum.LineJoinMode.Miter

    squareFrame.Parent = drawingUI
    uiStroke.Parent = squareFrame

    local bs = table.create(0)
    table.insert(drawings, bs)

    return setmetatable(bs, {
        __newindex = function(_, index, value)
            if squareObj[index] == nil then return end

            squareObj[index] = value
            if index == "Size" then
                squareFrame.Size = UDim2.fromOffset(value.X, value.Y)
            elseif index == "Position" then
                squareFrame.Position = UDim2.fromOffset(value.X, value.Y)
            elseif index == "Thickness" then
                uiStroke.Thickness = math.clamp(value, 0.6, math.huge)
            elseif index == "Filled" then
                squareFrame.BackgroundTransparency = value and convertTransparency(squareObj.Transparency) or 1
                uiStroke.Enabled = not value
            elseif index == "Visible" then
                squareFrame.Visible = value
            elseif index == "ZIndex" then
                squareFrame.ZIndex = value
            elseif index == "Transparency" then
                local transparency = convertTransparency(value)
                squareFrame.BackgroundTransparency = squareObj.Filled and transparency or 1
                uiStroke.Transparency = transparency
            elseif index == "Color" then
                squareFrame.BackgroundColor3 = value
                uiStroke.Color = value
            end
        end,
        __index = function(self, index)
            if index == "Remove" or index == "Destroy" then
                return function()
                    squareFrame:Destroy()
                    squareObj.Remove(self)
                    for k in pairs(bs) do
                        bs[k] = nil
                    end
                end
            end
            return squareObj[index]
        end
    })
	elseif drawingType == "Image" then
    local imageObj = {
        Data = "",
        DataURL = "rbxassetid://0",
        Size = Vector2.zero,
        Position = Vector2.zero
    }

    for key, value in pairs(baseDrawingObj) do
        imageObj[key] = value
    end

    local imageFrame = Instance.new("ImageLabel")
    imageFrame.Name = drawingIndex
    imageFrame.BorderSizePixel = 0
    imageFrame.ScaleType = Enum.ScaleType.Stretch
    imageFrame.BackgroundTransparency = 1
    imageFrame.Visible = imageObj.Visible
    imageFrame.ZIndex = imageObj.ZIndex
    imageFrame.ImageTransparency = convertTransparency(imageObj.Transparency)
    imageFrame.ImageColor3 = imageObj.Color
    imageFrame.Image = imageObj.DataURL
    imageFrame.Size = UDim2.fromOffset(imageObj.Size.X, imageObj.Size.Y)
    imageFrame.Position = UDim2.fromOffset(imageObj.Position.X, imageObj.Position.Y)

    imageFrame.Parent = drawingUI

    local bs = table.create(0)
    table.insert(drawings, bs)

    return setmetatable(bs, {
        __newindex = function(_, index, value)
            if imageObj[index] == nil then return end

            imageObj[index] = value
            if index == "Data" then
            --We can use it with getcustommasset
            elseif index == "DataURL" then
                imageFrame.Image = value
            elseif index == "Size" then
                imageFrame.Size = UDim2.fromOffset(value.X, value.Y)
            elseif index == "Position" then
                imageFrame.Position = UDim2.fromOffset(value.X, value.Y)
            elseif index == "Visible" then
                imageFrame.Visible = value
            elseif index == "ZIndex" then
                imageFrame.ZIndex = value
            elseif index == "Transparency" then
                imageFrame.ImageTransparency = convertTransparency(value)
            elseif index == "Color" then
                imageFrame.ImageColor3 = value
            end
        end,
        __index = function(self, index)
            if index == "Remove" or index == "Destroy" then
                return function()
                    imageFrame:Destroy()
                    imageObj.Remove(self)
                    for k in pairs(bs) do
                        bs[k] = nil
                    end
                end
            elseif index == "Data" then
                return nil --We can use it with getcustommasset
            end
            return imageObj[index]
        end
    })
	elseif drawingType == "Quad" then
    local quadObj = {
        PointA = Vector2.zero,
        PointB = Vector2.zero,
        PointC = Vector2.zero,
        PointD = Vector2.zero,
        Thickness = 1,
        Filled = false
    }

    for key, value in pairs(baseDrawingObj) do
        quadObj[key] = value
    end

    local _linePoints = {
        A = DrawingLib.new("Line"),
        B = DrawingLib.new("Line"),
        C = DrawingLib.new("Line"),
        D = DrawingLib.new("Line")
    }

    local bs = {}
    table.insert(drawings, bs)

    local function updateLines()
        _linePoints.A.From = quadObj.PointA
        _linePoints.A.To = quadObj.PointB
        _linePoints.B.From = quadObj.PointB
        _linePoints.B.To = quadObj.PointC
        _linePoints.C.From = quadObj.PointC
        _linePoints.C.To = quadObj.PointD
        _linePoints.D.From = quadObj.PointD
        _linePoints.D.To = quadObj.PointA
    end

    updateLines()

    return setmetatable(bs, {
        __newindex = function(_, index, value)
            if quadObj[index] == nil then return end

            quadObj[index] = value
            if index == "PointA" or index == "PointB" or index == "PointC" or index == "PointD" then
                updateLines()
            elseif index == "Thickness" or index == "Visible" or index == "Color" or index == "ZIndex" then
                for _, linePoint in pairs(_linePoints) do
                    linePoint[index] = value
                end
            elseif index == "Filled" then
			--I didnt make that
            end
        end,
        __index = function(self, index)
            if index == "Remove" or index == "Destroy" then
                return function()
                    for _, linePoint in pairs(_linePoints) do
                        linePoint:Destroy()
                    end
                    quadObj.Remove(self)
                    for k in pairs(bs) do
                        bs[k] = nil
                    end
                end
            end
            return quadObj[index]
        end
    })

elseif drawingType == "Triangle" then
    local triangleObj = {
        PointA = Vector2.zero,
        PointB = Vector2.zero,
        PointC = Vector2.zero,
        Thickness = 1,
        Filled = false
    }

    for key, value in pairs(baseDrawingObj) do
        triangleObj[key] = value
    end

    local _linePoints = {
        A = DrawingLib.new("Line"),
        B = DrawingLib.new("Line"),
        C = DrawingLib.new("Line")
    }

    local bs = {}
    table.insert(drawings, bs)

    local function updateLines()
        _linePoints.A.From = triangleObj.PointA
        _linePoints.A.To = triangleObj.PointB
        _linePoints.B.From = triangleObj.PointB
        _linePoints.B.To = triangleObj.PointC
        _linePoints.C.From = triangleObj.PointC
        _linePoints.C.To = triangleObj.PointA
    end

    updateLines()

    return setmetatable(bs, {
        __newindex = function(_, index, value)
            if triangleObj[index] == nil then return end

            triangleObj[index] = value
            if index == "PointA" or index == "PointB" or index == "PointC" then
                updateLines()
            elseif index == "Thickness" or index == "Visible" or index == "Color" or index == "ZIndex" then
                for _, linePoint in pairs(_linePoints) do
                    linePoint[index] = value
                end
            elseif index == "Filled" then
                -- Placeholder for future functionality
            end
        end,
        __index = function(self, index)
            if index == "Remove" or index == "Destroy" then
                return function()
                    for _, linePoint in pairs(_linePoints) do
                        linePoint:Destroy()
                    end
                    triangleObj.Remove(self)
                    for k in pairs(bs) do
                        bs[k] = nil
                    end
                end
            end
            return triangleObj[index]
        end
    })
end
end

getgenv()["Drawing"] = DrawingLib
getgenv()["Drawing"]["Fonts"] = {
    ['UI'] = 0,
    ['System'] = 1,
    ['Plex'] = 2,
    ['Monospace'] = 3
}

getgenv()["cleardrawcache"] = newcclosure(function()
    for _, v in pairs(Drawings) do
        v:Remove()
    end
    table.clear(drawings)
end)

getgenv()["clear_draw_cache"] = cleardrawcache
getgenv()["ClearDrawCache"] = cleardrawcache

getgenv()["isrenderobj"] = newcclosure(function(Inst)
    for _, v in pairs(drawings) do
        if v == Inst and type(v) == "table" then
            return true
        end
    end
    return false
end)

getgenv()["is_render_obj"] = isrenderobj
getgenv()["IsRenderObj"] = isrenderobj

getgenv()["getrenderproperty"] = newcclosure(function(a, b)
    return a[b]
end)

getgenv()["get_render_property"] = getrenderproperty
getgenv()["GetRenderProperty"] = getrenderproperty

getgenv()["setrenderproperty"] = newcclosure(function(a, b, c)
    local success, err = pcall(function()
        a[b] = c
    end)
    if not success and err then warn(err) end
end)

getgenv()["set_render_property"] = getrenderproperty
getgenv()["SetRenderProperty"] = setrenderproperty