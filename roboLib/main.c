#include "config.h"
#include "position.h"
#include "movement.h"

task main()
{
  initConfig(JIMMY);
  initPosition(true);

  MovementResult const* result = NULL;
  getMovementResult(&result);

  PosData const* pos = NULL;

  PosData target;
  target.x = 200;
  target.y = -200;
  target.orientation = 0;
  moveTo_forceStraight(&target);
  getPosition(&pos);
  displayBigTextLine(1,"%f",pos->x);
  displayBigTextLine(3,"%f",pos->y);
  displayBigTextLine(5,"%f",pos->orientation);
  target.x = 200;
  target.y = 200;
  target.orientation = 0;
  moveTo_forceStraight(&target);
  getPosition(&pos);
  displayBigTextLine(1,"%f",pos->x);
  displayBigTextLine(3,"%f",pos->y);
  displayBigTextLine(5,"%f",pos->orientation);
  target.x = -200;
  target.y = 200;
  target.orientation = 180;
  moveTo_forceStraight(&target);
  getPosition(&pos);
  displayBigTextLine(1,"%f",pos->x);
  displayBigTextLine(3,"%f",pos->y);
  displayBigTextLine(5,"%f",pos->orientation);
  target.x = -200;
  target.y = -200;
  target.orientation = 180;
  moveTo_forceStraight(&target);
  getPosition(&pos);
  displayBigTextLine(1,"%f",pos->x);
  displayBigTextLine(3,"%f",pos->y);
  displayBigTextLine(5,"%f",pos->orientation);

  while (result->state != DONE) {
  	wait1Msec(10);
  	getMovementResult(&result);
  	getPosition(&pos);
  	displayBigTextLine(1,"%f",pos->x);
  	displayBigTextLine(3,"%f",pos->y);
  	displayBigTextLine(5,"%f",pos->orientation);
  }
  while(true) {wait1Msec(100);}

}
