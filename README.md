# Transport-Layer service over UDP 
Implements connection handshake and termination as well as dinamic RTO from RDT 3.0.

Accepts seven command line arguments:
- Destination server (hostname or IP)
- Buffer size (power of 2)
- Sender window (in  packets)
- Round Trip Delay (in seconds)
- Probability of loss in each direction
- Speed of the bottleneck link (in Mbps)


Usage `RDT acampos0297.github.io 24 50000 0.2 0.00001 0.0001 100`
