#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);

  const int number_of_aliens = 10;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto alienPos = Point2((canvas_w/number_of_aliens) * i, 200.0f);
    alien->SetPosition(alienPos);
    aliens.push_back(alien);
  }

  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto coinPos  = Point2((canvas_w/4), 100);
  coin->SetPosition(coinPos);
  coins.push_back(coin);

  auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
  auto wallPos  = Point2(canvas_w/2, canvas_h/2);
  wall->SetPosition(wallPos);
  walls.push_back(wall);
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
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
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
      }
    }
  }

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
