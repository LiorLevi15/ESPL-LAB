#! /usr/bin/awk -f

BEGIN { FS = "," }
{
    if(split($2, sep, " ") < 3) {  
        count++
    }
}
END {
    print "number of poems: " count
}
