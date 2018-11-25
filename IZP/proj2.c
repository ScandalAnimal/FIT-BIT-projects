/*
 * Subor:   proj2.c
 * Datum:   2014/11/30
 * Autor:   Maros Vasilisin, xvasil02@stud.fit.vutbr.cz, 1BIT
 * Projekt: Iteracne vypocty, 2. projekt IZP
 * Popis:   Projekt vypocita vzdialenost a vysku meraneho objektu pomocou udajov
 *          zo senzorov natocenia meracieho pristroja.
 *          V projekte je vytvorena funkcia na vypocet tangensu pomocou Taylorovho polynomu,
 *          a tiez funkcia pre vypocet tangensu pomocou zretazeneho zlomku.
 *          Program porovnava oba vysledky s funkciou tan z math.h.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <ctype.h>

#define PRESNOST 1.0e-10
#define VYSKA 1.5
#define ITERACIE 13

void Help() // funkcia vypise napovedu a skonci program, nema ziadnu navratovu hodnotu
  {
    printf("Napoveda k programu Iteracne vypocty.\n"
    "Vypracoval Maros Vasilisin, xvasil02, 1BIT\n"
    "Program vypocita vzdialenost a vysku meraneho objektu\n"
    "pomocou udajov zo senzorov natocenia meracieho pristroja.\n"
    "Program sa spusta v podobe:\n"
    "   ./proj2 --help   (zobrazi napovedu k programu a skonci)\n"
    "   ./proj2 --tan A N M\n"
    "   ./proj2 [-c X] -m A [B]\n"
    "*Argument --tan porovna presnosti vypoctov tangens uhlu A v radianoch\n"
    "medzi volanim tan z matematickej kniznice, vypoctu Taylorovho polynomu\n"
    "a zretazeneho zlomku. Argumenty M a N udavaju, v ktorych iteraciach\n"
    "ma porovnanie prebiehat. 0 < N <= M < 14\n"
    "Vypis bude: poradie iteracie, funkcia tangens z matematickej kniznice,\n"
    "tangens pomocou Taylorovho polynomu, absolutna chyba Taylorovho polynomu,\n"
    "tangens pomocou zretazenych zlomkov, absolutna chyba zretazenych zlomkov.\n"
    "*Argument -m vypocita a vzdialenosti.\n"
    "Program vypocita a vypise vzdialenost meraneho objektu pomocou uhla A.\n"
    "0 < A <= 1.4 < pi/2  (v radianoch)\n"
    "Ak je zadany, pomocou uhla B v radianoch urcime vysku meraneho objektu.\n"
    "0 < B <= 1.4 < pi/2\n"
    "Argument -c oznacuje vysku meracieho pristroja, argument je volitelny.\n"
    "(implicitne je urceny na 1.5). Vyska je dana ako X (0 < X <= 100).\n");
  }

 enum errors // zoznam vsetkych chyb, ktore sa mozu vyskytnut za behu programu
  {
    ERRARG,     // ak je argument nespravne zadany, napr. skomolene slovo alebo ak tam nie je cislo
    ERRNOARG,   // ziadne argumenty - zavola sa ak nezadame do prikazoveho riadku nic
  };

const char *ERRMSG[] = // samotne chybove hlasenia pre jednotlive chyby.. budu sa vypisovat na standardny chybovy vystup stderr
  {
    [ERRARG] =   "Chyba: Nespravne zadane argumenty prikazoveho riadku.\n",
    [ERRNOARG] = "Chyba: Nezadali ste ziadny argument.\n",
  };

/* funkcia pre vypocet tangensu nejakeho uhla pomocou metody Taylorovho polynomu
 * ma dva parametre: uhol v rozsahu [0,1.4] radianov a pocet clenov polynomu, ktory je nastaveny na max 13 podla zadania
 * jednotlive koeficienty citatela a menovatela su ulozene v poliach
 * funkcia vracia hodnotu tangensu pre dany pocet clenov polynomu
 */
double taylor_tan(double x, unsigned int n)
  {
    double citatel[] = {1,1,2,17,62,1382,21844,929569,6404582,443861162,18888466084,113927491862,58870668456604};
    double menovatel[] = {1,3,15,315,2835,155925,6081075,638512875,10854718875,1856156927625,194896477400625,49308808782358125,3698160658676859375};
    double vysledok = 0; // premenna vysledok, kde sa uklada priebezny sucet
    double exp = x; // premenna exponent, je inicializovana na x, co je exponent pre 1. clen polynomu
    unsigned int i; // premenna pre cyklus for

    for (i = 0; i < n; i++)
      {
        vysledok += ((citatel[i]*exp) / menovatel[i]); // vzorec pre kazdy clen Taylorovho polynomu
        exp = exp*x*x; // zvysovanie exponentu pre x
      }
    return vysledok;
  }

/* funkcia pre vypocet tangensu nejakeho uhla pomocou metody zretazeneho zlomku
 * ma dva parametre: uhol v rozsahu [0,1.4] radianov a pocet zanoreni zlomku
 * pocet zanoreni pre --tan je nastaveny na max 13 podla zadania a pre -m je odvodeny podla presnosti na 10 miest
 * funkcia vracia hodnotu tangensu pre dany pocet zanoreni
 */
double cfrac_tan(double x, unsigned int n)
  {
    double vysledok = 0.0; // premenna vysledok uklada priebezny vysledok funkcie
    double citatel = x*x; // citatel je vzdy x^2 okrem najvyssieho citatela, kedy je len x
    int cislo = (2*n)-1; // toto je cislo v menovateli, ktore zavisi od poctu zanoreni zlomku n

    while (cislo > 1) // cyklus, ktory sa opakuje pre vsetky urovne zanorenia okrem najvyssej
      {
        vysledok = citatel/(cislo - vysledok); // vypocet aktualneho clena
        cislo -= 2; // uprava cisla v menovateli
      }
    if (cislo == 1) // ak sme na najvyssom zanoreni tak sa prevedie iny prikaz ako pri ostatnych
      {
        vysledok = x/(1 - vysledok);
      }

    return vysledok;
  }

/* vlastna funkcia pre vypocet absolutnej odchylky, pretoze nie je povolene vyuzit funkciu fabs() z matematickej kniznice
 * funkcia len vypocita absolutnu hodnotu rozdielu dvoch cisiel typu double a podla vysledku vrati hodnotu
 */

double my_fabs(double a, double b)
  {
    double vysledok = a-b;
    if (vysledok < 0)
      {
        return -vysledok;
      }
    else
      {
        return vysledok;
      }
  }

/* funkcia pre vypocet poctu iteracii pre -m
 * porovnava sa predosle zanorenie so sucasnym a ak je rozdiel mensi ako presnost na 10 desatinnych miest tak skonci
 */

double pocet_iteracii(double x)
  {
    double predosly = 0.0; // premenna pre predosly clen
    double vysledok = 0.0; // tu sa bude ukladat vysledok
    double rozdiel;
    double presnost = PRESNOST; // presnost, ktoru chceme dosiahnut
    int n = 1;

    do
      {
        predosly = vysledok;
        vysledok = cfrac_tan(x,n); // zavola sa funkcia vypoctu tangensu pomocou zretazeneho zlomku
        rozdiel = my_fabs(predosly,vysledok); // porovna sa s predoslym clenom
        n++;
      }
    while (rozdiel >= presnost);
    return n;
  }

/* funkcia pre vypis --tan , potrebuje 3 parametre a to uhol a rozsah iteracii, ktore chceme vypisat
 * jednotlive cisla vypisuje ako double v poradi I M T TE C CE
 * I je pocet iteracii iteracneho vypoctu, M je vysledok funkcie tan z matematickej kniznice,
 * T je vysledok z funkcie taylor_tan, TE je absolutna chyba medzi taylor_tan a tan z math.h
 * C je vysledok funkcie cfrac_tan a CE je absolutna chyba medzi cfrac_tan a tan z math.h
 * vypisuje vsetky hodnoty na jeden riadok
 */

int my_tan(double A, int M, int N)
  {
    for (int i = M; i <= N; i++)
      {
        printf("%d %e %e %e %e %e\n", i, tan(A), taylor_tan(A,i), my_fabs(tan(A),taylor_tan(A,i)),
                cfrac_tan(A,i), my_fabs(tan(A),cfrac_tan(A,i)));
      }
    return EXIT_SUCCESS;
  }

/* funkcia pre -m, ak mame zadany len uhol alfa , vypocita vzdialenost pomocou tangensu z cfrac_tan
 * a vypise v podobe cisla double na 10 desatinnych miest
 */

void my_merA(double A, double X)
  {
    double vzdialenost;
    vzdialenost = X / cfrac_tan(A,pocet_iteracii(A)); // vypocet vzdialenosti podla vzorca pre tangens v trojuholniku
    printf("%.10e\n", vzdialenost);
  }

/* funkcia pre -m, ak mame zadany uhol alfa aj uhol beta, vypocita vzdialenost aj vysku pomocou tangensu z cfrac_tan
 * a vypise v podobe cisla double na 10 desatinnych miest, kazdy udaj na samostatny riadok
 */

void my_merAB(double A, double B, double X)
 {
    double vzdialenost, vyska;
    vzdialenost = X / cfrac_tan(A,pocet_iteracii(A)); // vypocet vzdialenosti podla vzorca pre tangens v trojuholniku
    vyska = vzdialenost * cfrac_tan(B,pocet_iteracii(B)); // vypocet vysky podla vzorca pre tangens v trojuholniku
    vyska += X; // pripocitanie vysky meracieho pristoja podla obrazku
    printf("%.10e\n%.10e\n", vzdialenost, vyska);
 }


/* vo funkcii kontrola argumentov sa kontroluje ci su spravne argumenty, ak ano zavola sa spravna funkcia
 * a ak nie vypise sa vhodne chybove hlasenie
 * kontroluje sa vzdy ci je spravny pocet argumentov, ci su v spravnom formate a rozsahu
 */

int kontrola_arg(int argc, char* argv[])
  {
    char *ptr1, *ptr2, *ptr3; // pointery na adresy, kde sa budu ukladat hodnoty pre kontrolu argumentov, ci neobsahuju pismena

    if (argc < 2)
      {
        fprintf(stderr,"%s",ERRMSG[ERRNOARG]);
        return EXIT_FAILURE;
      }

    if (strcmp("--help",argv[1]) == 0)
      {
        if (argc == 2)
          {
            Help();
            return EXIT_SUCCESS;
          }
        else if (argc > 2)
          {
            fprintf(stderr,"%s",ERRMSG[ERRARG]);
            return EXIT_FAILURE;
          }
      }

    if ((argc == 5) &&
       (((strtod(argv[2],&ptr1)) > 0) && ((strtod(argv[2],&ptr1)) <= 1.4)) &&
       (strcmp("--tan",argv[1]) == 0) &&
       ((strtod(argv[3],&ptr2)) > 0) && ((strtod(argv[4],&ptr3)) >= (strtod(argv[3],&ptr2))) &&
       ((strtod(argv[4],&ptr3)) <= ITERACIE))
         {
            if ((strcmp("\0",ptr1) == 0) && (strcmp("\0",ptr2) == 0) && (strcmp("\0",ptr3) == 0))
              {
                my_tan(strtod(argv[2],NULL), atoi(argv[3]), atoi(argv[4]));
                return EXIT_SUCCESS;
              }
            else
              {
                fprintf(stderr,"%s",ERRMSG[ERRARG]);
                return EXIT_FAILURE;
              }
         }

    else if ((strcmp("-m",argv[1]) == 0) && ((strtod(argv[2],&ptr1)) > 0) && ((strtod(argv[2],&ptr1)) <= 1.4))
       {
         if (argc == 3)
           {
             if (strcmp("\0",ptr1) == 0)
               {
                 my_merA(strtod(argv[2],NULL),VYSKA);
                 return EXIT_SUCCESS;
               }
             else
               {
                 fprintf(stderr,"%s",ERRMSG[ERRARG]);
                 return EXIT_FAILURE;
               }
           }
         if ((argc == 4) && ((strtod(argv[3],&ptr2)) > 0) && ((strtod(argv[3],&ptr2)) <= 1.4))
           {
             if ((strcmp("\0",ptr1) == 0) && (strcmp("\0",ptr2) == 0))
               {
                 my_merAB(strtod(argv[2],NULL), strtod(argv[3],NULL),VYSKA);
                 return EXIT_SUCCESS;
               }
             else
               {
                 fprintf(stderr,"%s",ERRMSG[ERRARG]);
                 return EXIT_FAILURE;
               }
           }
       }

    else if ((strcmp("-c",argv[1]) == 0) &&
            ((strtod(argv[2],&ptr1)) > 0) && ((strtod(argv[2],&ptr1)) <= 100) &&
            (strcmp("-m",argv[3]) == 0) &&
            ((strtod(argv[4],&ptr2)) > 0) && ((strtod(argv[4],&ptr2)) <= 1.4))
              {
                if (argc == 5)
                  {
                    if ((strcmp("\0",ptr1) == 0) && (strcmp("\0",ptr2) == 0))
                      {
                        my_merA(strtod(argv[4],NULL), strtod(argv[2],NULL));
                        return EXIT_SUCCESS;
                      }
                    else
                      {
                        fprintf(stderr,"%s",ERRMSG[ERRARG]);
                        return EXIT_FAILURE;
                      }
                  }
                if ((argc == 6) && ((strtod(argv[5],&ptr3)) > 0) && ((strtod(argv[5],&ptr3)) <= 1.4))
                  {
                    if ((strcmp("\0",ptr1) == 0) && (strcmp("\0",ptr2) == 0) && (strcmp("\0",ptr3) == 0))
                      {
                        my_merAB(strtod(argv[4],NULL),strtod(argv[5],NULL),strtod(argv[2],NULL));
                        return EXIT_SUCCESS;
                      }
                    else
                      {
                        fprintf(stderr,"%s",ERRMSG[ERRARG]);
                        return EXIT_FAILURE;
                      }
                  }
               }
    else
         {
           fprintf(stderr,"%s",ERRMSG[ERRARG]);
           return EXIT_FAILURE;
         }
    return EXIT_SUCCESS;
  }

int main(int argc, char *argv[])
  {
    kontrola_arg(argc,argv);
  }
