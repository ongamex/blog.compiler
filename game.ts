g_dt = 0.0;
g_allGameObjects = array{};
g_nextId = 0;

makePlayer = fn(x, y) {
	r = {
		id = g_nextId;
		type = "player";
		x = x;
		y = y;
		radius = 32;
		health = 3;
	};
	g_nextId = g_nextId + 1;
	return r;
};

makeProjectle = fn(x, y) {
	r = {
		id = g_nextId;
		type = "projectile";
		x = x;
		y = y;
		radius = 8;
	};
	g_nextId = g_nextId + 1;
	return r;
};

makeEnemy = fn(x, y) {
	r = {
		id = g_nextId;
		type = "enemy";
		x = x;
		y = y;
		radius = 16;
	};
	g_nextId = g_nextId + 1;
	return r;
};


initGame = fn() {
	array_push(g_allGameObjects, makePlayer(168, 300));
	array_push(g_allGameObjects, makeEnemy(50  - 16, -64));
	array_push(g_allGameObjects, makeEnemy(100 - 16, -32));
	array_push(g_allGameObjects, makeEnemy(150 - 16, -64));
	array_push(g_allGameObjects, makeEnemy(200 - 16, -32));
	array_push(g_allGameObjects, makeEnemy(250 - 16, -64));
	array_push(g_allGameObjects, makeEnemy(300 - 16, -32));
	array_push(g_allGameObjects, makeEnemy(350 - 16, -64));

	array_push(g_allGameObjects, makeEnemy(50  - 16, -164));
	array_push(g_allGameObjects, makeEnemy(100 - 16, -132));
	array_push(g_allGameObjects, makeEnemy(150 - 16, -164));
	array_push(g_allGameObjects, makeEnemy(200 - 16, -132));
	array_push(g_allGameObjects, makeEnemy(250 - 16, -164));
	array_push(g_allGameObjects, makeEnemy(300 - 16, -132));
	array_push(g_allGameObjects, makeEnemy(350 - 16, -164));
};

updateGame = fn() {
	
	id2del = array{};

	for t = 0; t < array_size(g_allGameObjects); t = t + 1 {
		obj = g_allGameObjects[t];
		if obj.type == "player" {
			obj.x = obj.x + g_dt * getXMoveInput() * 200.0;

			if isFireBtnPressed() {
				array_push(g_allGameObjects, makeProjectle(obj.x + 32 - 8, obj.y - 32));
			}
		}

		if obj.type == "enemy" {
			obj.y = obj.y + g_dt * 200.0;

			if obj.y > 464 {
				obj.y = -64;
			}
		}

		if obj.type == "projectile" {
			obj.y = obj.y - (g_dt * 400.0);

			bx = obj.x;
			by = obj.y;

			for e = 0; e < array_size(g_allGameObjects); e = e + 1 {
				enemy = g_allGameObjects[e];
				if enemy.type == "enemy" {
					ex = enemy.x;
					ey = enemy.y;

					dx = ex - bx;
					dy = ey - by;

					d2 = dx*dx + dy*dy;
					r2 = (obj.radius + enemy.radius) * (obj.radius + enemy.radius);
					if d2 < r2 {
						array_push(id2del, obj.id);
						enemy.x = enemy.radius*2 + (400 - enemy.radius*2) * getRandomNmbr();
						enemy.y = -enemy.radius*2;
					}
				}
			}

			if obj.y < -obj.radius {
				array_push(id2del, obj.id);
			}
		}

		
	}

	for t = 0; t < array_size(id2del); t = t + 1 {
		for i = 0; i < array_size(g_allGameObjects); i = i + 1 {
			if g_allGameObjects[i].id == id2del[t] {
				array_pop(g_allGameObjects, i);
				i = i - 1;
			}
		} 
	}
};
