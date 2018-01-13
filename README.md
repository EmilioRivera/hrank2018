# Prerequisites
You will need to have NodeJS and install the dependency/ies (`npm install` in this directory will install the dependencies for you.)
# About this code 
This code attempts to be as minmal as possible: there thus no error checking. 
The code assumes the list of words being named _liste_des_mots.txt_.
It's interesting to see that if we were to save the answers in a array *separately from the seeds_decomposition object*, we would increase the memory consumption causing upwards of 10GB on 5000 lines of 80 characters. In fact, the memory consumption is *drastically lower* when putting the array in the decomposition object.
# How to run
There are various ways to start it, but the most appropriate way to use it would be:
`node --max-old-space-size=8192 gv.js`
where the extra option is to allow more heap space.