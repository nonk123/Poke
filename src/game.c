#include "game.h"

const int TICKRATE = 60;
const Fixed TIMESTEP = Fdiv(Fx1, FxFrom(TICKRATE));
