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

## Comunicarea dintre Daemon si lirarie
O sa folosim pipe-uri, mai precis FIFO.

* Daemonul o sa creeze un FIFO numit '/tmp/TelemetryRequests/'.
* Flowul de comunicare intre Daemon si procese e urmatorul:
    * Daca un proces vrea sa ii transmita ceva Daemonului, o sa scrie requestul
      intr-un FIFO (/temp), si ii transmite numele Daemonului, care
      il citeste, si dupa il sterge.
    * Daca daemonul vrea sa transmita ceva unui proces, ii scrie acestuia in
      FIFO-ul numit '/tmp/TelemetryClient/%PID%', daca acesta este deschis.
    * Cand FIFO-ul care corespunde unui proces se inchide, Daemonul considera ca s-a
      terminat procesul, si sunt sterse toate recordurile ale acestuia.

## Tipuri de mesage de la client la Daemon

Tip 1: Broadcast -- Cerere de la client la daemon

``` log
1
channel_length
channel
message_length
message
```

Tip 2: Cerere de callback -- Cerere de la client la daemon

``` log
2
token
channel_length
channel
```

Tip 3: Callback -- De la daemon la client

``` log
token
channel_length
channel
message_length
message
```
