/*
  ================================================================
  ASKELMOOTTORIN OHJAUS ARDUINOLLA
  A4988 / DRV8825 -askelmoottoriohjain
  ================================================================

  Tämä esimerkkiohjelma ohjaa askelmoottoria Arduino-kortilla
  käyttäen A4988- tai DRV8825-askelmoottoriohjainta.

  Tässä ohjelmassa EI käytetä erillistä stepper-moottorikirjastoa.
  Moottoria ohjataan suoraan Arduino-pinneillä.

  Ohjauksessa käytetään kahta tärkeää signaalia:

    DIR  = Direction
           Määrittää moottorin pyörimissuunnan.

    STEP = Step
           Jokainen STEP-signaalin pulssi käskee ajuria
           siirtämään moottoria yhden askeleen.

  Tärkeä periaate:

      DIR-signaali määrittää SUUNNAN
      STEP-pulssit määrittävät LIIKKEEN MÄÄRÄN

  Esimerkiksi:

      200 STEP-pulssia
              ↓
      200 askelta
              ↓
      1 kierros

  Tämä pätee tässä ohjelmassa, koska stepsPerRevolution = 200.

  HUOM!
  Todellinen kierrosmäärä voi muuttua, jos A4988/DRV8825-ajurissa
  käytetään microstepping-asetuksia.

  ================================================================
*/


// ----------------------------------------------------------------
// 1. ARDUINON JA ASKELMOOTTORIAJURIN YHTEYDET
// ----------------------------------------------------------------

/*
  #define-komennolla annetaan ohjelmassa käytettävälle luvulle
  selkeä nimi.

  Tässä:

      dirPin = 2

  tarkoittaa, että Arduino käyttää digitaalista pinniä 2
  askelmoottoriohjaimen DIR-tuloon.

  Käytännössä:

      Arduino pin 2  --->  A4988/DRV8825 DIR
*/
#define dirPin 2


/*
  stepPin = 3

  Arduino käyttää digitaalista pinniä 3
  askelmoottoriohjaimen STEP-tuloon.

  Käytännössä:

      Arduino pin 3  --->  A4988/DRV8825 STEP

  Arduino synnyttää tähän pinniin sähköisiä pulsseja.

  Jokainen kelvollinen STEP-pulssi saa ajurin tekemään
  yhden moottoriaskeleen.
*/
#define stepPin 3


/*
  stepsPerRevolution kertoo, kuinka monta askelta moottorin
  oletetaan tarvitsevan yhden täyden kierroksen tekemiseen.

  Tässä käytetään arvoa:

      200 askelta / kierros

  Tämä tarkoittaa:

      1 askel  = 1/200 kierrosta
      200 askelta = 1 täysi kierros

  Moottorin datasheet kannattaa kuitenkin aina tarkistaa.

  Lisäksi A4988/DRV8825 voi käyttää microstepping-toimintoa.
  Jos esimerkiksi käytetään 1/2-, 1/4-, 1/8- tai 1/16-mikroaskelia,
  tarvittavien STEP-pulssien määrä yhtä täyttä moottorin
  kierrosta kohti muuttuu.

  Esimerkiksi:

      Full step:       200 pulssia / kierros
      1/2 step:        400 pulssia / kierros
      1/4 step:        800 pulssia / kierros
      jne.

  Tämän vuoksi tämä arvo riippuu sekä moottorista että
  ajurin microstepping-asetuksista.
*/
#define stepsPerRevolution 200



// ----------------------------------------------------------------
// 2. SETUP-FUNKTIO
// ----------------------------------------------------------------

/*
  setup() suoritetaan Arduino-ohjelman käynnistyessä
  yhden kerran.

  Se suoritetaan esimerkiksi:

      - kun Arduino käynnistyy
      - kun Arduino resetoidaan
      - kun uusi ohjelma ladataan Arduinoon

  setup()-funktiossa tehdään yleensä ohjelman alustukset.

  Tässä tapauksessa määritellään, että kaksi Arduino-pinniä
  toimivat OUTPUT- eli lähtöpinneinä.
*/
void setup() {

  /*
    pinMode() määrittelee Arduino-pinnin toimintatavan.

    Muoto:

        pinMode(pinni, toimintatapa);

    Tässä:

        stepPin = 3
        OUTPUT = Arduino lähettää sähköisen signaalin ulospäin.

    Arduino pystyy siis muuttamaan pinni 3:n tilaa esimerkiksi:

        LOW  -> noin 0 V
        HIGH -> noin 5 V

    Arduino UNO -kortilla.

    Tätä signaalia käytetään STEP-pulssien lähettämiseen
    A4988/DRV8825-ajurille.
  */
  pinMode(stepPin, OUTPUT);


  /*
    Myös DIR-pinni määritellään OUTPUT-tyyppiseksi.

    Arduino lähettää siis pinniin 2 joko:

        HIGH
        tai
        LOW

    Tällä valitaan askelmoottorin pyörimissuunta.

    Huomaa, että HIGH ei välttämättä tarkoita kaikissa
    moottorikytkennöissä juuri "myötäpäivään".

    Todellinen suunta riippuu esimerkiksi moottorin
    käämien kytkennästä.

    Tässä ohjelmassa kuitenkin oletetaan:

        HIGH = clockwise
        LOW  = counterclockwise
  */
  pinMode(dirPin, OUTPUT);
}



// ----------------------------------------------------------------
// 3. LOOP-FUNKTIO
// ----------------------------------------------------------------

/*
  loop() suoritetaan Arduino-ohjelmassa jatkuvasti uudelleen.

  Kun loop()-funktion lopussa saavutaan, Arduino aloittaa
  loop()-funktion alusta.

  Tämän ohjelman kokonaisrakenne on siis:

      1. 1 kierros hitaasti myötäpäivään
      2. odota 1 sekunti

      3. 1 kierros nopeasti vastapäivään
      4. odota 1 sekunti

      5. 5 kierrosta nopeasti myötäpäivään
      6. odota 1 sekunti

      7. 5 kierrosta nopeasti vastapäivään
      8. odota 1 sekunti

      9. aloita alusta

  Tämän vuoksi moottori jatkaa liikettä niin kauan kuin
  Arduino on päällä.
*/
void loop() {



  // ==============================================================
  // 4. YKSI HITAASTI TEHTÄVÄ KIERROS MYÖTÄPÄIVÄÄN
  // ==============================================================


  /*
    Valitaan moottorin pyörimissuunnaksi myötäpäivä.

    digitalWrite() asettaa digitaalisen pinnin tilan.

    Muoto:

        digitalWrite(pinni, tila);

    Tässä:

        dirPin = pinni 2
        HIGH   = digitaalinen HIGH-tila

    Eli:

        Arduino pin 2 = HIGH

    A4988/DRV8825 tulkitsee tämän DIR-signaalina.

    Tässä esimerkissä:

        HIGH = clockwise = myötäpäivään
  */
  digitalWrite(dirPin, HIGH);



  /*
    Seuraavaksi tehdään 200 askelta.

    for-silmukan rakenne:

        for (alkuarvo; ehto; muutos)

    Tässä:

        int i = 0

            Luodaan kokonaislukumuuttuja i ja asetetaan
            sen alkuarvoksi 0.

        i < stepsPerRevolution

            Silmukkaa jatketaan niin kauan kuin i on pienempi
            kuin stepsPerRevolution.

            Koska:

                stepsPerRevolution = 200

            silmukka suoritetaan arvoilla:

                i = 0
                i = 1
                i = 2
                ...
                i = 199

            Tämä tarkoittaa yhteensä 200 kierrosta silmukassa.

        i++

            i:tä kasvatetaan jokaisen kierroksen jälkeen yhdellä.

            i++ tarkoittaa käytännössä:

                i = i + 1

    Näin syntyy yhteensä 200 STEP-pulssia,
    eli tässä tapauksessa yksi täysi kierros.
  */
  for (int i = 0; i < stepsPerRevolution; i++) {


    /*
      ------------------------------------------------------------
      YHDEN ASKELEN MUODOSTAMINEN
      ------------------------------------------------------------

      Seuraavat neljä komentoa muodostavat yhden STEP-pulssin.

          1. STEP HIGH
          2. odotus
          3. STEP LOW
          4. odotus

      Tätä tapahtumasarjaa kutsutaan STEP-pulssiksi.
    */


    /*
      STEP-pinnin tila muutetaan HIGH-tilaan.

      Arduino pin 3:

          LOW
           |
           |
           +-------- HIGH
                    ^
                    |
                 STEP-pulssi

      A4988/DRV8825 havaitsee STEP-signaalin muutoksen
      ja valmistautuu suorittamaan yhden askeleen.
    */
    digitalWrite(stepPin, HIGH);


    /*
      Odotetaan 2000 mikrosekuntia.

      1 millisekunti (ms) = 1000 mikrosekuntia (µs)

      Näin:

          2000 µs = 2 ms

      STEP-pinni pysyy siis HIGH-tilassa 2 millisekunnin ajan.

      delayMicroseconds() on tarkoitettu erittäin lyhyisiin
      viiveisiin.

      Se eroaa delay()-funktiosta siten, että:

          delay(1000)
              = 1000 millisekuntia
              = 1 sekunti

          delayMicroseconds(1000)
              = 1000 mikrosekuntia
              = 1 millisekunti
    */
    delayMicroseconds(2000);


    /*
      STEP-pinni muutetaan takaisin LOW-tilaan.

      HIGH-pulssi päättyy tähän.

      Yksinkertaistettuna:

          HIGH
            ┌──────────┐
            │          │
            │          │
          ──┘          └────────
          LOW         LOW

              <--->
               2000 µs

      Tämä HIGH -> LOW -muutos muodostaa STEP-pulssin
      loppuosan.
    */
    digitalWrite(stepPin, LOW);


    /*
      STEP-pulssien väliin jätetään vielä 2000 mikrosekunnin
      eli 2 millisekunnin tauko.

      Tämän jälkeen for-silmukka siirtyy seuraavaan
      moottoriaskeleeseen.

      Yhden askeleen kokonaisajaksi muodostuu tässä
      yksinkertaistetusti:

          2000 µs HIGH
        + 2000 µs LOW
        ----------------
          4000 µs

      eli:

          4 ms / askel

      200 askelta:

          200 × 4 ms = 800 ms

      eli noin:

          0,8 sekuntia / kierros

      Todellinen nopeus riippuu myös ajurista, moottorista,
      Arduino-ohjelman suoritusajasta ja microsteppingistä.
    */
    delayMicroseconds(2000);
  }



  /*
    Kun 200 askelta on tehty, ohjelma odottaa yhden sekunnin.

    delay() käyttää millisekunteja.

        delay(1000)

    tarkoittaa:

        1000 ms = 1 sekunti

    Moottori pysyy tämän ajan paikallaan.
  */
  delay(1000);





  // ==============================================================
  // 5. YKSI NOPEA KIERROS VASTAPÄIVÄÄN
  // ==============================================================


  /*
    Vaihdetaan pyörimissuunta.

        LOW = tässä ohjelmassa vastapäivään

    Nyt A4988/DRV8825 saa DIR-signaaliksi LOW-tilan.
  */
  digitalWrite(dirPin, LOW);


  /*
    Tehdään jälleen 200 askelta.

    Koska:

        stepsPerRevolution = 200

    for-silmukka tuottaa:

        200 STEP-pulssia

    ja moottorin pitäisi tämän vuoksi pyöriä yhden
    täyden kierroksen vastapäivään.
  */
  for (int i = 0; i < stepsPerRevolution; i++) {


    /*
      Aloitetaan yksi STEP-pulssi asettamalla STEP-pinni HIGH.
    */
    digitalWrite(stepPin, HIGH);


    /*
      STEP-pinni pidetään HIGH-tilassa 1000 mikrosekuntia.

          1000 µs = 1 ms
    */
    delayMicroseconds(1000);


    /*
      Lopetetaan STEP-pulssi asettamalla STEP-pinni LOW.
    */
    digitalWrite(stepPin, LOW);


    /*
      Odotetaan vielä 1000 mikrosekuntia ennen seuraavaa
      STEP-pulssia.

      Yhden kokonaisen askeljakson pituus on tässä:

          1000 µs HIGH
        + 1000 µs LOW
        ----------------
          2000 µs

      eli:

          2 ms / askel

      Tämä on nopeampi kuin edellisessä vaiheessa,
      jossa käytettiin 4 ms / askel.
    */
    delayMicroseconds(1000);
  }


  /*
    Odotetaan yksi sekunti ennen seuraavaa liikettä.
  */
  delay(1000);





  // ==============================================================
  // 6. VIISI NOPEAA KIERROSTA MYÖTÄPÄIVÄÄN
  // ==============================================================


  /*
    Vaihdetaan jälleen suunta myötäpäivään.

        HIGH = tässä ohjelmassa myötäpäivään
  */
  digitalWrite(dirPin, HIGH);


  /*
    Nyt moottoria halutaan pyörittää viisi kierrosta.

    Yksi kierros:

        stepsPerRevolution = 200 askelta

    Viisi kierrosta:

        5 * stepsPerRevolution

        = 5 * 200

        = 1000 askelta

    Siksi for-silmukka suoritetaan 1000 kertaa.

    Jokainen kierros tuottaa yhden STEP-pulssin.
  */
  for (int i = 0; i < 5 * stepsPerRevolution; i++) {


    /*
      STEP-pulssi alkaa.

      Arduino pin 3 -> HIGH
    */
    digitalWrite(stepPin, HIGH);


    /*
      STEP-pinni on HIGH vain 500 mikrosekuntia.

          500 µs = 0,5 ms
    */
    delayMicroseconds(500);


    /*
      STEP-pulssi lopetetaan.

      Arduino pin 3 -> LOW
    */
    digitalWrite(stepPin, LOW);


    /*
      Odotetaan 500 mikrosekuntia ennen seuraavaa pulssia.

      Yksi askel kestää tässä:

          500 µs HIGH
        + 500 µs LOW
        ----------------
          1000 µs

      eli:

          1 ms / askel

      Tämä on jälleen nopeampi kuin aikaisemmat
      kaksi moottorin liikettä.
    */
    delayMicroseconds(500);
  }


  /*
    Kun 1000 askelta on tehty:

        1000 / 200 = 5 kierrosta

    Moottori pysähtyy ja odottaa yhden sekunnin.
  */
  delay(1000);





  // ==============================================================
  // 7. VIISI NOPEAA KIERROSTA VASTAPÄIVÄÄN
  // ==============================================================


  /*
    Vaihdetaan suunta vastapäivään.

        LOW = tässä ohjelmassa vastapäivään
  */
  digitalWrite(dirPin, LOW);


  /*
    Tehdään jälleen:

        5 × 200 = 1000 askelta

    Tämä vastaa viittä täyttä kierrosta.
  */
  for (int i = 0; i < 5 * stepsPerRevolution; i++) {


    /*
      Aloitetaan STEP-pulssi.
    */
    digitalWrite(stepPin, HIGH);


    /*
      HIGH-tila kestää 500 mikrosekuntia.
    */
    delayMicroseconds(500);


    /*
      Lopetetaan STEP-pulssi.
    */
    digitalWrite(stepPin, LOW);


    /*
      Odotetaan 500 mikrosekuntia ennen seuraavaa askelta.
    */
    delayMicroseconds(500);
  }


  /*
    Viiden kierroksen jälkeen moottori pysähtyy sekunniksi.
  */
  delay(1000);


  /*
    Tähän loop()-funktio päättyy.

    Arduino EI kuitenkaan lopeta ohjelmaa.

    Sen sijaan Arduino palaa automaattisesti loop()-funktion
    alkuun ja suorittaa kaiken uudelleen:

        1 kierros myötäpäivään
             ↓
        odotus
             ↓
        1 kierros vastapäivään
             ↓
        odotus
             ↓
        5 kierrosta myötäpäivään
             ↓
        odotus
             ↓
        5 kierrosta vastapäivään
             ↓
        odotus
             ↓
        ALOITA UUDELLEEN

    Tämä jatkuu niin kauan kuin Arduino saa virtaa.
  */
}
