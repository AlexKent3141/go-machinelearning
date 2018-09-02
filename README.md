# go-machinelearning
This repository contains my code for applying machine learning to the game of Go.

Deepmind's AlphaGo proved that convolutional neural networks can be used very effectively in the game of Go. AlphaGo used two
neural networks:
1) A Selection network which proposed moves which were likely to be good
2) A Value network which estimated the winning probability for the current player

I am planning to train similar networks using supervised learning (based on games by strong human players).

# Territory network
One idea that I am experimenting with is generating a Territory network rather than an AlphaGo style Value network. The
Territory network will output the probabilities of each point on the board becoming the current player's territory. This
offers some benefits over just a Value network:
1) The ability to handle different values of komi (IMO the most major limitation with current neural network Go AI's)
2) The ability to calculate an estimated score

# Application in OnePunchGo
I am planning to utilise these neural networks in my OnePunchGo program.
