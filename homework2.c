/* IONESCU Petru-Vlad - 315CC */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pixel
{
  unsigned char red, green, blue;
} pixel;

typedef struct Nod
{
  unsigned char tip;
  unsigned char red, green, blue;
  unsigned int dimensiune;
  struct Nod *stanga_sus, *dreapta_sus, *dreapta_jos, *stanga_jos;
} Nod;

typedef struct NodCerinta3
{
  unsigned char tip;
  unsigned char red, green, blue;
  unsigned int dimensiune;
  int start_stanga, start_sus;
  struct NodCerinta3 *stanga_sus, *dreapta_sus, *dreapta_jos, *stanga_jos;
} NodCerinta3;

Nod *creare_nod_terminal(unsigned char red, unsigned char green, unsigned char blue, unsigned int dimensiune)
{
  Nod *nod = (Nod *)malloc(sizeof(Nod));
  nod->tip = 1;
  nod->red = red;
  nod->green = green;
  nod->blue = blue;
  nod->dimensiune = dimensiune;
  nod->stanga_sus = nod->dreapta_sus = nod->dreapta_jos = nod->stanga_jos = NULL;
  return nod;
}

Nod *creare_arbore(Nod *root, pixel *imagine[], int start_stanga, int start_sus, unsigned int dimensiune, int factor)
{
  int i, j;
  unsigned long long red, green, blue, mean;
  red = green = blue = mean = 0;

  for (i = start_sus; i < start_sus + dimensiune; i++)
  {
    for (j = start_stanga; j < start_stanga + dimensiune; j++)
    {
      red += imagine[i][j].red;
      green += imagine[i][j].green;
      blue += imagine[i][j].blue;
    }
  }

  red = red / (dimensiune * dimensiune);
  green = green / (dimensiune * dimensiune);
  blue = blue / (dimensiune * dimensiune);

  for (i = start_sus; i < start_sus + dimensiune; i++)
  {
    for (j = start_stanga; j < start_stanga + dimensiune; j++)
    {
      mean += (imagine[i][j].red - red) * (imagine[i][j].red - red);
      mean += (imagine[i][j].green - green) * (imagine[i][j].green - green);
      mean += (imagine[i][j].blue - blue) * (imagine[i][j].blue - blue);
    }
  }

  mean = mean / (3 * dimensiune * dimensiune);

  if (mean <= factor)
  {
    return creare_nod_terminal(red, green, blue, dimensiune);
  }

  root = (Nod *)malloc(sizeof(Nod));
  root->tip = 0;

  // daca nu e nod terminal, apelez recursiv pentru fiecare nod copil
  root->stanga_sus = creare_arbore(root->stanga_sus, imagine, start_stanga, start_sus, dimensiune / 2, factor);
  root->dreapta_sus = creare_arbore(root->dreapta_sus, imagine, start_stanga + dimensiune / 2, start_sus, dimensiune / 2, factor);
  root->dreapta_jos = creare_arbore(root->dreapta_jos, imagine, start_stanga + dimensiune / 2, start_sus + dimensiune / 2, dimensiune / 2, factor);
  root->stanga_jos = creare_arbore(root->stanga_jos, imagine, start_stanga, start_sus + dimensiune / 2, dimensiune / 2, factor);

  return root;
}

unsigned int numar_niveluri(Nod *root)
{
  if (root == NULL)
  {
    return 0;
  }
  else
  {
    unsigned int stanga_sus = numar_niveluri(root->stanga_sus);
    unsigned int dreapta_sus = numar_niveluri(root->dreapta_sus);
    unsigned int dreapta_jos = numar_niveluri(root->dreapta_jos);
    unsigned int stanga_jos = numar_niveluri(root->stanga_jos);

    if (stanga_sus >= dreapta_sus && stanga_sus >= dreapta_jos && stanga_sus >= stanga_jos)
    {
      return stanga_sus + 1;
    }
    else if (dreapta_sus >= stanga_sus && dreapta_sus >= dreapta_jos && dreapta_sus >= stanga_jos)
    {
      return dreapta_sus + 1;
    }
    else if (dreapta_jos >= stanga_sus && dreapta_jos >= dreapta_sus && dreapta_jos >= stanga_jos)
    {
      return dreapta_jos + 1;
    }
    else
    {
      return stanga_jos + 1;
    }
  }
}

unsigned int numar_blocuri_colorate(Nod *root)
{
  if (root == NULL)
  {
    return 0;
  }
  else
  {
    if (root->tip == 1)
    {
      return 1;
    }
    else
    {
      return numar_blocuri_colorate(root->stanga_sus) + numar_blocuri_colorate(root->dreapta_sus) + numar_blocuri_colorate(root->dreapta_jos) + numar_blocuri_colorate(root->stanga_jos);
    }
  }
}

unsigned int latura_maxima(Nod *root)
{
  if (root == NULL)
  {
    return 0;
  }
  else if (root->tip == 1)
  {
    return root->dimensiune;
  }
  else
  {
    unsigned int stanga_sus = latura_maxima(root->stanga_sus);
    unsigned int dreapta_sus = latura_maxima(root->dreapta_sus);
    unsigned int dreapta_jos = latura_maxima(root->dreapta_jos);
    unsigned int stanga_jos = latura_maxima(root->stanga_jos);

    if (stanga_sus >= dreapta_sus && stanga_sus >= dreapta_jos && stanga_sus >= stanga_jos)
    {
      return stanga_sus;
    }
    else if (dreapta_sus >= stanga_sus && dreapta_sus >= dreapta_jos && dreapta_sus >= stanga_jos)
    {
      return dreapta_sus;
    }
    else if (dreapta_jos >= stanga_sus && dreapta_jos >= dreapta_sus && dreapta_jos >= stanga_jos)
    {
      return dreapta_jos;
    }
    else
    {
      return stanga_jos;
    }
  }
}

void eliberare_memorie(Nod *root)
{
  if (root == NULL)
  {
    return;
  }
  else
  {
    eliberare_memorie(root->stanga_sus);
    eliberare_memorie(root->dreapta_sus);
    eliberare_memorie(root->dreapta_jos);
    eliberare_memorie(root->stanga_jos);
    free(root);
  }
}

void afisare_nivel(Nod *root, int nivel, FILE *fout)
{
  if (root == NULL)
  {
    return;
  }
  else if (nivel == 0)
  {
    if (root->tip == 0)
    {
      fwrite(&root->tip, sizeof(unsigned char), 1, fout);
    }
    else
    {
      fwrite(&root->tip, sizeof(unsigned char), 1, fout);
      fwrite(&root->red, sizeof(unsigned char), 1, fout);
      fwrite(&root->green, sizeof(unsigned char), 1, fout);
      fwrite(&root->blue, sizeof(unsigned char), 1, fout);
    }
  }
  else if (nivel > 0)
  {
    afisare_nivel(root->stanga_sus, nivel - 1, fout);
    afisare_nivel(root->dreapta_sus, nivel - 1, fout);
    afisare_nivel(root->dreapta_jos, nivel - 1, fout);
    afisare_nivel(root->stanga_jos, nivel - 1, fout);
  }
}

void afisare_parcurgere_la_nivel(Nod *root, FILE *fout)
{
  int i;
  for (i = 0; i < numar_niveluri(root); i++)
  {
    afisare_nivel(root, i, fout);
  }
}

unsigned int numar_niveluri_cerinta3(NodCerinta3 *root)
{
  if (root == NULL)
  {
    return 0;
  }
  else
  {
    unsigned int stanga_sus = numar_niveluri_cerinta3(root->stanga_sus);
    unsigned int dreapta_sus = numar_niveluri_cerinta3(root->dreapta_sus);
    unsigned int dreapta_jos = numar_niveluri_cerinta3(root->dreapta_jos);
    unsigned int stanga_jos = numar_niveluri_cerinta3(root->stanga_jos);

    if (stanga_sus >= dreapta_sus && stanga_sus >= dreapta_jos && stanga_sus >= stanga_jos)
    {
      return stanga_sus + 1;
    }
    else if (dreapta_sus >= stanga_sus && dreapta_sus >= dreapta_jos && dreapta_sus >= stanga_jos)
    {
      return dreapta_sus + 1;
    }
    else if (dreapta_jos >= stanga_sus && dreapta_jos >= dreapta_sus && dreapta_jos >= stanga_jos)
    {
      return dreapta_jos + 1;
    }
    else
    {
      return stanga_jos + 1;
    }
  }
}

int prindere_nod(NodCerinta3 *root, unsigned char tip, int start_stanga, int start_sus, unsigned int dimensiune, FILE *fin, int nivel)
{
  if (root == NULL)
  {
    return 0;
  }
  else if (nivel == 0)
  {
    if (root->tip == 0 && root->stanga_sus == NULL)
    {
      NodCerinta3 *nou = (NodCerinta3 *)malloc(sizeof(NodCerinta3));
      nou->tip = tip;
      if (nou->tip == 1)
      {
        fread(&nou->red, sizeof(unsigned char), 1, fin);
        fread(&nou->green, sizeof(unsigned char), 1, fin);
        fread(&nou->blue, sizeof(unsigned char), 1, fin);
        nou->start_stanga = start_stanga;
        nou->start_sus = start_sus;
        nou->dimensiune = dimensiune;
      }
      nou->stanga_sus = NULL;
      nou->dreapta_sus = NULL;
      nou->dreapta_jos = NULL;
      nou->stanga_jos = NULL;
      root->stanga_sus = nou;
      return 1;
    }
    else if (root->tip == 0 && root->dreapta_sus == NULL)
    {
      NodCerinta3 *nou = (NodCerinta3 *)malloc(sizeof(NodCerinta3));
      nou->tip = tip;
      if (nou->tip == 1)
      {
        fread(&nou->red, sizeof(unsigned char), 1, fin);
        fread(&nou->green, sizeof(unsigned char), 1, fin);
        fread(&nou->blue, sizeof(unsigned char), 1, fin);
        nou->start_stanga = start_stanga + dimensiune;
        nou->start_sus = start_sus;
        nou->dimensiune = dimensiune;
      }
      nou->stanga_sus = NULL;
      nou->dreapta_sus = NULL;
      nou->dreapta_jos = NULL;
      nou->stanga_jos = NULL;
      root->dreapta_sus = nou;
      return 1;
    }
    else if (root->tip == 0 && root->dreapta_jos == NULL)
    {
      NodCerinta3 *nou = (NodCerinta3 *)malloc(sizeof(NodCerinta3));
      nou->tip = tip;
      if (nou->tip == 1)
      {
        fread(&nou->red, sizeof(unsigned char), 1, fin);
        fread(&nou->green, sizeof(unsigned char), 1, fin);
        fread(&nou->blue, sizeof(unsigned char), 1, fin);
        nou->start_stanga = start_stanga + dimensiune;
        nou->start_sus = start_sus + dimensiune;
        nou->dimensiune = dimensiune;
      }
      nou->stanga_sus = NULL;
      nou->dreapta_sus = NULL;
      nou->dreapta_jos = NULL;
      nou->stanga_jos = NULL;
      root->dreapta_jos = nou;
      return 1;
    }
    else if (root->tip == 0 && root->stanga_jos == NULL)
    {
      NodCerinta3 *nou = (NodCerinta3 *)malloc(sizeof(NodCerinta3));
      nou->tip = tip;
      if (nou->tip == 1)
      {
        fread(&nou->red, sizeof(unsigned char), 1, fin);
        fread(&nou->green, sizeof(unsigned char), 1, fin);
        fread(&nou->blue, sizeof(unsigned char), 1, fin);
        nou->start_stanga = start_stanga;
        nou->start_sus = start_sus + dimensiune;
        nou->dimensiune = dimensiune;
      }
      nou->stanga_sus = NULL;
      nou->dreapta_sus = NULL;
      nou->dreapta_jos = NULL;
      nou->stanga_jos = NULL;
      root->stanga_jos = nou;
      return 1;
    }
    return 0;
  }
  else if (nivel > 0)
  {
    if (prindere_nod(root->stanga_sus, tip, start_stanga, start_sus, dimensiune / 2, fin, nivel - 1))
    {
      return 1;
    }
    else if (prindere_nod(root->dreapta_sus, tip, start_stanga + dimensiune, start_sus, dimensiune / 2, fin, nivel - 1))
    {
      return 1;
    }
    else if (prindere_nod(root->dreapta_jos, tip, start_stanga + dimensiune, start_sus + dimensiune, dimensiune / 2, fin, nivel - 1))
    {
      return 1;
    }
    else if (prindere_nod(root->stanga_jos, tip, start_stanga, start_sus + dimensiune, dimensiune / 2, fin, nivel - 1))
    {
      return 1;
    }

    return 0;
  }
  return 0;
}

NodCerinta3 *parcurgere_la_nivel(NodCerinta3 *root, unsigned char tip, unsigned int dimensiune, FILE *fin)
{
  int numar_niveluri = numar_niveluri_cerinta3(root);
  int i;

  for (i = 0; i < numar_niveluri; i++)
  {
    if (prindere_nod(root, tip, 0, 0, dimensiune, fin, i) == 1)
    {
      break;
    }
  }
  return root;
}

void creare_imagine(NodCerinta3 *root, pixel *imagine[])
{
  if (root == NULL)
  {
    return;
  }
  else if (root->tip == 1)
  {
    int i, j;
    for (i = root->start_sus; i < root->start_sus + root->dimensiune; i++)
    {
      for (j = root->start_stanga; j < root->start_stanga + root->dimensiune; j++)
      {
        imagine[i][j].red = root->red;
        imagine[i][j].green = root->green;
        imagine[i][j].blue = root->blue;
      }
    }
  }
  else
  {
    creare_imagine(root->stanga_sus, imagine);
    creare_imagine(root->dreapta_sus, imagine);
    creare_imagine(root->dreapta_jos, imagine);
    creare_imagine(root->stanga_jos, imagine);
  }
}

void eliberare_memorie_cerinta3(NodCerinta3 *root)
{
  if (root == NULL)
  {
    return;
  }
  else if (root->tip == 0)
  {
    eliberare_memorie_cerinta3(root->stanga_sus);
    eliberare_memorie_cerinta3(root->dreapta_sus);
    eliberare_memorie_cerinta3(root->dreapta_jos);
    eliberare_memorie_cerinta3(root->stanga_jos);
  }
  free(root);
}

int main(int argc, char *argv[])
{

  if (strcmp(argv[1], "-c1") == 0)
  {
    // cerinta 1
    int factor = atoi(argv[2]);
    FILE *fin = fopen(argv[3], "rb");
    FILE *fout = fopen(argv[4], "wb");

    // citire antet
    char gunoi[4], alt_gunoi;
    int max_val, i, j;
    unsigned int m, dimensiune_imagine;
    fread(gunoi, sizeof(char), 3, fin);
    fscanf(fin, "%u %u\n%d", &m, &dimensiune_imagine, &max_val);
    fread(&alt_gunoi, sizeof(char), 1, fin);

    // alocare memorie pentru imagine
    pixel **imagine = (pixel **)malloc(dimensiune_imagine * sizeof(pixel *));
    for (i = 0; i < dimensiune_imagine; i++)
    {
      imagine[i] = (pixel *)malloc(dimensiune_imagine * sizeof(pixel));
    }

    // citire imagine
    for (i = 0; i < dimensiune_imagine; i++)
    {
      for (j = 0; j < dimensiune_imagine; j++)
      {
        fread(&imagine[i][j].red, sizeof(unsigned char), 1, fin);
        fread(&imagine[i][j].green, sizeof(unsigned char), 1, fin);
        fread(&imagine[i][j].blue, sizeof(unsigned char), 1, fin);
      }
    }

    Nod *root = NULL;
    root = creare_arbore(root, imagine, 0, 0, dimensiune_imagine, factor);

    fprintf(fout, "%u\n", numar_niveluri(root));
    fprintf(fout, "%u\n", numar_blocuri_colorate(root));
    fprintf(fout, "%u\n", latura_maxima(root));

    // eliberare memorie
    for (i = 0; i < dimensiune_imagine; i++)
    {
      free(imagine[i]);
    }
    free(imagine);
    eliberare_memorie(root);

    fclose(fin);
    fclose(fout);
  }
  else if (strcmp(argv[1], "-c2") == 0)
  {
    // cerinta 2
    int factor = atoi(argv[2]);
    FILE *fin = fopen(argv[3], "rb");
    FILE *fout = fopen(argv[4], "wb");

    // citire antet
    char gunoi[4], alt_gunoi;
    int max_val, i, j;
    unsigned int m, dimensiune_imagine;
    fread(gunoi, sizeof(char), 3, fin);
    fscanf(fin, "%u %u\n%d", &m, &dimensiune_imagine, &max_val);
    fread(&alt_gunoi, sizeof(char), 1, fin);

    // alocare memorie pentru imagine
    pixel **imagine = (pixel **)malloc(dimensiune_imagine * sizeof(pixel *));
    for (i = 0; i < dimensiune_imagine; i++)
    {
      imagine[i] = (pixel *)malloc(dimensiune_imagine * sizeof(pixel));
    }

    // citire imagine
    for (i = 0; i < dimensiune_imagine; i++)
    {
      for (j = 0; j < dimensiune_imagine; j++)
      {
        fread(&imagine[i][j].red, sizeof(unsigned char), 1, fin);
        fread(&imagine[i][j].green, sizeof(unsigned char), 1, fin);
        fread(&imagine[i][j].blue, sizeof(unsigned char), 1, fin);
      }
    }

    Nod *root = NULL;
    root = creare_arbore(root, imagine, 0, 0, dimensiune_imagine, factor);

    fwrite(&dimensiune_imagine, sizeof(unsigned int), 1, fout);
    afisare_parcurgere_la_nivel(root, fout);

    // eliberare memorie
    for (i = 0; i < dimensiune_imagine; i++)
    {
      free(imagine[i]);
    }
    free(imagine);
    eliberare_memorie(root);

    fclose(fin);
    fclose(fout);
  }
  else
  {
    // cerinta 3
    FILE *fin = fopen(argv[2], "rb");
    FILE *fout = fopen(argv[3], "wb");

    int i, j;
    unsigned int dimensiune_imagine;
    fread(&dimensiune_imagine, sizeof(unsigned int), 1, fin);

    fprintf(fout, "P6\n");
    fprintf(fout, "%u %u\n", dimensiune_imagine, dimensiune_imagine);
    fprintf(fout, "255\n");

    // creare arbore
    NodCerinta3 *root = (NodCerinta3 *)malloc(sizeof(NodCerinta3));
    fread(&root->tip, sizeof(unsigned char), 1, fin);
    if (root->tip == 1)
    {
      fread(&root->red, sizeof(unsigned char), 1, fin);
      fread(&root->green, sizeof(unsigned char), 1, fin);
      fread(&root->blue, sizeof(unsigned char), 1, fin);
      root->dimensiune = dimensiune_imagine;
      root->start_sus = 0;
      root->start_stanga = 0;
    }
    root->stanga_sus = NULL;
    root->dreapta_sus = NULL;
    root->dreapta_jos = NULL;
    root->stanga_jos = NULL;

    // inserare noduri
    unsigned char tip;
    while (feof(fin) == 0)
    {
      fread(&tip, sizeof(unsigned char), 1, fin);
      root = parcurgere_la_nivel(root, tip, dimensiune_imagine / 2, fin);
    }

    // alocare memorie pentru imagine
    pixel **imagine = (pixel **)malloc(dimensiune_imagine * sizeof(pixel *));
    for (i = 0; i < dimensiune_imagine; i++)
    {
      imagine[i] = (pixel *)malloc(dimensiune_imagine * sizeof(pixel));
    }

    creare_imagine(root, imagine);

    for (i = 0; i < dimensiune_imagine; i++)
    {
      for (j = 0; j < dimensiune_imagine; j++)
      {
        fwrite(&imagine[i][j].red, sizeof(unsigned char), 1, fout);
        fwrite(&imagine[i][j].green, sizeof(unsigned char), 1, fout);
        fwrite(&imagine[i][j].blue, sizeof(unsigned char), 1, fout);
      }
    }

    // eliberare memorie
    for (i = 0; i < dimensiune_imagine; i++)
    {
      free(imagine[i]);
    }
    free(imagine);
    eliberare_memorie_cerinta3(root);

    fclose(fin);
    fclose(fout);
  }

  return 0;
}
