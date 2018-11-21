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
g_timeSpentDead = 0;

//-----------------------------------------------------
//
//-----------------------------------------------------
g_screenWidth = 800;
g_screenHeight = 800;

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
		gunLevel = 0;

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
		health = 1;
		hitCooldown = 0;
		speed = getRandomNmbr() * 200 + 150;
		phase = getRandomNmbr() * 100;
		phaseSpeed = getRandomNmbr()*3.0  + 1.0;
		phaseMag = getRandomNmbr() * 1.2;
	};
	g_nextId = g_nextId + 1;
	return r;
};

makeEnemyBig = fn(x, y) {
	r = {
		id = g_nextId;
		type = "enemyBig";
		x = x;
		y = y;
		radius = 35;
		health = 3;
		hitCooldown = 0;
		speed = getRandomNmbr() * 100 + 50;
		phase = 50 + getRandomNmbr() * 100;
		phaseSpeed = getRandomNmbr()*3.0  + 1.0;
		phaseMag = getRandomNmbr() * 2.6;
		shootTimer = 0;
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
		radius = 32;
		speedY = -900;
		speedX = 0;
	};
	g_nextId = g_nextId + 1;
	return r;
};

makeEnemyProjectile = fn(x, y, speedX) {
	r = {
		id = g_nextId;
		type = "enemyProjectile";
		x = x;
		y = y;
		radius = 16;
		speedY = 400;
		speedX = speedX;
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
		radius = 75;
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
	g_timeSpentDead = 0;
	g_allGameObjects = array{};
	g_nextId = 0;

	array_push(g_allGameObjects, makePlayer(g_screenWidth * 0.5, g_screenHeight * 0.8));

	array_push(g_allGameObjects, makeEnemy(100, -64));
	array_push(g_allGameObjects, makeEnemyBig(200, -32));
	array_push(g_allGameObjects, makeEnemy(300, -64));
	array_push(g_allGameObjects, makeEnemyBig(400, -32));
	array_push(g_allGameObjects, makeEnemy(500, -64));
	array_push(g_allGameObjects, makeEnemyBig(600, -32));
	array_push(g_allGameObjects, makeEnemy(700, -64));
	
	array_push(g_allGameObjects, makeEnemyBig(100, -64));
	array_push(g_allGameObjects, makeEnemy(200, -32));
	array_push(g_allGameObjects, makeEnemyBig(300, -64));
	array_push(g_allGameObjects, makeEnemy(400, -32));
	array_push(g_allGameObjects, makeEnemyBig(500, -64));
	array_push(g_allGameObjects, makeEnemy(600, -32));
	array_push(g_allGameObjects, makeEnemyBig(700, -64));
	
	array_push(g_allGameObjects, makeEnemy(200, -132));
	array_push(g_allGameObjects, makeEnemyBig(400, -132));
	array_push(g_allGameObjects, makeEnemy(600, -132));
};

//------------------------------------------------------
//
//------------------------------------------------------
doCollide = fn(o1, o2) {
	dx = o1.x - o2.x;
	dy = o1.y - o2.y;

	d2 = (dx*dx) + (dy*dy);
	r2 = (o1.radius + o2.radius) * (o1.radius + o2.radius);

	return d2 <= r2;
};

clampPositionToScreenEdge = fn(obj, xOnly) {
	if obj.x < obj.radius {
		obj.x = obj.radius;
	}
	
	if obj.x > g_screenWidth - obj.radius {
		obj.x = g_screenWidth - obj.radius;
	}

	if xOnly != 1 {
		if obj.y < obj.radius {
			obj.y = obj.radius;
		}
		if obj.y > g_screenHeight - obj.radius {
			obj.y = g_screenHeight - obj.radius;
		}
	}
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
				obj.x = getMouseX();
				obj.y = getMouseY();
			} else {
				obj.x = obj.x + g_dt * getXMoveInput() * 400.0;
				obj.y = obj.y + g_dt * getYMoveInput() * 400.0;
			}

			// Clamp the position to the edges of the screen.
			clampPositionToScreenEdge(obj, 0);

			if isFireBtnPressed() {
				if obj.gunLevel == 0 {
					jitter = (getRandomNmbr() * 2 - 1) * 12;
					prjectile = makeProjectle(obj.x + jitter, obj.y - 32);
					prjectile.speedX = jitter * 6.3;
					array_push(g_allGameObjects, prjectile);
					obj.recoil = 1.0;
				} else {
					jitter = (getRandomNmbr() * 2 - 1) * 12;
					prjectile = makeProjectle(obj.x - 32 + jitter, obj.y - 32);
					prjectile.speedX = -132;
					array_push(g_allGameObjects, prjectile);

					if obj.gunLevel > 1 {
						prjectile = makeProjectle(obj.x + jitter, obj.y - 32);
						prjectile.speedX = 0;
						array_push(g_allGameObjects, prjectile);
					}

					prjectile = makeProjectle(obj.x + 32 + jitter, obj.y - 32);
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

			// Check if the player ship is coliding with any enemies.
			// If so apply damage to it.
			for e = 0; e < array_size(g_allGameObjects); e = e + 1 {
				enemy = g_allGameObjects[e];
				if (enemy.type == "enemy") + (enemy.type == "enemyBig") + (enemy.type == "enemyProjectile") {
					if doCollide(obj, enemy) {
						// Kill the player.
						array_push(id2del, obj.id);
						array_push(g_allGameObjects, makeExplosion(obj.x, obj.y));
						g_isGameOver = 1;
					}
				}
			}
		}

		// Enemy.
		if (obj.type == "enemy") + (obj.type == "enemyBig") {
			obj.phase = obj.phase + g_dt;
			obj.y = obj.y + g_dt * obj.speed;
			xMovement = sin(obj.phase * obj.phaseSpeed) * obj.phaseMag;
			obj.x = obj.x + xMovement;

			obj.hitCooldown = obj.hitCooldown - g_dt;

			if obj.type == "enemyBig" {
				obj.shootTimer = obj.shootTimer + g_dt;
				if obj.shootTimer > 1 {
					obj.shootTimer = 0;

					shootingChance = 0.1;
					if g_player.gunLevel >= 1 { shootingChance = 0.15; }
					if g_player.gunLevel >= 2 { shootingChance = 0.20; }

					if getRandomNmbr() <= shootingChance {
						array_push(g_allGameObjects, makeEnemyProjectile(obj.x, obj.y, xMovement * 150));
					}
				}
			}

			if obj.y > g_screenHeight + obj.radius {
				obj.y = -obj.radius*2 - getRandomNmbr() * obj.radius * 2;
				obj.x = obj.radius + (g_screenWidth - obj.radius*4) * getRandomNmbr();
			}

			clampPositionToScreenEdge(obj, 1);
		}

		// Projectiles spawned by the player.
		if obj.type == "projectile" {
			obj.y = obj.y + (g_dt * obj.speedY);
			obj.x = obj.x + (g_dt * obj.speedX);

			for e = 0; e < array_size(g_allGameObjects); e = e + 1 {
				enemy = g_allGameObjects[e];
				if (enemy.type == "enemy") + (enemy.type == "enemyBig") {
					if doCollide(obj, enemy) * (enemy.hitCooldown <= 0) {

						array_push(id2del, obj.id);

						// Drops it's health.
						enemy.hitCooldown = 0.150;
						enemy.health = enemy.health - 1;

						if enemy.health <= 0 {
							// Kill the enemy.	
							array_push(g_allGameObjects, makeExplosion(enemy.x, enemy.y));
							
							enemy.x = enemy.radius*2 + (g_screenWidth - enemy.radius*2) * getRandomNmbr();
							enemy.y = -enemy.radius*2;

							// Chance to spawn a power up.
							if getRandomNmbr() >= 0.975 {
								array_push(g_allGameObjects, makePowerUp(enemy.x, enemy.y));
							}
						}
					}
				}
			}

			if obj.y < -obj.radius {
				array_push(id2del, obj.id);
			}
		}

		if obj.type == "enemyProjectile" {
			obj.x = obj.x + (g_dt * obj.speedX);
			obj.y = obj.y + (g_dt * obj.speedY);

			if obj.y > g_screenHeight + obj.radius {
				array_push(id2del, obj.id);
				
			}
		}


		// Power ups
		if obj.type == "powerUp" {
			obj.y = obj.y + (g_dt * obj.speedY);

			if doCollide(obj, g_player) {
				array_push(id2del, obj.id);
				g_player.gunLevel = g_player.gunLevel + 1;
			}
			
		}

		// Explosions.
		if obj.type == "explosion" {
			if obj.progress > 0.250 {
				array_push(id2del, obj.id);
			}

			obj.progress = obj.progress + g_dt;
		}
	}

	if g_isGameOver {
		g_timeSpentDead = g_timeSpentDead + g_dt;
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
