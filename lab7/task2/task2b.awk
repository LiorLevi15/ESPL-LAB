#! /usr/bin/awk -f

BEGIN { FS = "," }
{
    if(NR> 1 && $3=="bachelor's degree") {
        count++
    }
}
END {
    print "number of students whose parents have bachelor's degree: "count
}