#! /usr/bin/awk -f

BEGIN { FS = "," }
{
    if ($0 ~ /^f[au]n | f[au]n$| f[au]n /) {
        count++
    }
}
END { print "he number of poem that have fun or fan is : "count}
