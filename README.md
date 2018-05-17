## COMUS v4 ##
## This is the version of CoMuS (comuscnv) that is able to simulation the evolution of copy number variants using a pre-order traversal


## LOG ##

## 17/5/2018
## add the possiblility to simulate copy number variants.


## 8/2/2017
## Add option to
## -- track migration events and
## -- microsatelites / repeats evolution


########### COMPILE ########
##To compile comus you can type

make

## To compile CoMuStats:
cd CoMuStats2
make

## comuscnv example
./comus4 2 10 10 1 -iphylo phylo.newick -t 10 -name testrun
## this will produce two main files:
## comus_Microsat.testrun : the copy number variants for each of the 10 individuals of every of the 2 species
## comus_Results.testrun : usual output of comus (ms-like output).
## the two files are not linked/associated. A different number of mutations is produced in each one. The first describes the evolution of copy number variants and the second the evolution of another region with exactly the same tree(s) as the first (but with a different mutation process). 
