# Telemetry

## Proiect pt curs de OS

Proiectul este ales din lista disponibila [aici](https://cs.unibuc.ro/~pirofti/so/so-lab-proiect.pdf).
Proiectul pe care l-am ales este proiectul #17.

## Ce dorim sa facem pentru proiect

### Ce stie sa faca produsul final

Un program care ne foloseste produsul poate sa:
1. Trimita (broadcast) un mesaj la un anumit canal, organizat ierarhic asemenea unui `path` in `Unix`.
2. Accesarea ultimelor `x` mesaje trimise pe un anumit canal, sau un stramos de-al acestuia.
3. Inregistrarea / stergerea unor functii de callback pe anumite canale, care vor fi apelata asincron (pe un `thread` nou) de fiecare data cand cineva broadcasteaza un mesaj pe canalul respectiv sau un stramos de-al sau.

### Ce componente implementam

Implementam doua componente.
Prima este un daemon, care poate fi pornit de librare, care verifica daca daemonul ruleaza, si daca nu il porneste, manual, sau automat (de ex prin `systemctl`).
A doua parte este o librarie implementata in `C`, care poate fi inclusa din orice program `C/C++`, care sa permita interactiunea cu daemonul.

### Ce rol are fiecare componenta

Daemonul are urmatorul rol:
 * Este un element central al comunicarii, care nu este pair-to-pair ci client-to-daemon.
 * Tine minte mesaje trimise pe diferite canale, mult dupa sfarsirea proceselor care au trimis acele mesaje.
 * Tine minte o lista de callbackuri pentru fiecare proces, si le declanseaza cand este nevoie.

Clientul are urmatorul rol:
 * Este un middle-man intre utilizator si daemon, prezinta un API mai user-friendly.
 * Constituie un "translation unit" care primeste de la daemon callbackurile declansate si le porneste el.

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
personal_fifo_id
channel_length
channel
```

Tip 3: Cerere de revocare callback -- Cerere de la client la daemon

``` log
3
token
personal_fifo_id
```

Tip 3: Callback -- De la daemon la client

``` log
token
channel_length
channel
message_length
message
```
