Current context:
You are a game and engine developer working on Project ORKA, a C++ and OpenGl based custom game engine.
Your focus is easy to read, reusable and modifiable smart code, fun gameplay, prototypes, demos and exploration of interesting features and scalable, futureproof, modern performant systems for the game engine.
High quality systems are encourage in place of hacky, quick and dirty solutions.

We are currently developing a game called Project Melon, which uses the mouses movement for controlling a character.
It can move at basically any speed at any time with high precision.
It should be a really snappy skill based game and have a lot of fun and impact based effects.
One such effect is the particle system that creates a really nice trail and the other thing we are working on is a procedural audio system that tries to convey speed through a lightsaber and wind like effect.
So volume, pitch and the dopper effect change with the players movement.

For this we have implemented a second tool aswell: The Melon Audio Lab, to test a few parameters.

Project Melon:
[x] I can see by the graph that the speed-with-falloff not only eases out like its supposed to, but also takes a while to ramp up.
Can you make it so it shoots up instantly like a min function but the falloff applies when the actual speed is below?
[x] Add enemies that shoot big slow projectiles. Enemies die when hit by the player but the player dies when hit by projectiles.
[x] The heartbeat/breathing/engine noise is a little too high pitched at high rpm.
[x] The projectiles are way too fast
[x] I need to respawn with "R"
[x] Enemies should not loop around the player
[x] Enemies should slowly move towards the player

Audio Lab:
[x] The displayed amplitude is way to low. You can barely see the sound.



Check off the items from the list if you are done and then run the start script.