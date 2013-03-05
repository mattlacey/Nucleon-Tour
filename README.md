Nucleon-Tour
============

There's not a whole lot here yet, but it's intended to a be a C prototype for some stuff I want to do on the Pi in assembly.

I make no apologies for confusing comments, magic numbers or bad practices - this code is rough and ready and not exactly intended to be a production level effort. And yes, everything is in one file, because this is my hobby project and I'm being extremely lazy. 

Building
---

Currently broken, because I pretty much have zero idea on auto tools. I've not committed something that's needed obviously, since it won't build on my Mac but builds on my Haiku box just fine. 

In the time being, it's only one source file and one library (the autotools stuff was more of a learning thing) so you can just build like so:

`gcc main.c -o nucleon -lSDL`


Haiku
---

If `git push` doesn't work in Haiku with Github, trying setting up SSH authentication instead, worked a treat for me. 

