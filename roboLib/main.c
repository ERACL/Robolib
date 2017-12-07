#include "config.h"
#include "position.h"
#include "movement.h"

task main()
{
  initConfig(TESTROBOT);
  initPosition(true);
  initMovement();

  struct PosData const* pos = NULL;
  getPosition(&pos);
  nxtDisplayTextLine(1, "%f", pos->x);
 	nxtDisplayTextLine(2, "%f", pos->y);
 	nxtDisplayTextLine(3, "%f", pos->orientation);

  straight(100);
  getPosition(&pos);
 	nxtDisplayTextLine(1, "%f", pos->x);
 	nxtDisplayTextLine(2, "%f", pos->y);
 	nxtDisplayTextLine(3, "%f", pos->orientation);

 	while(true) {}
}
