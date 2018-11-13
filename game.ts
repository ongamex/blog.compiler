g_dt = 0.0;
g_allGameObjects = array{};

makePlayer = fn(x, y) {
	result = {
		type = "player";
		x = x;
		y = y;
		health = 3;
	};

	return result;
};

initGame = fn() {
	print "initGame";
	array_push(g_allGameObjects, makePlayer(168, 300));
};

updateGame = fn() {
	for t = 0; t < array_size(g_allGameObjects); t = t + 1 {
		obj = g_allGameObjects[t];
		if obj.type == "player" {
			obj.x = obj.x + g_dt * getXMoveInput() * 100.0;
		}
	}
};
