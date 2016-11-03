#!/usr/bin/env python2

import sys
import math


if __name__ == "__main__":

    if len(sys.argv) < 3:
        print "usage: %s proc_count star_count" % sys.argv[0]
        sys.exit(1)
        
    proc_count = int(sys.argv[1])
    star_count = int(sys.argv[2])

    if (proc_count > 2):
        digit_count = int(math.floor(math.log(proc_count-1, 10)) + 1) 
    else:
        digit_count = 0
    
    if proc_count > star_count:
        print "less stars than processor is not valid."
        sys.exit(1)
    
    star_per_file = star_count / proc_count
    extra_files = star_count % proc_count
        
    output_filename = "output.png"

    print "#!/usr/bin/gnuplot"
    print "set term png enhanced"
    print "set nokey"# right bottom"
    print "set size ratio -1"
    print "set xlabel \"x\""
    print "set ylabel \"y\""
    print "set output \"%s\"" % (output_filename)

    print "plot ",
    l = []
    for i in xrange(0, proc_count):
        s = star_per_file+1 if i < extra_files else star_per_file
        for k in range(0, s):
            if i < extra_files:
                star_id = i*s+k
            else:
                star_id = extra_files*(star_per_file+1)+(i-extra_files)*s+k
            l.append("\t\"output.1.%0*d\" using %d:%d with lines "#title \"star %d\""
                    % (digit_count, i, 2*k+1, 2*k+2))#, star_id))
    print ", \\\n".join(l)
