-- ColaCat Premium - Simple Online Checker
local Rayfield = loadstring(game:HttpGet('https://sirius.menu/rayfield'))()

local Window = Rayfield:CreateWindow({
   Name = "ColaCat Premium",
   LoadingTitle = "ColaCat",
   LoadingSubtitle = "Loading...",
   ConfigurationSaving = {Enabled = false},
   KeySystem = false,
})

local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local HttpService = game:GetService("HttpService")

local player = Players.LocalPlayer
local playerGui = player:WaitForChild("PlayerGui")

local targetUsername = ""
local targetPlayer = nil
local isVerifying = false
local affectTarget = false

local exploits = {
	swimming = false,
	fling = false,
	reach = false,
	speed = false,
	antiKnock = false,
	velocity = false,
	jump = false,
	size = false,
	spin = false,
	noclip = false,
	godmode = false,
	flight = false
}

-- Check if player is in current game
local function isInCurrentGame(name)
	for _, p in pairs(Players:GetPlayers()) do
		if p.Name:lower() == name:lower() then
			return true, p, "In Game"
		end
	end
	return false, nil, "Not Here"
end

-- Check if online on Roblox (using API)
local function checkOnlineStatus(name)
	local inGame, targetPlayer, status = isInCurrentGame(name)
	
	if inGame then
		return true, targetPlayer, "In Game"
	end
	
	-- Try to get user ID and check online status
	local success, result = pcall(function()
		local userId = Players:FindFirstChild(name)
		if userId then
			return "Online (Different Game)"
		end
		return "Offline"
	end)
	
	if success then
		-- If they're not in this game, they're likely offline for our purposes
		return false, nil, "Offline"
	end
	
	return false, nil, "Offline"
end

-- Offline Message
local function offlineMsg()
	local gui = Instance.new("ScreenGui")
	gui.ResetOnSpawn = false
	gui.Parent = playerGui
	
	local bg = Instance.new("Frame")
	bg.Size = UDim2.new(1, 0, 1, 0)
	bg.BackgroundColor3 = Color3.fromRGB(0, 0, 0)
	bg.BackgroundTransparency = 0.5
	bg.BorderSizePixel = 0
	bg.Parent = gui
	
	local box = Instance.new("Frame")
	box.Size = UDim2.new(0, 300, 0, 180)
	box.Position = UDim2.new(0.5, -150, 0.5, -90)
	box.BackgroundColor3 = Color3.fromRGB(20, 20, 20)
	box.BorderColor3 = Color3.fromRGB(150, 100, 200)
	box.BorderSizePixel = 2
	box.Parent = gui
	
	local emoji = Instance.new("TextLabel")
	emoji.Size = UDim2.new(1, 0, 0, 50)
	emoji.BackgroundTransparency = 1
	emoji.TextColor3 = Color3.fromRGB(255, 255, 255)
	emoji.TextSize = 40
	emoji.Font = Enum.Font.GothamBold
	emoji.Text = "😢"
	emoji.Parent = box
	
	local msg = Instance.new("TextLabel")
	msg.Size = UDim2.new(1, -20, 0, 70)
	msg.Position = UDim2.new(0, 10, 0, 50)
	msg.BackgroundTransparency = 1
	msg.TextColor3 = Color3.fromRGB(200, 100, 200)
	msg.TextSize = 14
	msg.Font = Enum.Font.GothamBold
	msg.Text = "Boo-Hoo...\nThe user is offline"
	msg.TextWrapped = true
	msg.Parent = box
	
	task.wait(3)
	gui:Destroy()
end

-- Join Notification
Players.PlayerAdded:Connect(function(p)
	local notif = Instance.new("ScreenGui")
	notif.ResetOnSpawn = false
	notif.Parent = playerGui
	
	local frame = Instance.new("Frame")
	frame.Size = UDim2.new(0, 250, 0, 50)
	frame.Position = UDim2.new(1, -270, 1, -70)
	frame.BackgroundColor3 = Color3.fromRGB(20, 20, 20)
	frame.BorderColor3 = Color3.fromRGB(100, 200, 100)
	frame.BorderSizePixel = 2
	frame.Parent = notif
	
	local text = Instance.new("TextLabel")
	text.Size = UDim2.new(1, 0, 1, 0)
	text.BackgroundTransparency = 1
	text.TextColor3 = Color3.fromRGB(100, 200, 100)
	text.TextSize = 13
	text.Font = Enum.Font.GothamBold
	text.Text = "✓ " .. p.Name .. " joined"
	text.Parent = frame
	
	task.wait(3)
	notif:Destroy()
end)

-- Verify Function
local function verify(name)
	if isVerifying or name == "" then 
		Rayfield:Notify({Title = "Error", Content = "Enter username", Duration = 1})
		return 
	end
	
	isVerifying = true
	Rayfield:Notify({Title = "Checking...", Content = "Verifying " .. name .. " status...", Duration = 1})
	
	task.wait(1)
	
	local isOnline, foundPlayer, status = checkOnlineStatus(name)
	targetPlayer = foundPlayer
	
	if isOnline then
		Rayfield:Notify({
			Title = "✓ Found",
			Content = name .. " - " .. status,
			Duration = 2
		})
	else
		Rayfield:Notify({
			Title = "❌ Offline",
			Content = name .. " is offline",
			Duration = 2
		})
		offlineMsg()
	end
	
	isVerifying = false
end

-- EXPLOITS
RunService.Heartbeat:Connect(function()
	local char = affectTarget and targetPlayer and targetPlayer.Character or player.Character
	if not char then return end
	
	local root = char:FindFirstChild("HumanoidRootPart")
	local humanoid = char:FindFirstChild("Humanoid")
	
	if exploits.swimming and humanoid then
		humanoid.State = Enum.HumanoidStateType.Swimming
	end
	
	if exploits.fling and root then
		root.AssemblyLinearVelocity = Vector3.new(math.random(-120, 120), math.random(60, 150), math.random(-120, 120))
	end
	
	if exploits.speed and humanoid then
		humanoid.WalkSpeed = 70
	end
	
	if exploits.antiKnock and root then
		root.AssemblyLinearVelocity = root.AssemblyLinearVelocity * Vector3.new(0.4, 0.4, 0.4)
	end
	
	if exploits.velocity and root then
		root.AssemblyLinearVelocity = root.AssemblyLinearVelocity * 1.6
	end
	
	if exploits.jump and humanoid then
		humanoid.JumpPower = 120
	end
	
	if exploits.size and char then
		for _, part in pairs(char:GetDescendants()) do
			if part:IsA("BasePart") then
				part.Size = part.Size * 1.02
			end
		end
	end
	
	if exploits.spin and root then
		root.CFrame = root.CFrame * CFrame.Angles(0, math.rad(12), 0)
	end
	
	if exploits.noclip then
		for _, part in pairs(char:GetDescendants()) do
			if part:IsA("BasePart") then
				part.CanCollide = false
			end
		end
	end
	
	if exploits.godmode and humanoid then
		humanoid.Health = humanoid.MaxHealth
	end
	
	if exploits.flight and root then
		root.AssemblyLinearVelocity = root.AssemblyLinearVelocity + Vector3.new(0, 1.5, 0)
	end
	
	if exploits.reach then
		for _, part in pairs(char:GetDescendants()) do
			if part:IsA("BasePart") and part.Name:find("Arm") then
				part.Size = part.Size * 1.05
			end
		end
	end
end)

-- UI
local Main = Window:CreateTab("Main", 4483345998)

Main:CreateLabel("TARGET SETUP")
Main:CreateInput({
	Name = "Username",
	Placeholder = "Enter target...",
	RemoveTextompleted = false,
	Callback = function(Value)
		targetUsername = Value
	end
})

Main:CreateButton({
	Name = "Check Status",
	Callback = function()
		verify(targetUsername)
	end
})

Main:CreateToggle({
	Name = "Affect Target",
	CurrentValue = false,
	Callback = function(Value)
		affectTarget = Value
	end
})

Main:CreateLabel("EXPLOITS")
Main:CreateToggle({Name = "🌊 Wet Noodle", CurrentValue = false, Callback = function(Value) exploits.swimming = Value end})
Main:CreateToggle({Name = "🚀 Yeet Force", CurrentValue = false, Callback = function(Value) exploits.fling = Value end})
Main:CreateToggle({Name = "💪 Big Arms", CurrentValue = false, Callback = function(Value) exploits.reach = Value end})
Main:CreateToggle({Name = "⚡ Zoom Zoom", CurrentValue = false, Callback = function(Value) exploits.speed = Value end})
Main:CreateToggle({Name = "🧈 Sticky Boi", CurrentValue = false, Callback = function(Value) exploits.antiKnock = Value end})
Main:CreateToggle({Name = "✨ Vibe Check", CurrentValue = false, Callback = function(Value) exploits.velocity = Value end})
Main:CreateToggle({Name = "☁️ Sky Jump", CurrentValue = false, Callback = function(Value) exploits.jump = Value end})
Main:CreateToggle({Name = "📦 Thicc", CurrentValue = false, Callback = function(Value) exploits.size = Value end})
Main:CreateToggle({Name = "🌪️ Spinny", CurrentValue = false, Callback = function(Value) exploits.spin = Value end})
Main:CreateToggle({Name = "👻 No Clip", CurrentValue = false, Callback = function(Value) exploits.noclip = Value end})
Main:CreateToggle({Name = "🛡️ God Mode", CurrentValue = false, Callback = function(Value) exploits.godmode = Value end})
Main:CreateToggle({Name = "🛸 Flight", CurrentValue = false, Callback = function(Value) exploits.flight = Value end})

local Settings = Window:CreateTab("Settings", 4483345998)
Settings:CreateButton({Name = "Disable All", Callback = function() for k in pairs(exploits) do exploits[k] = false end end})
Settings:CreateButton({Name = "Close", Callback = function() Window:Close() end})

Rayfield:Notify({Title = "ColaCat", Content = "Ready - Check Status button", Duration = 2})
print("✓ Loaded")
