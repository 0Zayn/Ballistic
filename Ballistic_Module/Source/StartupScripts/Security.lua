--!native
--!optimize 2

if not game:IsLoaded() then game.Loaded:Wait() end

-- Some games like Arsenal watch for errors as a detection mechanism.
local ErrorWatchGames = {
	286090429, -- Arsenal
}

for _, v in pairs(ErrorWatchGames) do
	if game.PlaceId == v then
		getgenv().error = newcclosure(function(...)
			return warn(...)
		end)
	end
end

-- TODO: Override print, warn and error functions to write to DevConsole manually to bypass LogService.MessageOut event connection.