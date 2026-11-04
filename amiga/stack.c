/* XLode keeps its arrays in globals and calls no deep recursion, but a
 * Shell's 4 KB is too little for SDL and the C library's stdio; libnix's
 * startup sets this much up instead.
 * The AmiCradle Team. GPL 2, like XLode. */
unsigned long __stack = 128 * 1024;
