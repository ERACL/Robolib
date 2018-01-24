#include "config.h"
#include "position.h"
#include "movement.h"

task main()
{
  initConfig(JIMMY);
  initPosition(true);

  MovementResult const* result = NULL;
  getMovementResult(&result);

  rotate(360);
  while (result->state != DONE) {
  	wait1Msec(10);
  	getMovementResult(&result);
  }
  PosData const* pos = NULL;
  getPosition(&pos);
  displayBigTextLine(1, "%f", pos->orientation);
  while(true) {wait1Msec(100);}

}
