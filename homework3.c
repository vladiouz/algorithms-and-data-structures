/*IONESCU Petru-Vlad - 315CC*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

// structurile pentru cerinta 1
typedef struct Nod
{
  char *nume;
  unsigned int cost;
  struct Nod *urm;
} Nod;

typedef struct Graf
{
  unsigned int nr_noduri;
  Nod **lista_adiacenta;
} Graf;

// structurile pentru cerinta 2
typedef struct Nod2
{
  char *nume;
  int adancime;
  int distanta;
  float scor;
  struct Nod2 *urm;
} Nod2;

typedef struct Graf2
{
  int nr_noduri;
  Nod2 **lista_adiacenta;
} Graf2;

// cauta tatal unui nod
int cauta_tata(int i, int *tata)
{
  while (tata[i] != i)
  {
    i = tata[i];
  }
  return i;
}

// uneste doua subcomponente
void uneste(int i, int j, int *tata)
{
  int tata_i = cauta_tata(i, tata);
  int tata_j = cauta_tata(j, tata);
  tata[tata_i] = tata_j;
}

// algoritmul Kruskal pentru subpunctul 1b
int kruskal(Graf g, int nr_comp, int nr_noduri)
{
  int cost_minim = 0, nr_muchii = 0, i, j;
  int *tata = (int *)malloc(g.nr_noduri * sizeof(int));

  for (i = 0; i < g.nr_noduri; i++)
  {
    tata[i] = i;
  }
  while (nr_muchii + 1 < nr_noduri)
  {
    nr_muchii++;
    int min = INT_MAX, a = -1, b = -1;
    for (i = 0; i < g.nr_noduri; i++)
    {
      if (g.lista_adiacenta[i]->cost == nr_comp)
      {
        for (j = 0; j < g.nr_noduri; j++)
        {
          if (g.lista_adiacenta[j]->cost == nr_comp && cauta_tata(i, tata) != cauta_tata(j, tata))
          {
            Nod *p = g.lista_adiacenta[i]->urm;
            while (p != NULL)
            {
              if (strcmp(p->nume, g.lista_adiacenta[j]->nume) == 0)
              {
                if (p->cost < min)
                {
                  min = p->cost;
                  a = i;
                  b = j;
                }
                break;
              }
              p = p->urm;
            }
          }
        }
      }
    }
    uneste(a, b, tata);
    cost_minim += min;
  }
  free(tata);
  return cost_minim;
}

int viziteaza(Graf *g, int *vizitat, int i, int nr_comp)
{
  int nr_noduri = 0;
  vizitat[i] = 1;
  // in acest spatiu retinem numarul componentei in care se afla al i-ulea nod
  g->lista_adiacenta[i]->cost = nr_comp;
  int j;
  Nod *p = g->lista_adiacenta[i]->urm;
  while (p != NULL)
  {
    for (j = 0; j < g->nr_noduri; j++)
    {
      if (strcmp(g->lista_adiacenta[j]->nume, p->nume) == 0)
      {

        if (vizitat[j] == 0)
        {
          nr_noduri++;
          nr_noduri += viziteaza(g, vizitat, j, nr_comp);
        }
        break;
      }
    }
    p = p->urm;
  }
  return nr_noduri;
}

void prima_cerinta(Graf *g, FILE *fout)
{
  int *vizitat = (int *)malloc(g->nr_noduri * sizeof(int));
  int i, j, nr_comp = 0;
  int costuri[1000];

  for (i = 0; i < g->nr_noduri; i++)
  {
    vizitat[i] = 0;
  }

  for (i = 0; i < g->nr_noduri; i++)
  {
    if (vizitat[i] == 0)
    {
      // de fiecare data cand gasim un nod dintr-o componenta nevizitata,
      // numarul de componente e incrementat si vizitam toate nodurile din componenta
      // cu ajutorul functiei viziteaza
      // in acelasi timp, calculam si numarul de noduri din componenta pe care o parcurgem
      int nr_noduri = 1 + viziteaza(g, vizitat, i, nr_comp);
      costuri[nr_comp] = kruskal(*g, nr_comp, nr_noduri);
      nr_comp++;
    }
  }

  // sortam costurile in ordine crescatoare
  for (i = 0; i < nr_comp; i++)
  {
    for (j = i + 1; j < nr_comp; j++)
    {
      if (costuri[i] > costuri[j])
      {
        int aux = costuri[i];
        costuri[i] = costuri[j];
        costuri[j] = aux;
      }
    }
  }

  // cerinta 1a
  fprintf(fout, "%d\n", nr_comp);

  // cerinta 1b
  for (i = 0; i < nr_comp; i++)
  {
    fprintf(fout, "%d\n", costuri[i]);
  }

  free(vizitat);
}

void afisare_drum(Graf2 *g, int nod, int *predecesor, int start, FILE *fout)
{
  if (nod == start)
  {
    fprintf(fout, "%s", g->lista_adiacenta[nod]->nume);
    return;
  }
  afisare_drum(g, predecesor[nod], predecesor, start, fout);
  fprintf(fout, " %s", g->lista_adiacenta[nod]->nume);
}

void dijkstra(Graf2 *g, FILE *fout, int greutate)
{
  float **costuri;
  costuri = (float **)malloc(g->nr_noduri * sizeof(float *));
  for (int i = 0; i < g->nr_noduri; i++)
  {
    costuri[i] = (float *)malloc(g->nr_noduri * sizeof(float));
  }
  int i, j, urm_nod = 0, count;
  float dist_min;
  int *vizitat, *predecesor;
  float *distanta;
  vizitat = (int *)malloc(g->nr_noduri * sizeof(int));
  predecesor = (int *)malloc(g->nr_noduri * sizeof(int));
  distanta = (float *)malloc(g->nr_noduri * sizeof(float));

  for (i = 0; i < g->nr_noduri; i++)
  {
    for (j = 0; j < g->nr_noduri; j++)
    {
      costuri[i][j] = FLT_MAX;
    }
  }

  for (i = 0; i < g->nr_noduri; i++)
  {
    Nod2 *p = g->lista_adiacenta[i]->urm;
    while (p != NULL)
    {
      for (j = 0; j < g->nr_noduri; j++)
      {
        if (strcmp(g->lista_adiacenta[j]->nume, p->nume) == 0)
        {
          costuri[i][j] = p->scor;
          break;
        }
      }
      p = p->urm;
    }
  }

  // aplicam mai intai Dijkstra pentru traseul Corabie - Insula
  for (i = 0; i < g->nr_noduri; i++)
  {
    vizitat[i] = 0;
    distanta[i] = costuri[1][i];
    predecesor[i] = 0;
  }

  distanta[1] = 0;
  vizitat[1] = 1;
  count = 1;

  while (count < g->nr_noduri - 1)
  {
    dist_min = FLT_MAX;

    for (i = 0; i < g->nr_noduri; i++)
    {
      if (distanta[i] < dist_min && !vizitat[i])
      {
        dist_min = distanta[i];
        urm_nod = i;
      }
    }

    vizitat[urm_nod] = 1;

    for (i = 0; i < g->nr_noduri; i++)
    {
      if (!vizitat[i])
      {
        if (dist_min + costuri[urm_nod][i] < distanta[i])
        {
          distanta[i] = dist_min + costuri[urm_nod][i];
          predecesor[i] = urm_nod;
        }
      }
    }
    count++;
  }

  // cazul in care nu exista drum de la Corabie la Insula
  if (distanta[0] == FLT_MAX)
  {
    fprintf(fout, "Echipajul nu poate ajunge la insula");
    free(vizitat);
    free(predecesor);
    free(distanta);
    for (i = 0; i < g->nr_noduri; i++)
    {
      free(costuri[i]);
    }
    free(costuri);
    return;
  }

  // acum aplicam Dijkstra pentru traseul Insula - Corabie
  for (i = 0; i < g->nr_noduri; i++)
  {
    vizitat[i] = 0;
    distanta[i] = costuri[0][i];
    predecesor[i] = 0;
  }

  distanta[0] = 0;
  vizitat[0] = 1;
  count = 1;

  while (count < g->nr_noduri - 1)
  {
    dist_min = FLT_MAX;

    for (i = 0; i < g->nr_noduri; i++)
    {
      if (distanta[i] < dist_min && !vizitat[i])
      {
        dist_min = distanta[i];
        urm_nod = i;
      }
    }

    vizitat[urm_nod] = 1;

    for (i = 0; i < g->nr_noduri; i++)
    {
      if (!vizitat[i])
      {
        if (dist_min + costuri[urm_nod][i] < distanta[i])
        {
          distanta[i] = dist_min + costuri[urm_nod][i];
          predecesor[i] = urm_nod;
        }
      }
    }
    count++;
  }

  // cazul in care nu exista drum de la Insula la Corabie
  if (distanta[1] == FLT_MAX)
  {
    fprintf(fout, "Echipajul nu poate transporta comoara inapoi la corabie");
    free(vizitat);
    free(predecesor);
    free(distanta);
    for (i = 0; i < g->nr_noduri; i++)
    {
      free(costuri[i]);
    }
    free(costuri);
    return;
  }

  // nodurile care formeaza cel mai scurt drum
  afisare_drum(g, 1, predecesor, 0, fout);
  fprintf(fout, "\n");

  // costul total al drumului
  int nod = 1, cost_total = 0;
  while (nod != 0)
  {
    Nod2 *p = g->lista_adiacenta[predecesor[nod]]->urm;
    while (p != NULL)
    {
      if (strcmp(p->nume, g->lista_adiacenta[nod]->nume) == 0)
      {
        cost_total += p->distanta;
        break;
      }
      p = p->urm;
    }
    nod = predecesor[nod];
  }

  fprintf(fout, "%d\n", cost_total);

  // greutatea maxima
  int greutate_maxima = INT_MAX;
  nod = 1;
  while (nod != 0)
  {
    Nod2 *p = g->lista_adiacenta[predecesor[nod]]->urm;
    while (p != NULL)
    {
      if (strcmp(p->nume, g->lista_adiacenta[nod]->nume) == 0)
      {
        if (g->lista_adiacenta[nod]->adancime < greutate_maxima && nod != 1)
        {
          greutate_maxima = g->lista_adiacenta[nod]->adancime;
        }
        break;
      }
      p = p->urm;
    }
    nod = predecesor[nod];
  }

  fprintf(fout, "%d\n", greutate_maxima);

  // numar de drumuri

  if (greutate_maxima != 0)
  {
    int nr_drumuri = greutate / greutate_maxima;
    if (greutate % greutate_maxima != 0)
    {
      nr_drumuri++;
    }
    fprintf(fout, "%d\n", nr_drumuri);
  }
  else
  {
    fprintf(fout, "0\n");
  }
  free(vizitat);
  free(predecesor);
  free(distanta);
  for (i = 0; i < g->nr_noduri; i++)
  {
    free(costuri[i]);
  }
  free(costuri);
}

int main(int argc, char *argv[])
{
  int nr_cerinta = atoi(argv[1]);

  FILE *fin = fopen("tema3.in", "r");
  FILE *fout = fopen("tema3.out", "w");

  if (nr_cerinta == 1)
  {
    // cerinta 1
    unsigned int n, m, i, j, cost;
    char buffer1[20], buffer2[20];

    fscanf(fin, "%u%u", &n, &m);
    Graf *graf = (Graf *)malloc(sizeof(Graf));
    graf->nr_noduri = n;
    graf->lista_adiacenta = (Nod **)malloc(n * sizeof(Nod *));
    for (i = 0; i < n; i++)
    {
      graf->lista_adiacenta[i] = NULL;
    }

    // citire muchii si construirea listei de adiacenta
    for (i = 0; i < m; i++)
    {
      fscanf(fin, "%s", buffer1);
      fscanf(fin, "%s", buffer2);
      fscanf(fin, "%u", &cost);
      for (j = 0; j < n; j++)
      {
        if (graf->lista_adiacenta[j] == NULL)
        {
          // cazul in care buffer2 este primul vecin al lui buffer1 pe care il citim
          graf->lista_adiacenta[j] = (Nod *)malloc(sizeof(Nod));
          graf->lista_adiacenta[j]->nume = (char *)malloc((strlen(buffer1) + 1) * sizeof(char));
          strcpy(graf->lista_adiacenta[j]->nume, buffer1);
          graf->lista_adiacenta[j]->cost = -1;

          graf->lista_adiacenta[j]->urm = (Nod *)malloc(sizeof(Nod));
          graf->lista_adiacenta[j]->urm->nume = (char *)malloc((strlen(buffer2) + 1) * sizeof(char));
          strcpy(graf->lista_adiacenta[j]->urm->nume, buffer2);
          graf->lista_adiacenta[j]->urm->cost = cost;
          graf->lista_adiacenta[j]->urm->urm = NULL;
          break;
        }
        else if (strcmp(graf->lista_adiacenta[j]->nume, buffer1) == 0)
        {
          // cazul in care nodul buffer1 este deja in lista de adiacenta
          // vom adauga buffer2 la inceputul listei de adiacenta a lui buffer1
          Nod *nod = (Nod *)malloc(sizeof(Nod));
          nod->nume = (char *)malloc((strlen(buffer2) + 1) * sizeof(char));
          strcpy(nod->nume, buffer2);
          nod->cost = cost;
          nod->urm = graf->lista_adiacenta[j]->urm;
          graf->lista_adiacenta[j]->urm = nod;
          break;
        }
      }

      // facem acelasi lucru si pentru buffer2
      for (j = 0; j < n; j++)
      {
        if (graf->lista_adiacenta[j] == NULL)
        {
          // cazul in care buffer2 este primul vecin al lui buffer1 pe care il citim
          graf->lista_adiacenta[j] = (Nod *)malloc(sizeof(Nod));
          graf->lista_adiacenta[j]->nume = (char *)malloc((strlen(buffer2) + 1) * sizeof(char));
          strcpy(graf->lista_adiacenta[j]->nume, buffer2);
          graf->lista_adiacenta[j]->cost = -1;

          graf->lista_adiacenta[j]->urm = (Nod *)malloc(sizeof(Nod));
          graf->lista_adiacenta[j]->urm->nume = (char *)malloc((strlen(buffer1) + 1) * sizeof(char));
          strcpy(graf->lista_adiacenta[j]->urm->nume, buffer1);
          graf->lista_adiacenta[j]->urm->cost = cost;
          graf->lista_adiacenta[j]->urm->urm = NULL;
          break;
        }
        else if (strcmp(graf->lista_adiacenta[j]->nume, buffer2) == 0)
        {
          // cazul in care nodul buffer1 este deja in lista de adiacenta
          // vom adauga buffer1 la inceputul listei de adiacenta a lui buffer2
          Nod *nod = (Nod *)malloc(sizeof(Nod));
          nod->nume = (char *)malloc((strlen(buffer1) + 1) * sizeof(char));
          strcpy(nod->nume, buffer1);
          nod->cost = cost;
          nod->urm = graf->lista_adiacenta[j]->urm;
          graf->lista_adiacenta[j]->urm = nod;
          break;
        }
      }
    }

    prima_cerinta(graf, fout);

    // dezalocare memorie
    for (i = 0; i < n; i++)
    {
      Nod *p = graf->lista_adiacenta[i];
      while (p != NULL)
      {
        Nod *q = p;
        p = p->urm;
        free(q->nume);
        free(q);
      }
    }
    free(graf->lista_adiacenta);
    free(graf);
  }
  else
  {
    // cerinta 2
    int n, m, i, j, distanta, adancime, greutate;
    char buffer1[20], buffer2[20];
    fscanf(fin, "%d%d", &n, &m);
    Graf2 *graf = (Graf2 *)malloc(sizeof(Graf2));
    graf->nr_noduri = n;
    graf->lista_adiacenta = (Nod2 **)malloc(n * sizeof(Nod2 *));

    // construim mai intai nodurile pentru Insula si Corabie
    graf->lista_adiacenta[0] = (Nod2 *)malloc(sizeof(Nod2));
    graf->lista_adiacenta[0]->nume = (char *)malloc(7 * sizeof(char));
    strcpy(graf->lista_adiacenta[0]->nume, "Insula");
    graf->lista_adiacenta[0]->urm = NULL;

    graf->lista_adiacenta[1] = (Nod2 *)malloc(sizeof(Nod2));
    graf->lista_adiacenta[1]->nume = (char *)malloc(8 * sizeof(char));
    strcpy(graf->lista_adiacenta[1]->nume, "Corabie");
    graf->lista_adiacenta[1]->urm = NULL;

    for (i = 2; i < n; i++)
    {
      graf->lista_adiacenta[i] = NULL;
    }

    // citire muchii
    for (i = 0; i < m; i++)
    {
      fscanf(fin, "%s", buffer1);
      fscanf(fin, "%s", buffer2);
      fscanf(fin, "%d", &distanta);

      for (j = 0; j < n; j++)
      {
        if (graf->lista_adiacenta[j] != NULL && strcmp(graf->lista_adiacenta[j]->nume, buffer1) == 0)
        {
          Nod2 *nod = (Nod2 *)malloc(sizeof(Nod2));
          nod->nume = (char *)malloc((strlen(buffer2) + 1) * sizeof(char));
          strcpy(nod->nume, buffer2);
          nod->distanta = distanta;
          nod->urm = graf->lista_adiacenta[j]->urm;
          graf->lista_adiacenta[j]->urm = nod;
          break;
        }
        else if (graf->lista_adiacenta[j] == NULL)
        {
          graf->lista_adiacenta[j] = (Nod2 *)malloc(sizeof(Nod2));
          graf->lista_adiacenta[j]->nume = (char *)malloc((strlen(buffer1) + 1) * sizeof(char));
          strcpy(graf->lista_adiacenta[j]->nume, buffer1);

          graf->lista_adiacenta[j]->urm = (Nod2 *)malloc(sizeof(Nod2));
          graf->lista_adiacenta[j]->urm->nume = (char *)malloc((strlen(buffer2) + 1) * sizeof(char));
          strcpy(graf->lista_adiacenta[j]->urm->nume, buffer2);
          graf->lista_adiacenta[j]->urm->distanta = distanta;
          graf->lista_adiacenta[j]->urm->urm = NULL;
          break;
        }
      }
    }

    // citire adancimi
    for (i = 0; i < n; i++)
    {
      fscanf(fin, "%s", buffer1);
      fscanf(fin, "%d", &adancime);
      for (j = 0; j < n; j++)
      {
        if (graf->lista_adiacenta[j] == NULL)
        {
          graf->lista_adiacenta[j] = (Nod2 *)malloc(sizeof(Nod2));
          graf->lista_adiacenta[j]->nume = (char *)malloc((strlen(buffer1) + 1) * sizeof(char));
          strcpy(graf->lista_adiacenta[j]->nume, buffer1);
          graf->lista_adiacenta[j]->adancime = adancime;
          graf->lista_adiacenta[j]->urm = NULL;
          break;
        }
        if (strcmp(graf->lista_adiacenta[j]->nume, buffer1) == 0)
        {
          graf->lista_adiacenta[j]->adancime = adancime;
          break;
        }
      }
    }

    fscanf(fin, "%d", &greutate);

    // completare cu scor
    for (i = 0; i < n; i++)
    {
      Nod2 *p = graf->lista_adiacenta[i]->urm;
      while (p != NULL)
      {
        for (j = 0; j < n; j++)
        {
          if (strcmp(graf->lista_adiacenta[j]->nume, p->nume) == 0)
          {
            p->scor = (float)p->distanta / (float)graf->lista_adiacenta[j]->adancime;
            break;
          }
        }
        p = p->urm;
      }
    }
    dijkstra(graf, fout, greutate);

    // dezalocare memorie
    for (i = 0; i < n; i++)
    {
      Nod2 *p = graf->lista_adiacenta[i];
      while (p != NULL)
      {
        Nod2 *q = p;
        p = p->urm;
        free(q->nume);
        free(q);
      }
    }
    free(graf->lista_adiacenta);
    free(graf);
  }

  fclose(fin);
  fclose(fout);
  return 0;
}
