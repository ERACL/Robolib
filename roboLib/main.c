#include "config.h"
#include "position.h"
#include "movement.h"

task main()
{
  initConfig(ERACL1_SMALL);
  initPosition(true);
  initMovement();

  struct PosData const* pos = NULL;
  getPosition(&pos);

 	nxtDisplayTextLine(0, "%f\t%f\t%f", pos->x, pos->y, pos->orientation);
  straight(100);
 	nxtDisplayTextLine(1, "%f\t%f\t%f", pos->x, pos->y, pos->orientation);
}
