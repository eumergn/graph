#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct vertex
{
    float a, b, c;
} Vertex;

typedef struct face
{
    int v1, v2, v3;
} Face;

typedef struct centoide
{
    Vertex centre;
} Centoide;

typedef struct arete
{
    // Vertex sommet1, sommet2;
    int num1, num2;
    int faceA;
} Arete;

typedef struct areted
{
    int f1, f2;
    struct areted *next;
} AreteD;

typedef struct areteavl
{
    int num1, num2;
    int faceA;
    struct areteavl *left;
    struct areteavl *right;
    int hauteur;
} AreteAVL;

typedef struct centoideC
{
    int distance;
} CentoideC;


/**
 * @brief   Lit le fichier .obj
 * @param   filename Nom du fichier (par exemple bunny.obj)
 * @param   vertex   Tableau des sommets
 * @param   numV     Nombre de sommets
 * @param   face     Tableau des faces
 * @param   numF     Nombre de faces
 * @return  1 en cas de succès, 0 en cas d'échec
 */
int readObj(const char *filename, Vertex **vertex, int *numV, Face **face, int *numF)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Impossible d'ouvrir le fichier .obj\n");
        return 0;
    }
    int vCount = 0;    //Compteur, pour stocker le nombre de sommets. 
    int fCount = 0;    //Pour stocker le nombre de face.
    char line[1024];

    while (fgets(line, 1024, file))
    {
        if (line[0] == 'v' && line[1] == ' ')
            vCount++;
        if (line[0] == 'f' && line[1] == ' ')
            fCount++;
    }

    *numV = vCount;
    *numF = fCount;

    *vertex = malloc(sizeof(Vertex) * vCount);
    *face = malloc(sizeof(Face) * fCount);

    rewind(file);

    int vIndice = 0;    //Un compteur temporaire
    int fIndice = 0;

    while (fgets(line, 1024, file))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            sscanf(line, "v %f %f %f", &(*vertex)[vIndice].a, &(*vertex)[vIndice].b, &(*vertex)[vIndice].c);
            vIndice++;
        }

        if (line[0] == 'f' && line[1] == ' ')
        {
            sscanf(line, "f %d %d %d", &(*face)[fIndice].v1, &(*face)[fIndice].v2, &(*face)[fIndice].v3);
            fIndice++;
        }
    }

    fclose(file);
    return 1;
}


/**
 * @brief   Écrit le fichier .obj
 * @param   filename Nom du fichier (par exemple test.obj)
 * @param   vertex   Tableau des sommets
 * @param   numV     Nombre de sommets
 * @param   face     Tableau des faces
 * @param   numF     Nombre de faces
 * @return  1 en cas de succès, 0 en cas d'échec
 */
int writeOBJ(const char *filename, Vertex *vertex, int numV, Face *face, int numF)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Impossible d'ouvrir le fichier .obj\n");
        return 0;
    }

    for (int i = 0; i < numV; i++)
    {
        fprintf(file, "v %f %f %f\n", vertex[i].a, vertex[i].b, vertex[i].c);
    }
    for (int a = 0; a < numF; a++)
    {
        fprintf(file, "f %d %d %d\n", face[a].v1, face[a].v2, face[a].v3);
    }

    fclose(file);
    return 1;
}


/**
 * @brief   Calcule les centroïdes des faces
 * @param   vertex   Tableau des sommets
 * @param   numV     Nombre de sommets
 * @param   face     Tableau des faces
 * @param   numF     Nombre de faces
 * @return  Tableau des centroïdes
 */
Centoide *calculateCentroids(Vertex *vertex, int numV, Face *face, int numF)
{
    Centoide *centroids = malloc(sizeof(Centoide) * numF);
    for (int i = 0; i < numF; i++)
    {
        centroids[i].centre.a = (vertex[face[i].v1 - 1].a + vertex[face[i].v2 - 1].a + vertex[face[i].v3 - 1].a) / 3.0;
        centroids[i].centre.b = (vertex[face[i].v1 - 1].b + vertex[face[i].v2 - 1].b + vertex[face[i].v3 - 1].b) / 3.0;
        centroids[i].centre.c = (vertex[face[i].v1 - 1].c + vertex[face[i].v2 - 1].c + vertex[face[i].v3 - 1].c) / 3.0;
    }
    return centroids;
}


/**
 * @brief   Généralise les arêtes à partir des faces
 * @param   f        Tableau des faces
 * @param   numF     Nombre de faces
 * @param   v        Tableau des sommets
 * @return  Tableau des arêtes généralisées
 */
Arete *generalise(Face *f, int numF, Vertex *v)
{
    int numEdges = numF * 3;    //Un face a 3 arêtes 
    Arete *aretes = (Arete *)malloc(numEdges * sizeof(Arete));

    for (int i = 0; i < numF; i++)
    {
        int v1Index = f[i].v1;    //index de sommets
        int v2Index = f[i].v2;
        int v3Index = f[i].v3;

        // Vertex v1 = v[v1Index];      L'idée initiale
        // Vertex v2 = v[v2Index];
        // Vertex v3 = v[v3Index];

        // aretes[i * 3].sommet1 = v1;
        // aretes[i * 3].sommet2 = v2;
        if (v1Index < v2Index)     //Toujours placer les plus petits en premier pour faciliter la comparaison et le tri.
        {
            aretes[i * 3].num1 = v1Index;   //En utilisant les positions des trois sommets dans le tableau de sommets à l'intérieur de la face, 
            aretes[i * 3].num2 = v2Index;   //attribuez des numéros aux deux sommets de l'arête.
        }
        else
        {
            aretes[i * 3].num1 = v2Index;
            aretes[i * 3].num2 = v1Index;
        }
        aretes[i * 3].faceA = i;    //Face associe

        // aretes[i * 3 + 1].sommet1 = v2;
        // aretes[i * 3 + 1].sommet2 = v3;
        if (v2Index < v3Index)
        {
            aretes[i * 3 + 1].num1 = v2Index;
            aretes[i * 3 + 1].num2 = v3Index;
        }
        else
        {
            aretes[i * 3 + 1].num1 = v3Index;
            aretes[i * 3 + 1].num2 = v2Index;
        }
        aretes[i * 3 + 1].faceA = i;

        // aretes[i * 3 + 2].sommet1 = v3;
        // aretes[i * 3 + 2].sommet2 = v1;
        if (v1Index < v3Index)
        {
            aretes[i * 3 + 2].num1 = v1Index;
            aretes[i * 3 + 2].num2 = v3Index;
        }
        else
        {
            aretes[i * 3 + 2].num1 = v3Index;
            aretes[i * 3 + 2].num2 = v1Index;
        }
        aretes[i * 3 + 2].faceA = i;    
    }

    return aretes;
}


/**
 * @brief   Vérifie si deux arêtes sont équivalentes
 * @param   a1       Première arête
 * @param   a2       Deuxième arête
 * @return  1 si équivalentes, 0 sinon
 */
int sontEquivalentes(Arete a1, Arete a2)
{
    // if ((a1.sommet1.a == a2.sommet1.a && a1.sommet1.b == a2.sommet1.b && a1.sommet1.c == a2.sommet1.c) &&
    //     (a1.sommet2.a == a2.sommet2.a && a1.sommet2.b == a2.sommet2.b && a1.sommet2.c == a2.sommet2.c))
    // {
    //     return 1;
    // }

    // if ((a1.sommet1.a == a2.sommet2.a && a1.sommet1.b == a2.sommet2.b && a1.sommet1.c == a2.sommet2.c) &&
    //     (a1.sommet2.a == a2.sommet1.a && a1.sommet2.b == a2.sommet1.b && a1.sommet2.c == a2.sommet1.c))
    // {
    //     return 1;
    // }

    if ((a1.num1 == a2.num1) && (a1.num2 == a2.num2))  //Si deux sommets de arête1 = arête2, deux arête sont equivalent
        return 1;
    return 0;
}


/**
 * @brief   Compare deux arêtes pour déterminer si l'arête 1 est supérieure à l'arête 2.
 *
 * La comparaison est basée sur les indices des sommets de l'arête.
 * Si les numéros des sommets sont égaux, la fonction compare les numéros des autres sommets.
 *
 * @param   arete1 Première arête
 * @param   arete2 Deuxième arête
 * @return  Un entier négatif si arete1 est inférieure à arete2,
 *          un entier positif si arete1 est supérieure à arete2,
 *          et zéro si les arêtes sont équivalentes.
 */
int estSuperieureA(Arete arete1, Arete arete2)
{
    if (arete1.num1 == arete2.num1)
        return arete1.num2 - arete2.num2;
    return arete1.num1 - arete2.num1;
}


/**
 * @brief   Crée une nouvelle arête dans la liste des arêtes équivalentes.
 * @param   f1 Numéro de la première face
 * @param   f2 Numéro de la deuxième face
 * @return  Un pointeur vers la nouvelle arête crée.
 */
AreteD *newareted(int f1, int f2)
{
    AreteD *newAreteD = malloc(sizeof(AreteD));
    newAreteD->f1 = f1;
    newAreteD->f2 = f2;
    newAreteD->next = NULL;
    return newAreteD;
}



// tri selection

/**
 * @brief   Trie les arêtes par sélection
 * @param   aretes   Tableau des arêtes
 * @param   numEdges Nombre d'arêtes
 * @return  Liste des arêtes équivalentes
 */
AreteD *triSelection(Arete *aretes, int numEdges)
{
    AreteD *equivalentEdgesList = NULL;

    for (int i = 0; i < numEdges - 1; i++)
    {
        int minIndex = i;

        for (int j = i + 1; j < numEdges; j++)
        {
            if (estSuperieureA(aretes[j], aretes[minIndex]) < 0)
            {
                minIndex = j;
            }
        }

        Arete temp = aretes[i];
        aretes[i] = aretes[minIndex];
        aretes[minIndex] = temp;
    }

    for (int i = 0; i < numEdges - 1; i++)
    {
        // printf("%d 1: %d %d    2: %d %d\n", i, aretes[i].num1,aretes[i].num2, aretes[i+1].num1,aretes[i+1].num2);
        if (sontEquivalentes(aretes[i], aretes[i + 1]))
        {
            AreteD *newAreteD = newareted(aretes[i].faceA, aretes[i + 1].faceA);  //Crée une nouvelle arête avec les numéros de faces fournies
            newAreteD->next = equivalentEdgesList;   //Ajoute la nouvelle arête en tête de la liste des arêtes équivalentes
            equivalentEdgesList = newAreteD;
        }
    }

    return equivalentEdgesList;
}


// tas sort

/**
 * @brief   Réorganise le tableau en tas à partir de l'indice spécifié.
 *
 * @param   aretes  Tableau d'arêtes
 * @param   n       Taille du tableau
 * @param   i       Indice à partir duquel réorganiser le tableau en tas
 */
void heapify(Arete *aretes, int n, int i)
{
    int largest = i;    //Initialisation de l'indice du plus grand élément, du fils gauche et du fils droit
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && estSuperieureA(aretes[left], aretes[largest]) > 0)     //Vérifie si le fils gauche est plus grand que le plus grand élément actuel
        largest = left;

    if (right < n && estSuperieureA(aretes[right], aretes[largest]) > 0)
        largest = right;

    if (largest != i)   //Si le plus grand élément n'est pas à l'indice d'origine, échange les éléments et continue la réorganisation
    {
        Arete temp = aretes[i];
        aretes[i] = aretes[largest];
        aretes[largest] = temp;

        heapify(aretes, n, largest);
    }
}


/**
 * @brief   Construit un tas à partir du tableau d'arêtes.
 *
 * @param   aretes  Tableau d'arêtes
 * @param   n       Taille du tableau
 */
void buildHeap(Arete *aretes, int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)   //Parcours du tableau d'arêtes à partir du milieu
        heapify(aretes, n, i);
}


/**
 * @brief   Trie les arêtes par tas
 * @param   aretes   Tableau des arêtes
 * @param   numEdges Nombre d'arêtes
 * @return  Liste des arêtes équivalentes
 */
AreteD *triTas(Arete *aretes, int numEdges)
{
    AreteD *equivalentEdgesList = NULL;

    buildHeap(aretes, numEdges); //Peut être placé dans la main

    for (int i = numEdges - 1; i > 0; i--)
    {
        Arete temp = aretes[0];     //Échange l'élément le plus grand avec le dernier élément non trié
        aretes[0] = aretes[i];
        aretes[i] = temp;

        heapify(aretes, i, 0);
    }

    for (int k = 0; k < numEdges; k++)
    {
        if (sontEquivalentes(aretes[k], aretes[k + 1]))  //Les côtés identiques sont toujours adjacents.
        {
            AreteD *newAreteD = newareted(aretes[k].faceA, aretes[k + 1].faceA);
            newAreteD->next = equivalentEdgesList;
            equivalentEdgesList = newAreteD;
        }
    }

    return equivalentEdgesList;
}


// AVL

/**
 * @brief   Calcule la hauteur de l'arbre AVL à partir du nœud spécifié.
 * @param   a   Nœud de l'arbre AVL
 * @return  La hauteur de l'arbre AVL
 */
int treeHeight(AreteAVL *a)
{
    if (a == NULL)
        return 0;

    int leftHeight = (a->left != NULL) ? a->left->hauteur : -1;
    int rightHeight = (a->right != NULL) ? a->right->hauteur : -1;

    return leftHeight - rightHeight;
}


/**
 * @brief   Calcule le facteur d'équilibre d'un nœud dans un arbre AVL.
 *
 * @param   a   Nœud de l'arbre AVL
 * @return  Le facteur d'équilibre du nœud (différence de hauteur entre le sous-arbre gauche et le sous-arbre droit)
 */
int balanceP(AreteAVL *a)
{
    if (a == NULL)
        return 0;
    else if (a->left == NULL || a->right == NULL)
        return 0;

    return a->left->hauteur - a->right->hauteur;
}


/**
 * @brief   Effectue une rotation droite pour rééquilibrer un nœud dans un arbre AVL.
 *
 * @param   t   Nœud à rééquilibrer
 * @return  Nouveau nœud après la rotation
 */
AreteAVL *rotaD(AreteAVL *t)
{
    AreteAVL *left = t->left;

    t->left = left->right;
    left->right = t;

    left->hauteur = max(treeHeight(left->left), treeHeight(left->right)) + 1;  //Met à jour les hauteurs après la rotation
    t->hauteur = max(treeHeight(t->left), treeHeight(t->right)) + 1;

    return left;
}

AreteAVL *rotaG(AreteAVL *t)   //Pareil comme avant
{
    AreteAVL *right = t->right;

    t->right = right->left;
    right->left = t;

    right->hauteur = max(treeHeight(right->left), treeHeight(right->right)) + 1;
    t->hauteur = max(treeHeight(t->left), treeHeight(t->right)) + 1;

    return right;
}


/**
 * @brief   Rééquilibre un nœud dans un arbre AVL après une opération d'insertion ou de suppression.
 *
 * @param   a   Nœud à rééquilibrer
 * @return  Nouveau nœud après la rééquilibrage
 */
AreteAVL *treeRebalance(AreteAVL *a)
{
    int factor = balanceP(a);    //Calcule le facteur d'équilibre du nœud
    if (factor > 1)    //Si le facteur d'équilibre est supérieur à 1, un déséquilibre à gauche est détecté
    {
        if (balanceP(a->left) >= 0) //LL
            return rotaD(a);
        else //LR
        {
            a->left = rotaG(a->left);
            return rotaD(a);
        }
    }
    else if (factor < -1)
    {
        if (balanceP(a->right) <= 0) //RR
            return rotaG(a);
        else //RL
        {
            a->right = rotaD(a->right);
            return rotaG(a);
        }
    }
    else 
    {
        a->hauteur = max(treeHeight(a->left), treeHeight(a->right)) + 1;
        return a;
    }
}


/**
 * @brief   Vérifie si un nœud dans un arbre AVL est équivalent à nœud on veut insert.
 *
 * @param   a1      Nœud à vérifier
 * @param   num1    Premier numéro du sommet
 * @param   num2    Deuxième numéro du sommet
 * @return  1 si le nœud est équivalent au couple de numéros, sinon 0
 */
int sontEquivalentesA(AreteAVL *a1, int num1, int num2)
{
    if ((a1->num1 == num1) && (a1->num2 == num2))
        return 1;
    return 0;
}

int estSuperieureAVL(AreteAVL *a, int num1, int num2)
{
    if (a->num1 == num1)
        return a->num2 - num2;
    return a->num1 - num1;
}


/**
 * @brief   Crée un nouveau nœud pour un arbre AVL.
 *
 * @param   num1    Premier sommet
 * @param   num2    Deuxième sommet
 * @param   faceA   Numéro de la face associée au nœud
 * @return  Nouveau nœud alloué dynamiquement
 */
AreteAVL *createNode(int num1, int num2, int faceA)
{
    AreteAVL *newNode = malloc(sizeof(AreteAVL));
    newNode->num1 = num1;
    newNode->num2 = num2;
    newNode->left = newNode->right = NULL;
    newNode->faceA = faceA;
    newNode->hauteur = 0;

    return newNode;
}


/**
 * @brief   Insère un nouveau nœud dans un arbre AVL et détecte les arêtes équivalentes.
 *
 * @param   a       Racine de l'arbre AVL
 * @param   num1    Premier sommet du nouveau nœud
 * @param   num2    Deuxième sommet du nouveau nœud
 * @param   faceA   Numéro de la face associée au nouveau nœud
 * @return  Arêtes équivalentes(dual) détectées pendant l'insertion
 */
AreteD *treeInsert(AreteAVL **a, int num1, int num2, int faceA)
{
    AreteAVL *treeRoot = *a;  //Récupère la racine de l'arbre AVL
    if (treeRoot == NULL)
    {
        *a = createNode(num1, num2, faceA);   //Si l'arbre est vide, crée un nouveau nœud à la racine
        return NULL;
    }
    else if (sontEquivalentesA(treeRoot, num1, num2))   // Si le nouveau nœud est équivalent à la racine, détecte les arêtes équivalentes
    {
        AreteD *newAreteD = malloc(sizeof(AreteD));
        newAreteD->f1 = treeRoot->faceA;
        newAreteD->f2 = faceA;
        newAreteD->next = NULL;
        return newAreteD;
    }
    else   //Si le nouveau nœud n'est pas équivalent à la racine, effectue l'insertion récursive
    {
        AreteD *equivalentEdgesList = NULL;

        if (estSuperieureAVL(treeRoot, num1, num2) < 0)   //Si nouveau nœud plus grande que racine
        {
            equivalentEdgesList = treeInsert(&(treeRoot->right), num1, num2, faceA);   //Insert à droite
            treeRoot->right = treeRebalance(treeRoot->right);
        }
        else       //Si nouveau nœud plus petite que racine
        {
            equivalentEdgesList = treeInsert(&(treeRoot->left), num1, num2, faceA);    //Insert à gauche
            treeRoot->left = treeRebalance(treeRoot->left);
        }

        treeRoot = treeRebalance(treeRoot);
        *a = treeRoot;
        return equivalentEdgesList;    //Retourne la liste des arêtes équivalentes détectées pendant l'insertion
    }
}


/**
 * @brief   Trie les arêtes par AVL
 * @param   a        Tableau des arêtes
 * @param   n        Nombre d'arêtes
 * @return  Liste des arêtes équivalentes
 */
AreteD *triAVL(Arete *a, int n)
{
    AreteAVL *newTree = NULL;    //Initialise l'arbre AVL à NULL
    AreteD *equivalentEdgesList = NULL;    

    for (int i = 0; i < n; i++)
    {
        AreteD *x = treeInsert(&newTree, a[i].num1, a[i].num2, a[i].faceA);    //Insère le nœud correspondant à l'arête
        if (x != NULL)    //Si des arêtes équivalentes sont détectées, les ajoute à la liste
        {
            AreteD *newAreteD = x;
            newAreteD->next = equivalentEdgesList;
            equivalentEdgesList = newAreteD;
        }
    }

    return equivalentEdgesList;
}


// coloration bfs
/**
 * @brief   Crée un tableau de centroïdes couleur
 * @param   aretes          Liste des arêtes dual
 * @param   numVertices     Nombre de sommets
 * @param   selectedPoint   Point sélectionné
 * @param   maxDistancePtr  Pointeur vers la distance maximale
 * @return  Tableau de centroïdes couleur
 */
CentoideC *createCentoideArray(AreteD *aretes, int numVertices, int selectedPoint, int *maxDistancePtr)
{
    CentoideC *centoideArray = (CentoideC *)malloc(numVertices * sizeof(CentoideC));

    for (int i = 0; i < numVertices; i++)  //Initialise les distances à -1 pour tous les sommets
    {
        centoideArray[i].distance = -1;
    }

    centoideArray[selectedPoint - 1].distance = 0;  //Initialise la distance du sommet sélectionné à 0
    *maxDistancePtr = 0;    

    int distance = 1;

    while (distance < numVertices)
    {
        for (int i = 0; i < numVertices; i++)
        {
            if (centoideArray[i].distance == distance - 1)  //Si le sommet a une distance égale à distance - 1
            {
                AreteD *tmp = aretes;
                while (tmp != NULL)
                {
                    if (tmp->f1 == i && centoideArray[tmp->f2].distance == -1)  //Met à jour la distance du sommet voisin non visité
                    {
                        centoideArray[tmp->f2].distance = distance;

                        if (distance > *maxDistancePtr)
                        {
                            *maxDistancePtr = distance;
                        }
                    }
                    else if (tmp->f2 == i && centoideArray[tmp->f1].distance == -1)
                    {
                        centoideArray[tmp->f1].distance = distance;

                        if (distance > *maxDistancePtr)
                        {
                            *maxDistancePtr = distance;
                        }
                    }
                    tmp = tmp->next;
                }
            }
        }
        distance++;
    }

    return centoideArray;   // Retourne le tableau de structures CentoideC
}


/**
 * @brief   Écrit un fichier .obj avec des couleurs basées sur la distance
 * @param   centoides       Tableau des centroïdes
 * @param   numface         Nombre de faces
 * @param   filename        Nom du fichier de sortie (par exemple bunny_colored.obj)
 * @param   equivalentAretes Liste des arêtes équivalentes
 */
void writeObjFile(Centoide *centoides, int numface, const char *filename, AreteD *equivalentAretes)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "write\n");
        return;
    }

    int numCentoides = numface;
    int maxDistance;
    CentoideC *cc = createCentoideArray(equivalentAretes, numface, equivalentAretes->f1, &maxDistance);     // crée centoide couleur
    printf("%d\n", maxDistance);
    float parametre = 1.0 / maxDistance;    //Calculer l'unité
    printf("%f\n", parametre);

    for (int i = 0; i < numCentoides; i++)   //Rouge 1 0 0  vert 0 1 0
    {
        fprintf(file, "v %f %f %f %f %f %f\n", centoides[i].centre.a, centoides[i].centre.b, centoides[i].centre.c, (1.0 - cc[i].distance) * parametre, cc[i].distance * parametre, 0.0);
    }

    AreteD *current = equivalentAretes;
    while (current != NULL)
    {
        fprintf(file, "l %d %d\n", current->f1 + 1, current->f2 + 1);
        current = current->next;
    }

    while (equivalentAretes != NULL)    //Free arête dual
    {
        AreteD *temp = equivalentAretes;
        equivalentAretes = equivalentAretes->next;
        free(temp);
    }

    free(cc);
    fclose(file);
}

/*
int facevoisin(Centoide c1, Centoide c2, Face *face)
{
    int tmp = 0;

    Face f1 = face[c1.faceAssocie];
    Face f2 = face[c2.faceAssocie];

    if (f1.v1 == f2.v1 || f1.v1 == f2.v2 || f1.v1 == f2.v3)
        tmp++;
    if (f1.v2 == f2.v1 || f1.v2 == f2.v2 || f1.v2 == f2.v3)
        tmp++;
    if (f1.v3 == f2.v1 || f1.v3 == f2.v2 || f1.v3 == f2.v3)
        tmp++;

    return (tmp == 2) ? 1 : 0;
}

Arete *selectSort(Centoide *c, Face *face, int numf, int numv, int *numA)
{
    int nbDeAreM = numf + numv;
    Arete *a = malloc(sizeof(Arete) * nbDeAreM);
    int tmp = 0;
    int i, j;
    for (i = 0; i < numf; i++)
    {
        for (j = i + 1; j < numf; j++)
        {
            if (facevoisin(c[i], c[j], face))
            {
                a[tmp].sommet1 = c[i];
                a[tmp].sommet2 = c[j];
                tmp++;
            }
        }
    }
    a = realloc(a, sizeof(Arete) * tmp);
    *numA = tmp;
    return a;
}

Vertex *AretetoV(Arete *a, int numA)
{
    int numV = numA * 2;
    Vertex *v = malloc(sizeof(Vertex) * numV);
    int i = 0;
    int j = 0;
    while (i < numV)
    {
        v[i] = a[j].sommet1.centre;
        i++;
        v[i] = a[j].sommet2.centre;
        i++;
        j++;
    }
    return v;
}

Face *AretetoF(Arete *a, int numA)
{
    Face *f = malloc(sizeof(Face) * numA);
    for (int i = 0; i < numA; i++)
    {
        f[i].v1 = i * 2 + 1;
        f[i].v2 = i * 2 + 1;
        f[i].v3 = i * 2 + 2;
    }
    return f;
} */

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Utilisation: %s fichier_entree fichier_sortie\n", argv[0]);
        return 1;
    }

    const char *file = argv[1];
    const char *fileDst = argv[2];
    int numV;      //Nombres des sommets
    int numF;      //Nombres des faces
    Vertex *v;     //Tableau des sommets
    Face *f;       //Tableau des faces
    Arete *a;      //Tableau des arêtes
    int numA;      //Aombres des arêtes
    AreteD *ad;    //Tableau des arêtes dual
    Centoide *c;   //Tableau des centoide

    if (readObj(file, &v, &numV, &f, &numF))
    {
        printf("Lecture réussie du fichier .obj\n");
    }
    else
    {
        printf("Erreur lors de la lecture du fichier .obj\n");
        return 1;
    }

    numA = numF * 3;
    a = generalise(f, numF, v);
    c = calculateCentroids(v, numV, f, numF);

    clock_t start_time = clock();
    ad = triAVL(a, numA);
    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Time used: %f s\n", cpu_time_used);
    writeObjFile(c, numF, fileDst, ad);

    free(v);
    free(f);
    free(a);
    free(c);

    return 0;
}
