# Telemetry

## Proiect pt curs de OS

Proiectul este ales din lista disponibila [aici](https://cs.unibuc.ro/~pirofti/so/so-lab-proiect.pdf).
Proiectul pe care l-am ales este proiectul #17.


## Ideea proiectului
Proiectul este compus din doua componente diferite:
1. Un `daemon` care ruleaza in fundal.
2. Un set de librarii care sa poata conecta un program de C/C++ cu `daemon`-ul.


## Cerintele proiectului
Proiectul cere crearea unui `daemon` care sa stie sa primeasca / dea broadcast la mesaje, date pe diferite canale.
Fiecare program poate prin intermediul unei librarii sa comunice cu daemonul.
Lista de functii disponibile poate fi gasita [aici](Library/telemetry.h).

## Functionarea interna
### Daemon
`Daemonul` primeste prin cv magie neagra doua tipuri de mesaje:
1. Tuplul `(PID, callback_id)`, care reprezinta:
    * Procesul care doreste sa integistreze callbackul este `PID`.
    * ID-ul callbackului pe care procesul il doreste este `callback_id`.
2. Tuplul `(PID, channel, message)`, care reprezinta:
    * Procesul care a lansat broadcastul este procesul `PID`.
    * Canalul pe care e dat broadcastul este canalul `channel`.
    * Mesajul trimis este mesajul `message`.

Functionarea `daemon`-ului este urmatoarea:
1. Daemonul memoreaza toate inregistrarile de callback pe care le primeste.\
Din cand in cand, `daemon`-ul scaneaza toate procesele, pentru a vedea daca anumite callbackuri nu mai exista (le-a murit procesul).
2. Cand `daemon`-ul primeste un broadcast, verifica callbackurile care trebuie apelate, si le apeleaza.

## Libraria
Libraria este un front-end pentru interactiunea cu `daemon`-ul.
Libraria face:
1. Cand este initializata, libraria deschide conectiunea cu `daemon`-ul.
2. Cand se primeste cererea de inregistrare / desinregistrare a unui callback, libraria salveaza callbackul, ii asociaza un ID, si trimite cererea la `daemon`.
3. Cand libraria afla de la `daemon` ca trebuie apelat un callback, il apeleaza cu mesajul primit.
