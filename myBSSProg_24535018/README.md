# BSS Protocol Implementation (C++)

This is a solution to a tutorial program for the Distributed Systems course
(2024-25) conducted in IIT ROORKEE. The problem asks for the implementaion of
Birman-Schiper-Stephenson (BSS) protocol with a specific input format of
processs/sites. The input format is mentioned in the pdf ``Tut 8.pdf``. Basically
the input is a list of processes/sites along with the recv_B events. A recv_B
is the actual BSS layer which is handling the messages before delivering it to
the application. The output format shows each event along with the vector
events. Additionally recv_A events are included in the output which are the
events when the messages are actually delivered to the applications.

# Compile and Run

To compile ``make``. The program can be checked with the sample inputs provdide.
To run do ``./bss input.txt``. Running without any filename expects the input in
STDIN.

# TODO

Error handling.

**NOTE**: The program has not been tested yet on input other than the included
ones.

**NOTE**: The program assumes that the input format is correct. Slight error in
the input format may result in unexpected behaviour. Error handling is due.

