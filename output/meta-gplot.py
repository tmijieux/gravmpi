#!/usr/bin/env python2

import sys

if __name__ == "__main__":

    if len(sys.argv) < 3:
        print "usage: %s proc_count star_count" % sys.argv[0]
        sys.exit(1)
        
    proc_count = int(sys.argv[1])
    star_count = int(sys.argv[2])
    if proc_count > star_count:
        print "less stars than processor is not valid."
        sys.exit(1)
    
    star_per_file = star_count / proc_count
    extra_files = star_count % proc_count
        
    output_filename = "output.png"

    print "#!/usr/bin/gnuplot"
    print "set term png enhanced"
    print "set key right bottom"
    print "set size ratio -1"
    print "set xlabel \"x\""
    print "set ylabel \"y\""
    print "set output \"%s\"" % (output_filename)
    
    for i in xrange(0, proc_count):
        s = star_per_file+1 if i < extra_files else star_per_file
        l = []
        for k in range(0, s):
            id = i*s+k if i < extra_files else extra_files*(star_per_file+1)+(i-extra_files)*s+k
            l.append("\t\"output.1.%d\" using %d:%d with lines title \"star %d\"" % (i, 2*k+1, 2*k+2, id))
        print "plot "+ ", \\\n".join(l)
