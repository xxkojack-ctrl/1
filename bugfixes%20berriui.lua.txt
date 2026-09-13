-- Berri Hub | Bug Fixes | Roblox
-- Auto Parry + FPS Booster | Delta compatible | Proximity filtered
-- Place ID: 100971017807798
-- UI: Standalone Berri UI Library (all-in-one, no external repo)

-- ─── Load Berri UI (standalone, all-in-one) ──────────────────────────────────

local Library, ThemeManager, SaveManager, Builder = (function()

local LPH_NO_VIRTUALIZE = LPH_NO_VIRTUALIZE or function(f) return f end
local PP_SCRAMBLE_STR   = PP_SCRAMBLE_STR   or function(s) return s end

return LPH_NO_VIRTUALIZE(function()
	local InputService = game:GetService("UserInputService")
	local Players      = game:GetService("Players")
	local RunService   = game:GetService("RunService")
	local TweenService = game:GetService("TweenService")
	local Lighting     = game:GetService("Lighting")

	repeat task.wait() until Players.LocalPlayer

	local RenderStepped = RunService.RenderStepped
	local LocalPlayer   = Players.LocalPlayer
	local IsMobile      = InputService.TouchEnabled and not InputService.KeyboardEnabled

	local ScreenGui = Instance.new("ScreenGui")
	ScreenGui.Name          = "BerriUI"
	ScreenGui.ResetOnSpawn  = false
	ScreenGui.ZIndexBehavior= Enum.ZIndexBehavior.Global

	local function attemptParenting()
		local s = pcall(function()
			if gethui then local hui = gethui() if hui then ScreenGui.Parent = hui end end
		end)
		if s and ScreenGui.Parent then return end
		s = pcall(function()
			if syn and syn.protect_gui then syn.protect_gui(ScreenGui) end
			ScreenGui.Parent = game:GetService("CoreGui")
		end)
		if s and ScreenGui.Parent then return end
		ScreenGui.Parent = LocalPlayer:WaitForChild("PlayerGui", 5)
	end
	attemptParenting()

	local Toggles   = {}
	local Options   = {}
	local Entries   = {}
	local ColorPickers = {}

	pcall(function()
		getgenv().Toggles = Toggles
		getgenv().Options  = Options
	end)

	-- ── Palette ──────────────────────────────────────────────────────────────
	local Palette = {
		Window        = Color3.fromRGB(17,  17,  17),
		Header        = Color3.fromRGB(23,  23,  23),
		Panel         = Color3.fromRGB(13,  13,  13),
		PanelStroke   = Color3.fromRGB(25,  25,  25),
		WindowStroke  = Color3.fromRGB(30,  30,  30),
		Control       = Color3.fromRGB(25,  25,  25),
		ControlHover  = Color3.fromRGB(33,  33,  33),
		ControlStroke = Color3.fromRGB(38,  38,  38),
		Track         = Color3.fromRGB(21,  21,  21),
		Accent        = Color3.fromRGB(162, 47,  229),
		AccentLight   = Color3.fromRGB(177, 53,  250),
		AccentDeep    = Color3.fromRGB(108, 33,  167),
		Text          = Color3.fromRGB(242, 242, 242),
		TextSoft      = Color3.fromRGB(219, 219, 219),
		TextDim       = Color3.fromRGB(150, 150, 150),
		PanelDark     = Color3.fromRGB(9,   9,   9),
	}

	local FONT_FAMILY = Font.fromEnum(Enum.Font.Gotham).Family
	local function MakeFont(w) local f = Font.new(FONT_FAMILY, w) f.Bold = false return f end
	local FONT_MEDIUM = MakeFont(Enum.FontWeight.Medium)
	local FONT_BOLD   = MakeFont(Enum.FontWeight.Bold)

	local TEXT_TITLE   = 16
	local TEXT_TAB     = 14
	local TEXT_HEADING = 14
	local TEXT_BODY    = 13
	local ROW_HEIGHT   = 28
	local ROW_LABEL    = 21
	local BUTTON_HEIGHT= 23
	local BOX_SIZE     = 18

	local TWEEN_FAST   = TweenInfo.new(0.18, Enum.EasingStyle.Quad,  Enum.EasingDirection.Out)
	local TWEEN_SMOOTH = TweenInfo.new(0.34, Enum.EasingStyle.Quint, Enum.EasingDirection.Out)

	-- ── Library object ───────────────────────────────────────────────────────
	local Library = {
		Registry        = {},
		HudRegistry     = {},
		FontColor       = Palette.Text,
		MainColor       = Palette.Panel,
		BackgroundColor = Palette.Window,
		AccentColor     = Palette.Accent,
		OutlineColor    = Palette.PanelStroke,
		RiskColor       = Color3.fromRGB(251, 146, 60),
		Black           = Color3.new(0,0,0),
		Font            = FONT_MEDIUM,
		Palette         = Palette,
		Signals         = {},
		ScreenGui       = ScreenGui,
		IsMobile        = IsMobile,
		Options         = Options,
		Toggles         = Toggles,
	}

	-- Rainbow ticker (kept for compat)
	local RainbowStep, Hue = 0, 0
	table.insert(Library.Signals, RenderStepped:Connect(function(Delta)
		local ni, ne = next(Entries)
		if ni and ne then Entries[ni] = nil ne() end
		RainbowStep = RainbowStep + Delta
		if RainbowStep >= (1/60) then
			RainbowStep = 0
			Hue = (Hue + 1/400) % 1
			Library.CurrentRainbowHue   = Hue
			Library.CurrentRainbowColor = Color3.fromHSV(Hue, 0.8, 1)
			for _, cp in next, ColorPickers do if cp.Rainbow then cp:Display() end end
		end
	end))

	-- ── Notification system ──────────────────────────────────────────────────
	local notifFrame = Instance.new("Frame")
	notifFrame.Name                 = "NotifContainer"
	notifFrame.BackgroundTransparency = 1
	notifFrame.Size                 = UDim2.new(0, 260, 1, 0)
	notifFrame.Position             = UDim2.new(1, -270, 0, 0)
	notifFrame.Parent               = ScreenGui
	local notifLayout = Instance.new("UIListLayout")
	notifLayout.SortOrder         = Enum.SortOrder.LayoutOrder
	notifLayout.Padding           = UDim.new(0, 6)
	notifLayout.VerticalAlignment = Enum.VerticalAlignment.Bottom
	notifLayout.Parent            = notifFrame

	function Library:Notify(opts)
		opts = opts or {}
		local title = tostring(opts.Title or "Notice")
		local desc  = tostring(opts.Description or "")
		local dur   = tonumber(opts.Time) or 3

		local card = Instance.new("Frame")
		card.BackgroundColor3 = Palette.Panel
		card.BorderColor3     = Palette.PanelStroke
		card.BorderSizePixel  = 1
		card.Size             = UDim2.new(1, 0, 0, desc ~= "" and 52 or 32)
		card.ClipsDescendants = true
		card.Parent           = notifFrame
		local cc = Instance.new("UICorner") cc.CornerRadius = UDim.new(0,6) cc.Parent = card

		local bar = Instance.new("Frame")
		bar.BackgroundColor3 = Palette.Accent
		bar.Size             = UDim2.new(0, 3, 1, 0)
		bar.BorderSizePixel  = 0
		bar.Parent           = card

		local t1 = Instance.new("TextLabel")
		t1.BackgroundTransparency = 1
		t1.Font       = Enum.Font.GothamBold
		t1.TextColor3 = Palette.Text
		t1.TextSize   = 13
		t1.Text       = title
		t1.TextXAlignment = Enum.TextXAlignment.Left
		t1.Size     = UDim2.new(1,-14,0,18)
		t1.Position = UDim2.new(0,10,0,7)
		t1.Parent   = card

		if desc ~= "" then
			local t2 = Instance.new("TextLabel")
			t2.BackgroundTransparency = 1
			t2.Font       = Enum.Font.Gotham
			t2.TextColor3 = Palette.TextDim
			t2.TextSize   = 12
			t2.Text       = desc
			t2.TextXAlignment = Enum.TextXAlignment.Left
			t2.TextWrapped    = true
			t2.Size     = UDim2.new(1,-14,0,18)
			t2.Position = UDim2.new(0,10,0,27)
			t2.Parent   = card
		end

		task.delay(dur, function()
			TweenService:Create(card, TWEEN_FAST, {Size=UDim2.new(1,0,0,0)}):Play()
			task.delay(0.2, function() card:Destroy() end)
		end)
	end

	-- ── Widget builders ──────────────────────────────────────────────────────
	local function makeLabel(text, parent, y, color)
		local lbl = Instance.new("TextLabel")
		lbl.BackgroundTransparency = 1
		lbl.Font       = Enum.Font.Gotham
		lbl.TextColor3 = color or Palette.Text
		lbl.TextSize   = TEXT_BODY
		lbl.Text       = text
		lbl.TextXAlignment = Enum.TextXAlignment.Left
		lbl.Size     = UDim2.new(1,-12,0,ROW_LABEL)
		lbl.Position = UDim2.new(0,6,0,y)
		lbl.Parent   = parent
		return lbl
	end

	local function buildToggle(id, opts, parent, yRef)
		opts = opts or {}
		local val = opts.Default or false
		local cb  = opts.Callback

		local row = Instance.new("Frame")
		row.BackgroundTransparency = 1
		row.Size     = UDim2.new(1,0,0,ROW_HEIGHT)
		row.Position = UDim2.new(0,0,0,yRef[1])
		row.Parent   = parent
		yRef[1]      = yRef[1] + ROW_HEIGHT

		local box = Instance.new("Frame")
		box.Size             = UDim2.new(0,BOX_SIZE,0,BOX_SIZE)
		box.Position         = UDim2.new(0,6,0.5,-BOX_SIZE/2)
		box.BackgroundColor3 = val and Palette.Accent or Palette.Control
		box.BorderColor3     = Palette.ControlStroke
		box.BorderSizePixel  = 1
		box.Parent           = row
		local bc = Instance.new("UICorner") bc.CornerRadius = UDim.new(0,4) bc.Parent = box

		makeLabel(opts.Text or id, row, (ROW_HEIGHT-ROW_LABEL)/2, Palette.TextSoft)

		local entry     = { Value = val, _box = box }
		local listeners = {}

		local function fire()
			box.BackgroundColor3 = entry.Value and Palette.Accent or Palette.Control
			if cb then pcall(cb, entry.Value) end
			for _, fn in ipairs(listeners) do pcall(fn) end
		end

		local btn = Instance.new("TextButton")
		btn.BackgroundTransparency = 1
		btn.Size   = UDim2.new(1,0,1,0)
		btn.Text   = ""
		btn.Parent = row
		btn.MouseButton1Click:Connect(function()
			entry.Value = not entry.Value
			fire()
		end)

		function entry:OnChanged(fn) table.insert(listeners, fn) end
		function entry:SetValue(v)   self.Value = v fire() end

		Toggles[id] = entry
		return entry
	end

	local function buildSlider(id, opts, parent, yRef)
		opts = opts or {}
		local minV  = opts.Min     or 0
		local maxV  = opts.Max     or 100
		local val   = opts.Default or minV
		local round = opts.Rounding or 0

		local row = Instance.new("Frame")
		row.BackgroundTransparency = 1
		row.Size     = UDim2.new(1,0,0,ROW_HEIGHT+12)
		row.Position = UDim2.new(0,0,0,yRef[1])
		row.Parent   = parent
		yRef[1]      = yRef[1] + ROW_HEIGHT + 12

		makeLabel(opts.Text or id, row, 2, Palette.TextSoft)

		local track = Instance.new("Frame")
		track.BackgroundColor3 = Palette.Track
		track.BorderColor3     = Palette.ControlStroke
		track.BorderSizePixel  = 1
		track.Size     = UDim2.new(1,-12,0,6)
		track.Position = UDim2.new(0,6,0,ROW_LABEL+6)
		track.Parent   = row
		local tc = Instance.new("UICorner") tc.CornerRadius = UDim.new(1,0) tc.Parent = track

		local fill = Instance.new("Frame")
		fill.BackgroundColor3 = Palette.Accent
		fill.BorderSizePixel  = 0
		fill.Size     = UDim2.new((val-minV)/(maxV-minV),0,1,0)
		fill.Parent   = track
		local fc = Instance.new("UICorner") fc.CornerRadius = UDim.new(1,0) fc.Parent = fill

		local valLbl = Instance.new("TextLabel")
		valLbl.BackgroundTransparency = 1
		valLbl.Font       = Enum.Font.Gotham
		valLbl.TextColor3 = Palette.TextDim
		valLbl.TextSize   = 11
		valLbl.Text       = tostring(val)
		valLbl.TextXAlignment = Enum.TextXAlignment.Right
		valLbl.Size     = UDim2.new(1,-6,0,ROW_LABEL)
		valLbl.Position = UDim2.new(0,0,0,2)
		valLbl.Parent   = row

		local entry     = { Value = val }
		local listeners = {}

		local function setVal(v)
			v = math.clamp(math.floor(v/(10^round)+0.5)*(10^round), minV, maxV)
			entry.Value   = v
			fill.Size     = UDim2.new((v-minV)/(maxV-minV),0,1,0)
			valLbl.Text   = tostring(v)
			for _, fn in ipairs(listeners) do pcall(fn) end
		end

		-- Mouse drag
		local dragging = false
		local hitbox = Instance.new("TextButton")
		hitbox.BackgroundTransparency = 1
		hitbox.Size   = UDim2.new(1,0,1,0)
		hitbox.Text   = ""
		hitbox.Parent = track
		hitbox.MouseButton1Down:Connect(function() dragging = true end)
		InputService.InputEnded:Connect(function(inp)
			if inp.UserInputType == Enum.UserInputType.MouseButton1 then dragging = false end
		end)
		InputService.InputChanged:Connect(function(inp)
			if dragging and inp.UserInputType == Enum.UserInputType.MouseMovement then
				local abs = track.AbsolutePosition
				local sz  = track.AbsoluteSize
				setVal(minV + math.clamp((inp.Position.X - abs.X)/sz.X, 0, 1)*(maxV-minV))
			end
		end)

		-- Touch drag (mobile)
		hitbox.TouchLongPress:Connect(function() end) -- prevent scroll eating it
		hitbox.TouchPan:Connect(function(_, positions)
			if #positions > 0 then
				local abs = track.AbsolutePosition
				local sz  = track.AbsoluteSize
				setVal(minV + math.clamp((positions[1].X - abs.X)/sz.X, 0, 1)*(maxV-minV))
			end
		end)

		function entry:OnChanged(fn) table.insert(listeners, fn) end
		function entry:SetValue(v)   setVal(v) end

		Options[id] = entry
		return entry
	end

	local function buildLabel(text, parent, yRef, id)
		local row = Instance.new("Frame")
		row.BackgroundTransparency = 1
		row.Size     = UDim2.new(1,0,0,ROW_LABEL)
		row.Position = UDim2.new(0,0,0,yRef[1])
		row.Parent   = parent
		yRef[1]      = yRef[1] + ROW_LABEL
		local lbl = makeLabel(text, row, 0, Palette.TextDim)
		local entry = {}
		function entry:SetText(t) lbl.Text = t end
		if id then Options[id] = entry end
		return entry
	end

	local function buildDivider(parent, yRef)
		local line = Instance.new("Frame")
		line.BackgroundColor3 = Palette.ControlStroke
		line.BorderSizePixel  = 0
		line.Size     = UDim2.new(1,-12,0,1)
		line.Position = UDim2.new(0,6,0,yRef[1]+4)
		line.Parent   = parent
		yRef[1] = yRef[1] + 10
	end

	local function buildButton(opts, parent, yRef)
		local text = opts.Text or "Button"
		local fn   = opts.Func or function() end
		local btn  = Instance.new("TextButton")
		btn.BackgroundColor3 = Palette.Control
		btn.BorderColor3     = Palette.ControlStroke
		btn.BorderSizePixel  = 1
		btn.Font       = Enum.Font.Gotham
		btn.TextColor3 = Palette.Text
		btn.TextSize   = TEXT_BODY
		btn.Text       = text
		btn.Size       = UDim2.new(1,-12,0,BUTTON_HEIGHT)
		btn.Position   = UDim2.new(0,6,0,yRef[1])
		btn.Parent     = parent
		local bc = Instance.new("UICorner") bc.CornerRadius = UDim.new(0,5) bc.Parent = btn
		btn.MouseButton1Click:Connect(function() pcall(fn) end)
		yRef[1] = yRef[1] + BUTTON_HEIGHT + 4
	end

	local function buildKeyPicker(id, opts)
		local entry = { Value = opts.Default or "RightShift", _listeners = {} }
		InputService.InputBegan:Connect(function(inp, gpe)
			if gpe then return end
			if inp.UserInputType ~= Enum.UserInputType.Keyboard then return end
			if inp.KeyCode.Name == entry.Value then
				for _, fn in ipairs(entry._listeners) do pcall(fn) end
			end
		end)
		function entry:OnChanged(fn) table.insert(self._listeners, fn) end
		Options[id] = entry
		return entry
	end

	-- ── Groupbox factory (shared between single-col and two-col) ─────────────
	local function makeGbFrame(parent, xOff, colW, yRef)
		local HEADER = 26
		local PAD    = 8

		local frame = Instance.new("Frame")
		frame.BackgroundColor3 = Palette.Panel
		frame.BorderColor3     = Palette.PanelStroke
		frame.BorderSizePixel  = 1
		frame.Size     = UDim2.new(0, colW, 0, HEADER + PAD)
		frame.Position = UDim2.new(0, xOff, 0, yRef[1])
		frame.Parent   = parent
		local corner = Instance.new("UICorner") corner.CornerRadius = UDim.new(0,6) corner.Parent = frame

		local headerLbl = Instance.new("TextLabel")
		headerLbl.BackgroundTransparency = 1
		headerLbl.Font       = Enum.Font.GothamBold
		headerLbl.TextColor3 = Palette.Accent
		headerLbl.TextSize   = TEXT_HEADING
		headerLbl.TextXAlignment = Enum.TextXAlignment.Left
		headerLbl.Size     = UDim2.new(1,-12,0,HEADER)
		headerLbl.Position = UDim2.new(0,6,0,0)
		headerLbl.Parent   = frame

		local inner_y = {HEADER + PAD}

		local function refresh()
			frame.Size = UDim2.new(0, colW, 0, inner_y[1] + PAD)
			yRef[1] = frame.Position.Y.Offset + frame.Size.Y.Offset + 6
		end

		local gb = { _headerLbl = headerLbl }

		function gb:SetName(n) headerLbl.Text = n end

		function gb:AddToggle(id, o)
			local t = buildToggle(id, o, frame, inner_y) refresh() return t
		end
		function gb:AddSlider(id, o)
			local s = buildSlider(id, o, frame, inner_y) refresh() return s
		end
		function gb:AddLabel(t, _, id)
			local l = buildLabel(t, frame, inner_y, id) refresh() return l
		end
		function gb:AddDivider()
			buildDivider(frame, inner_y) refresh()
		end
		function gb:AddButton(o)
			buildButton(o, frame, inner_y) refresh()
		end
		function gb:AddKeyPicker(id, o)
			local kp = buildKeyPicker(id, o)
			local hint = Instance.new("TextLabel")
			hint.BackgroundTransparency = 1
			hint.Font       = Enum.Font.Gotham
			hint.TextColor3 = Palette.TextDim
			hint.TextSize   = 11
			hint.Text       = "Keybind: " .. (o.Default or "?")
			hint.TextXAlignment = Enum.TextXAlignment.Left
			hint.Size     = UDim2.new(1,-12,0,ROW_LABEL)
			hint.Position = UDim2.new(0,6,0,inner_y[1])
			hint.Parent   = frame
			inner_y[1]    = inner_y[1] + ROW_LABEL
			refresh()
			return kp
		end

		return gb
	end

	-- ── Tab ──────────────────────────────────────────────────────────────────
	local function makeTab(tabBar, contentHost, tabName)
		local btn = Instance.new("TextButton")
		btn.BackgroundTransparency = 1
		btn.Font       = Enum.Font.Gotham
		btn.TextColor3 = Palette.TextDim
		btn.TextSize   = TEXT_TAB
		btn.Text       = tabName
		btn.Size       = UDim2.new(0, 90, 1, 0)
		btn.Parent     = tabBar

		local page = Instance.new("ScrollingFrame")
		page.BackgroundTransparency    = 1
		page.ScrollBarThickness        = 3
		page.ScrollBarImageColor3      = Palette.Accent
		page.CanvasSize                = UDim2.new(0,0,0,0)
		page.AutomaticCanvasSize       = Enum.AutomaticSize.Y
		page.Size    = UDim2.new(1,0,1,0)
		page.Visible = false
		page.Parent  = contentHost

		local leftY  = {6}
		local rightY = {6}

		local tab = { _btn = btn, _page = page }

		function tab:AddGroupbox(opts)
			local side   = opts.Side or "Left"
			local isRight= side == "Right"
			local hostW  = contentHost.AbsoluteSize.X
			local colW   = math.max(100, (hostW / 2) - 10)
			local xOff   = isRight and (colW + 10) or 4
			local yRef   = isRight and rightY or leftY
			local gb = makeGbFrame(page, xOff, colW, yRef)
			gb:SetName(opts.Name or "")
			return gb
		end

		return tab
	end

	-- ── Window ───────────────────────────────────────────────────────────────
	function Library:CreateWindow(opts)
		opts = opts or {}
		local W, H = 700, 480

		local main = Instance.new("Frame")
		main.BackgroundColor3 = Palette.Window
		main.BorderColor3     = Palette.WindowStroke
		main.BorderSizePixel  = 1
		main.Size     = UDim2.new(0,W,0,H)
		main.Position = UDim2.new(0.5,-W/2, 0.5,-H/2)
		main.Active   = true
		main.Draggable= true
		main.Parent   = ScreenGui
		local mc = Instance.new("UICorner") mc.CornerRadius = UDim.new(0,8) mc.Parent = main

		local header = Instance.new("Frame")
		header.BackgroundColor3 = Palette.Header
		header.BorderSizePixel  = 0
		header.Size     = UDim2.new(1,0,0,38)
		header.Parent   = main
		local hc = Instance.new("UICorner") hc.CornerRadius = UDim.new(0,8) hc.Parent = header

		local titleLbl = Instance.new("TextLabel")
		titleLbl.BackgroundTransparency = 1
		titleLbl.Font       = Enum.Font.GothamBold
		titleLbl.TextColor3 = Palette.Text
		titleLbl.TextSize   = TEXT_TITLE
		titleLbl.Text       = opts.Title or "Berri Hub"
		titleLbl.TextXAlignment = Enum.TextXAlignment.Left
		titleLbl.Size     = UDim2.new(1,-100,1,0)
		titleLbl.Position = UDim2.new(0,14,0,0)
		titleLbl.Parent   = header

		local tabBar = Instance.new("Frame")
		tabBar.BackgroundTransparency = 1
		tabBar.Size     = UDim2.new(1,0,0,30)
		tabBar.Position = UDim2.new(0,0,0,38)
		tabBar.Parent   = main
		local barLayout = Instance.new("UIListLayout")
		barLayout.FillDirection = Enum.FillDirection.Horizontal
		barLayout.SortOrder     = Enum.SortOrder.LayoutOrder
		barLayout.Padding       = UDim.new(0,2)
		barLayout.Parent        = tabBar

		local host = Instance.new("Frame")
		host.BackgroundTransparency = 1
		host.Size     = UDim2.new(1,0,1,-70)
		host.Position = UDim2.new(0,0,0,70)
		host.ClipsDescendants = true
		host.Parent   = main

		local tabs      = {}
		local activeTab = nil
		local windowOpen = true

		local function switchTo(tab)
			if activeTab then
				activeTab._page.Visible    = false
				activeTab._btn.TextColor3  = Palette.TextDim
			end
			activeTab = tab
			tab._page.Visible   = true
			tab._btn.TextColor3 = Palette.Accent
		end

		-- ── Mobile draggable toggle button ───────────────────────────────────
		-- Visible on ALL platforms but especially useful on mobile.
		-- Drag-threshold = 6px; tap (no drag) toggles UI open/close.
		local BTN_SIZE    = 48
		local DRAG_THRESH = 6

		local floatBtn = Instance.new("TextButton")
		floatBtn.Name            = "BerriToggle"
		floatBtn.Size            = UDim2.new(0, BTN_SIZE, 0, BTN_SIZE)
		floatBtn.Position        = UDim2.new(0, 12, 0.5, -BTN_SIZE/2)
		floatBtn.BackgroundColor3= Palette.Accent
		floatBtn.BorderSizePixel = 0
		floatBtn.Text            = "☰"
		floatBtn.Font            = Enum.Font.GothamBold
		floatBtn.TextColor3      = Palette.Text
		floatBtn.TextSize        = 20
		floatBtn.ZIndex          = 20
		floatBtn.Parent          = ScreenGui
		local fb_c = Instance.new("UICorner") fb_c.CornerRadius = UDim.new(1,0) fb_c.Parent = floatBtn

		-- Drag state
		local dragStart      = nil   -- Vector2 of touch/mouse start
		local btnDragStart   = nil   -- UDim2 position at drag start
		local wasDrag        = false

		local function screenVec(pos)
			return Vector2.new(pos.X, pos.Y)
		end

		-- Mouse support
		floatBtn.MouseButton1Down:Connect(function(x, y)
			dragStart    = Vector2.new(x, y)
			btnDragStart = floatBtn.Position
			wasDrag      = false
		end)
		InputService.InputChanged:Connect(function(inp)
			if dragStart and inp.UserInputType == Enum.UserInputType.MouseMovement then
				local delta = Vector2.new(inp.Position.X, inp.Position.Y) - dragStart
				if delta.Magnitude > DRAG_THRESH then wasDrag = true end
				if wasDrag then
					local vp    = ScreenGui.AbsoluteSize
					local newX  = math.clamp(btnDragStart.X.Offset + delta.X, 0, vp.X - BTN_SIZE)
					local newY  = math.clamp(btnDragStart.Y.Offset + delta.Y, 0, vp.Y - BTN_SIZE)
					floatBtn.Position = UDim2.new(0, newX, 0, newY)
				end
			end
		end)
		InputService.InputEnded:Connect(function(inp)
			if inp.UserInputType == Enum.UserInputType.MouseButton1 then
				dragStart = nil
				btnDragStart = nil
			end
		end)

		-- Touch support
		floatBtn.TouchLongPress:Connect(function() end)
		floatBtn.InputBegan:Connect(function(inp)
			if inp.UserInputType == Enum.UserInputType.Touch then
				dragStart    = screenVec(inp.Position)
				btnDragStart = floatBtn.Position
				wasDrag      = false
			end
		end)
		floatBtn.InputChanged:Connect(function(inp)
			if inp.UserInputType == Enum.UserInputType.Touch and dragStart then
				local delta = screenVec(inp.Position) - dragStart
				if delta.Magnitude > DRAG_THRESH then wasDrag = true end
				if wasDrag then
					local vp   = ScreenGui.AbsoluteSize
					local newX = math.clamp(btnDragStart.X.Offset + delta.X, 0, vp.X - BTN_SIZE)
					local newY = math.clamp(btnDragStart.Y.Offset + delta.Y, 0, vp.Y - BTN_SIZE)
					floatBtn.Position = UDim2.new(0, newX, 0, newY)
				end
			end
		end)
		floatBtn.InputEnded:Connect(function(inp)
			if inp.UserInputType == Enum.UserInputType.Touch then
				if not wasDrag then
					-- tap: toggle window
					windowOpen = not windowOpen
					main.Visible = windowOpen
					floatBtn.Text            = windowOpen and "☰" or "◉"
					floatBtn.BackgroundColor3= windowOpen and Palette.Accent or Palette.AccentDeep
					TweenService:Create(floatBtn, TWEEN_FAST, {
						BackgroundColor3 = windowOpen and Palette.Accent or Palette.AccentDeep
					}):Play()
				end
				dragStart    = nil
				btnDragStart = nil
			end
		end)

		-- Desktop: RightShift still works
		InputService.InputBegan:Connect(function(inp, gpe)
			if gpe then return end
			if inp.KeyCode == Enum.KeyCode.RightShift then
				windowOpen   = not windowOpen
				main.Visible = windowOpen
				floatBtn.Text            = windowOpen and "☰" or "◉"
				floatBtn.BackgroundColor3= windowOpen and Palette.Accent or Palette.AccentDeep
			end
		end)

		local win = { Holder = main, Library = Library }

		function win:AddTab(name)
			local tab = makeTab(tabBar, host, name)
			table.insert(tabs, tab)
			tab._btn.MouseButton1Click:Connect(function() switchTo(tab) end)
			if #tabs == 1 then switchTo(tab) end
			return tab
		end

		-- expose the floatBtn so Unload can destroy it
		Library._floatBtn = floatBtn

		return win
	end

	function Library:Unload()
		for _, sig in ipairs(self.Signals) do pcall(function() sig:Disconnect() end) end
		if self._floatBtn then self._floatBtn:Destroy() end
		ScreenGui:Destroy()
	end

	-- ── ThemeManager / SaveManager stubs ─────────────────────────────────────
	local ThemeManager = {}
	function ThemeManager:SetLibrary(_) end
	function ThemeManager:SetFolder(_) end
	function ThemeManager:ApplyToTab(_) end

	local SaveManager = {}
	function SaveManager:SetLibrary(_) end
	function SaveManager:IgnoreThemeSettings() end
	function SaveManager:SetIgnoreIndexes(_) end
	function SaveManager:SetFolder(_) end
	function SaveManager:BuildConfigSection(_) end
	function SaveManager:LoadAutoloadConfig() end

	local Builder = {}

	return Library, ThemeManager, SaveManager, Builder
end)()
end)()

-- ─── Aliases ─────────────────────────────────────────────────────────────────

local Options = Library.Options
local Toggles = Library.Toggles

-- ─── Ping measurement (runs BEFORE UI sliders are created) ───────────────────
-- We ping-sample for ~1.5 s using workspace:GetServerTimeNow() round-trip
-- approximation, then use that to set adaptive defaults for every ms-based
-- slider. This runs async so the UI still appears instantly.

local measuredPing = 60  -- fallback ms until sampled

local function measurePing()
	-- Roblox exposes Stats.Network.ServerStatsItem["Data Ping"] on the client.
	-- We read it three times over 1.5 s and take the median.
	local stats = game:GetService("Stats")
	local samples = {}
	for i = 1, 3 do
		local ok, val = pcall(function()
			return stats.Network.ServerStatsItem["Data Ping"].Value
		end)
		if ok and type(val) == "number" and val > 0 then
			table.insert(samples, math.floor(val))
		end
		task.wait(0.5)
	end
	if #samples == 0 then return end
	table.sort(samples)
	measuredPing = samples[math.ceil(#samples / 2)]  -- median
end

-- ── Ping-to-timing formula ────────────────────────────────────────────────────
-- Base delay should absorb half of round-trip latency plus a reaction window.
-- BlockDelay ≈ ping/2 + 340ms (reaction window in Bug Fixes is ~340 at 0ms ping)
-- HoldTime   ≈ ping/2 + 450ms
-- Per-weapon offsets scale by ping/2 (weapons vary ~34ms at 0ms ping)
local function adaptToMs(ping)
	local half = math.floor(ping / 2)
	local bd   = math.clamp(340 + half, 0, 800)       -- BlockDelay
	local ht   = math.clamp(450 + half, 50, 800)       -- HoldTime
	local wpn  = math.clamp(34  + math.floor(half/3), -200, 200)  -- weapon offset

	if Options.BlockDelay  then Options.BlockDelay:SetValue(bd)   end
	if Options.HoldTime    then Options.HoldTime:SetValue(ht)     end
	if Options.DelayKatana    then Options.DelayKatana:SetValue(wpn)    end
	if Options.DelayWakizashi then Options.DelayWakizashi:SetValue(wpn) end
	if Options.DelayTanto     then Options.DelayTanto:SetValue(wpn)     end
	if Options.DelayFist      then Options.DelayFist:SetValue(math.min(wpn + 5, 200)) end
	if Options.DelayOther     then Options.DelayOther:SetValue(math.max(wpn - 4, -200)) end
end

-- ─── Window ───────────────────────────────────────────────────────────────────

local Window = Library:CreateWindow({
	Title           = "Berri Hub | Bug Fixes",
	Footer          = "v3",
	NotifySide      = "Right",
	ShowCustomCursor= true,
})

local Tabs = {
	Main            = Window:AddTab("Main"),
	Misc            = Window:AddTab("Misc"),
	["UI Settings"] = Window:AddTab("UI Settings"),
}

-- ─── PARRY SETTINGS ──────────────────────────────────────────────────────────

local MainGroup = Tabs.Main:AddGroupbox({ Side="Left",  Name="Parry Settings" })
local TuneGroup = Tabs.Main:AddGroupbox({ Side="Right", Name="Per-Weapon Offset (ms)" })

MainGroup:AddToggle("AutoParry",   { Text="Enable Auto Parry",       Default=false })
MainGroup:AddDivider()
MainGroup:AddSlider("BlockDelay",  { Text="Block Delay (ms)",  Default=402, Min=0,   Max=800, Rounding=0,
	Tooltip="Auto-set from your ping on load — adjust manually if needed" })
MainGroup:AddSlider("HoldTime",    { Text="Hold Time (ms)",    Default=487, Min=50,  Max=800, Rounding=0 })
MainGroup:AddSlider("DetectRange", { Text="Detect Range (studs)", Default=15, Min=5, Max=60,  Rounding=0 })
MainGroup:AddDivider()
MainGroup:AddToggle("BlockFist",   { Text="Block Fist Attacks", Default=true  })
MainGroup:AddToggle("ShowNotifs",  { Text="Show Notifications", Default=false })
MainGroup:AddDivider()
MainGroup:AddLabel("● Idle", false, "StatusLabel")
MainGroup:AddLabel("Ping: measuring…", false, "PingLabel")

TuneGroup:AddSlider("DelayKatana",    { Text="Katana",        Default=34, Min=-200, Max=200, Rounding=0 })
TuneGroup:AddSlider("DelayWakizashi", { Text="Wakizashi",     Default=34, Min=-200, Max=200, Rounding=0 })
TuneGroup:AddSlider("DelayTanto",     { Text="Tanto",         Default=34, Min=-200, Max=200, Rounding=0 })
TuneGroup:AddSlider("DelayFist",      { Text="Fist",          Default=39, Min=-200, Max=200, Rounding=0 })
TuneGroup:AddSlider("DelayOther",     { Text="Other Weapons", Default=30, Min=-200, Max=200, Rounding=0 })

-- ─── FPS BOOSTER ─────────────────────────────────────────────────────────────

local FpsGroup = Tabs.Misc:AddGroupbox({ Side="Left", Name="FPS Booster" })

FpsGroup:AddToggle("FpsBooster",        { Text="Enable FPS Booster",     Default=false })
FpsGroup:AddSlider("TargetFps",         { Text="FPS Cap",                Default=60,  Min=30, Max=240, Rounding=0 })
FpsGroup:AddDivider()
FpsGroup:AddToggle("DisableParticles",  { Text="Remove Particles",       Default=true  })
FpsGroup:AddToggle("DisableShadows",    { Text="Remove Shadows",         Default=true  })
FpsGroup:AddToggle("DisablePostFX",     { Text="Remove Post FX",         Default=true  })
FpsGroup:AddToggle("DisableDecals",     { Text="Remove Decals/Textures", Default=false })
FpsGroup:AddToggle("DisableBillboards", { Text="Remove Billboards",      Default=false })
FpsGroup:AddToggle("LowRenderDistance", { Text="Low Render Distance",    Default=false })
FpsGroup:AddDivider()
FpsGroup:AddLabel("FPS: --", false, "FpsLabel")

-- ─── Services ─────────────────────────────────────────────────────────────────

local Players    = game:GetService("Players")
local RunService = game:GetService("RunService")
local Lighting   = game:GetService("Lighting")
local Workspace  = game:GetService("Workspace")

local lp = Players.LocalPlayer

-- ─── Parry State ──────────────────────────────────────────────────────────────

local connections = {}
local watched     = {}
local parryLock   = false
local lastParryAt = 0
local PARRY_CD    = 0.25

local ATTACK_REMOTES = { "atac", "down", "left", "right", "epicthing" }

local function setStatus(text)
	local lbl = Options.StatusLabel
	if lbl and lbl.SetText then lbl:SetText("● " .. text) end
end

local function isInRange(attackerPlayer)
	local myChar = lp.Character
	if not myChar then return false end
	local myRoot = myChar:FindFirstChild("HumanoidRootPart")
	if not myRoot then return false end
	local theirChar = attackerPlayer.Character
	if not theirChar then return false end
	local theirRoot = theirChar:FindFirstChild("HumanoidRootPart")
	if not theirRoot then return false end
	return (myRoot.Position - theirRoot.Position).Magnitude <= Options.DetectRange.Value
end

local function getLocalRemotes()
	local sources = {}
	local bp = lp:FindFirstChild("Backpack")
	local ch = lp.Character
	if bp then for _, v in ipairs(bp:GetChildren()) do table.insert(sources, v) end end
	if ch then for _, v in ipairs(ch:GetChildren()) do
		if v:IsA("Tool") then table.insert(sources, v) end
	end end
	local priority = {"katana","wakizashi","tanto","fist"}
	for _, name in ipairs(priority) do
		for _, tool in ipairs(sources) do
			if tool.Name:lower() == name then
				local b = tool:FindFirstChild("bloc")
				local u = tool:FindFirstChild("unbloc")
				if b and u then return b, u, tool.Name end
			end
		end
	end
	for _, tool in ipairs(sources) do
		local b = tool:FindFirstChild("bloc")
		local u = tool:FindFirstChild("unbloc")
		if b and u then return b, u, tool.Name end
	end
	return nil, nil, nil
end

local function getWeaponOffset(wname)
	wname = wname:lower()
	if wname == "katana"    then return Options.DelayKatana.Value    end
	if wname == "wakizashi" then return Options.DelayWakizashi.Value end
	if wname == "tanto"     then return Options.DelayTanto.Value     end
	if wname == "fist"      then return Options.DelayFist.Value      end
	return Options.DelayOther.Value
end

-- ─── Core Parry ──────────────────────────────────────────────────────────────

local function doParry(weaponName)
	if not Toggles.AutoParry.Value then return end
	if parryLock then return end
	local now = tick()
	if now - lastParryAt < PARRY_CD then return end

	parryLock   = true
	lastParryAt = now

	local baseDelay  = Options.BlockDelay.Value / 1000
	local offset     = getWeaponOffset(weaponName) / 1000
	local totalDelay = math.max(0, baseDelay + offset)
	local holdTime   = Options.HoldTime.Value / 1000

	setStatus("Incoming — " .. weaponName)

	task.delay(totalDelay, function()
		if not Toggles.AutoParry.Value then
			parryLock = false
			setStatus("Active")
			return
		end
		local bloc, unbloc = getLocalRemotes()
		if not bloc then
			parryLock = false
			setStatus("Active — no weapon")
			return
		end
		pcall(function() bloc:FireServer() end)
		setStatus("BLOCKING ← " .. weaponName)
		if Toggles.ShowNotifs.Value then
			Library:Notify({
				Title       = "Parried",
				Description = weaponName .. " — " .. math.floor(totalDelay*1000) .. "ms",
				Time        = 1.5,
			})
		end
		task.delay(holdTime, function()
			pcall(function() unbloc:FireServer() end)
			task.delay(0.1, function()
				parryLock = false
				setStatus("Active")
			end)
		end)
	end)
end

-- ─── Detection ───────────────────────────────────────────────────────────────

local function tryHookRemote(remote, weaponName, attackerPlayer)
	if firesignal then
		pcall(function()
			firesignal(remote.OnServerEvent, function(firer)
				if firer ~= lp and firer == attackerPlayer then
					if isInRange(attackerPlayer) then doParry(weaponName) end
				end
			end)
		end)
	end
	local ok, conn = pcall(function()
		return remote.OnClientEvent:Connect(function()
			if isInRange(attackerPlayer) then doParry(weaponName) end
		end)
	end)
	if ok and conn then table.insert(connections, conn) end
end

local function hookTool(tool, attackerPlayer)
	if not tool:IsA("Tool") then return end
	local wName = tool.Name
	if wName:lower() == "fist" and not Toggles.BlockFist.Value then return end
	for _, remoteName in ipairs(ATTACK_REMOTES) do
		local remote = tool:FindFirstChild(remoteName)
		if not remote then remote = tool:WaitForChild(remoteName, 1) end
		if remote and remote:IsA("RemoteEvent") then
			tryHookRemote(remote, wName, attackerPlayer)
		end
	end
end

local function watchCharacter(player, char)
	if not char then return end
	local hum = char:FindFirstChildOfClass("Humanoid") or char:WaitForChild("Humanoid", 4)
	if not hum then return end
	local animator = hum:FindFirstChildOfClass("Animator") or hum:WaitForChild("Animator", 4)
	local lastAnimFire = 0
	if animator then
		local conn = animator.AnimationPlayed:Connect(function()
			if not Toggles.AutoParry.Value then return end
			if not isInRange(player) then return end
			local now = tick()
			if now - lastAnimFire < 0.2 then return end
			lastAnimFire = now
			local eq = "katana"
			for _, child in ipairs(char:GetChildren()) do
				if child:IsA("Tool") then eq = child.Name break end
			end
			if eq:lower() == "fist" and not Toggles.BlockFist.Value then return end
			doParry(eq)
		end)
		table.insert(connections, conn)
	end
	for _, child in ipairs(char:GetChildren()) do task.spawn(hookTool, child, player) end
	local addConn = char.ChildAdded:Connect(function(child)
		if child:IsA("Tool") then task.wait(0.1) task.spawn(hookTool, child, player) end
	end)
	table.insert(connections, addConn)
end

local function watchPlayer(player)
	if player == lp then return end
	if watched[player] then return end
	watched[player] = true
	if player.Character then task.spawn(watchCharacter, player, player.Character) end
	local conn = player.CharacterAdded:Connect(function(char)
		task.wait(0.6)
		task.spawn(watchCharacter, player, char)
	end)
	table.insert(connections, conn)
end

local function startParry()
	for _, p in ipairs(Players:GetPlayers()) do task.spawn(watchPlayer, p) end
	local conn = Players.PlayerAdded:Connect(watchPlayer)
	table.insert(connections, conn)
	setStatus("Active")
end

local function stopParry()
	for _, conn in ipairs(connections) do pcall(function() conn:Disconnect() end) end
	connections = {}
	watched     = {}
	parryLock   = false
	setStatus("Idle")
end

Toggles.AutoParry:OnChanged(function()
	if Toggles.AutoParry.Value then
		startParry()
		Library:Notify({
			Title       = "Auto Parry ON",
			Description = "Ping: " .. measuredPing .. "ms | Delay: " .. Options.BlockDelay.Value .. "ms",
			Time        = 3,
		})
	else
		stopParry()
		Library:Notify({ Title = "Auto Parry OFF", Time = 2 })
	end
end)

-- ─── FPS BOOSTER ─────────────────────────────────────────────────────────────

local fpsBoosterActive   = false
local removedObjects     = {}
local originalProperties = {}
local fpsDisplayConn     = nil

local function startFpsCounter()
	if fpsDisplayConn then fpsDisplayConn:Disconnect() end
	local frameCount = 0
	local lastTime   = tick()
	fpsDisplayConn = RunService.RenderStepped:Connect(function()
		frameCount = frameCount + 1
		local now  = tick()
		if now - lastTime >= 0.5 then
			local fps = math.floor(frameCount / (now - lastTime))
			local lbl = Options.FpsLabel
			if lbl and lbl.SetText then lbl:SetText("FPS: " .. fps) end
			frameCount = 0
			lastTime   = now
		end
	end)
end

local function stopFpsCounter()
	if fpsDisplayConn then fpsDisplayConn:Disconnect() fpsDisplayConn = nil end
	local lbl = Options.FpsLabel
	if lbl and lbl.SetText then lbl:SetText("FPS: --") end
end

local function stripFromWorkspace(classFilter, storeList)
	for _, obj in ipairs(Workspace:GetDescendants()) do
		for _, cls in ipairs(classFilter) do
			if obj:IsA(cls) then
				table.insert(storeList, obj)
				obj.Parent = nil
				break
			end
		end
	end
end

local function applyFpsBooster()
	if fpsBoosterActive then return end
	fpsBoosterActive = true
	if setfpscap then setfpscap(Options.TargetFps.Value) end
	if Toggles.DisableShadows.Value then
		originalProperties.GlobalShadows  = Lighting.GlobalShadows
		originalProperties.ShadowSoftness = Lighting.ShadowSoftness
		Lighting.GlobalShadows  = false
		Lighting.ShadowSoftness = 0
	end
	if Toggles.DisablePostFX.Value then
		for _, obj in ipairs(Lighting:GetChildren()) do
			if obj:IsA("PostEffect") or obj:IsA("BlurEffect")
			or obj:IsA("ColorCorrectionEffect") or obj:IsA("BloomEffect")
			or obj:IsA("SunRaysEffect") or obj:IsA("DepthOfFieldEffect") then
				table.insert(removedObjects, obj)
				obj.Parent = nil
			end
		end
	end
	if Toggles.DisableParticles.Value  then stripFromWorkspace({"ParticleEmitter","Smoke","Fire","Sparkles"}, removedObjects) end
	if Toggles.DisableDecals.Value     then stripFromWorkspace({"Decal","Texture"}, removedObjects) end
	if Toggles.DisableBillboards.Value then stripFromWorkspace({"BillboardGui"}, removedObjects) end
	if Toggles.LowRenderDistance.Value then
		originalProperties.StreamingMin = Workspace.StreamingMinRadius
		Workspace.StreamingMinRadius    = 64
		Workspace.StreamingTargetRadius = 256
	end
	Library:Notify({ Title="FPS Booster ON", Description="Cap: "..Options.TargetFps.Value.."fps", Time=3 })
end

local function removeFpsBooster()
	if not fpsBoosterActive then return end
	fpsBoosterActive = false
	for _, obj in ipairs(removedObjects) do
		pcall(function()
			if obj:IsA("PostEffect") or obj:IsA("BlurEffect")
			or obj:IsA("ColorCorrectionEffect") or obj:IsA("BloomEffect")
			or obj:IsA("SunRaysEffect") or obj:IsA("DepthOfFieldEffect") then
				obj.Parent = Lighting
			else
				obj.Parent = Workspace
			end
		end)
	end
	removedObjects = {}
	if originalProperties.GlobalShadows ~= nil then
		Lighting.GlobalShadows  = originalProperties.GlobalShadows
		Lighting.ShadowSoftness = originalProperties.ShadowSoftness
	end
	if originalProperties.StreamingMin ~= nil then
		Workspace.StreamingMinRadius = originalProperties.StreamingMin
	end
	originalProperties = {}
	if setfpscap then setfpscap(0) end
	Library:Notify({ Title="FPS Booster OFF", Description="Settings restored.", Time=2 })
end

Toggles.FpsBooster:OnChanged(function()
	if Toggles.FpsBooster.Value then applyFpsBooster() startFpsCounter()
	else removeFpsBooster() stopFpsCounter() end
end)

Options.TargetFps:OnChanged(function()
	if Toggles.FpsBooster.Value and setfpscap then setfpscap(Options.TargetFps.Value) end
end)

-- ─── UI Settings ─────────────────────────────────────────────────────────────

local MenuGroup = Tabs["UI Settings"]:AddGroupbox({ Side="Left", Name="Menu" })

MenuGroup:AddToggle("ShowCustomCursor", {
	Text     = "Custom Cursor",
	Default  = true,
	Callback = function(v) Library.ShowCustomCursor = v end,
})
MenuGroup:AddDivider()
MenuGroup:AddLabel("Menu Keybind (Desktop: RightShift | Mobile: float button)")
MenuGroup:AddKeyPicker("MenuKeybind", { Default="RightShift", NoUI=true, Text="Menu keybind" })

MenuGroup:AddButton({ Text="Re-measure Ping & Adapt Delays", Func = function()
	task.spawn(function()
		Library:Notify({ Title="Measuring ping…", Time=2 })
		measurePing()
		adaptToMs(measuredPing)
		local lbl = Options.PingLabel
		if lbl and lbl.SetText then lbl:SetText("Ping: " .. measuredPing .. "ms (adapted)") end
		Library:Notify({
			Title       = "Ping adapted",
			Description = measuredPing .. "ms → BlockDelay " .. Options.BlockDelay.Value .. "ms",
			Time        = 3,
		})
	end)
end })

MenuGroup:AddButton({ Text="Unload", Func = function()
	removeFpsBooster()
	stopFpsCounter()
	stopParry()
	Library:Unload()
end })

Library.ToggleKeybind = Options.MenuKeybind

ThemeManager:SetLibrary(Library)
SaveManager:SetLibrary(Library)
SaveManager:IgnoreThemeSettings()
SaveManager:SetIgnoreIndexes({ "MenuKeybind" })
ThemeManager:SetFolder("BerriHub")
SaveManager:SetFolder("BerriHub/configs")
SaveManager:BuildConfigSection(Tabs["UI Settings"])
ThemeManager:ApplyToTab(Tabs["UI Settings"])
SaveManager:LoadAutoloadConfig()

setStatus("Idle — toggle to start")

-- ─── Ping measure + adapt (async, runs after UI is built) ────────────────────
task.spawn(function()
	measurePing()
	adaptToMs(measuredPing)
	local lbl = Options.PingLabel
	if lbl and lbl.SetText then
		lbl:SetText("Ping: " .. measuredPing .. "ms (adapted)")
	end
	Library:Notify({
		Title       = "Berri Hub loaded",
		Description = "Ping " .. measuredPing .. "ms → delays auto-set",
		Time        = 4,
	})
end)
