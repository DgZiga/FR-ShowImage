.gba
.thumb
.open "./BPRE0.gba","./multi.gba", 0x08000000

.org freespace


.importobj "./build/linked.o"

.close
