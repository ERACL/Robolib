#include "config.c"
#include "position.c"
#include "movement.c"

task main()
{
  initConfig(TULLIUS);
  initPosition(false);

  MovementResult const* result = NULL;
  getMovementResult(&result);

  PosData const* pos = NULL;

	moveTo_forceStraight(-400, -400);
	moveTo_forceStraight(-800, -400);
	moveTo_forceStraight(-500, 470, 90);

  while (result->state != ONGOING) {
  	wait1Msec(10);
  	getMovementResult(&result);
  	getPosition(&pos);
  	displayBigTextLine(1,"%f",pos->x);
  	displayBigTextLine(3,"%f",pos->y);
  	displayBigTextLine(5,"%f",pos->orientation);
  }
  while(true) {wait1Msec(100);}

}
