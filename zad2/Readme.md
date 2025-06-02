# Wielowątkowy chat na socketowej strukturze Server - Client

## Opis projektu

Celem projektu było stworzenie wielowątkowego serwera czatu w języku C++, umożliwiającego komunikację wielu klientów równocześnie, z zapewnieniem synchronizacji dostępu do współdzielanych danych. Aplikacja została zrealizowana z wykorzystaniem biblioteki WinSock2, a całość komunikacji sieciowej oraz obsługi klientów zorganizowano w oparciu o standardowe wątki (`std::thread`) oraz mechanizmy ochrony sekcji krytycznych (`std::mutex`).

Serwer obsługuje wielu klientów w sposób równoległy, rozgłasza wiadomości do wszystkich użytkowników, przechowuje historię czatu i umożliwia korzystanie z prostych komend tekstowych. Po stronie klienta wiadomości są kolorowane (zielony – własne, żółty – innych użytkowników), a obsługa odbywa się w osobnym wątku odbierającym dane od serwera. Nowi użytkownicy otrzymują ostatnie 100 wiadomości z historii czatu, co zwiększa spójność komunikacji.

Projekt został przetestowany w środowisku Windows i w pełni działa w terminalu (PowerShell, Windows Terminal, Git Bash, CLion console itp.).

## Instrukcja uruchomienia

1. Kompilacja (MinGW lub CLion z toolchainem Windows):
   
   g++ Server.cpp -o server.exe -lws2_32 -std=c++17
   g++ Client.cpp -o client.exe -lws2_32 -std=c++17
   

2. Uruchomienie:
   - Należy uruchomić `server.exe` w pierwszym terminalu.
   - W kolejnych terminalach uruchamiać `client.exe`.
   - Klient zapyta o nick, który zostanie przekazany do serwera.

3. Zależności:
   - system operacyjny Windows
   - biblioteka WinSock2
   - kompilator C++17 lub nowszy

## Opis działania i problemów

Aplikacja składa się z dwóch głównych komponentów: serwera (`Server.cpp`) oraz klienta (`Client.cpp`). Po połączeniu klient wysyła swój nick, otrzymuje ostatnie wiadomości z historii czatu, a następnie może wysyłać wiadomości tekstowe do wszystkich uczestników.

W celu umożliwienia obsługi wielu klientów równocześnie, serwer tworzy dla każdego połączenia osobny wątek. Każdy wątek działa niezależnie i przetwarza wiadomości klienta, obsługuje komendy, oraz reaguje na rozłączenie użytkownika.

Synchronizacja dostępu do danych współdzielonych przez wątki (np. lista klientów, mapa nicków, historia wiadomości) została zrealizowana poprzez zastosowanie `std::mutex` i `std::lock_guard`.

## Wątki i co reprezentują

Każdy klient podłączony do serwera otrzymuje osobny wątek:

```cpp
std::thread t(handle_client, client_socket);
t.detach();
```

Wątek `handle_client` odpowiada za:
- odbiór początkowego nicku,
- przesłanie historii czatu,
- przetwarzanie wiadomości i komend,
- obsługę rozłączenia klienta i czyszczenie zasobów.

Dzięki `detach()`, wątki nie muszą być łączone (`join()`), co upraszcza zarządzanie.

Po stronie klienta działa osobny wątek `receive_messages`, który cały czas nasłuchuje dane od serwera i wypisuje je w terminalu, jednocześnie pozwalając użytkownikowi pisać wiadomości w głównym wątku `main()`.

## Sekcje krytyczne i ich rozwiązanie

W celu zapewnienia bezpieczeństwa dostępu do współdzielonych struktur zastosowano `std::mutex`:

- `clients` – wektor z socketami aktywnych klientów
- `client_names` – mapa przypisująca socketowi nazwę użytkownika
- `messages_history` – lista przechowująca ostatnie wiadomości

Każda operacja na tych strukturach odbywa się wewnątrz bloku:

```cpp
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    // operacje na zasobach współdzielonych przez wątki
}
```

Dzięki temu mamy gwarancję, że tylko jeden wątek w danym momencie modyfikuje zasób, co zapobiega race condition i błędom odczytu/zapisu.

## Komendy dostępne w czacie

W kliencie użytkownik może korzystać z następujących komend:

- `/who` – wypisuje aktualnie podłączonych użytkowników
- `/nick nowy_nick` – zmienia nick użytkownika i powiadamia o tym wszystkich

Dodatkowo serwer automatycznie powiadamia wszystkich, gdy:
- ktoś dołącza: `UżytkownikX has joined the chat.`
- ktoś wychodzi: `UżytkownikX has left the chat.`
- ktoś zmienia nick: `StaryNick changed nickname to NowyNick.`

## Kolorowanie wiadomości

W celu poprawy czytelności komunikatów, klient koloruje teksty w konsoli za pomocą kodów ANSI:

- Zielony nick (`\033[32m`) – Twoje własne wiadomości
- Żółty nick (`\033[33m`) – Wiadomości innych użytkowników

## Historia wiadomości

Serwer przechowuje ostatnie 100 wiadomości w pamięci (`std::vector<std::string>`). Gdy nowy klient się połączy, serwer wysyła mu całą historię przed rozpoczęciem dalszej komunikacji.

Dzięki temu użytkownik widzi kontekst rozmowy od momentu dołączenia i nie zaczyna „od zera”. Rozmiar historii można łatwo zmienić przez edycję zmiennej `HISTORY_SIZE`.

---

## Podsumowanie

Projekt w pełni realizuje założenia zadania:

- Serwer wielowątkowy z równoległą obsługą klientów
- Synchronizacja współdzielonych danych z użyciem `mutex`
- Sprawna komunikacja i rozsyłanie wiadomości
- Historia czatu i obsługa prostych komend
- Czytelny interfejs terminalowy z kolorowaniem
