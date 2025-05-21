# Write Protect Fault Handler

Handles write faults for specific registered ranges by redirecting them to a secondary shared mapping of the same underlying memory pages -- backed by a memfd.


To run the PoC

```sh
make run
```

## Benchmarks

```sh
$ ./write-protect -i 1000000
Write protection disabled
Iterations: 1000000
Average write time: 23.17 ns
Standard deviation: 82.21 ns

-------

$ ./write-protect -i 1000000 -w
Write protection enabled
Iterations: 1000000
Average write time: 341.41 ns
Standard deviation: 227.95 ns
```