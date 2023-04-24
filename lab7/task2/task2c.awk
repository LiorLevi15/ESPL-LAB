#! /usr/bin/awk -f

BEGIN { FS = ","
        print "========"
        print "\"Success Student List\""
        print "========"
    }
{
    if(NR > 1 && $6 >= 80 && $7 >= 80 && $8 >= 80) {
        count++
        print $0
    }
}
END {
    print "The number of students : "count
}