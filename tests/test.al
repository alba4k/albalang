var comparison_result = 1;
var comparison_result_2 = 27;

var variable = "ciao";
var number = 25.5;

compare(${variable}, "ciao", comparison_result);
compare(${number}, 25.3, comparison_result_2);

if not ${comparison_result} {
    print("CIAO");
}
else {
    print("ciao");
}

if not ${comparison_result_2} {
    print("BOH");
}
else {
    print("boh");
}
