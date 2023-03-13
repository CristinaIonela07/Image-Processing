#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>

// structura vector
typedef struct QuadtreeNode{
    unsigned char blue, green, red;
    uint32_t area;
    int32_t top_left, top_right;
    int32_t bottom_left, bottom_right;
}__attribute__((packed)) QuadtreeNode;

// structura matrice de pixeli
typedef struct Pixel{
    unsigned char red, green, blue;
}Pixel;

// structura arbore cuaternar
typedef struct Quadtree{
    unsigned char red, green, blue;
    unsigned long long int area;
    struct Quadtree *top_l, *top_r, *bottom_r, *bottom_l;
}Quadtree;

// functie de initializare arbore
void init(Quadtree **tree, unsigned char r, unsigned char g, unsigned char b, int size){
    *tree = (Quadtree *)malloc(sizeof(Quadtree));
    (*tree)->area = size * size;
    (*tree)->red = r;
    (*tree)->green = g;
    (*tree)->blue = b;
    (*tree)->top_l = NULL;
    (*tree)->top_r = NULL;
    (*tree)->bottom_l = NULL;
    (*tree)->bottom_r = NULL;
}

// functie de calculare a valorii medii a unei culori 
long long int medium_color(Pixel **p, int row, int col, int size, int c){
    long long int a, s, r;
    a = size * size;
    s = 0;
    if(a == 0) return 0;
    long int i, j;
    for (i = col; i < col + size; i++)
        for (j = row; j < row + size; j++){
            if(c == 1)
                s = s + p[i][j].red;
            if(c == 2)
                s = s + p[i][j].green;
            if(c == 3)
                s = s + p[i][j].blue;
        }
    return s / a;
}

// functie care calculeaza scorul similaritatii pentru un bloc
long long int mean (Pixel **p, int row, int col, int size){
    int i, j;
    long long int s, blue, green, red;
    s = 0;
    if (size == 0)
        return 0;
    
    blue = medium_color(p, row, col, size, 3);
    green = medium_color(p, row, col, size, 2);
    red = medium_color(p, row, col, size, 1);

    for (i = col; i < col + size; i++)
        for (j = row; j < row + size; j++)    
            s = s + pow((red - p[i][j].red), 2) + pow((green - p[i][j].green), 2) + pow((blue - p[i][j].blue), 2);
    
   long long int m = s / (3* size * size);

    return m;
}

// functie care transforma matricea in arbore
void matrix_tree(Pixel **p, int row, int col, int size, Quadtree **tree, int step){
   
    long long int b, g, r;
    b = medium_color(p, row, col, size, 3);
    g = medium_color(p, row, col, size, 2);
    r = medium_color(p, row, col, size, 1);
    
    init(tree, r, g, b, size);

    // daca scorul similaritatii este mai mare decat un prag inpus, atunci se divide 
    if(mean(p, row, col, size) > step){
        matrix_tree(p, row, col, size/2, &((*tree)->top_l), step);
        matrix_tree(p, row+size/2, col, size/2, &((*tree)->top_r), step);
        matrix_tree(p, row+size/2, col+size/2, size/2, &((*tree)->bottom_r), step);
        matrix_tree(p, row, col+size/2, size/2, &((*tree)->bottom_l), step);
        
    }
    else {
        (*tree)->top_l = NULL;
        (*tree)->top_r = NULL;
        (*tree)->bottom_r = NULL;
        (*tree)->bottom_l = NULL;
        return;
    }
}

// functie care transforma un arbore intr-un vector
void tree_vector(Quadtree *tree, QuadtreeNode *a, int *i){
    int j = *i;
	if (tree == NULL) return;
	
    a[j].area = tree->area;
	a[j].red = tree->red;
    a[j].green = tree->green;
	a[j].blue = tree->blue;
	
    // daca nodul nu este nod terminal, se continua pana se ajunge la frunze
	if (tree->top_l != NULL && tree->top_r != NULL && tree->bottom_l != NULL && tree->bottom_r != NULL){
		(*i)++;
		a[j].top_left = *i;
		tree_vector(tree->top_l, a, i);

		a[j].top_right = *i;
		tree_vector(tree->top_r, a, i);

		a[j].bottom_right = *i;
		tree_vector(tree->bottom_r, a, i);

        a[j].bottom_left = *i;
		tree_vector(tree->bottom_l, a, i);
	}
    // daca nodul este frunza, elementele vectorului primesc valoarea -1
	else{
		a[j].top_left = -1;
		a[j].top_right = -1;
		a[j].bottom_right = -1;
        a[j].bottom_left = -1;
		(*i)++;		
	}
}

// functie care transforma un vector intr-un arbore
void vector_tree(QuadtreeNode *a,  int *i, int n, Quadtree **tree){
    int size = sqrt(a[*i].area);
    
    // conditia de oprire
    if (*i > n) return;

    // initializare subarbore
    init(tree, a[*i].red, a[*i].green, a[*i].blue, size);

    // cata vreme nu se ajunge la o frunza, se construieste fiecare subarbore
    if (a[*i].top_left != -1 && a[*i].top_right != -1 && a[*i].bottom_right != -1 && a[*i].bottom_left != -1){
        (*i)++;
        vector_tree(a, i, n, &((*tree)->top_l));

        (*i)++;
        vector_tree(a, i, n, &((*tree)->top_r));

        (*i)++;
        vector_tree(a, i, n, &((*tree)->bottom_r));
        
        (*i)++;
        vector_tree(a, i, n, &((*tree)->bottom_l));
    }
}

// functie care transforma un arbore intr-o matrice
void tree_matrix(Quadtree *tree, Pixel **p, int row, int col, int size){
    int i, j;
    
    if (row == size) return;
 
    // cand se ajunge la o frunza, matricea este recompusa cu pixeli
    if (tree->top_l == NULL ) {
        for (i = row; i < row + sqrt(tree->area); i++)
            for (j = col; j < col + sqrt(tree->area); j++){
                p[i][j].red = tree->red;
                p[i][j].green = tree->green;
                p[i][j].blue = tree->blue;
            }
    }
    else{
        tree_matrix(tree->top_l, p, row, col, size);
        tree_matrix(tree->top_r, p, row, col + sqrt(tree->area) / 2, size);
        tree_matrix(tree->bottom_r, p, row + sqrt(tree->area) / 2, col + sqrt(tree->area) / 2, size);
        tree_matrix(tree->bottom_l, p, row + sqrt(tree->area) / 2, col, size);
    }
}

// functie ce calculeaza numarul de noduri din arbore
int nr_nodes(Quadtree *tree){
    if (tree == NULL) 
        return 0;
    return 1 + nr_nodes(tree->top_l) + nr_nodes(tree->top_r) + nr_nodes(tree->bottom_l) + nr_nodes(tree->bottom_r);
}

// functie ce calculeaza numarul de frunze din arbore/numarul de blocuri cu informatie utila
int nr_leaves(Quadtree *tree){
    if (tree->top_l == NULL) 
        return 1;
    return nr_leaves(tree->top_r) + nr_leaves(tree->top_l) + nr_leaves(tree->bottom_r) + nr_leaves(tree->bottom_l);
}

// functie de oglindire verticala a arborelui
void mirror_v(Quadtree *tree){
    if (tree == NULL) return;
    Quadtree *aux;
    mirror_v(tree->top_l);
    mirror_v(tree->top_r);
    mirror_v(tree->bottom_r);
    mirror_v(tree->bottom_l);

    aux = tree->top_l;
    tree->top_l = tree->bottom_l;
    tree->bottom_l = aux;

    aux = tree->top_r;
    tree->top_r = tree->bottom_r;
    tree->bottom_r = aux;
}

// functie de oglindire orizontala a arborelui
void mirror_h(Quadtree *tree){
    if (tree == NULL) return;
    Quadtree *aux;
    mirror_h(tree->top_l);
    mirror_h(tree->top_r);
    mirror_h(tree->bottom_r);
    mirror_h(tree->bottom_l);

    aux = tree->top_l;
    tree->top_l = tree->top_r;
    tree->top_r = aux;

    aux = tree->bottom_r;
    tree->bottom_r = tree->bottom_l;
    tree->bottom_l = aux;
}

// functie de scriere in fisierul PPM a informatiilor 
// necesare fomatului + matricea de pixeli
void write_mat(FILE *f, Quadtree *tree, Pixel **p, int size, int c){
    int h, w, i, j;
    h = w = size;
    
    p = (Pixel **) malloc(w * sizeof(Pixel*));
    
    for (i = 0; i < w; i++)
        p[i]= malloc(h * sizeof(Pixel));
   
    tree_matrix(tree, p, 0, 0, w);
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (i = 0; i < h; i++)
        for (j = 0; j < w; j++)
            fwrite(&p[i][j], sizeof(Pixel), 1, f); 
    
    for (i = 0; i < h; i++)
        free(p[i]);
    free(p);

}

// eliberare memorie alocata pentru arbore
int free_tree (Quadtree **tree){
    if ((*tree) == NULL) return 0;

    free_tree(&(*tree)->top_r);
    free_tree(&(*tree)->top_l);
    free_tree(&(*tree)->bottom_r);
    free_tree(&(*tree)->bottom_l);
    free(*tree);
}

int main( int argc, char *argv[]){
    /* size = o dimensiune a matricei
       h = inaltimea imaginii/matricei
       w = latimea imaginii/matriei
       step = factor de compresie (acolo unde este cazul)
       max = valoarea maxima pe care o poate lua o culoare 
    */
    int i, j, size, step, h, w, max;
    char type[10], smth;

    // daca primul argument este "-c", se va rezolva task-ul 1
    if (strcmp(argv[1], "-c") == 0){
        step = atoi(argv[2]);
        //se extrag informatiile din fisierul binar
        FILE *fp = fopen(argv[3], "rb");
        fscanf(fp, "%s", type);
        fscanf(fp, "%d", &w);
        fscanf(fp, "%d\n", &h);
        fscanf(fp, "%d", &max);
        fread(&smth, sizeof(char), 1, fp);
        
        // se construieste matricea de pixeli
        Pixel **mat_p = (Pixel **) malloc(h * sizeof(Pixel*));         
        for (i = 0; i < h; i++)
            mat_p[i] = (Pixel *) malloc(w * sizeof(Pixel));

        for (i = 0; i < h; i++)
            for (j = 0; j < w; j++)
                fread(&mat_p[i][j], sizeof(Pixel), 1, fp);
        
        fclose(fp);
        
        // se transforma matricea in arbore
        Quadtree *tree;
        matrix_tree(mat_p, 0, 0, h, &tree, step);

        // v = vectorul de compresie
        QuadtreeNode *v = (QuadtreeNode *) malloc(nr_nodes(tree)*sizeof(QuadtreeNode));
        i = 0;
        tree_vector(tree, v, &i);

        /* nr_lv = numarul de frunze din arbore
           nr_nod = numarul de noduri din arbore
           Se scrie fisierul comprimat
	*/
        FILE *p = fopen(argv[4], "wb");
        int nr_lv = nr_leaves(tree);
        int nr_nod = nr_nodes(tree);
 
        fwrite(&nr_lv, sizeof(uint32_t), 1, p);
        fwrite(&nr_nod, sizeof(uint32_t), 1, p);

        for (i = 0; i < nr_nod; i++)
            fwrite(&v[i], sizeof(QuadtreeNode),1, p);
    
        fclose(p);

        // eliberare memorie
        free_tree(&tree);

        for (i = 0; i < w; i++)
            free(mat_p[i]);

        free(mat_p);

        free(v);
    }

    // daca primul argument este "-d", se va rezolva task-ul 2
    else if (strcmp(argv[1], "-d") == 0){
        // se extrag informatiile din fisierul comprimat
        FILE *fp = fopen(argv[2], "rb");
        int nr_lv, nr_nod;

        fread(&nr_lv, sizeof(int), 1, fp);
        fread(&nr_nod, sizeof(int), 1, fp);

        QuadtreeNode *v = (QuadtreeNode *) malloc(nr_nod * sizeof(QuadtreeNode));
        
        for (i = 0; i < nr_nod; i++)
            fread(&v[i], sizeof(QuadtreeNode), 1, fp);

        fclose(fp);

        // se construieste arborele pe baza informatiilor extrase
        // apoi din arbore se formeaza matricea de pixeli ce va 
        // fi scrisa in fisier, urmand eliberarea memoriei
        Quadtree *tree;
        i = 0;
        vector_tree(v, &i, nr_nod, &tree);

        size = sqrt(tree->area);

        Pixel **p;
        FILE *f = fopen(argv[3], "wb");

        write_mat(f, tree, p, size, 1);
        
        fclose(f);
        
        free_tree(&tree);
        free(v);
    }
    // daca primul argument este "-m", se va rezolva task-ul 3
    else if (strcmp(argv[1], "-m") == 0){
        // se extrag informatiile din fisierul binar
        int step = atoi(argv[3]);

        FILE *fp = fopen(argv[4], "rb");
        Pixel **mat_p;
        Quadtree *tree;
        char type[10];
        int w, h, max;

        fscanf(fp, "%s", type);
        fscanf(fp, "%d", &w);
        fscanf(fp, "%d\n", &h);
        fscanf(fp, "%d", &max);
        char smth;
        fread(&smth, sizeof(char), 1, fp);
        
        // mat_p = matrice de pixeli
        mat_p = (Pixel **) malloc(h * sizeof(Pixel*));         
        for (i = 0; i < h; i++)
            mat_p[i] = (Pixel *) malloc(w * sizeof(Pixel));

        for (i = 0; i < h; i++)
            for (j = 0; j < w; j++)
                fread(&mat_p[i][j], sizeof(Pixel), 1, fp);
    
        matrix_tree(mat_p, 0, 0, h, &tree, step);
   
        fclose(fp);
        
        // daca al doilea argument este "h" se va oglindi pe orizontala
        // daca al doilea argument este "v" se va oglindi pe verticala
        if (strcmp(argv[2], "h")==0)
            mirror_h(tree);
        else if (strcmp(argv[2], "v")==0)
            mirror_v(tree);

        // din arbore se formeaza matricea de pixeli ce va 
        // fi scrisa in fisier, urmand eliberarea memoriei
        Pixel **p;
        FILE *f = fopen(argv[5], "wb");

        write_mat(f, tree, p, w, 2);
    
        fclose(f);
        free_tree(&tree);

        for (i = 0; i < w; i++)
            free(mat_p[i]);
        free(mat_p);
    }

    return 0;
}