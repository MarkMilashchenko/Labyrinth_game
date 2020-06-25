local socket = require("socket")

local ACTIONS = {
	['1'] = 'move',
	['2'] = 'shoot',
	['3'] = 'bomb'
}

local OBJECTS = {
	['0'] = 'пустоту',
	['1'] = 'Арсенал',
	['2'] = 'Аптеку',
	['3'] = 'Телепорт'
}

local action_results = {
	move = {
		['true'] = {
			'Ты вступил в темноту', 'Ты нашел сундук. Теперь на выход!', 'Ты попал в '
		},
		['false'] = {
			'Ты ударился лбом об стену', 'Ты наступил на ногу игроку с ID ', 'Без сундука вытйи нельзя'
		}
	},
	shoot = {
		['true'] = {
			'Еще одна дырка в игроке с ID '
		},
		['false'] = {
			'Зря стрелял', 'Патронов нетЬ'
		}
	},
	bomb = {
		['true'] = {
			'Ты разворотил стену'
		},
		['false'] = {
			"Ты взорвал воздух и еще пару воробьев", 'Стена оказалась внешней - она неразрушима', 'Бомб нетЬ'
		}
	}
}

local client = {}

function client.str_toboolean(str)
	return str == 'true' or str == 'TRUE' or str == 'True'
end

function client.split(str)
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

function client.valueToString(value)
    local type = type(value)
    if "number" == type or "boolean" == type then
        return tostring(value)
    else
        return string.format("%q", value)
    end
end

function client.parse_answer(answer)
	local res, err = nil, "error: answer wrong type!!!"
	if type(answer) == 'string' then
		res = client.split(answer)
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

function client:start(host, port)
	self.host = host or 'localhost'
	self.port = port or ''
	print("Attempting connection to host '" ..self.host.. "' and port " ..self.port.. "...")
	self.connection = socket.connect(self.host, self.port)
	if self.connection then
		print("Connected to "..self.host..':'..self.port..'!')
		if self.connection:send('test_connection\n') then
			local strout, e = self.connection:receive()
			if not e then
				strout = client.parse_answer(strout)
				if strout and client.str_toboolean(strout[2]) then
					return true
				else
					print('Can not create connection. Wrong answer.')
				end
			elseif e then
				print('error: '..e)
			end
		else
			print("Can not send msg to host: test_connection")
		end
	else
		print("Can not connect to "..self.host..':'..self.port..'!')
	end
end

function client:start_game(player_num)
	if self.room_id and self.player_idx then
		print('Player with ID: '..self.player_idx..' allready in room #'..self.room_id)
		return
	end
	if not player_num then
		print("Unknown players count")
		return
	end
	self.connection = socket.connect(self.host, self.port)
	if not self.connection then
		print("Can not connect to "..self.host..':'..self.port..'!')
		return
	end
	local info = 'start_game:'..client.valueToString(player_num)..'\n'
	if self.connection:send(info) then
		local strout, e = self.connection:receive()
		if not e then
			strout = client.parse_answer(strout)
			if strout then
				self.room_id = tonumber(strout[2])
				if self.room_id > -1 then
					self.player_idx = 0
					return true
				else
					print('Incorrect room ID: '..self.room_id)
				end
			else
				print('Can not start game. Wrong answer.')
			end
		elseif e then
			print('error: '..e)
		end
	else
		print("Can not send msg to host: "..info)
	end
end

function client:connect_to_game(room_id)
	if self.room_id and self.player_idx then
		print('Player with ID: '..self.player_idx..' allready in room #'..self.room_id)
		return
	end
	if not room_id then
		print("Unknown room ID")
		return
	end
	self.connection = socket.connect(self.host, self.port)
	if not self.connection then
		print("Can not connect to "..self.host..':'..self.port..'!')
		return
	end
	local info = 'connect_to_game:'..client.valueToString(room_id)..'\n'
	if self.connection:send(info) then
		local strout, e = self.connection:receive()
		if not e then
			strout = client.parse_answer(strout)
			if strout then
				self.player_idx = tonumber(strout[2])
				if self.player_idx > 0 then
					self.room_id = room_id
					return true
				else
					print('Incorrect player ID')
				end
			else
				print('Can not connect to game. Wrong answer.')
			end
		elseif e then
			print('error: '..e)
		end
	else
		print("Can not send msg to host: "..info)
	end
end

function client:leave_game()
	if not self.room_id then
		self.player_idx = nil
		return true
	end
	self.connection = socket.connect(self.host, self.port)
	if not self.connection then
		print("Can not connect to "..self.host..':'..self.port..'!')
		return
	end
	local info = 'leave_game:'..client.valueToString(self.room_id)..':'..client.valueToString(self.player_idx)..'\n'
	if self.connection:send(info) then
		local strout, e = self.connection:receive()
		if not e then
			strout = client.parse_answer(strout)
			if strout then
				if client.str_toboolean(strout[2]) then
					self.player_idx = nil
					self.room_id = nil
					return true
				end
			else
				print('Can not leave the game. Wrong answer.')
			end
		elseif e then
			print('error: '..e)
		end
	else
		print("Can not send msg to host: "..info)
	end
end

function client:is_game_started()
	if not self.room_id or not self.player_idx then
		print('Incorrect player info')
		return
	end
	self.connection = socket.connect(self.host, self.port)
	if not self.connection then
		print("Can not connect to "..self.host..':'..self.port..'!')
		return
	end
	local info = 'is_game_started:'..client.valueToString(self.room_id)..'\n'
	if self.connection:send(info) then
		local strout, e = self.connection:receive()
		if not e then
			strout = client.parse_answer(strout)
			if strout then
				return client.str_toboolean(strout[2])
			else
				print('Can not check game status. Wrong answer.')
			end
		elseif e then
			print('error: '..e)
		end
	else
		print("Can not send msg to host: "..info)
	end
end

function client:get_player_info()
	if not self.room_id or not self.player_idx then
		print('Incorrect player info')
		return
	end
	self.connection = socket.connect(self.host, self.port)
	if not self.connection then
		print("Can not connect to "..self.host..':'..self.port..'!')
		return
	end
	local info = 'get_player_info:'..client.valueToString(self.room_id)..':'..client.valueToString(self.player_idx)..'\n'
	if self.connection:send(info) then
		local strout, e = self.connection:receive()
		if not e then
			strout = client.parse_answer(strout)
			if strout then
				return {
					health = strout[2],
					ammo = strout[3],
					bomb = strout[4],
					chest = strout[5],
					current_player_idx = tonumber(strout[6])
				}
			else
				print('Can not get player info. Wrong answer.')
			end
		elseif e then
			print('error: '..e)
		end
	else
		print("Can not send msg to host: "..info)
	end
end

function client:make_turn()
	if not self.room_id or not self.player_idx then
		print('Incorrect player info')
		return
	end
	self.connection = socket.connect(self.host, self.port)
	if not self.connection then
		print("Can not connect to "..self.host..':'..self.port..'!')
		return
	end
	local info = 'make_turn:'..client.valueToString(self.room_id)..':'..client.valueToString(self.player_idx)
	if self.action_type and self.action_dir then
		info = info..":"..self.action_type..":"..self.action_dir..'\n'
	else
		info = info..":0:0\n" 
	end
	print(info)
	if self.connection:send(info) then
		local strout, e = self.connection:receive()
		if not e then
			strout = client.parse_answer(strout)
			if strout then
				if not self.action_type or not self.action_dir then
					return
				end
				local success = strout[2]
				local k1 = tonumber(strout[3])
				local k2 = strout[4] or ''
				local info = action_results[ACTIONS[tostring(self.action_type)]][success]
				self.action_type = nil
				self.action_dir = nil
				if success and self.action_type == 1 and k1 == 3 then
					return info[k1]..OBJECTS[k2]
				else
					return info[k1]..k2
				end
			else
				print('Can not make turn. Wrong answer.')
			end
		elseif e then
			print('error: '..e)
		end
	else
		print("Can not send msg to host: "..info)
	end
end

return client
