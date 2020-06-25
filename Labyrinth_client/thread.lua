local thread = {}

function thread:init()
	self.threads = {}
	return self
end

function thread:addThread(f)
	local t = coroutine.create(f)
	table.insert(self.threads, t)
	return t
end

function thread:removeThread(t)
	for i, trd in ipairs(self.threads) do
		if trd == t then
			table.remove(self.threads, i)
			return
		end
	end
end

function thread:update()
	local need_to_remove = {}
	for i, trd in ipairs(self.threads) do
		if coroutine.status(trd) == 'suspended' then
			coroutine.resume(trd)
		end
		if coroutine.status(trd) == 'dead' then
			table.insert(need_to_remove, i)
		end
	end
	local count = #need_to_remove
	for i = count, 1, -1 do
		table.remove(self.threads, need_to_remove[i])
	end
end

function thread:start()
	local update = function()
		self:update()
	end
	timer.performWithDelay(100, update, 0)
end

return thread
