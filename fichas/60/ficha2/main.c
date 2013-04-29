#include <stdio.h>
#include <stdlib.h>

typedef struct sLivres
{
	int tamanho;
	struct sLivres *next;
} *Livres, tLivres;

char MEMORIA[65536];
Livres LIVRES = NULL;

void * mymalloc(size_t size);
void myfree(void * ptr, size_t size);

Livres reserva(Livres l, int n, void **reservado){
	// 1. vazia
	if(l==NULL){
		int v = n; //arredondar n aos 64k mais proximos

		if(v%65536 != 0)
			v = ((v/65536) + 1) * 65536;


		void* maisMem = sbrk(v);

		// 1.1. o SO tinha memoria
		if(maisMem != NULL){
			l = liberta(l, maisMem, v);
			return reserva(l, n, reservado);
		}

		// 1.2. o SO nao tinha memoria
		*reservado = NULL;
		return NULL;
	}
	
	// 2 server exactamene
	if(l->tamanho == n){
		Livres resto = l->next;
		return resto;
	}

	// 3 serve e sobra
	if(l->tamanho > n){
		*reservado = ((void*)l) + (l->tamanho - n);
		l->tamanho -= n; // e se o resto é menor que sizeof(Livres) ?? poooooooooooois... :\
		return l;
	}

	// 4 nao serve
	l->next =reserva(l->next, n, reservado);
	return l;
}

Livres liberta(Livres l, void * lugar, int n){
	Livres novo = (Livres) malloc(sizeof(tLivres));
	novo->tamanho = n;
	novo->next = l;
	return novo;
}

void * mymalloc(size_t size){
	void * reservado;

	// para guardar o size enquanto ocupado
	size += sizeof(int);
	// garantir que depois de libertado, o bloco pode conter a struct Livres
	if(size < sizeof(tLivres))
		size = sizeof(tLivres);

	if(size % sizeof(int) != 0)
		size = ((size/sizeof(int)) + 1) * sizeof(int);

	LIVRES = reserva(LIVRES, size, &reservado);

	*(int*)reservado = size;

	return reservado + sizeof(int);
}

void myfree(void * ptr, size_t size){
	int *size = ptr - sizeof(int);
	LIVRES = liberta(LIVRES,ptr,*size);
}

int main(){
	LIVRES = liberta(NULL, &MEMORIA, 100);

	char* p = (char*) mymalloc(5);
	strcpy(p, "ola!");
	myfree(p); // <<<<<<<<<<<<<<<<<<<<<<<< fase 3.3
}