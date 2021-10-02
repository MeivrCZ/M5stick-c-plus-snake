# M5stick C plus snake V1.0

This is snake on M5stick c plus

# controlls

BtnA to turn left (front button)
BtnB to turn right (right side button)

BtnA in game over to restart game

# how to play

eat food (green block) to gain score
each time you eat, you will grow one block
don't crash in to your body, you will die

# tips

blue blocks will show sides you can turn

use your ability to go trough the walls to get on the other side of the map

# bugs

You may turn 2 times in row just because second another cicle cought your request to turn, to purge this bug delay with 100 milliseconds was added before request gathering. This delay may cause nonturning when button is pressed because button read wasn't called at time of press but when the delay was active.

Score at game over screen is not in center