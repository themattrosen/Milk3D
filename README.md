# Milk3D
## A game engine done quick (GEDQ)

As our last winter break of college came to a close, we decided to have a game _engine_ jam in which the three of us would write a game engine from scratch in 12 hours. 
 
In this case we three are DigiPen Institute of Technology seniors 
* Matthew Rosen: BSCSDA (2020), https://github.com/themattrosen
* Christopher Taylor: BSCSRTIS (2020), https://github.com/christaylor2449
* Morgen Hyde: BSCSDA (2020), https://github.com/mmhyde

## Guidelines
We were okay with copying and pasting code from previous projects in order to get things working as quickly as possible, so much of the code wasn't done spontaneously. However, we did try to make some innovations with past code in order to make the project unique!
 
Making an entire game engine is definitely more than 36 man hours of work (12 hours for 3 people not including breaks), but the goal was to do what we could. If we liked what we created, then we could continue working on it after the 12 hours were up.

## Goals vs. Reality
Initially, the goal was to create both a robust engine _and_ editor so that users can actually use the code as a product. For the engine, we wanted a full custom 3D rendering pipeline (done by Chris), a full custom audio engine (done by Morgen), an event driven GameObject based core architecture (done by Matt), and a modestly featured custom 3D physics engine (also done by Matt). 

In reality, we did get a full 3D rendering pipeline, including but not featuring PBR, a custom audio engine that can handle asynchronous starting and stopping of sounds without artifacts, and a core architecture similar to the one laid out in the beginning. The physics engine only received about 1 hour of real work, before being mostly abandoned. Only sphere v sphere collision was implemented, and impulse resolution was done without correction, error testing or friction. A single editor window existed using ImGui, but there was no "editor." Panning of sounds existed, but no 3D spatialization of audio or audio objects existed. 

## Conclusions
Overall, it was fun. After the 12 hours were up, Matt went and actually hooked up components and created an asset management system, and got graphics to render game objects properly (which took about 6 hours alone). In the future, we'll probably look at this for fun again and maybe make improvements and a full editor along with maybe bringing in a dedicated person to work on physics.