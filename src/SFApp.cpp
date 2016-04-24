#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : headingDown(false), headingUp(false), headingLeft(false), headingRight(false), fire(0), coinCount(0), totalLevels(3), is_running(true), levelComplete(false), sf_window(window) {
	int canvas_w, canvas_h;
	SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

	app_box = make_shared < SFBoundingBox	> (Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
	player = make_shared < SFAsset > (SFASSET_PLAYER, sf_window);


}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
	SFEVENT the_event = event.GetCode();

	switch (the_event) {
	case SFEVENT_QUIT:
		is_running = false;
		break;

	case SFEVENT_UPDATE:
		OnUpdateWorld();
		OnRender();
		break;

	case SFEVENT_PLAYER_LEFT:
		headingLeft = true;
    break;

	case SFEVENT_PLAYER_RIGHT:
		headingRight = true;
		break;

	case SFEVENT_PLAYER_UP:
		headingUp = true;
		break;

	case SFEVENT_PLAYER_DOWN:
		headingDown = true;
		break;

	case SFEVENT_PLAYER_LEFT_RELEASE:
		headingLeft = false;
    break;

	case SFEVENT_PLAYER_RIGHT_RELEASE:
		headingRight = false;
		break;

	case SFEVENT_PLAYER_UP_RELEASE:
		headingUp = false;
		break;

	case SFEVENT_PLAYER_DOWN_RELEASE:
		headingDown = false;
		break;

	case SFEVENT_FIRE:
		fire++;
		FireProjectile();
		break;
	}
}

int SFApp::OnExecute() {
	// Execute the app
	SDL_Event event;
	levelComplete = false;

	while (coinCount < totalLevels && is_running) {
		InitLevel (coinCount);
		while (SDL_WaitEvent(&event) && is_running && levelComplete == false) {
			// wrap an SDL_Event with our SFEvent
			SFEvent sfevent((const SDL_Event) event);
			// handle our SFEvent
			OnEvent(sfevent);
		}

		if (levelComplete == true) {
			levelComplete = false;
			DestroyLevel();
		}
	}
	int score = player->getScore();
	if (player->IsAlive() && coinCount == totalLevels) {
		cout << "You Win ------ score = " << score << endl;
	} else if (!player->IsAlive()) {
		cout << "Game Over ------ score = " << score << endl;
	}
}

void SFApp::InitLevel(int currentLevel) { // crestes all level assets and all level layout is stored in this function
  // sets out height and width
  int screenWidth =   640;
  int screenHeight =  480;

  int spriteSize =     32;
  
	switch (currentLevel) {

	case 0: {
	  auto player_pos = Point2(spriteSize, spriteSize);
	  player->SetPosition(player_pos);

		int number_of_aliens = 10;
		for (int i = 0; i < number_of_aliens; i++) {

			// place an alien evenly across the screen
			MakeAlien(((screenWidth / number_of_aliens) * i) + (spriteSize/2), screenHeight/2);
		}

    MakeCoin(screenWidth-spriteSize, screenHeight-spriteSize);

    break;
	}

	case 1: {
	  auto player_pos = Point2(screenWidth-spriteSize, spriteSize);
	  player->SetPosition(player_pos);

		MakeWall(2*spriteSize, screenHeight - (3*spriteSize));
		MakeWall(2*spriteSize, screenHeight - (4*spriteSize));
		MakeWall(3*spriteSize, screenHeight - (3*spriteSize));
		MakeWall(3*spriteSize, screenHeight - (4*spriteSize));

    MakeWall(screenWidth - (4*spriteSize), 5*spriteSize);

    MakeWall(screenWidth - (5*spriteSize), 9*spriteSize);
    MakeWall(screenWidth - (6*spriteSize), 9*spriteSize);
    MakeWall(screenWidth - (5*spriteSize), 10*spriteSize);
    MakeWall(screenWidth - (6*spriteSize), 10*spriteSize);

		MakeAlien(screenWidth/2-spriteSize, screenHeight-spriteSize);
    MakeAlien(screenWidth/2+spriteSize, screenHeight-spriteSize);
		MakeAlien(spriteSize, screenHeight - (3*spriteSize));
		MakeAlien(spriteSize, screenHeight - (4*spriteSize));
    MakeAlien(screenWidth - (5*spriteSize), 5*spriteSize);

    MakeCoin(spriteSize, screenHeight - spriteSize);
		break;
	}

  case 2: {
	  auto player_pos = Point2(screenWidth/2, screenHeight/2);
	  player->SetPosition(player_pos);
    for(int i = 4*spriteSize; i < screenWidth -(3*spriteSize); i+=spriteSize){
      MakeWall(i, screenHeight - (3*spriteSize));
    }
    for(int i = screenHeight - (3*spriteSize); i > 0 - spriteSize; i-=spriteSize){
      MakeWall(screenWidth -(3*spriteSize), i);
    }

		MakeAlien(spriteSize, screenHeight - (2*spriteSize));
		MakeAlien(spriteSize, screenHeight - (4*spriteSize));
		MakeAlien(2*spriteSize, screenHeight - (2*spriteSize));
		MakeAlien(2*spriteSize, screenHeight - (4*spriteSize));

    MakeCoin(screenWidth-spriteSize, spriteSize);
  }
	}
}

void SFApp::MakeCoin(int x, int y){
		auto coin = make_shared < SFAsset > (SFASSET_COIN, sf_window);
		auto coinPos = Point2(x, y);
		coin->SetPosition(coinPos);
		coins.push_back(coin);
}

void SFApp::MakeWall(int x, int y){
		auto wall = make_shared < SFAsset > (SFASSET_WALL, sf_window);
		auto wallPos = Point2(x, y);
		wall->SetPosition(wallPos);
		walls.push_back(wall);
}

void SFApp::MakeAlien(int x, int y){
		auto alien = make_shared < SFAsset > (SFASSET_ALIEN, sf_window);
		auto alienPos = Point2(x, y);
		alien->SetPosition(alienPos);
		aliens.push_back(alien);
}

void SFApp::DestroyLevel() {
	for (auto w : walls) {
		w->SetNotAlive();
	}
	DestroyWalls();

	for (auto a : aliens) {
		a->SetNotAlive();
	}
	DestroyAliens();

	for (auto c : coins) {
		c->SetNotAlive();
	}
	DestroyCoins();
}

void SFApp::OnUpdateWorld() {
  MovePlayer();

	// Update projectile positions
	for (auto p : projectiles) {
		p->GoNorth();
	}

	for (auto c : coins) {
		if (c->CollidesWith(player)) {
			coinCount++;
      player->addScore();
			levelComplete = true;
		}
	}

	for (auto a : aliens) {
		if (a->CollidesWith(player)) {
      player->SetNotAlive();
			is_running = false;
		}
	}

	// Update enemy positions
	for (auto a : aliens) {
		// do something here
	}

	// Detect collisions with projectiles and aliens
	for (auto p : projectiles) {
		for (auto a : aliens) {
			if (p->CollidesWith(a)) {
				p->HandleCollision(a);
				player->addScore();
			}
		}
    //Detect collisions with projectiles and walls
    for (auto w : walls) {
      if (p->CollidesWith(w)) {
				p->HandleCollision(w);
      }
    }
	}
	DestroyAliens();
}

void SFApp::DestroyAliens() {
	// remove dead aliens (the long way)
	list<shared_ptr<SFAsset>> tmp;
	for (auto a : aliens) {
		if (a->IsAlive()) {
			tmp.push_back(a);
		}
	}
	aliens.clear();
	aliens = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::DestroyCoins() {
	// remove dead aliens (the long way)
	list<shared_ptr<SFAsset>> tmp;
	for (auto c : coins) {
		if (c->IsAlive()) {
			tmp.push_back(c);
		}
	}
	coins.clear();
	coins = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::DestroyWalls() {
	// remove dead aliens (the long way)
	list<shared_ptr<SFAsset>> tmp;
	for (auto w : walls) {
		if (w->IsAlive()) {
			tmp.push_back(w);
		}
	}
	walls.clear();
	walls = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::OnRender() {
	SDL_RenderClear(sf_window->getRenderer());

	// draw the player
	player->OnRender();

	for (auto p : projectiles) {
		if (p->IsAlive()) {
			p->OnRender();
		}
	}

	for (auto a : aliens) {
		if (a->IsAlive()) {
			a->OnRender();
		}
	}

	for (auto c : coins) {
		c->OnRender();
	}

	for (auto w : walls) {
		w->OnRender();
	}

	// Switch the off-screen buffer to be on-screen
	SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::MovePlayer() {// called every update to move the player in the direction of the keys being pressed down
  bool collision = false;

  if(headingLeft){
    player->GoWest();
    for (auto w : walls) {
		  if ((w->CollidesWith(player))) {
			  collision = true;
			}
		}
		if (collision == true) {
			player->GoEast();
			collision = false;
		}
  }


  if(headingRight){
    player->GoEast();
    for (auto w : walls) {
		  if ((w->CollidesWith(player))) {
			  collision = true;
			}
		}
		if (collision == true) {
			player->GoWest();
			collision = false;
		}
  }


  if(headingUp){
    player->GoNorth();
    for (auto w : walls) {
		  if ((w->CollidesWith(player))) {
			  collision = true;
			}
		}
		if (collision == true) {
			player->GoSouth();
			collision = false;
		}
  }


  if(headingDown){
    player->GoSouth();
    for (auto w : walls) {
		  if ((w->CollidesWith(player))) {
			  collision = true;
			}
		}
		if (collision == true) {
			player->GoNorth();
			collision = false;
		}
  }
}

void SFApp::FireProjectile() {
	auto pb = make_shared < SFAsset > (SFASSET_PROJECTILE, sf_window);
	auto v = player->GetPosition();
	pb->SetPosition(v);
	projectiles.push_back(pb);
}
