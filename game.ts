//-----------------------------------------------------
// Globals for the game.
//------------------------------------------------------
g_allGameObjects = array{};
g_nextId = 0;
g_player = 0;
//------------------------------------------------------
// Globals updated by the so called "engine".
//------------------------------------------------------
g_dt = 0.0;

//------------------------------------------------------
// Global that are read by the "engine".
//------------------------------------------------------
g_isGameOver = 0;

//------------------------------------------------------
// Game objects definitions.
//------------------------------------------------------
makePlayer = fn(x, y) {
	r = {
		id = g_nextId;
		type = "player";
		x = x;
		y = y;
		radius = 64;
		health = 3;
		recoil = 0;
		gunLevel = 1;

	};
	g_player = r;
	g_nextId = g_nextId + 1;
	return r;
};

makeEnemy = fn(x, y) {
	r = {
		id = g_nextId;
		type = "enemy";
		x = x;
		y = y;
		radius = 32;
		speed = getRandomNmbr() * 200 + 150;
		phase = getRandomNmbr() * 100;
		phaseSpeed = getRandomNmbr()*3.0  + 1.0;
		phaseMag = getRandomNmbr() * 1.2;
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
		radius = 16;
		speedY = -900;
		speedX = 0;
	};
	g_nextId = g_nextId + 1;
	return r;
};

makePowerUp = fn(x, y) {
	r = {
		id = g_nextId;
		type = "powerUp";
		x = x;
		y = y;
		radius = 8;
		speedY = 50;
	};
	g_nextId = g_nextId + 1;
	return r;
};

makeExplosion = fn(x, y) {
	r = {
		id = g_nextId;
		type = "explosion";
		x = x;
		y = y;
		radius = 256;
		progress = 0;
	};
	g_nextId = g_nextId + 1;
	return r;
};

//------------------------------------------------------
// Called once at the begining of the game to initialize
// everything to it's defaults.
//------------------------------------------------------
initGame = fn() {
	g_isGameOver = 0;
	g_allGameObjects = array{};
	g_nextId = 0;

	array_push(g_allGameObjects, makePlayer(400 - 32, 640));

	array_push(g_allGameObjects, makeEnemy(100 - 32, -64));
	array_push(g_allGameObjects, makeEnemy(200 - 32, -32));
	array_push(g_allGameObjects, makeEnemy(300 - 32, -64));
	array_push(g_allGameObjects, makeEnemy(400 - 32, -32));
	array_push(g_allGameObjects, makeEnemy(500 - 32, -64));
	array_push(g_allGameObjects, makeEnemy(600 - 32, -32));
	array_push(g_allGameObjects, makeEnemy(700 - 32, -64));

	array_push(g_allGameObjects, makeEnemy(100 - 32, -64));
	array_push(g_allGameObjects, makeEnemy(200 - 32, -32));
	array_push(g_allGameObjects, makeEnemy(300 - 32, -64));
	array_push(g_allGameObjects, makeEnemy(400 - 32, -32));
	array_push(g_allGameObjects, makeEnemy(500 - 32, -64));
	array_push(g_allGameObjects, makeEnemy(600 - 32, -32));
	array_push(g_allGameObjects, makeEnemy(700 - 32, -64));

	array_push(g_allGameObjects, makeEnemy(200 - 32, -132));
	array_push(g_allGameObjects, makeEnemy(400 - 32, -132));
	array_push(g_allGameObjects, makeEnemy(600 - 32, -132));
};

//------------------------------------------------------
// The update function called every frame.
//------------------------------------------------------
updateGame = fn() {
	id2del = array{};

	// Iterate through all game objects and perform their update logic.
	for t = 0; t < array_size(g_allGameObjects); t = t + 1
	{
		obj = g_allGameObjects[t];

		// Player
		if obj.type == "player" {
			
			if shouldUseMouseForInput() {
				obj.x = getMouseX() - obj.radius;
				obj.y = getMouseY() - obj.radius;
			} else {
				obj.x = obj.x + g_dt * getXMoveInput() * 400.0;
				obj.y = obj.y + g_dt * getYMoveInput() * 400.0;
			}

			if isFireBtnPressed() {
				if obj.gunLevel == 0 {
					jitter = (getRandomNmbr() * 2 - 1) * 12;
					prjectile = makeProjectle(obj.x + 48 + jitter, obj.y - 32);
					prjectile.speedX = jitter * 6.3;
					array_push(g_allGameObjects, prjectile);
					obj.recoil = 1.0;
				} else {
					jitter = (getRandomNmbr() * 2 - 1) * 12;
					prjectile = makeProjectle(obj.x + 32 + jitter, obj.y - 32);
					prjectile.speedX = -132;
					array_push(g_allGameObjects, prjectile);

					prjectile = makeProjectle(obj.x + 48 + jitter, obj.y - 32);
					prjectile.speedX = 0;
					array_push(g_allGameObjects, prjectile);

					prjectile = makeProjectle(obj.x + 64 + jitter, obj.y - 32);
					prjectile.speedX = 132;
					array_push(g_allGameObjects, prjectile);

					obj.recoil = 1.0;
				}
			}

			// Update the time passed needed for the recoil "animation".
			obj.recoil = obj.recoil - g_dt * 7.35;
			if obj.recoil < 0 {
				obj.recoil = 0;
			}

			// Clamp the position to the edges of the screen.
			if obj.x < 0 {
				obj.x = 0;
			}
			if obj.x > 800 - obj.radius*2 {
				obj.x = 800 - obj.radius*2;
			}

			if obj.y < 0 {
				obj.y = 0;
			}
			if obj.y > 800 - obj.radius*2 {
				obj.y = 800 - obj.radius*2;
			}

			// Check if the player ship is coliding with any enemies.
			// If so apply damage to it.
			for e = 0; e < array_size(g_allGameObjects); e = e + 1 {
				enemy = g_allGameObjects[e];
				if enemy.type == "enemy" {
					ex = enemy.x;
					ey = enemy.y;

					dx = ex - obj.x;
					dy = ey - obj.y;

					d2 = dx*dx + dy*dy;
					r2 = (obj.radius + enemy.radius) * (obj.radius + enemy.radius);

					if d2 < r2 {
						array_push(id2del, obj.id);
						g_isGameOver = 1;
					}
				}
			}
		}

		// Enemy.
		if obj.type == "enemy" {
			obj.phase = obj.phase + g_dt;
			obj.y = obj.y + g_dt * obj.speed;
			obj.x = obj.x + sin(obj.phase * obj.phaseSpeed) * obj.phaseMag;

			if obj.y > 928 {
				obj.y = -obj.radius*2 - getRandomNmbr() * obj.radius * 2;
				obj.x = obj.radius + (800 - obj.radius*4) * getRandomNmbr();
			}

			if obj.x < 0 {
				obj.x = 0;
			}
			if obj.x > 800 - obj.radius*2 {
				obj.x = 800 - obj.radius*2;
			}
		}

		// Projectiles spawned by the player.
		if obj.type == "projectile" {
			obj.y = obj.y + (g_dt * obj.speedY);
			obj.x = obj.x + (g_dt * obj.speedX);

			for e = 0; e < array_size(g_allGameObjects); e = e + 1 {
				enemy = g_allGameObjects[e];
				if enemy.type == "enemy" {
					ex = enemy.x;
					ey = enemy.y;

					dx = ex - obj.x;
					dy = ey - obj.y;

					d2 = dx*dx + dy*dy;
					r2 = (obj.radius + enemy.radius) * (obj.radius + enemy.radius);

					if d2 < r2 {
						// Kill the enemy.
						array_push(id2del, obj.id);
						array_push(g_allGameObjects, makeExplosion(ex - 75, ey - 75));

						enemy.x = enemy.radius*2 + (800 - enemy.radius*2) * getRandomNmbr();
						enemy.y = -enemy.radius*2;

						// Chance to spawn a power up.
						if getRandomNmbr() >= 0.9 {
							array_push(g_allGameObjects, makePowerUp(ex, ey));
						}
					}
				}
			}

			if obj.y < -obj.radius {
				array_push(id2del, obj.id);
			}
		}

		// Power ups
		if obj.type == "powerUp" {
			obj.y = obj.y + (g_dt * obj.speedY);


			dx = obj.x - g_player.x;
			dy = obj.y - g_player.y;

			d2 = dx*dx + dy*dy;
			r2 = (obj.radius + g_player.radius) * (obj.radius + g_player.radius);

			if d2 < r2 {
				array_push(id2del, obj.id);
				g_player.gunLevel = g_player.gunLevel + 1;
			}
			
		}

		// Explosions.
		if obj.type == "explosion" {
			if obj.progress > 0.125 {
				array_push(id2del, obj.id);
			}

			obj.progress = obj.progress + g_dt;
		}

	}

	// Delete all game objects that aren't going to play anymore.
	for t = 0; t < array_size(id2del); t = t + 1 {
		for i = 0; i < array_size(g_allGameObjects); i = i + 1 {
			if g_allGameObjects[i].id == id2del[t] {
				array_pop(g_allGameObjects, i);
				i = i - 1;
			}
		} 
	}
};
