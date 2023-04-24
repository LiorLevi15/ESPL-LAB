#! /usr/bin/awk -f

BEGIN { FS = "," 
        OFS = "---"}
{
    if ($0 ~ /spring/) {
        print $2, $3, $4
    }
}
