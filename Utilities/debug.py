def startDebug():
    import pdb;
    print "\n\nIn DEBUG mode! Type name of variable to print it, 's' steps into functions, 'n' steps over functions, 'r' continue until this function returns, 'c' continue until new breakpoint, 'b' [[filename:]lineno] set break points, 'q' quit the program...\n" 
    pdb.set_trace()