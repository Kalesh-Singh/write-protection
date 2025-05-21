# Write Protect Fault Handler

Handles write faults for specific registered ranges by redirecting them to a secondary shared mapping of the same underlying memory pages -- backed by a memfd.


To run the PoC

```sh
make run
```