var MAX = 9;

var i = 0;
add(i, ${MAX});
while ${i} {
    print("#", 0);

    subtract(i, 1);
}
print("");

var n_righe = -2;
add(n_righe, ${MAX});
while ${n_righe} {
    print("#", 0);

    var i = -2;
    add(i, ${MAX});
    while ${i} {
        print(" ", 0);

        subtract(i, 1);
    }
    print("#");

    subtract(n_righe, 1);
}

# var i = 0;
add(i, ${MAX});
while ${i} {
    print("#", 0);

    subtract(i, 1);
}
print("");
