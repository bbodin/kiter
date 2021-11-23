# Some notes about TDMA

## How to generate S4NoC TDMA slot tables



Clone the repository https://github.com/t-crest/s4noc, and compile the noc-sched tool.

```
git clone  https://github.com/t-crest/s4noc
cd s4noc/noc-sched
cmake .
make
```

### Exact solutions

The tool can produce LP files, you can solve them with your favorite LP solver.
For example:

```
/src/noc-sched --mesh 2 --lp 2x2.lp
gurobi_cl ResultFile=2x2.sol 2x2.lp
```

The linear problems generated grows fast:
```
-rw-rw-r--. 1 toky toky    47863 Aug 15 14:51  2x2.lp
-rw-rw-r--. 1 toky toky   578435 Aug 15 14:52  3x3.lp
-rw-rw-r--. 1 toky toky  3630015 Aug 15 14:52  4x4.lp
-rw-rw-r--. 1 toky toky 14188244 Aug 15 14:52  5x5.lp
```

The solving time grows fast too, 5x5 was out of reach on my machine.

### Approximative solutions

A different technique is available, this is the pattern based heuristic. It generate pattern schedulethe TDMA:
```
./src/noc-sched --mesh 2 --hshd /dev/stdout
network: 2 side length, 4 nodes, 8 links, 2 capacity bound, 2 bisection bound
time: 7e-06s patterns, 7.7e-05s schedule, 8.4e-05s total
patterns: 12 number, 1 average length, 8 scheduled
schedule: 10


ne
 nw
  es
    sw
      n
       e
        s
         w

```

I read this output as, each communication pattern is available to any cores to communicate to another core reachable using this communication pattern.
To be confirmed.

Both output (exact .sol file, or the patterns from the heuristic) need some post processing.


## Package listing


