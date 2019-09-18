local min = 0
local max = 1
local pieces = {}
pieces[0] = "o"
pieces[1] = "x"
local board
local infinity = math.huge
local totalNodes = 0
local leafs = 0

-- function allEqual( ... )
-- 	local arg = table.pack(...)
-- 	local a1 = ...
-- 	for i=0,#arg do
-- 		print(i)
-- 		if arg[i] ~= a1 then
-- 			return false
-- 		end
-- 	end
-- 	return true
-- end

function otherPlayer(player)
	return (player + 1) % 2
end

function loadGame()
	local game = {}
	for i=1,3 do
		local q = {}
		for j=1,3 do
			q[j] = ' '
		end
		game[i] = q
	end
	board = boardNode(game, 0, 0)
end

function boardNode(game, pl, depth, i, j)
	return {
		state = game,
		player = pl,
		depth = depth,
		best = nil,
		moves = {},
		rowToMe = i,
		colToMe = j,
		possibleMoves = function(self)
			-- if checkMax(self) ~= 0 or checkMin(self) ~= 0 then
			-- 	self:display()
			-- end
			if #self.moves > 0 then
				return self.moves
			end
			local moves = {}
			local piece = pieces[otherPlayer(self.player)]
			if checkMax(self) == 0 and checkMin(self) == 0 then 
				for i,r in ipairs(self.state) do
					for j,v in ipairs(r) do
						if v == ' ' then
							table.insert(moves, 
								boardNode(
									copyAndSubstitue(self.state, i, j, piece),
									otherPlayer(self.player),
									self.depth + 1, i, j
								)
							)
						end
					end
				end
			-- else
			-- 	for j,i in ipairs(self.state) do
			-- 		for q,v in ipairs(i) do
			-- 			io.write(v, " ")
			-- 		end
			-- 		io.write("\n")
			-- 	end
			end
			self.moves = moves
			return moves
		end,
		display = function(self)
			for j,i in ipairs(self.state) do
				for q,v in ipairs(i) do
					io.write(v, " ")
				end
				io.write("\n")
			end
		end,
		gameOver = function(self)
			return checkMax(self) ~= 0 or checkMin(self) ~= 0
		end
	}
end

function copyAndSubstitue(state, i, j, piece)
	local ret = {}
	for x, r in ipairs(state) do
		local temp = {}
		for y, v in ipairs(r) do
			temp[y] = v
		end
		ret[x] = temp
	end
	ret[i][j] = piece
	return ret
end

function column(state, col, piece)
	for i=1,#state do
		if state[i][col] ~= piece then 
			return false
		end
	end
	return true
end

function row(state, r, piece)
	for i=1,#state[1] do
		if state[r][i] ~= piece then 
			return false
		end
	end
	return true
end

function diagCount(state, piece)
	local ct = 0
	if state[1][1] == piece and state[2][2] == piece and state[3][3] == piece then
		ct = 1
	end
	if state[1][3] == piece and state[2][2] == piece and state[3][1] == piece then
		return ct + 1
	end
	return ct
end

function checkPlayer(state, piece)
	local wins = 0
	-- check columns
	for col=1, #state do
		if column(state, col, piece) then wins = wins + 1 end
	end
	-- check rows
	for r=1, #state do
		if row(state, r, piece) then wins = wins + 1 end
	end
	-- check diagonals
	wins = wins + diagCount(state, piece)
	return wins
	-- if wins > 0 then
	-- 	return 1
	-- end
	-- return 0
end

function checkMax(board)
	return checkPlayer(board.state, 'x')
end

function checkMin(board)
	return 0 - checkPlayer(board.state, 'o')
end

function minimax(state)
	totalNodes = totalNodes + 1
	state.score = 0
	local best
	if state.player == max then
		best = {move = nil, score = -infinity}
	else
		best = {move = nil, score = infinity}
	end
	if state.depth == 9 or state:gameOver() then
		-- state:display()
		-- print("------")
		leafs = leafs + 1
		local scr = checkMin(state) + checkMax(state)
		state.best = {move = nil, score = scr}
		state.score = scr
		return {move = nil, score = scr}
	end
	for _, possibleMove in ipairs(state:possibleMoves()) do
		local gotBack = minimax(possibleMove)
		local move = gotBack.move
		local score = gotBack.score
		if state.player == max and score > best.score then
			best.move = possibleMove
			best.score = score
		end
		if state.player == min and score < best.score then
			best.move = possibleMove
			best.score = score
		end
		state.score = state.score + possibleMove.score
	end
	state.best = best
	return best
end


loadGame()
board.state[1][1] = "o"

board:display()
print("------")
minimax(board)
print("board score: ", board.score)
while checkMax(board) == 0 and checkMin(board) == 0 do
	io.write("row, column: ")
	a,b = io.read("*n","*n")
	local piece = pieces[otherPlayer(board.player)]
	board = boardNode(copyAndSubstitue(board.state, a, b, piece),
		otherPlayer(board.player), board.depth + 1)
	board:display()
	print("------\ncomputer move:")
	minimax(board)
	board = board.best.move
	board:display()
end

-- print(board.best.move.best.move.best.score)
print("total nodes: ", totalNodes)
print("total leafs: ", leafs)
-- for i,v in ipairs(board:possibleMoves()) do
-- 	for j,q in ipairs(v:possibleMoves()) do
-- 		for k,s in ipairs(q:possibleMoves()) do
-- 			s:display()
-- 			-- print(s.depth)
-- 			abc = abc + 1
-- 			print("------")
-- 		end
-- 	end
-- end
-- print(abc)

-- print(-infinity)