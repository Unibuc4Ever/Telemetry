# Telemetry

## Usage

`make` for building:
* `daemon.out`, daemon runtime
* `libtelemetry.a`, static lib
* `main.out`, demo

For closing the daemon, use the following command:
```bash
➜  SScHat git:(main) pkill -f "./daemon.out"
➜  SScHat git:(main) ps -x | grep "daemon"
```

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

## Comunicarea dintre Daemon si librarie

O sa folosim pipe-uri, mai precis FIFO.


* Toate FIFO-urile denumite mai jos se afla in folderul `/tmp`, astfel FIFO-ul `exampleFifo`
defapt are flly qualified name `/tmp/exampleFifo`
* Daemonul o sa creeze un FIFO numit `TelemetryRequests`.
* Flow-ul de comunicare intre Daemon si procese e urmatorul:
    * Daca un proces vrea sa ii transmita ceva Daemonului, o sa scrie requestul
      in FIFO-ul `TelemetryQueryNr{PID}{RRR}`, (PID este pid-ul clientului, iar RRRR formeaza un numar generat aleator de la 1 la 1000) si ii transmite numele fifo-ului acesta Daemonului in `TelemetryRequests`, care il citeste, si dupa il sterge.
    * Daca daemonul vrea sa transmita ceva unui proces, ii scrie acestuia in
      FIFO-ul numit `TelemetryReceiveNr{PID}`, daca acesta este deschis.
    * Cand FIFO-ul care corespunde unui proces se inchide, Daemonul considera ca s-a
      terminat procesul, si sunt sterse toate recordurile ale acestuia.
      Acest cleanup are loc periodic.

## Tipuri de mesage de la Client la Daemon

Tip 1: Broadcast

``` log
1
channel_length
channel
message_length
message
```

Tip 2: Cerere de callback

``` log
2
token
personal_fifo_id
channel_length
channel
```

Tip 3: Cerere de revocare callback

``` log
3
token
personal_fifo_id
```

Tip 4: Cerere de history
``` log
4
personal_fifo_id
history_count
channel_length
channel
```

## Tipuri de mesage de la Daemon la Client

Tip 1: Callback

``` log
1
token
channel_length
channel
message_length
message
```

Tip 2: History
``` log
2
count_history
ch_1_length
ch_1
message_1_length
message_1
ch_2_length
ch_2
message_2_length
message_2
...
...
```