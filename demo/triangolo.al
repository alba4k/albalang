var MAX  = 7;
var n_righe = 0;
add(n_righe, ${MAX});

while ${n_righe} {
    var i = 1;
    add(i, ${MAX});
    subtract(i, ${n_righe});

    while ${i} {
        print("#", 0);

        subtract(i, 1);
    }

    print("");
    
    subtract(n_righe, 1);
}
