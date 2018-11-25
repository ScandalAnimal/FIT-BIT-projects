/*
 * Subor:   proj1.c
 * Datum:   2014/11/16
 * Autor:   Maros Vasilisin, xvasil02@stud.fit.vutbr.cz, 1BIT
 * Projekt: Vypocty v tabulke, 1. projekt IZP
 * Popis:   Program je jednoduchy tabulkovy kalkulator. Implementuje
 *          funkcie vyhladania maxima, minima, suctu a aritmetickeho priemeru vybranych buniek.
 *          Tabulku vo forme textoveho suboru bude program ocakavat na standardnom vstupe.
 *          Pozadovanu operaciu a vyber buniek specifikuje pouzivatel v argumente prikazoveho riadku.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

enum errors // zoznam vsetkych chyb, ktore sa mozu vyskytnut za behu programu
  {
    ERRARG,     // chybny pocet argumentov, ci uz je to viac alebo menej ako ma byt
    ERRNOARG,   // ziadne argumenty - zavola sa ak nezadame do prikazoveho riadku nic
    ERROPRTN,   // zle zadana operacia, resp. nespravny nazov operacie
    ERRSLCT,    // zle zadany vyber buniek - zavola sa ak budu argumenty vyber_buniek nespravne
    ERRNONMBRS, // ak dame pocitat matematicku funkciu na rozsahu, kde nebude ziadne cislo
    ERROVER,    // ak zadame argument vyber_buniek mimo rozsah tabulky
  };

const char *ERRMSG[] = // samotne chybove hlasenia pre jednotlive chyby.. budu sa vypisovat na standardny chybovy vystup stderr
  {
  [ERRARG] =     "Chyba: Zle zadany pocet argumentov.\n",
  [ERRNOARG] =   "Chyba: Nezadali ste ziadny argument.\n",
  [ERROPRTN] =   "Chyba: Zle zadany argument operacia.\n",
  [ERRSLCT] =    "Chyba: Zle zadany argument vyber_buniek.\n",
  [ERRNONMBRS] = "Chyba: V danom rozsahu sa nenachadza ziadne cislo.\n",
  [ERROVER] =    "Chyba: Argument vyber_buniek je mimo rozsah zadanej tabulky.\n"
  };


void Help() // funkcia pre vypis napovedy, nema navratovu hodnotu, vypis napovedu ak zadame argument --help
  {
    printf("Toto je napoveda k programu Vypocty v tabulke.\n"
    "Program je jednoduchy tabulkovy kalkulator.\n"
    "Program vie pocitat s celymi a desatinnymi cislami.\n"
    "Pri vstupe ocakava tabulku vo forme textu.\n"
    "Pozadovanu operaciu specifikuje pouzivatel.\n"
    "Program sa spusta v podobe:\n"
    "   ./proj1 --help   (zobrazi napovedu k programu a skonci)\n"
    "   ./proj1 operacia vyber_buniek\n"
    "Argument operacia moze byt:\n"
    "   select :vytlaci vybrane bunky z tabulky\n"
    "   min alebo max :vytlaci minimalnu/maximalnu hodnotu z daneho rozsahu\n"
    "   sum :vytlaci sumu hodnot vybranych buniek\n"
    "   avg :vytlaci aritmeticky priemer hodnot vybranych buniek\n"
    "Argument vyber_buniek moze byt:\n"
    "  row X :vyber vsetkych buniek na riadku X ( X > 0)\n"
    "  col X :vyber vsetkych buniek v stlpci X ( X > 0)\n"
    "  rows X Y :vyber vsetkych buniek od riadku X po riadok Y (0 < X <= Y)\n"
    "  cols X Y :vyber vsetkych buniek od stlpca X po stlpec Y (0 < X <= Y)\n"
    "  range A B X Y :vyber buniek od riadku A po riadok B\n"
    "                a od stlpca X po stlpec Y(vratane danych)\n"
    "                (0 < A <= B),(0 < X <= Y)\n");
  }

void vypis(int c,char *operacia) // vypis by sa opakoval viackrat v kode , tak som ho dal ako funkciu samostatne
  {
    if (strcmp("select",operacia) == 0) // ak je select tak vypis na stdout, inak nevypisuje nic
      {
        printf("%c", c);
      }
  }

int vypocet(char *operacia, char pole[]) // funkcia vypocet sa vola s argumentom operacia,
                         // ktory oznacuje operaciu, ktoru chceme urobit, a argumentom pole,
                         // ktore oznacuje pole na ktorom chceme tuto operaciu urobit
                         // navratova hodnota je int, pretoze moze byt len EXIT_SUCCESS = 0 alebo EXIT_FAILURE = 1
  {
    double sum = 0; // premenna pre bypocet sumy, teda suctu hodnot vsetkych zadanych buniek
    double max = DBL_MIN; // premenna pre vypocet maxima, je inicializovana na najmensiu moznu hodnotu daneho datoveho typu/double
    double min = DBL_MAX; // premenna pre vypocet minima, je inicializovana na najvacsiu moznu hodnotu daneho datoveho typu/double
    double avg; // premenna pre vypocet priemeru
    int pom = 0; // pomocna premenna pre operaciu avg, urcuje pocet buniek, budeme nou delit sumu
    double cislo = 0; // premenna kde sa uklada bunka, s ktorym pracujeme
    char *token; // premenna ktora oznacuje token z pola, teda usek pola s ktorym prave pracujeme

    token = strtok(pole, " "); // rozdelovanie pola pole[] na tokeny, ktore sa koncia prvou medzerou

    while (token != NULL) // robi to az po koniec pola, teda kym nebude pole token prazdne
      {
        //printf("%s\n", token); // pomocny vypis vypise prave vytvoreny token
         cislo = strtod(token, NULL); // prevod retazca na cislo double
         pom++; // zvysenie pomocnej premennej pom o 1 , je to pocet slov v poli
         if (cislo >= max) // ak je dane cislo > ako doterajsie max tak sa stava novym maximom
           {
              max = cislo;
           }
         if (cislo <= min) // ak je cislo < ako min tak sa stava novym minimom
           {
              min = cislo;
           }
         sum = sum + cislo; // cislo sa vzdy pridava do sumy, ktora bola doteraz vypocitana

         token = strtok(NULL, " "); // nacitanie dalsieho tokenu v cykle
      }
      if (pom == 0) // kontrola ci v zadanom rozsahu bolo aspon jedno cislo, aby sa mohli
                    // vypisovat matematicke funkcie min, max, sum a int
        {
          fprintf(stderr,"%s",ERRMSG[ERRNONMBRS]);
          return EXIT_FAILURE;
        }
    avg = sum / pom; // vypocet priemeru vydelenim sumy poctom slov
    if (strcmp("min",operacia) == 0) // vypisy danych vysledkov podla toho, co je zadane v prikazovom riadku
      {
        printf("%.10g", min);
      }
    if (strcmp("max",operacia) == 0)
      {
        printf("%.10g", max);
      }
    if (strcmp("sum",operacia) == 0)
      {
        printf("%.10g", sum);
      }
    if (strcmp("avg",operacia) == 0)
      {
        printf("%.10g", avg);
      }
    return EXIT_SUCCESS;

  }

/* funkcia rows() sa zavola ak do prikazoveho riadku zadame select row zaciatok alebo select rows zaciatok koniec,
 * pricom zaciatok je poradie riadku v tabulke ktory chceme vypisat ako prvy a
 * koniec je poradie riadku ktory chceme vypisat ako posledny,
 * funkcia vlastne vypise riadky od zaciatok po koniec (vratane)
 * funkcia vypise cele riadky tak, ze kazdy prvok zapise do samostateho riadku
 * funkcia vypisuje riadok po jednotlivych znakoch
 * vracia nulu ak vsetko prebehlo v poriadku a 1 ak nastala chyba + chybove hlasenie
 */

int rows(char *operacia, int zaciatok, int koniec) // volanie funkcie s troma parametrami
                    // aka operacia sa ma vykonat a zaciatocny a konecny riadok
  {
    int c; // premenna do ktorej nacitavame znak
    int i = 1;
    bool flag = 0; // premenna flag kontroluje ci predchadzajuci znak bola medzera, je to potrebne kontrolovat vtedy,
                   // ked mame viac medzier za sebou a chceme ich vypisat na koniec riadku a nie na zaciatok,
                   // aby sme mali jednotlive prvky pekne zarovnane podla okraja terminalu


    char str[(koniec-zaciatok+1)*1025]; //pole do ktoreho budem ukladat prvky len ak su cisla, biele znaky alebo desatinna bodka
    int por = 0; // index pola str[]

    while (i <= koniec) // cyklus prechadza tabulku od prveho riadku az po riadok,
      {                 // ktory chceme vypisat ako posledny
                        // i je poradie riadku, na ktorom prave sme
                        // premenna koniec oznacuje riadok, ktory chceme vypisat ako posledny
                        // pri row sa vypise len jeden riadok , teda zaciatok = koniec

        if ((c = getchar()) == '\n') // na konci riadku sa len zvysi premenna
          {
            i++;
            flag = 0;
            str[por] = ' '; // ak ano tak znak zapiseme do pola
            por++;
          }
        if (isblank(c) != 0) // ak je znak biely znak
          {
            str[por] = ' '; // biely znak sa zapise aj do pola ako medzera
            por++;
            flag = 1;
          }
        if ((i >= zaciatok) && (i <= koniec)) // ak sme medzi riadkami, ktore chceme vypisovat
          {
            if ((isblank(c) == 0) && (flag == 1))
              {
                flag = 0;
                if (strcmp("select",operacia) == 0) // ak je select tak vypis na stdout, inak nevypisuje nic
                  {
                    printf("\n");
                  }
              }
            vypis(c,operacia);
            if (((c >= '0') && (c <= '9')) || (c == '.') || (c == '-')) // kontrola ci je znak cislo, alebo desatinna bodka
              {
                str[por] = c; // ak ano tak znak zapiseme do pola
                por++;
              }

          }
        if (c == EOF) // ak sme uz na konci tabulky
          {
            if ((koniec >= i) || (zaciatok >= i)) // a ak zistime ze argumenty prikazoveho riadku nesedia, tak vyhodime chybu
              {
                fprintf(stderr,"%s\n",ERRMSG[ERROVER]);
                return EXIT_FAILURE;
              }
          }
        }

      str[por] = '\0'; // na koniec pola vzdy treba dopisat znak ukoncovacej nuly
      if (strcmp("select",operacia) != 0)
        {
          vypocet(operacia,str); // zavola sa funkcia pre vypocty
        }
    return EXIT_SUCCESS;
  }

/* funkcia cols() sa zavola ak do prikazoveho riadku zadame select col zaciatok alebo select cols zacatok koniec,
 * pricom zaciatok je poradie stlpca v tabulke, ktory chceme vypisat ako prvy a
 * koniec je poradie stlpca. ktory chceme vypisat ako posledny
 * funkcia vlastne vypise stlpce od zaciatok po koniec (vratane)
 * jednotlive bunky sa vypisuju kazda na samostatny riadok
 * vracia nulu ak vsetko prebehlo v poriadku a 1 ak nastala chyba + chybove hlasenie
 */

int cols(char *operacia, int zaciatok, int koniec) // funkcia ma 3 parametre a to operacia a zaciatocny a koncovy stlpec
  {
    int c; // premenna do ktorej nacitavame znak
    bool flag = 1; // premenna flag kontroluje ci predchadzajuci znak bola medzera, je to potrebne kontrolovat vtedy,
                   // ked mame viac medzier za sebou a chceme ich vypisat na koniec riadku a nie na zaciatok,
                   // aby sme mali jednotlive prvky pekne zarovnane podla okraja terminalu

    int ps = 0; // pocet slov/buniek v riadku

    char str[1025]; //pole do ktoreho budem ukladat prvky len ak su cisla, biele znaky alebo desatinna bodka
    int por = 0; // index pola str[]

    while ((c = getchar()) != EOF) // prechadzame celu tabulku po znakoch
      {
        if (isblank(c) != 0) // ak je biely znak tak zmenime priznak
          {
            flag = 1;
            str[por] = ' '; // ak ano tak znak zapiseme do pola
            por++;
          }
        if ((isblank(c) == 0) && (flag == 1)) // ak sme na prvom znaku slova , tak zvysime pocet slov
          {
            ps++;
            flag = 0;
            if ((ps >= zaciatok) && (ps <= koniec))
              {
                if (strcmp("select",operacia) == 0) // ak je select tak vypis na stdout, inak nevypisuje nic
                  {
                    printf("\n");
                  }
              }
          }
        if (c == '\n') // ak sme na konci riadku tak testujeme, ci mame spravne argumenty
          {
            if ((koniec > ps) || (zaciatok > ps)) // a ak zistime ze argumenty prikazoveho riadku nesedia, tak vyhodime chybu
              {
                fprintf(stderr,"%s\n",ERRMSG[ERROVER]);
                return EXIT_FAILURE;
              }
            ps = 0; // na konci riadku sa vynuluje pocitadlo pocet slov v riadku
            flag = 1;
            str[por] = ' '; // ak ano tak znak zapiseme do pola
            por++;
          }
        if ((ps >= zaciatok) && (ps <= koniec)) // ak sme medzi stlpcami, ktore chceme vypisovat tak vypisujeme
          {
            vypis(c,operacia);
            if (((c >= '0') && (c <= '9')) || (c == '.') || (c == '-')) // kontrola ci je znak cislo, alebo desatinna bodka
              {
                str[por] = c; // ak ano tak znak zapiseme do pola
                por++;
              }
          }
      }
      str[por] = '\0'; // na koniec pola vzdy treba dopisat znak ukoncovacej nuly
      if (strcmp("select",operacia) != 0)
        {
          vypocet(operacia,str); // zavola sa funkcia pre vypocty
        }
    return EXIT_SUCCESS;
  }


/* funkcia selectrange() sa zavola ak do prikazoveho riadku zadame select range r1, r2, c1, c2, pricom r1 je poradie riadka
 * v tabulke od ktoreho chceme zacat vypis a r2 je poradie riadku, kde sa ma vypis skoncit
 * parameter c1 udava poradie stlpca od ktoreho chceme zacat vypisovat a parameter c2 konecny stlpec
 * vracia nulu ak vsetko prebehlo v poriadku a ak nastala chyba --> chybove hlasenie
 */

int range(char *operacia, int r1, int r2, int c1, int c2) // volanie funkcie s 5 parametrami
              // parameter operacia urcuje, ktora operacia sa ma vykonat, a parametre r1, r2, c1, c2 urcuju rozsah
  {
    int c; // premenna kde sa nacitava znak z prikazoveho riadku
    bool flag = 1; // premenna flag kontroluje ci predchadzajuci znak bola medzera, je to potrebne kontrolovat vtedy,
                   // ked mame viac medzier za sebou a chceme ich vypisat na koniec riadku a nie na zaciatok,
                   // aby sme mali jednotlive prvky pekne zarovnane podla okraja terminalu
    int ps = 0; // premenna pocet slov v riadku
    int i = 1; // premenna pocet riadkov

    char str[(r2-r1+1)*1025]; //pole do ktoreho budem ukladat prvky len ak su cisla, biele znaky alebo desatinna bodka
    int por = 0; // index pola str[]


    while (i <= r2)     // cyklus prechadza tabulku od prveho riadku az po riadok,
      {                 // ktory chceme vypisat ako posledny
                        // i je poradie riadku, na ktorom prave sme
                        // premenna r2 oznacuje riadok, ktory chceme vypisat ako posledny

        c = getchar(); // do premennej c si nacitame znak
        if (isblank(c) != 0) // ak je znak biely znak
          {
            flag = 1;
            str[por] = ' '; // biely znak sa zapise aj do pola ako medzera
            por++;
          }
        if ((isblank(c) == 0) && (flag == 1)) // ak sme na prvom znaku slova/bunky
          {
            ps++;
            flag = 0;
          }
        if ((i >= r1) && (i <= r2)) // ak sme medzi riadkami, ktore chceme vypisovat
          {
            if ((ps >= c1) && (ps <= c2)) // ak sme medzi stlpcami, ktore chceme vypisovat
              {
                if ((isblank(c) == 0) && (flag == 1))
                  {
                    ps++; // zvysime pocet slov v riadku
                    flag = 1;
                    if (strcmp("select",operacia) == 0) // ak je select tak vypis na stdout, inak nevypisuje nic
                      {
                        printf("\n");
                      }
                  }
                vypis(c,operacia); // ak je select tak vypis
                if (((c >= '0') && (c <= '9')) || (c == '.') || (c == '-')) // kontrola ci je znak cislo, alebo desatinna bodka
                  {
                    str[por] = c; // ak ano tak znak zapiseme do pola
                    por++;
                  }
               }
          }
         if (c == '\n') // na konci riadku sa zvysi premenna
          {
            i++;
            flag = 1;
            if ((c2 > ps) || (c1 > ps)) // a ak zistime ze argumenty prikazoveho riadku nesedia, tak vyhodime chybu
              {
                fprintf(stderr,"%s\n",ERRMSG[ERROVER]);
                return EXIT_FAILURE;
              }
            ps = 0; // na konci riadku sa vynuluje pocitadlo pocet slov v riadku
            str[por] = ' '; // ak ano tak znak zapiseme do pola
            por++;
          }
        if (c == EOF) // ak sme uz na konci tabulky
          {
            if ((r2 >= i) || (r1 >= i)) // a ak zistime ze argumenty prikazoveho riadku nesedia, tak vyhodime chybu
              {
                fprintf(stderr,"%s\n",ERRMSG[ERROVER]);
                return EXIT_FAILURE;
              }
          }
        }
        str[por] = '\0'; // na koniec pola vzdy treba dopisat znak ukoncovacej nuly
        if (strcmp("select",operacia) != 0)
        {
          vypocet(operacia,str); // zavola sa funkcia pre vypocty
        }
        return EXIT_SUCCESS;
    }


int main(int argc, char *argv[])
  {
    if (argc <= 1) // pocet argumentov je najviac jedna , teda len nazov programu
      {
        fprintf(stderr,"%s",ERRMSG[ERRNOARG]);
        return EXIT_FAILURE;
      }
    if (strcmp("--help",argv[1]) == 0) // jeden argument --help
      {
        if (argc == 2) // ak je to jediny argument , zavola sa help
          {
            Help();
            return EXIT_SUCCESS;
          }
        else // inak sa zavola chyba
          {
            fprintf(stderr,"%s",ERRMSG[ERRARG]);
            return EXIT_FAILURE;
          }
      }

    if ((strcmp("select",argv[1]) == 0) ||  // ak je operacia jedna zo zvolenych operacii
       (strcmp("min",argv[1]) == 0) ||
       (strcmp("max",argv[1]) == 0) ||
       (strcmp("sum",argv[1]) == 0) ||
       (strcmp("avg",argv[1]) == 0))
      {
        if ((strcmp("row",argv[2]) == 0) || (strcmp("col",argv[2]) == 0))
            // ak je treti argument row alebo col (su v podmienke spolu kvoli setreniu riadkov)
          {
            if (argc == 4) // opytam sa ci ma funkcia 4 argumenty
              {
                if (strtod(argv[3],NULL) > 0) // ak je 4. argument cislo >0
                  {
                    if (strcmp("row",argv[2]) == 0) // ak je to row ,vypis row
                      {
                        rows(argv[1],strtod(argv[3],NULL),strtod(argv[3],NULL));
                        return EXIT_SUCCESS;
                      }
                    else if (strcmp("col",argv[2]) == 0) // ak je to col vypis col
                      {
                        cols(argv[1],strtod(argv[3],NULL),strtod(argv[3],NULL));
                        return EXIT_SUCCESS;
                      }
                  }
                else // inak zavolaj chybu
                  {
                    fprintf(stderr,"%s",ERRMSG[ERRSLCT]);
                    return EXIT_FAILURE;
                  }
              }
            else // inak zavolaj chybu
              {
                fprintf(stderr,"%s",ERRMSG[ERRARG]);
                return EXIT_FAILURE;
              }
          }
        else if ((strcmp("rows",argv[2]) == 0) || (strcmp("cols",argv[2]) == 0))
          // ak je argument vyber buniek rows alebo cols
          {
            if (argc == 5) // ak je 5 argumentov
              {
                if ((strtod(argv[3],NULL) > 0) && (strtod(argv[4],NULL) >= strtod(argv[3],NULL)))
                       // ak je 2. argument vacsi ako 0 a 3. je vacsi alebo rovny ako 2.
                  {
                    if (strcmp("rows",argv[2]) == 0) // ak je to rows zavolaj rows
                      {
                        rows(argv[1],strtod(argv[3],NULL),strtod(argv[4],NULL));
                        return EXIT_SUCCESS;
                      }
                    else if (strcmp("cols",argv[2]) == 0) // ak je to cols zavolaj cols
                      {
                        cols(argv[1],strtod(argv[3],NULL),strtod(argv[4],NULL));
                        return EXIT_SUCCESS;
                      }
                  }
                else // inak zavolaj chybu
                  {
                    fprintf(stderr,"%s",ERRMSG[ERRSLCT]);
                    return EXIT_FAILURE;
                  }
              }
            else // inak zavolaj chybu
              {
                fprintf(stderr,"%s",ERRMSG[ERRARG]);
                return EXIT_FAILURE;
              }
          }
      else  if (strcmp("range",argv[2]) == 0) // ak je prvy argument range
          {
            if (argc == 7) // a ak mame 7 argumentov
              {
                if (((strtod(argv[3],NULL) > 0) && (strtod(argv[4],NULL) >= strtod(argv[3],NULL))) &&
                   ((strtod(argv[5],NULL) > 0) && (strtod(argv[6],NULL) >= strtod(argv[5],NULL))))
                    // ak je 2. a 4. argument >0 a ak 3.>2. a 5.>6.
                     {
                       range(argv[1],strtod(argv[3],NULL),strtod(argv[4],NULL),strtod(argv[5],NULL),strtod(argv[6],NULL));
                          // zavola sa funkcia range
                       return EXIT_SUCCESS;
                     }
                else // inak chyba
                  {
                    fprintf(stderr,"%s",ERRMSG[ERRSLCT]);
                    return EXIT_FAILURE;
                  }
              }
            else // inak sa zavola chyba
              {
                fprintf(stderr,"%s",ERRMSG[ERRARG]);
                return EXIT_FAILURE;
              }
          }

    else // inak sa zavola chyba
      {
        fprintf(stderr,"%s",ERRMSG[ERRSLCT]);
        return EXIT_FAILURE;
      }
      }
  else // inak sa zavola chyba
    {
      fprintf(stderr,"%s",ERRMSG[ERROPRTN]);
      return EXIT_FAILURE;
    }
  }


