# multithread_lookup
array.c and array.h contain all information about the shared bounded buffer. multi-lookup.c and multi-lookup.h implement the multi-lookup program given by the following man page. util.c and util.h used to perform actual DNS lookup.

NAME
multi-lookup - resolve a set of hostnames to IP addresses

SYNOPSIS
multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]

DESCRIPTION
The file names specified by <data file> are passed to the pool of requester threads which place information into a shared data area. Resolver threads read the shared data area and find the corresponding IP address.

<# requesters> number of requestor threads to place into the thread pool
<# resolvers> number of resolver threads to place into the thread pool
<requester log> name of the file into which requested hostnames are written
<resolver log> name of the file into which hostnames and resolved IP addresses are written
<data file> filename to be processed. Each file contains a list of host names, one per line, that are to be resolved

SAMPLE INVOCATION
./multi-lookup 5 5 serviced.txt resolved.txt input/names1*.txt

SAMPLE CONSOLE OUTPUT
thread 0f9c0700 serviced 1 files
thread 0f1bf700 serviced 1 files
thread 109c2700 serviced 1 files
thread 101c1700 serviced 1 files
thread 0e9be700 serviced 2 files
thread 121c5700 resolved 26 hostnames
thread 131c7700 resolved 34 hostnames
thread 111c3700 resolved 23 hostnames
thread 129c6700 resolved 35 hostnames
thread 119c4700 resolved 5 hostnames
./multi-lookup: total time is 25.323361 seconds
