var str = "tests/include-b.al";

print("inside include-a.al");



include ${str};



print("include-b.al gave b, containing: ", 0);
print(${b});