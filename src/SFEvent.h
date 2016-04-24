#ifndef SFEVENT_H
#define SFEVENT_H

#include <SDL2/SDL.h>

/**
 * Most of these SFEVENTs map onto game concepts apart from _NULL and _LAST.
 * SFEVENT_NULL means to do nothing.  It is created if we get an event that we
 * do not recognise.  SFEVENT_LAST marks the maximal element in the SFEVENT
 * enumeration.  This is a common C/C++ _idiom_.
 */
enum SFEVENT {SFEVENT_NULL, SFEVENT_QUIT, SFEVENT_UPDATE, SFEVENT_PLAYER_LEFT, SFEVENT_PLAYER_RIGHT, SFEVENT_PLAYER_UP, SFEVENT_PLAYER_DOWN, SFEVENT_FIRE, SFEVENT_COLLISION, SFEVENT_LAST, SFEVENT_PLAYER_LEFT_RELEASE, SFEVENT_PLAYER_RIGHT_RELEASE, SFEVENT_PLAYER_UP_RELEASE, SFEVENT_PLAYER_DOWN_RELEASE, SFEVENT_NULL_RELEASE};

/**
 * Abstracts away from SDL_Event so that our game event management needs no SDL-specific code.
 */
class SFEvent {
public:
  SFEvent(const SDL_Event &);
  SFEVENT GetCode();
private:
  SFEVENT code;
};

#endif
