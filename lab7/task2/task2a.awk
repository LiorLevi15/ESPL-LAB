#! /usr/bin/awk -f

BEGIN { FS = "," }
{
    if(NR> 1 && $4=="standard") {
        count["math"]+=$6
        count["reading"]+=$7
        count["writing"]+=$8
        count[1]++
    }
}
END {
    print "Math average score: "count["math"]/count[1]
    print "Reading average score: "count["reading"]/count[1]
    print "Writing average score: "count["writing"]/count[1]
}