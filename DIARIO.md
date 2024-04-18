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