var i = 0;
var a = 10;

while ${a} {
    add(i, 1);

    var check = 7;
    subtract(check, ${i});

    var check2 = 4;
    subtract(check2, ${i});

    if not ${check2} {
        continue;
    }

    subtract(a, 1);
    print(${i});

    if not ${check} {
        break;
    }
}
