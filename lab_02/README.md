### Сборка:

В директории `lab_02` написать в терминале `make`:

```
$ make
mkdir obj
g++ -c -Wall -Wextra -Werror -I include/ -std=c++14 src/main.cpp -o obj/main.o
g++ -c -Wall -Wextra -Werror -I include/ -std=c++14 src/cache.cpp -o obj/cache.o
g++ -c -Wall -Wextra -Werror -I include/ -std=c++14 src/function.cpp -o obj/function.o
g++ obj/main.o obj/cache.o obj/function.o -o lab_02
```
### Запуск:

В той же директории запустить программу через `./lab_02`, передав ей в качестве аргумента вещественное число, которое будет использоваться как параметр для функции `void f(double w);`

Пример:
```
./lab_02 3.14
```