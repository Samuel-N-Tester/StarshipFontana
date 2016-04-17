#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), number_of_aliens(10), is_running(true), levelComplete(false), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);

}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  bool collision = false;

  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;

  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;

  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    for(auto w : walls) {
      if((w->CollidesWith(player))) {
      collision = true;
      }
    }
    if (collision == true) {
      player->GoEast();
      collision = false;
    }
    break;


  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    for(auto w : walls) {
      if((w->CollidesWith(player))) {
      collision = true;
      }
    }
    if (collision == true) {
      player->GoWest();
      collision = false;
    }
    break;


  case SFEVENT_PLAYER_UP:
    player->GoNorth();
    for(auto w : walls) {
      if((w->CollidesWith(player))) {
      collision = true;
      }
    }
    if (collision == true) {
      player->GoSouth();
      collision = false;
    }
    break;


  case SFEVENT_PLAYER_DOWN:
    player->GoSouth();
      for(auto w : walls) {
        if((w->CollidesWith(player))) {
        collision = true;
      }
    }
    if (collision == true) {
      player->GoNorth();
      collision = false;
    }
    break;


  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  levelComplete == false;

  for (int currentLevel = 0; currentLevel < 2; currentLevel++) {
    InitLevel(currentLevel);
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
  cout << "you win ------ score = " << score << endl;
}

void SFApp::InitLevel(int currentLevel) { // crestes all level assets
  switch(currentLevel) {

  case 0: {
    for(int i=0; i<number_of_aliens; i++) {
      // place an alien at width/number_of_aliens * i
      auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
      auto alienPos = Point2((640/number_of_aliens) * i, 200.0f);
      alien->SetPosition(alienPos);
      aliens.push_back(alien);
    }

    auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
    auto coinPos  = Point2(160, 100);
    coin->SetPosition(coinPos);
    coins.push_back(coin);

    auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
    auto wallPos  = Point2(320, 150);
    wall->SetPosition(wallPos);
    walls.push_back(wall);
    break;
  }

  case 1: {
    number_of_aliens =1;

    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto alienPos = Point2(200, 200);
    alien->SetPosition(alienPos);
    aliens.push_back(alien);
    break;
		}
  }
}

void SFApp::DestroyLevel(){
  for(auto w : walls) {
    w->SetNotAlive();
  }
  DestroyWalls();

  for(auto a : aliens) {
    a->SetNotAlive();
  }
  DestroyAliens();


  for (auto c : coins) {
    c->SetNotAlive();
  }
  DestroyCoins();
}

void SFApp::OnUpdateWorld() {

  //test win conditions
  if(number_of_aliens < 1) {
    levelComplete = true;
  }

  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  for(auto c: coins) {
    c->GoNorth();
  }

  // Update enemy positions
  for(auto a : aliens) {
    // do something here
  }

  // Detect collisions with projectiles and aliens
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
        number_of_aliens--;
        player->addScore();
      }
    }
  }
  DestroyAliens();
}

void SFApp::DestroyAliens() {
  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::DestroyCoins() {
  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto c: coins) {
    if(c->IsAlive()) {
      tmp.push_back(c);
    }
  }
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::DestroyWalls() {
  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto w : walls) {
    if(w->IsAlive()) {
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

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    c->OnRender();
  }

  for(auto w: walls) {
    w->OnRender();
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
