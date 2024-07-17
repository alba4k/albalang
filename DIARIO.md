## obiettivo: poter stampare il seguente output in albalang (28.03.2024)

```
    *
   ***
  *****
 *******
*********
 *******
  *****
   ***
    *
```

L'idea è nata parlando di che aspetto avrebbe un linguaggio di programmazione che, pur mantenendo funzionalità, richieda meno caratteri possibile. È evoluto in un LAM diverso, con uno pseudolinguaggio generico

# 11.04.2024
È ora possibile salvare delle variabili, ma non esiste modo di accedere ai valori salvati, ed usare commenti.
```albalang
var str abc <- "ciao";
var num 123 <- "1.23";
```

# 18.04.2024
Sintassi leggermente cambiata per le variabili, in
```albalang
var str abc = "ciao";
var num 123 = 1.23;
```
Inoltre ora è possibile stampare il contenuto di una variabile con
```albalang
print(${abc});
```
Tutti i valori salvati sono però costanti e non possono essere aggiornati, né è possibile utilizzare dell'aritmetica.

# 25.04.2024
- Implementati dei log extra per debug quando si compila con `make DEBUG=1`
- Uniti gli struct Number e String in un unico Variable, definito come 
```C
struct Variable {
    char *name;

    double *number;
    char *string;

    struct Variable *prev;
    struct Variable *next;
};
```
- Non è più necessario specificare il tipo di una variabile, è sufficiente mettere o no le virgolette.
```albalang
var abc = "ciao";
var 123 = 1.23;
```

# 02.05.2024
- Creata una serie di operazioni che possono essere effettuate su di una variabile numerica
```albalang
var a = 2;           # 2
var b = 1;

add(a, ${b});        # 3
divide(a, 1.5);      # 2
multiply(a, 2);      # 4
power(a, 3);         # 64
sqrt(a);             # 8
subtract(a, 5);      # 3

print(${a});         # >>> 3
```
- Un operatore permette di concatenare delle stringhe
```albalang
var a = "hello";
var b = " world";

concatenate(a, ${b});
print(${a});         # >>> hello world
```
- È ora possibile cambiare il valore di una variabile
```albalang
var a = 3;
print(${a});         # >>> 3

var b = "ciao";(
print(${a});         # >>> ciao
```

# 16.05.2024
Ora dove si può accedere ad una variabile con `${var}` è accettata anche un numero o una stringa grazie all'implementazione di eval() che gestisce i valori inseriti. Questa unifica l'implementazione del parsing per variabili, numeri e stringhe.

# 23.05.2024
Quando vi è un errore sarà stampato un messaggio più specifico oltre all'ultima riga eseguita
```
[ERROR] An error occurred while running the following line
        >>> add(w, ${v}); <<<
       <!> Variable not found
```

# 6.6.2024
Non dovrebbero più esserci particolari problemi quando si cerca di usare ';' in una stringa, ad esempio
```albalang
print(";");
```
non verrà più letto come
```albalang
print(";
```

# 10.6.2024
Ho aggiunto dei rudimentali if / else (per ora è considerato true un numero diverso da 0)
```albalang
var a = 12;

if ${a} {
    print("1");
}

if 0 {
    print("2");
} else {
    print("3");
}
```
Stamperà:
```
1
3
```

# 11.6.2024
Ora si possono includere altri file all'interno di uno già aperto, il contenuto sarà eseguito in mezzo a quello corrente
```albalang
include "file.al";

var filename = "file2.al";
include ${filename};
```

# 17.7.2024
Aggiunte due funzioni di casting
```albalang
var a = "54";

num(a);
str(a);
```
