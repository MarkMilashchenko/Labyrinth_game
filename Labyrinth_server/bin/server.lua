local lua_socket = require("socket")

local lua_Server = {}

function lua_Server.boolean_to_str(value)
    local type = type(value)
    assert(type == 'boolean', "Incorrect type for converting")
    return value and 'true' or 'false'
end

function lua_Server.split(str)
    local res = {}
    local char, sub_str = '', ''
    local str_len = string.len(str)
    for i = 1, str_len do
        char = string.sub(str, i, i)
        if char == ':' then
            table.insert(res, sub_str)
            sub_str = ''
        else
            sub_str = sub_str .. char
        end
    end
    table.insert(res, sub_str)
    return res
end

function lua_Server.parse_answer(answer)
	local res, err = nil, "error: answer wrong type!!!"
	if type(answer) == 'string' then
		res = lua_Server.split(answer)
		for _, arg in ipairs(res) do
			local num = tonumber(arg)
			if num then
				arg = num
			end
		end
		err = nil
	end
	return res, err
end

function lua_Server:test_connection()
    return 'test_connection:true', nil
end

function lua_Server:start_game(players_num)
    local res = startGame(tonumber(players_num))
    return 'start_game:'..res, not res and 'StartGame: incorrect output'
end

function lua_Server:connect_to_game(room_id)
    local res = connectToGame(tonumber(room_id))
    return 'connect_to_game:'..res, not res and 'connectToGame: incorrect output'
end

function lua_Server:leave_game(room_id, player_idx)
    local res = leaveGame(tonumber(room_id), tonumber(player_idx))
    return 'leave_game:'..lua_Server.boolean_to_str(res), res == nil and 'leaveGame: incorrect output'
end

function lua_Server:is_game_started(room_id)
    local res = isGameStarted(tonumber(room_id))
    return 'is_game_started:'..lua_Server.boolean_to_str(res), res == nil and 'isGameStarted: incorrect output'
end

function lua_Server:get_player_info(room_id, player_idx)
    -- TODO: push a Lua_Table from cpp
    local health, ammo, bomb, chest, current_p_idx = getPlayerInfo(tonumber(room_id), tonumber(player_idx))
    if not (health and ammo and bomb and chest ~= nil and current_p_idx) then
        return nil, 'getPlayerInfo: incorrect output'
    end
    return 'get_player_info:'..health..':'..ammo..':'..bomb..':'..lua_Server.boolean_to_str(chest)..':'..current_p_idx, nil
end

function lua_Server:make_turn(room_id, player_idx, action_type, dir)
    local status, case_id, key_1 = makeTurn(tonumber(room_id), tonumber(player_idx), tonumber(action_type), tonumber(dir))
    if not (status ~= nil and case_id) then
        return nil, 'makeTurn: incorrect output'
    end
    return 'make_turn:'..lua_Server.boolean_to_str(status)..':'..case_id..':'..(key_1 or ''), nil
end

function lua_Server:process(answer)
    local res, err = lua_Server.parse_answer(answer)
    if not err and self[res[1]] then
        local num = #res
        local args = {}
        for i = 2, num do
            table.insert(args, res[i])
        end
        return self[res[1]](self, unpack(args))
    else
        return nil, "Wrong parsing result"
    end
end

function lua_Server:Update()
    cpp_update()
end

function lua_Server:start(host, port)
    print("Start server...")
    self.host = host or 'localhost'
    self.port = port or 81
    print("Binding to host '" ..host.. "' and port " ..port.. "...")
    self.socket = assert(lua_socket.bind(host, port))
    print("Started!")

    local ip, port = self.socket:getsockname()
    assert(ip and port, 'Incorrect ip and port')
    print("Waiting connection from client on " .. ip .. ":" .. port .. "...")

    while true do
        local connection = assert(self.socket:accept())
        connection:settimeout(10)
        local line, err = connection:receive()
        if not err and line then
            local res, err = self:process(line)
            if not err then
                connection:send(res..'\n')
            else
                print('error: '..err)
                connection:send('\n')
            end
        else
            print(err)
        end
        connection:close()
        self:Update()
    end
end

lua_Server:start('*', 81)
