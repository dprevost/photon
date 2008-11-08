List of examples in this directory:

1. HashMap.    Generic hash map example

2. HashMapLoop Shows how to loop on a hash map.

3. Queue       Three different programs to show a possible way to use queues.
  - QueueIn    Reads a file and input data on an input queue
  - QueueWork  Reads the input queue, do "work" and put results in output queue
  - QueueOut   Reads output queue and dump data (to /dev/null)


How to run these programs:

Lauch the server (quasar) first. 

./HashMapLoop ../iso_3166.tab 10701
./HashMap     ../iso_3166.tab 10701

where
 iso_3166.tab  is the name of the file of the iso 3166 codes (use as an 
               example). It is located in the Examples directory.
 10701         is the ip port of the server

./RunQueue.sh ../iso_3166.tab 10701 1000000 10 5000 
or
cscript RunQueue.vbs ../iso_3166.tab 10701 1000000 10 5000 
(for Windows)

 1000000       is the number of data records to process
 10 and 5000   controls the speed of data insertions:
    10            number of millisecs to sleep
    5000          a sleep period after each cycle of X records inserted.
 
The issue here is that without this, the insertions (done by QueueIn) will 
overflow the shared memory since QueueIn process records faster than the
other two. So, we need a method to slow the rate of insertions (the method I 
choose was the fastest to implement but is far from the best).

By playing with these 2 factors, I was able to limit the insertion rate to 
about 198,000 records per second (using 10ms and a 5000 insertions cycle)
[100,000,000 iterations: 8m25s to run].
You will have to ajust the sleep period and the cycle based on your computer
performance (this was done on an Intel(R) Core duo E6600).

