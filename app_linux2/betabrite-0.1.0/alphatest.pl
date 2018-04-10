#!/usr/bin/perl
# Get the attention of the sign
print "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

# Tell the sign to print the message
$message = "Ben Fuckin Rocks!";
print "\001" . "Z" . "00" . "\002" . "AA" . "\x1B" . " b" . $message . "\004";



