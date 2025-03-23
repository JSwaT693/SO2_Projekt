# SO2_Projekt

1. Jak uruchomić program.

Aby uruchomić program w cmd/powershell należy wejść w folder z projektem, następnie skompilować program używając komendy:

- Na systemie Windows (przez WSL lub z zainstalowanym MinGW):
  
g++ -std=c++11 -o <Nazwa_programu> main.cpp Philosopher.cpp

- Na systemach Linux lub MacOS:
  
g++ -std=c++11 -pthread -o <Nazwa_programu> main.cpp Philosopher.cpp

I uruchomić program z ilością filozofów podaną jako argument:

./<Nazwa_programu> <ilość_filozofów>

LUB jeśli na Windows nie działa to:

./<Nazwa_programu>.exe <ilość_filozofów> lub <Nazwa_programu>.exe <ilość_filozofów>

2. Opis problemu jedzących filozofów.

Problem Jedzących Filozofów jest klasycznym problemem synchronizacji w informatyce, zaproponowanym przez Edsgera Dijkstrę w 1965 roku. Jest to model problemu z zakresu współbieżności, który ilustruje konieczność stosowania mechanizmów synchronizacji, aby unikać zakleszczenia (deadlocka) lub zagłodzenia.

Wizualizacja tego problemu to x filozofów siedzach przy jednym okrągłym stole. Obok każdego z nich znajdują się dwa widelce wspólne z jego sąsiadami. Każdy z nich ma trzy stany - myślenie, bycie głodnym lub jedzenie. Aby zjeść filozof potrzebuje dwa widelce. Po skończeniu jedzenia odkłada oba widelce i wraca do myślenia. Podstawowy problem polega na tym, aby uniknąć sytuacji, w której każdy filozof trzyma jeden widelec i czeka na drugi, co prowadzi do wiecznego blokowania oraz sytuacji, w której jeden z filozofów nigdy nie może zdobyć dwóch sztućców, ponieważ inni ciągle je odbierają.

Ten problem jest przykładem bardziej ogólnego problemu zarządzania zasobami w systemach współbieżnych i jest używany jako standardowy test dla algorytmów synchronizacji w systemach operacyjnych i programowaniu wielowątkowym.

3. Opis mojego rozwiązania.

Moje rozwiązanie zostało zaimplementowane w języku C++ z wykorzystaniem wielowątkowości (std::thread) i mechanizmów synchronizacji (std::mutex). W programie każdy filozof jest reprezentowany przez osobny wątek (std::thread) dodawany do wektora wątków filozofów threads i wykonujący funkcję philosophersLife(), a wyświetlanie stanu filozofów jest obsługiwane przez osobny wątek displayThread wykonujący funkcję display() wyświetlającą co stały okres czasu stan wszystkich filozofów.  Wątek displayThread jest uruchamiany w tle (detach()), ponieważ jego zadaniem jest tylko wyświetlanie stanu filozofów, natomiast każdy wątek filozofa działa równolegle, wykonując funkcję philosophersLife() dla swojego filozofa. Główny wątek (main()) oczekuje na zakończenie wątków filozofów za pomocą join().

Każdy filozof zaczyna swoje działanie od myślenia, Następnie po losowym czasie od 2 do 4 sekund robi się głodny i próbuje podnieść sztućce. Filozofowie mają swój poziom głodu (hunger), który zwiększa się o 1 co 2 sekundy, jeśli nie udaje się im zdobyć sztućców, a także maksymalny głód zdefiniowany w Philosophers.cpp. Jeśli osiągną ten maksymalny głód to ich stan zmienia się na DEAD i nie wykonują już żadnych działań. Natomiast jeśli filozofowi uda się zdobyć sztućce to je tak długo aż jego hunger osiągnie 0 i zaczyna od nowa myśleć.
ROZWIĄŻANIE GŁÓWNYCH PROBLEMÓW:

Aby uniknąć deadlocka, zastosowałem asymetryczne podnoszenie sztućców:

- Filozofowie o parzystych ID próbują najpierw podnieść lewy sztuciec, a potem prawy sztuciec.
- Filozofowie o nieparzystych ID próbują najpierw podnieść prawy sztuciec, a potem lewy sztuciec.
Dzięki temu nigdy nie dojdzie do sytuacji, w której wszyscy filozofowie trzymają tylko jeden sztuciec i czekają na drugi.

Aby zminimalizować szanse na zagłodzenie, dodałem dwa mechanizmy:

- Losowe czasy oczekiwania między próbami zdobycia sztućców:
Filozofowie czekają losowy czas (od 50 do 250 ms) zanim ponownie spróbują zdobyć sztućce, dzięki czemu filozofowie, którzy byli wcześniej pomijani, mają większe szanse na zdobycie sztućców.

- Losowe czasy jedzenia i myślenia:
- Filozofowie myślą przez losowy czas (od 2 do 4 sekund) oraz jedzą przez losowy czas (od 500 ms do 2 sekund).
  
To zapewnia bardziej naturalne rozłożenie czasu i zmniejsza prawdopodobieństwo długiego oczekiwania przez jakiegokolwiek filozofa.
