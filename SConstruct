# Build file for gapfilling code
#
# To build , simply type 'scons' on the commandline. This will produce
# an executable named 'gapfill'
#

# setup environments and add cuda support
dbg = Environment(CFLAGS = '-g -O0 -Wall -std=c99 -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wmissing-prototypes -Wnested-externs')
#dbg.Tool('cuda')
opt = Environment(CFLAGS = '-O3 -Wall -std=c99 -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wmissing-prototypes -Wnested-externs')
#opt.Tool('cuda')

# iterate through debug and optimized builds - do not duplicate source code in build dirs
for dir, env in (('opt', opt), ('dbg', dbg)):
  env.SConscript('src/SConscript', 'env', variant_dir=dir, duplicate=0)

