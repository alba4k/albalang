var MAX  = 7;
var n_righe = ${MAX};

while ${n_righe} {
    var i = ${MAX};
    subtract(n_righe, 1);
    subtract(i, ${n_righe});

    while ${i} {
        print("#", 0);

        subtract(i, 1);
    }

    print("");
}

