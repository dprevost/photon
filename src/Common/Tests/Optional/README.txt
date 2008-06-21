The two tests ProcessLockShouldFail and ThreadLockShouldFail might not
always fail as they are supposed to in the time allocated to them. That's
why they are not included in the regression suite.

Basically what these two tests do is to violate the "contract" of using
a lock by periodically writing to the locked object when the thread/process
does not own the lock.

The idea here was to give us a tool to evaluate the robustness of the 
locking code by inject a defect once every X loops and measuring the time.

If it takes less than 30 seconds (the default of the more standard 
LockConcurrency tests) to detect a rate of failure of 1/X than we can
be certain that the rate of failure of our locking code is less than
1/X. 

The main problem we're facing is that we cannot determine how long does
the code need to run, on average, before detecting one of the injected
defects. The architecture of the computer has a tremendous impact on
the amount of time:

 - On a dual core machine it took about 5 seconds (different runs, from 4.2 
   to 6.0 s) with a defect rate of 1/1,000,000 (ProcessLockShouldFail).
 - On a uniprocessor (Pentium M, 1.5 GHz), the same test aborted after 300
   seconds for a much bigger rate of failure (1/10,000). 

Conclusions:
 - Do not include these tests in the regression suite.
 - Run these tests manually when a change to the locking code is done (or
   when porting to a new platform) - always use a multi-core/multi-processor
   machine if you can.
 - If developing in a uniprocessor environment only, bugs in the lock code
   might go undetected.
