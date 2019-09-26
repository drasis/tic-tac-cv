local max = 0
local min = 1
local pieces = {}
pieces[0] = "o"
pieces[1] = "x"
pieces[2] = " "
local board
local infinity = math.huge
local totalNodes = 0
local leafs = 0

function otherPlayer(player)
	return (player + 1) % 2
end

function loadGame(pl)
	local game = {}
	for i=1,3 do
		local q = {}
		for j=1,3 do
			q[j] = ' '
		end
		game[i] = q
	end
	return boardNode(game, pl, 0)
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
			--print("max of self: ", checkMax(self))
			--print("min of self: ", checkMin(self))
			if checkMax(self) == 0 and checkMin(self) == 0 then 
				--print("we in here")
				for i,r in ipairs(self.state) do
					for j,v in ipairs(r) do
						-- print("this is v: ", v)
						if v == 0 then
						    -- print("we inserting in here")
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
			print("self: ", self)
			print("self.state: ", self.state)
			for j,i in ipairs(self.state) do
				for q,v in ipairs(i) do
					io.write(v, " ")
				end
				io.write("\n")
			end
		end,
		gameOver = function(self)
			return checkMax(self) ~= 0 or checkMin(self) ~= 0
		end,
		bestMove = function(self)
		if true then
			return self:possibleMoves()[math.random(1, #self:possibleMoves())]
		end

			if #self.moves == 0 then
				return nil
			end
			-- if self.player == max then
				local bestscore = -infinity
				local bestState = 1
			-- else
			-- end
			for i,v in ipairs(self:possibleMoves()) do
				if v.best.score > bestscore then
					bestState = i
					bestscore = v.best.score
				end
				if checkMax(v) then
					bestState = i
					bestscore = v.score
					break
				end
			end
			print("row: ", self:possibleMoves()[bestState].rowToMe)
			print("col: ", self:possibleMoves()[bestState].colToMe)
			return self:possibleMoves()[bestState]
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
		if state.player == max then
			if score > best.score then
				best.move = possibleMove
				best.score = score
			end
		end
		if state.player == max then
			if score < best.score then
				best.move = possibleMove
				best.score = score
			end
		end
		--state.score = state.score + possibleMove.score
	end
	state.best = best
	return best
end


function getPlotterMove(p0, p1, p2, p3, p4, p5, p6, p7, p8, player)
	local board = loadGame(player)
	board.state[1][1] = p0
	board.state[1][2] = p1
	board.state[1][3] = p2
	board.state[2][1] = p3
	board.state[2][2] = p4
	board.state[2][3] = p5
	board.state[3][1] = p6
	board.state[3][2] = p7
	board.state[3][3] = p8
	minimax(board)
	print(board)
	board:display()
	
	board = board:bestMove()
	print("board.state: ", board.state)
	-- board:bestMove().display()
	rtm = board.rowToMe - 1
	ctm = board.colToMe - 1
	return rtm * 3 + ctm
end

-- loadGame(1)

-- board:display()
-- -- print("------")
-- -- minimax(board)
-- -- print("board score: ", board.score)
-- while checkMax(board) == 0 and checkMin(board) == 0 do
-- 	io.write("row, column: ")
-- 	a,b = io.read("*n","*n")
-- 	local piece = pieces[otherPlayer(board.player)]
-- 	board = boardNode(copyAndSubstitue(board.state, a, b, piece),
-- 		otherPlayer(board.player), board.depth + 1)
-- 	board:display()
-- 	if checkMax(board) ~= 0 or checkMin(board) ~= 0 then
-- 		break
-- 	end
-- 	print("------\ncomputer move:")
-- 	getPlotterMove(board.state[1][1], board.state[1][2], board.state[1][3], 
-- 					board.state[2][1], board.state[2][2], board.state[2][3], 
-- 					board.state[3][1], board.state[3][2], board.state[3][3], board.player)
-- 	-- board = board:bestMove()
-- 	board:display()
-- end

-- print(board.best.move.best.move.best.score)
-- print("total nodes: ", totalNodes)
-- print("total leafs: ", leafs)
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