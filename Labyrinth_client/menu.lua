local composer = require("composer")
local widget = require("widget")
local client = require("client")
local thread = require ('thread')

local scene = composer.newScene()

local connectBtn, ip_w, port_w
local new_gameBtn, players_count_w
local connectToGameBtn, room_id_w

local status_bar, health_bar, ammo_bar, bomb_bar, chest_bar
local shootBtn, bombBtn, moveBtn, leaveGameBtn, makeTurnBtn
local leftBtn, upBtn, rightBtn, downBtn

local function onGameStarted(status)
	status_bar.isVisible = status
	leaveGameBtn.isVisible = status
	new_gameBtn.isVisible = not status
	players_count_w.isVisible = not status
	room_id_w.isVisible = not status
	connectToGameBtn.isVisible = not status

	if status then
		local main_thread = thread.instance
		local new_thread = main_thread:addThread(function()
			while not client:is_game_started() do
				if not client.player_idx or not client.room_id then
					return
				end
				coroutine.yield()
			end
			if client.player_idx and client.room_id then
				health_bar.isVisible = true
				ammo_bar.isVisible = true
				bomb_bar.isVisible = true
				chest_bar.isVisible = true
				main_thread:addThread(function()
					while true do
						local player_info = client:get_player_info()
						health_bar.text = "Health: "..player_info.health
						ammo_bar.text = "Ammo: "..player_info.ammo
						bomb_bar.text = "Bomb: "..player_info.bomb
						chest_bar.text = "Chest: "..player_info.chest
						if player_info.current_player_idx == client.player_idx then
							break
						end
						status_bar.text = "Ждем игрока с ID "..player_info.current_player_idx
						coroutine.yield()
					end
					shootBtn.isVisible = true
					bombBtn.isVisible = true
					moveBtn.isVisible = true
					makeTurnBtn.isVisible = true
					makeTurnBtn:setLabel('Продолжить')
					status_bar.text = "Ваш ход"
				end)
			else
				print("Can not begin game. Incorrect player info.")
			end
		end)
	end
end

local function onContinue()
	local status = client:make_turn()
	if status then
		status_bar.text = status
		makeTurnBtn:setLabel('Завершить ход')
	else
		makeTurnBtn.isVisible = false
		onGameStarted(true)
	end
	leftBtn.isVisible = false
	upBtn.isVisible = false
	rightBtn.isVisible = false
	downBtn.isVisible = false
	shootBtn.isVisible = false
	bombBtn.isVisible = false
	moveBtn.isVisible = false
	return true
end

local function createBtn(label, size, on_release)
	return widget.newButton{
		label = label,
		labelColor = {default = {255}, over = {128}},
		default = "button.png",
		over = "button-over.png",
		width = size.w, height = size.h,
		onRelease = on_release
	}
end

local function onActionBtnRelease()
	leftBtn.isVisible = true
	upBtn.isVisible = true
	rightBtn.isVisible = true
	downBtn.isVisible = true
end

local function onConnectBtnRelease()
	if client:start(ip_w.text, port_w.text) then
		ip_w.isVisible = false
		port_w.isVisible = false
		connectBtn.isVisible = false
		new_gameBtn.isVisible = true
		players_count_w.isVisible = true
		room_id_w.isVisible = true
		connectToGameBtn.isVisible = true
		return true
	end
end

local function onNewGameBtnRelease()
	if client:start_game(tonumber(players_count_w.text)) then
		onGameStarted(true)
		return true
	end
end

local function onConnectToGameBtnRelease()
	if client:connect_to_game(tonumber(room_id_w.text)) then
		onGameStarted(true)
		return true
	end
end

local function onLeaveGame()
	if client:leave_game() then
		health_bar.isVisible = false
		ammo_bar.isVisible = false
		bomb_bar.isVisible = false
		chest_bar.isVisible = false
		shootBtn.isVisible = false
		bombBtn.isVisible = false
		moveBtn.isVisible = false
		makeTurnBtn.isVisible = false
		onGameStarted(false)
		return true
	end
end

function scene:create(event)
	local sceneGroup = self.view

	local background = display.newImageRect("background.jpg", display.actualContentWidth, display.actualContentHeight )
	background.anchorX = 0
	background.anchorY = 0
	background.x = 0 + display.screenOriginX 
	background.y = 0 + display.screenOriginY

	local title = display.newText("Labyrinth", display.contentCenterX, 0, native.systemFont, 26)
	title:setFillColor(1.0, 1.0, 1.0)

	status_bar = display.newText("Ждем других игроков", display.contentCenterX, 200, native.systemFont, 26)
	status_bar:setFillColor(1.0, 1.0, 1.0)
	status_bar.isVisible = false

	health_bar = display.newText("Health: 5", display.contentCenterX + 70, 40, native.systemFont, 26)
	health_bar:setFillColor(1.0, 1.0, 1.0)
	health_bar.isVisible = false
	health_bar.size = 20

	ammo_bar = display.newText("Ammo: 5", display.contentCenterX + 70, 80, native.systemFont, 26)
	ammo_bar:setFillColor(1.0, 1.0, 1.0)
	ammo_bar.isVisible = false
	ammo_bar.size = 20

	bomb_bar = display.newText("Bombs: 5", display.contentCenterX + 70, 120, native.systemFont, 26)
	bomb_bar:setFillColor(1.0, 1.0, 1.0)
	bomb_bar.isVisible = false
	bomb_bar.size = 20

	chest_bar = display.newText("Chest: false", display.contentCenterX + 70, 160, native.systemFont, 26)
	chest_bar:setFillColor(1.0, 1.0, 1.0)
	chest_bar.isVisible = false
	chest_bar.size = 20

	status_bar = display.newText("Ждем других игроков", display.contentCenterX, 200, native.systemFont, 26)
	status_bar:setFillColor(1.0, 1.0, 1.0)
	status_bar.isVisible = false

	ip_w = native.newTextField(display.contentCenterX - 40, 200, 180, 36)
	ip_w.size = 18
	ip_w.text = "10.54.73.188"
	ip_w.isEditable = true

	port_w = native.newTextField(display.contentCenterX + 100, 200, 70, 36)
	port_w.size = 18
	port_w.text = "81"
	port_w.isEditable = true

	players_count_w = native.newTextField(display.contentCenterX + 100, 200, 70, 36)
	players_count_w.size = 18
	players_count_w.text = "2"
	players_count_w.isEditable = true
	players_count_w.isVisible = false

	room_id_w = native.newTextField(display.contentCenterX + 100, 240, 70, 36)
	room_id_w.size = 18
	room_id_w.text = "0"
	room_id_w.isEditable = true
	room_id_w.isVisible = false
	
	connectBtn = createBtn("Connect", {w = 154, h = 40}, onConnectBtnRelease)
	connectBtn.x = display.contentCenterX
	connectBtn.y = 260

	new_gameBtn = createBtn("Start new game", {w = 154, h = 40}, onNewGameBtnRelease)
	new_gameBtn.x = display.contentCenterX - 70
	new_gameBtn.y = 200
	new_gameBtn.isVisible = false

	connectToGameBtn = createBtn("Connect to room", {w = 154, h = 40}, onConnectToGameBtnRelease)
	connectToGameBtn.x = display.contentCenterX - 70
	connectToGameBtn.y = 240
	connectToGameBtn.isVisible = false

	makeTurnBtn = createBtn("Продолжить", {w = 154, h = 40}, onContinue)
	makeTurnBtn.x = display.contentCenterX
	makeTurnBtn.y = 430
	makeTurnBtn.isVisible = false

	leaveGameBtn = createBtn("Покинуть игру", {w = 154, h = 40}, onLeaveGame)
	leaveGameBtn.x = display.contentCenterX
	leaveGameBtn.y = 470
	leaveGameBtn.isVisible = false

	shootBtn = createBtn("Стрелять", {w = 154, h = 40}, function()
		client.action_type = 2
		onActionBtnRelease()
	end)
	shootBtn.x = display.contentCenterX - 70
	shootBtn.y = 80
	shootBtn.isVisible = false

	moveBtn = createBtn("Ходить", {w = 154, h = 40}, function()
		client.action_type = 1
		onActionBtnRelease()
	end)
	moveBtn.x = display.contentCenterX - 70
	moveBtn.y = 40
	moveBtn.isVisible = false

	bombBtn = createBtn("Бомба", {w = 154, h = 40}, function()
		client.action_type = 3
		onActionBtnRelease()
	end)
	bombBtn.x = display.contentCenterX - 70
	bombBtn.y = 120
	bombBtn.isVisible = false

	leftBtn = createBtn("<", {w = 50, h = 50}, function() client.action_dir = 0 end)
	leftBtn.x = display.contentCenterX - 70
	leftBtn.y = display.contentCenterY + 80
	leftBtn.isVisible = false

	upBtn = createBtn("^", {w = 50, h = 50}, function() client.action_dir = 1 end)
	upBtn.x = display.contentCenterX
	upBtn.y = display.contentCenterY + 30
	upBtn.isVisible = false

	rightBtn = createBtn(">", {w = 50, h = 50}, function() client.action_dir = 2 end)
	rightBtn.x = display.contentCenterX + 70
	rightBtn.y = display.contentCenterY + 80
	rightBtn.isVisible = false

	downBtn = createBtn("v", {w = 50, h = 50}, function() client.action_dir = 3 end)
	downBtn.x = display.contentCenterX
	downBtn.y = display.contentCenterY + 130
	downBtn.isVisible = false
	
	sceneGroup:insert(background)
	sceneGroup:insert(ip_w)
	sceneGroup:insert(port_w)
	sceneGroup:insert(connectBtn)
	sceneGroup:insert(new_gameBtn)
	sceneGroup:insert(connectToGameBtn)
	sceneGroup:insert(players_count_w)
	sceneGroup:insert(room_id_w)
	sceneGroup:insert(leaveGameBtn)
	sceneGroup:insert(shootBtn)
	sceneGroup:insert(moveBtn)
	sceneGroup:insert(bombBtn)
	sceneGroup:insert(downBtn)
	sceneGroup:insert(rightBtn)
	sceneGroup:insert(upBtn)
	sceneGroup:insert(leftBtn)
	sceneGroup:insert(makeTurnBtn)
end

scene:addEventListener("create", scene)
scene:addEventListener("show", scene)
scene:addEventListener("hide", scene)
scene:addEventListener("destroy", scene)

return scene