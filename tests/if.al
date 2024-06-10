var a = 1;
var b = 0;

if ${a} {
    print("1");
}

if ${b} {
    print("2");
    
    if ${a} {
        print("3");
    }
} else {
    print("4");
}
