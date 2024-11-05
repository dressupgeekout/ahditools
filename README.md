# AHDI Tools

This program lets you interact with images of hard disks partitioned under
the old Atari TOS partitioning scheme. 


## Background

On the Atari ST and similar computers, the way data is laid out on a hard
disk depends in part on the hard disk driver you use. The popular ones you
might have heard of include HDDRIVER, PPDRIVER, ICD Pro and CBHD. They all
differ from one another in terms of the functionality they provide and their
actual monetary cost.

ICD Pro is free (as in beer), which is the main reason that I use it with my
UltraSatan. But it will only format a hard disk with the old TOS
partitioning scheme, not a standard Master Boot Record (MBR) that you'd
expect nowadays. The end result is that I have a SD card with FAT16 file
systems that I *should* be able to mount on any modern PC, but I can't
access them on modern PCs because they don't know how to read the partition
map.

These programs try to fix that problem.


## License

AHDI Tools are provided to you under the terms of a BSD-style license. See
the LICENSE document for more information.
