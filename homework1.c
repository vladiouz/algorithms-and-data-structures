/* IONESCU Petru-Vlad - 315CC */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Nod
{
  char elem;
  struct Nod *next;
  struct Nod *prev;
} Nod, *List;

typedef struct Punct
{
  List lst;
  struct Nod *finger;
} Punct, *Banda;

typedef struct NodUpdate
{
  char elem[20];
  struct NodUpdate *next;
} NodUpdate, *ListUpdate;

typedef struct Coada
{
  struct NodUpdate *head;
  struct NodUpdate *tail;
} *Coada, Ceva;

typedef struct NodStiva
{
  struct Nod *finger;
  struct NodStiva *next;
} NodStiva, *Stiva;

int move_left(Banda *b, FILE *out)
{
  if ((*b)->finger->prev == (*b)->lst)
  {
    return 0;
  }
  else
  {
    (*b)->finger = (*b)->finger->prev;
    return 1;
  }
}

void move_right(Banda *b)
{
  if ((*b)->finger->next == NULL)
  {
    List p = malloc(sizeof(Nod));
    p->elem = '#';
    p->next = NULL;
    p->prev = (*b)->finger;
    (*b)->finger->next = p;
  }
  (*b)->finger = (*b)->finger->next;
}

void move_left_char(Banda *b, char c, FILE *out)
{
  int ok = 0;
  List p = (*b)->finger;
  while (p->prev != (*b)->lst)
  {
    if (p->elem == c)
    {
      (*b)->finger = p;
      ok = 1;
      break;
    }
    p = p->prev;
  }
  if (!ok && p->elem == c)
  {
    (*b)->finger = p;
    ok = 1;
  }
  if (!ok)
  {
    fprintf(out, "ERROR\n");
  }
}

void move_right_char(Banda *b, char c)
{
  int ok = 0;
  while ((*b)->finger->next != NULL)
  {
    if ((*b)->finger->elem == c)
    {
      ok = 1;
      break;
    }
    (*b)->finger = (*b)->finger->next;
  }
  if (!ok && (*b)->finger->elem == c)
  {
    ok = 1;
  }
  if (!ok)
  {
    List p = malloc(sizeof(Nod));
    p->elem = '#';
    p->next = NULL;
    p->prev = (*b)->finger;
    (*b)->finger->next = p;
    (*b)->finger = p;
  }
}

void insert_left(Banda *b, char c, FILE *out)
{
  if ((*b)->finger->prev == (*b)->lst)
  {
    fprintf(out, "ERROR\n");
  }
  else
  {
    List p = malloc(sizeof(Nod));
    p->elem = c;
    p->next = (*b)->finger;
    p->prev = (*b)->finger->prev;
    (*b)->finger->prev->next = p;
    (*b)->finger->prev = p;
    (*b)->finger = p;
  }
}

void insert_right(Banda *b, char c)
{
  if ((*b)->finger->next == NULL)
  {
    List p = malloc(sizeof(Nod));
    p->elem = c;
    p->next = NULL;
    p->prev = (*b)->finger;
    (*b)->finger->next = p;
    (*b)->finger = p;
  }
  else
  {
    List p = malloc(sizeof(Nod));
    p->elem = c;
    p->next = (*b)->finger->next;
    p->prev = (*b)->finger;
    (*b)->finger->next->prev = p;
    (*b)->finger->next = p;
    (*b)->finger = p;
  }
}

void write(Banda *b, char c)
{
  (*b)->finger->elem = c;
}

void adhas(List *lst)
{
  List p = malloc(sizeof(Nod));
  p->elem = '#';
  p->prev = *lst;
  if ((*lst)->next == NULL)
  {
    p->next = NULL;
  }
  else
  {
    p->next = (*lst)->next;
    (*lst)->next->prev = p;
  }
  (*lst)->next = p;
}

void show_current(Banda b, FILE *out)
{
  fprintf(out, "%c\n", b->finger->elem);
}

void show(Banda b, FILE *out)
{
  List p = b->lst;
  p = p->next;
  while (p->next != NULL)
  {
    if (p == (*b).finger)
    {
      fprintf(out, "|%c|", p->elem);
    }
    else
    {
      fprintf(out, "%c", p->elem);
    }
    p = p->next;
  }
  if (p == (*b).finger)
  {
    fprintf(out, "|%c|", p->elem);
  }
  else
  {
    fprintf(out, "%c", p->elem);
  }
  fprintf(out, "\n");
}

void enqueue_update(Coada *c, char elem[])
{
  if ((*c)->head == NULL)
  {
    ListUpdate p = (ListUpdate)malloc(sizeof(NodUpdate));
    strcpy(p->elem, elem);
    p->next = NULL;
    (*c)->head = p;
    (*c)->tail = p;
  }
  else
  {
    ListUpdate p = (ListUpdate)malloc(sizeof(NodUpdate));
    strcpy(p->elem, elem);
    p->next = NULL;
    (*c)->tail->next = p;
    (*c)->tail = p;
  }
}

void dequeue_update(Coada *c)
{
  ListUpdate p = (ListUpdate)malloc(sizeof(NodUpdate));
  p = (*c)->head;
  (*c)->head = (*c)->head->next;
  free(p);
}

// se cheama doar dupa ce folosim move_left si merge
void push_undo_after_move_left(Stiva *op_undo, Banda b)
{
  if ((*op_undo) == NULL)
  {
    (*op_undo)->finger = b->finger->next;
  }
  else
  {
    Stiva nou = (Stiva)malloc(sizeof(NodStiva));
    nou->finger = b->finger->next;
    nou->next = *op_undo;
    *op_undo = nou;
  }
}

// se cheama doar dupa ce folosim move_right
void push_undo_after_move_right(Stiva *op_undo, Banda b)
{
  if ((*op_undo) == NULL)
  {
    (*op_undo)->finger = b->finger->prev;
  }
  else
  {
    Stiva nou = (Stiva)malloc(sizeof(NodStiva));
    nou->finger = b->finger->prev;
    nou->next = *op_undo;
    *op_undo = nou;
  }
}

void undo(Stiva *op_undo, Stiva *op_redo, Banda *b)
{
  if ((*op_redo)->finger == NULL)
  {
    (*op_redo)->finger = (*b)->finger;

    (*b)->finger = (*op_undo)->finger;

    Stiva p = (Stiva)malloc(sizeof(NodStiva));
    p = *op_undo;
    (*op_undo) = (*op_undo)->next;
    free(p);
  }
  else
  {
    Stiva p = (Stiva)malloc(sizeof(NodStiva));
    p->finger = (*b)->finger;
    p->next = *op_redo;
    *op_redo = p;

    (*b)->finger = (*op_undo)->finger;

    Stiva q = (Stiva)malloc(sizeof(NodStiva));
    q = *op_undo;
    (*op_undo) = (*op_undo)->next;
    free(q);
  }
}

void redo(Stiva *op_undo, Stiva *op_redo, Banda *b)
{
  if ((*op_undo)->finger == NULL)
  {
    (*op_undo)->finger = (*b)->finger;

    (*b)->finger = (*op_redo)->finger;

    Stiva p = (Stiva)malloc(sizeof(NodStiva));
    p = *op_redo;
    (*op_redo) = (*op_redo)->next;
    free(p);
  }
  else
  {
    Stiva p = (Stiva)malloc(sizeof(NodStiva));
    p->finger = (*b)->finger;
    p->next = *op_undo;
    *op_undo = p;

    (*b)->finger = (*op_redo)->finger;

    Stiva q = (Stiva)malloc(sizeof(NodStiva));
    q = *op_redo;
    (*op_redo) = (*op_redo)->next;
    free(q);
  }
}

int main()
{
  Banda banda = malloc(sizeof(Punct));
  List lst = malloc(sizeof(Nod));
  lst->next = NULL;
  banda->lst = lst;
  adhas(&lst);
  banda->finger = lst->next;

  Coada op_update = malloc(sizeof(Ceva));
  op_update->head = NULL;
  op_update->tail = NULL;

  Stiva op_undo = malloc(sizeof(NodStiva));
  op_undo->finger = NULL;
  op_undo->next = NULL;

  Stiva op_redo = malloc(sizeof(NodStiva));
  op_redo->finger = NULL;
  op_redo->next = NULL;

  FILE *in = fopen("tema1.in", "r");
  FILE *out = fopen("tema1.out", "w");
  char buff[100];

  int a;
  fscanf(in, "%d", &a);
  fgetc(in);

  while (fgets(buff, 20, in))
  {
    if (buff[strlen(buff) - 1] == '\n')
    {
      buff[strlen(buff) - 1] = '\0';
    }

    if (strcmp(buff, "SHOW") == 0)
    {
      show(banda, out);
    }
    else if (strcmp(buff, "SHOW_CURRENT") == 0)
    {
      show_current(banda, out);
    }
    else if (strcmp(buff, "EXECUTE") == 0)
    {
      if (strcmp(op_update->head->elem, "MOVE_LEFT") == 0)
      {
        if (move_left(&banda, out) == 1)
        {
          push_undo_after_move_left(&op_undo, banda);
        }
      }
      else if (strcmp(op_update->head->elem, "MOVE_RIGHT") == 0)
      {
        move_right(&banda);
        push_undo_after_move_right(&op_undo, banda);
      }
      else
      {
        char comanda[20], caracter;
        strncpy(comanda, op_update->head->elem, strlen(op_update->head->elem) - 2);
        comanda[strlen(op_update->head->elem) - 2] = '\0';
        caracter = op_update->head->elem[strlen(op_update->head->elem) - 1];
        if (strcmp(comanda, "MOVE_LEFT_CHAR") == 0)
        {
          move_left_char(&banda, caracter, out);
        }
        else if (strcmp(comanda, "MOVE_RIGHT_CHAR") == 0)
        {
          move_right_char(&banda, caracter);
        }
        else if (strcmp(comanda, "WRITE") == 0)
        {
          write(&banda, caracter);
        }
        else if (strcmp(comanda, "INSERT_LEFT") == 0)
        {
          insert_left(&banda, caracter, out);
        }
        else if (strcmp(comanda, "INSERT_RIGHT") == 0)
        {
          insert_right(&banda, caracter);
        }
      }

      dequeue_update(&op_update);
    }
    else if (strcmp(buff, "UNDO") == 0)
    {
      undo(&op_undo, &op_redo, &banda);
    }
    else if (strcmp(buff, "REDO") == 0)
    {
      redo(&op_undo, &op_redo, &banda);
    }
    else
    {
      enqueue_update(&op_update, buff);
    }
  }
  fclose(in);
  fclose(out);
  return 0;
}
