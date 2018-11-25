/*
 * Subor:   proj3.c
 * Datum:   2014/12/07
 * Autor:   Maros Vasilisin, xvasil02@stud.fit.vutbr.cz, 1BIT
 * Projekt: Prechod bludiskom, 3. projekt IZP
 * Popis:   Program hlada v zadanom bludisku cestu von.
 *          Bludisko je ulozene v textovom subore vo forme matice.
 *          Program overuje ci je dane bludisko spravne zadane a
 *          vypisuje cestu von z bludiska na standardny vystup.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


enum errors // zoznam vsetkych chyb, ktore sa mozu vyskytnut za behu programu
  {
    ERRARG,     // chybny pocet argumentov, ci uz je to viac alebo menej ako ma byt
    ERRFILE,    // ak sa nepodari otvorit subor
    ERRCLOSE,   // nepodarilo sa uzavriet subor
    ERRDATA,    // nieco je nespravne v subore
    ERRALOC,    // nevysla alokacia pamate
    ERRBORDER   // zle zadana hranica v isborder()
  };

const char *ERRMSG[] = // samotne chybove hlasenia pre jednotlive chyby.. budu sa vypisovat na standardny chybovy vystup stderr
  {
  [ERRARG] =     "Chyba: Nespravne zadane argumenty.\n",
  [ERRFILE] =    "Chyba: Nepodarilo sa otvorit subor.\n",
  [ERRCLOSE] =   "Chyba: Nepodarilo sa uzavriet subor.\n",
  [ERRDATA] =    "Chyba: Data v subore su nespravne.\n",
  [ERRALOC] =    "Chyba: Chyba pri alokacii pamate.\n",
  [ERRBORDER] =  "Chyba: Zle zadane parametre funkcie isborder().\n"
  };


void Help() // funkcia pre vypis napovedy, nema navratovu hodnotu, vypis napovedu ak zadame argument --help
  {
    printf("Toto je napoveda k programu Prechod bludiskom.\n"
    "Pri vstupe ocakava textovy subor, ktory obsahuje spravnu\n"
    "definiciu bludiska. Pozadovanu operaciu specifikuje pouzivatel.\n"
    "Program sa spusta v podobe:\n"
    "   ./proj3 --help\n"
    " -zobrazi napovedu k programu a skonci\n"
    "   ./proj3 --test subor.txt\n"
    " -skontroluje ci dany subor obsahuje spravnu mapu bludiska\n"
    "   ./proj3 --rpath(lpath) R C bludisko.txt\n"
    " -hlada prechod bludiskom na vstupe na riadku R a stlpci C\n"
    " -prechod hlada pomocou pravidla pravej(lavej) ruky\n"
    "Textovy subor ma na prvom riadku pocet riadkov a stlpcov bludiska\n"
    "a na ostatnych riadkoch su v obdlznikovej matici\n"
    "specifikovane hrany kazdeho policka.\n");

  }

  typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
  } Map;

 /*
  * funkcia na alokovanie miesta v pamati pre nase bludisko
  */

 unsigned char* alloc2D(int rows, int cols, Map *arr)
   {
     arr->rows = rows;
     arr->cols = cols;

     arr->cells = (unsigned char*)malloc(rows * cols * sizeof(unsigned char));

     return arr->cells;
   }
 /*
  * funkcia na vyber prvku z matice arr
  */

 unsigned char* getItem(Map * arr, int row, int col) /// funkcia pre vyber prvku z matice
   {
     return &arr->cells[arr->cols * row + col];
   }

 /*
  * kontrolna funkcia pre zobrazenie matice znakov
  */
 void displayMatrixCHAR(Map *arr) /// kontrolna funkcia pre zobrazenie matice znakov
   {
     for (int i = 0; i < arr->rows; i++)
       {
         for (int j = 0; j < arr->cols; j++)
           {
             printf("%c ", *getItem(arr, i, j));
           }
         printf("\n");
       }
   }

 /*
  * kontrolna funkcia pre zobrazenie maticce cisel
  */

 void displayMatrixINT(Map *arr) /// kontrolna funkcia pre zobrazenie matice cisel
   {
     for (int i = 0; i < arr->rows; i++)
       {
         for (int j = 0; j < arr->cols; j++)
           {
             printf("%d ", *getItem(arr, i, j));
           }
         printf("\n");
       }
   }

 /* funkcia urcuje ci je na danej hranici stena alebo prechod
  * true = stena, false = prechod
  */
 bool isborder(Map *map, int r, int c, int border)
   {
     return ((*getItem(map,r-1,c-1) - '0') & border);
   }

 /*
  * funkcia kontroluje ci su susedne steny spravne zadane v matici
  */
 bool BorderControl(Map * map)
   {
     bool Left;
     bool Right;
     bool Up;
     bool Down;
     bool result;

     int row = 1;
     int col = 1;

     int cols = map->cols; ///pre skratenie zapisu som si vytvori kratsie premenne
     int max = ((map->rows)*(map->cols));

     for (int i = 0; i < max; i++) /// cyklus prechadza celu maticu
       {
         result = true;
         /// podmienka pre porovnanie pravych a lavych sikmych stien
         if ((col >= 2) && (col <= cols))
           {
              Left = isborder(map,row,col,1);
              Right = isborder(map,row,col-1,2);
              if (Left != Right)
                 result = false;
           }
       /// podmienka pre porovnanie hornych/dolnych stien
         if (((row % 2 == 1) && (col % 2 == 1)) || ((row % 2 == 0) && (col % 2 == 0)))
           {
             if (row != 1)
               {
                 Up = isborder(map,row,col,4);
                 Down = isborder(map,row-1,col,4);
                 if (Up != Down)
                     result = false;
               }
           }
        /// podmienka pre upravenie indexov podla daneho riadku, stlpca
         if (i%cols == (cols -1))
           {
             row++;
             col = 0;
           }
         col++;
         if (result == false)  /// podmienka ze ak narazi na chybu nech skonci cyklus
             i = max;
       }
     return result;
   }

 /*
  * kontroluje spravnost vstupneho suboru
  */

 int FileControl(char *file,int argument)
   {
     char array[2*sizeof(unsigned int)]; /// tu sa uklada prvy riadok
     FILE *fp; /// subor bludiste.txt

     int number = 0;
     char *token;
     int p = 0;

     int rows,cols; /// tu sa ulozia hodnoty z prveho riadku
     int rowcount,colcount = 0;

     fp = fopen(file, "r"); /// otvorenie suboru
     if (fp == NULL)
       {
         fprintf(stderr,"%s",ERRMSG[ERRFILE]);
         return EXIT_FAILURE;
       }

     int c;
     int i = 0; /// index pola

     while ((c = getc(fp)) != '\n') /// nacitanie prveho riadku po znaku do pola array[]
       {
         if (!(((c >= '0') && (c <= '9')) || (isspace(c))))
           {
             if (fclose(fp) != 0)
                 fprintf(stderr,"%s",ERRMSG[ERRCLOSE]);
             if (argument == 2)
                 printf("Invalid\n");
             return EXIT_FAILURE;
           }
         array[i] = c; /// zapis do pola
         i++;
       }
      array[i] = '\0'; /// na koniec pola pridam ten znak

      token = strtok(array, " "); /// rozdelenie na tokeny

      while (token != NULL)
        {
          number = atoi(token); /// prevod na cislo
          p++;
          if (p == 1)
              rows = number; /// prve cislo je rows
          else if (p == 2)
              cols = number; /// druhe cislo je cols
          else if (p > 2)
            {
              if (fclose(fp) != 0)
                  fprintf(stderr,"%s",ERRMSG[ERRCLOSE]);
              if (argument == 2)
                  printf("Invalid\n");
              return EXIT_FAILURE;
            }

          token = strtok(NULL, " ");
        }
      if (p < 2)
            {
              if (fclose(fp) != 0)
                  fprintf(stderr,"%s",ERRMSG[ERRCLOSE]);
              if (argument == 2)
                  printf("Invalid\n");
              return EXIT_FAILURE;
            }

        /// dalej zvysok suboru nacitam do matice, ktoru si naalokujem... ukladam len ciselne znaky
      Map arr;
      alloc2D(rows, cols, &arr);
      i = 0;

      while ((c = getc(fp)) != EOF) /// citanie az do konca suboru
        {
          if (!(((c >= '0') && (c <= '7')) || (isspace(c))))
            {
              if (fclose(fp) != 0)
                  fprintf(stderr,"%s",ERRMSG[ERRCLOSE]);
              if (argument == 2)
                  printf("Invalid\n");
              free(arr.cells);
              return EXIT_FAILURE;
            }
          if (c == '\n') /// kontrola spravneho poctu buniek
            {
              rowcount++;
              if (colcount == cols)
                  colcount = 0;
              else
                {
                  if (argument == 2)
                      printf("Invalid\n");
                  free(arr.cells);
                  return EXIT_FAILURE;
                }
            }
          if ((c >= '0') && (c <= '7')) /// samotny zapis do matice
            {
              arr.cells[i] = c;
              i++;
              colcount++;
            }
        }

      if (fclose(fp) != 0) /// uzavretie suboru
        {
          fprintf(stderr,"%s",ERRMSG[ERRCLOSE]);
          return EXIT_FAILURE;
        }
        /// vytvorenie miesta v pamati, na ktore posleme nase pole2D[]

      if (arr.cells == NULL) /// kontrola alokacie
        {
          fprintf(stderr,"%s",ERRMSG[ERRALOC]);
          free(arr.cells);
          return EXIT_FAILURE;
        }

      /// tieto 2 podmienky volaju funkciu na kontrolu hranic v bludisku , je popisana vyssie
      if (BorderControl(&arr) == true)
        {
          if (argument == 2)
            {
              printf("Valid\n");
            }
          free(arr.cells);
          return EXIT_SUCCESS;
        }
      else if (BorderControl(&arr) == false)
        {
          if (argument == 2)
            {
              printf("Invalid\n");
            }
          free(arr.cells);
          return EXIT_FAILURE;
        }
      free(arr.cells);
      free(arr.cells);
      return EXIT_SUCCESS;
   }

 /*
  * funkcia urcuje ktora hrana sa ma po vstupe nasledovat
  */
 int start_border(Map *map, int r, int c, int leftright)
   /* 1 = nasleduje lava stena v trojuholniku A
    * 2 = nasleduje prava stena v trojuholniku A
    * 3 = nasleduje dolna stena v trojuholniku A
    * 4 = nasleduje lava stena v trojuholniku V
    * 5 = nasleduje prava stena v trojuholniku V
    * 6 = nasleduje horna stena v trojuholniku V
    */
   {
     int start = 0;
      ///osetrenie rohovych buniek
       if ((c == 1) && (r == 1)) ///lavy horny okraj
         {
           if ((isborder(map,r,c,1) == false) && (isborder(map,r,c,4) == true))
             {
               if (leftright == 2) start = 5;
               if (leftright == 1)  start = 6;
             }
           else if ((isborder(map,r,c,1) == true) && (isborder(map,r,c,4) == false))
             {
               if (leftright == 2) start = 4;
               if (leftright == 1)  start = 5;
             }
           else if ((isborder(map,r,c,1) == true) && (isborder(map,r,c,4) == true))
             {
               start = 0;
             }
         }
       else if ((c == map->cols) && (r == 1) && (c % 2 == 1)) ///pravy horny okraj
         {
           if ((isborder(map,r,c,2) == false) && (isborder(map,r,c,4) == true))
             {
               if (leftright == 2) start = 6;
               if (leftright == 1)  start = 4;
             }
           else if ((isborder(map,r,c,2) == true) && (isborder(map,r,c,4) == false))
             {
               if (leftright == 2) start = 4;
               if (leftright == 1)  start = 5;
             }
           else if ((isborder(map,r,c,2) == true) && (isborder(map,r,c,4) == true))
             {
               start = 0;
             }
         }
       else if ((c == 1) && (r == map->rows) && (r % 2 == 0)) ///lavy dolny okraj
         {
           if ((isborder(map,r,c,1) == false) && (isborder(map,r,c,4) == true))
             {
               if (leftright == 2) start = 3;
               if (leftright == 1)  start = 2;
             }
           else if ((isborder(map,r,c,1) == true) && (isborder(map,r,c,4) == false))
             {
               if (leftright == 2) start = 2;
               if (leftright == 1)  start = 1;
             }
           else if ((isborder(map,r,c,1) == true) && (isborder(map,r,c,4) == true))
             {
               start = 0;
             }
         }
       else if ((c == map->cols) && (r == map->rows)) ///pravy dolny okraj
         {
           if (((r % 2 == 1) && (c % 2 == 0)) || ((r % 2 == 0) && (c % 2 == 1)))
             {
               if ((isborder(map,r,c,2) == false) && (isborder(map,r,c,4) == true))
                 {
                   if (leftright == 2) start = 1;
                   if (leftright == 1)  start = 3;
                 }
               else if ((isborder(map,r,c,2) == true) && (isborder(map,r,c,4) == false))
                 {
                   if (leftright == 2) start = 2;
                   if (leftright == 1)  start = 1;
                 }
               else if ((isborder(map,r,c,2) == true) && (isborder(map,r,c,4) == true))
                 {
                   start = 0;
                 }
             }
         }
   /// osetrenie ostatnych buniek
       else if (c == 1) /// pri vstupe zlava
         {
           if (r % 2 == 1) /// na neparnych riadkoch
             {
               if (leftright == 2) start = 5;
               if (leftright == 1)  start = 6;
               if (isborder(map,r,c,1) == true)
                 {
                   start = 0;
                 }
             }
           else if (r % 2 == 0) /// na parnych riadkoch
             {
               if (leftright == 2) start = 3;
               if (leftright == 1)  start = 2;
               if (isborder(map,r,c,1) == true)
                 {
                   start = 0;
                 }
             }
         }
       else if ((r == 1) && (c % 2 == 1)) /// pri vstupe zhora
         {
           if (leftright == 2) start = 4;
           if (leftright == 1)  start = 5;
           if (isborder(map,r,c,4) == true)
             {
               start = 0;
             }
         }
       else if ((r == 1) && (c % 2 == 0))
         {
           start = 0;
         }
       else if (r == map->rows) /// pri vstupe zdola
         {
           if ((map->rows % 2 == 1) && (map->cols % 2 == 0))
             {
               if (leftright == 2) start = 2;
               if (leftright == 1)  start = 1;
               if (isborder(map,r,c,4) == true)
                 {
                   start = 0;
                 }
             }
           if ((map->rows % 2 == 0) && (map->cols % 2 == 1))
             {
               if (leftright == 2) start = 2;
               if (leftright == 1)  start = 1;
               if (isborder(map,r,c,4) == true)
                 {
                   start = 0;
                 }
             }
           if (((map->rows % 2 == 0) && (map->cols % 2 == 0)) || ((map->rows % 2 == 1) && (map->cols % 2 == 1)))
             {
               start = 0;
             }
          }
       else if (c == map->cols) /// pri vstupe z pravej strany
         {
           if (((r % 2 == 1) && (c % 2 == 1)) || ((r % 2 == 0) && (c % 2 == 0))) /// pri bunke s hornou hranou
             {
               if (leftright == 2) start = 6;
               if (leftright == 1)  start = 4;
               if (isborder(map,r,c,2) == true)
                 {
                   start = 0;
                 }
             }
           else if (((r % 2 == 1) && (c % 2 == 0)) || ((r % 2 == 0) && (c % 2 == 1))) /// pri bunke s dolnou hranou
             {
               if (leftright == 2) start = 1;
               if (leftright == 1)  start = 3;
               if (isborder(map,r,c,2) == true)
                 {
                   start = 0;
                 }
             }
         }
       else
         {
           start = 0;
         }
     return start;
   }

 /*
  * funkcia pre hladanie cesty podla pravidla lavej / pravej ruky
  */

int PathFinder(char *path, char*file, int row, int col)
  {
    FILE *fp;

    fp = fopen(file,"r"); /// otvorenie suboru
    if (fp == NULL)
      {
        fprintf(stderr,"%s",ERRMSG[ERRFILE]);
        return EXIT_FAILURE;
      }
    int rows = 0;
    int cols = 0;
    int token;

    Map arr;
    alloc2D(rows,cols,&arr); /// alokacia pamate

    int i = 0;
    int j = 0;

    while (i < 2) /// prve dve cisla ulozene do rows a cols
      {
        fscanf(fp,"%d",&token);
        if (i == 0)
          arr.rows = token;
        if (i == 1)
          arr.cols = token;
        i++;
      }

    for (i = 0; i < arr.rows; i++) /// ostatne bunky ulozene do cells
      {
        for (j = 0; j < arr.cols; j++)
          {
            fscanf(fp,"%d",&token);
            arr.cells[arr.cols * i + j] = (unsigned char)token;
          }
      }

    if (fclose(fp) != 0) /// uzavretie suboru
      {
        fprintf(stderr,"%s",ERRMSG[ERRCLOSE]);
        free(arr.cells);
        return EXIT_FAILURE;
      }

    if ((row != 1) && (col != 1) && (row != arr.rows) && (col != arr.cols))
      {
        fprintf(stderr,"%s",ERRMSG[ERRARG]);
        return EXIT_FAILURE;
      }


    int move = 0;
    if (strcmp("--rpath",path) == 0)   /// vsetky mozne pravidla pre rpath
      {
        move = start_border(&arr,row,col,2);
        if (move == 0)
          {
            fprintf(stderr,"%s",ERRMSG[ERRARG]);
            return EXIT_FAILURE;
          }
        while ((row >= 1) && (row <= arr.rows) && (col >= 1) && (col <= arr.cols))
          {
            printf("%d,%d\n",row,col);
            if (move == 1)
              {
                if (!isborder(&arr,row,col,1))
                  {
                    col--;
                    move = 6;
                  }
                else if (!isborder(&arr,row,col,4))
                  {
                    row++;
                    move = 4;
                  }
                else
                  {
                    col++;
                    move = 5;
                  }
              }
            else if (move == 2)
              {
                if (!isborder(&arr,row,col,2))
                  {
                    col++;
                    move = 5;
                  }
                else if (!isborder(&arr,row,col,1))
                  {
                    col--;
                    move = 6;
                  }
                else
                  {
                    row++;
                    move = 4;
                  }
              }
            else if (move == 3)
              {
                if (!isborder(&arr,row,col,4))
                  {
                    row++;
                    move = 4;
                  }
                else if (!isborder(&arr,row,col,2))
                  {
                    col++;
                    move = 5;
                  }
                else
                  {
                    col--;
                    move = 6;
                  }
              }
            else if (move == 4)
              {
                if (!isborder(&arr,row,col,1))
                  {
                    col--;
                    move = 1;
                  }
                else if (!isborder(&arr,row,col,2))
                  {
                    col++;
                    move = 3;
                  }
                else
                  {
                    row--;
                    move = 2;
                  }
              }
            else if (move == 5)
              {
                if (!isborder(&arr,row,col,2))
                  {
                    col++;
                    move = 3;
                  }
                else if (!isborder(&arr,row,col,4))
                  {
                    row--;
                    move = 2;
                  }
                else
                  {
                    col--;
                    move = 1;
                  }
              }
            else if (move == 6)
              {
                if (!isborder(&arr,row,col,4))
                  {
                    row--;
                    move = 2;
                  }
                else if (!isborder(&arr,row,col,1))
                  {
                    col--;
                    move = 1;
                  }
                else
                  {
                    col++;
                    move = 3;
                  }
              }
            else if (move == 0)
              {
                break;
              }
          }
      }

   if (strcmp("--lpath",path) == 0) /// vsetky pravidla pre lpath
      {
        move = start_border(&arr,row,col,1);
        while ((row >= 1) && (row <= arr.rows) && (col >= 1) && (col <= arr.cols))
          {
            printf("%d,%d\n",row,col);
            if (move == 1)
              {
                if (!isborder(&arr,row,col,1))
                  {
                    col--;
                    move = 4;
                  }
                else if (!isborder(&arr,row,col,2))
                  {
                    col++;
                    move = 6;
                  }
                else
                  {
                    row++;
                    move = 5;
                  }
              }
            else if (move == 2)
              {
                if (!isborder(&arr,row,col,2))
                  {
                    col++;
                    move = 6;
                  }
                else if (!isborder(&arr,row,col,4))
                  {
                    row++;
                    move = 5;
                  }
                else
                  {
                    col--;
                    move = 4;
                  }
              }
            else if (move == 3)
              {
                if (!isborder(&arr,row,col,4))
                  {
                    row++;
                    move = 5;
                  }
                else if (!isborder(&arr,row,col,1))
                  {
                    col--;
                    move = 4;
                  }
                else
                  {
                    col++;
                    move = 6;
                  }
              }
            else if (move == 4)
              {
                if (!isborder(&arr,row,col,1))
                  {
                    col--;
                    move = 3;
                  }
                else if (!isborder(&arr,row,col,4))
                  {
                    row--;
                    move = 1;
                  }
                else
                  {
                    col++;
                    move = 2;
                  }
              }
            else if (move == 5)
              {
                if (!isborder(&arr,row,col,2))
                  {
                    col++;
                    move = 2;
                  }
                else if (!isborder(&arr,row,col,1))
                  {
                    col--;
                    move = 3;
                  }
                else
                  {
                    row--;
                    move = 1;
                  }
              }
            else if (move == 6)
              {
                if (!isborder(&arr,row,col,4))
                  {
                    row--;
                    move = 1;
                  }
                else if (!isborder(&arr,row,col,2))
                  {
                    col++;
                    move = 2;
                  }
                else
                  {
                    col--;
                    move = 3;
                  }
              }
            else if (move == 0)
              {
                break;
              }
          }
      }
    return EXIT_SUCCESS;
    }


int ArgControl(int argc, char *argv[]) /// funkcia pre kontrolu argumentov prik. riadku
  {
    int result = 0;
    char *ptr1,*ptr2;

    if (argc < 2)
      result = 4;

    else if ((strcmp("--help",argv[1]) == 0) && argc==2)
      {
        result =  1;
      }
    else if ((strcmp("--test",argv[1]) == 0) && (argc == 3))
      {
        result =  2;
      }
    else if (((strcmp("--rpath",argv[1]) == 0) || (strcmp("--lpath",argv[1]) == 0)) &&
        (strtol(argv[2],&ptr1,10) > 0) && (strtol(argv[3],&ptr2,10) > 0) &&
        (argc == 5) && (strcmp("\0",ptr1) == 0) && (strcmp("\0",ptr2) == 0))
      {
        result =  3;
      }
    else
      {
        result =  4;
      }
    return result;
  }

int main(int argc, char *argv[])
  {
    if (ArgControl(argc,argv) == 1)
      {
        Help();
        return EXIT_SUCCESS;
      }
    else if (ArgControl(argc,argv) == 2)
      {
        FileControl(argv[2],2);
        return EXIT_SUCCESS;
      }
    else if (ArgControl(argc,argv) == 3)
      {
        if (FileControl(argv[4],3) != 0)
          return EXIT_FAILURE;
        PathFinder(argv[1],argv[4],atoi(argv[2]),atoi(argv[3]));
        return EXIT_SUCCESS;
      }
    else if (ArgControl(argc,argv) == 4)
      {
        fprintf(stderr,"%s",ERRMSG[ERRARG]);
        return EXIT_FAILURE;
      }
  }

