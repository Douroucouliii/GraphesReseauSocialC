#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//Récursivité croisée dans nos structures donc appel obligatoire avant
typedef struct p Personne;
typedef struct l MaillonPersonne;

typedef struct p{
    char *nom;
    char *ville;
    MaillonPersonne* abonnements;
    MaillonPersonne* suivi;
} Personne;
//Structure d'une personne, son nom, sa ville et ses abonnements (liste chainée de personne).

typedef struct l{
    Personne *p;
    struct l* suivant;
}MaillonPersonne;
//Structure d'une liste chainée de Personne, une personne et le suivant

//Variables globales du programme
char **villes;
int **distance_villes;
int indVilles;
int indPersonnes;
Personne* tab_personnes;
int *tab_composantes;
int *tab_composantes_fc;


void ajoute_personne(char *nom, char *ville){
    Personne p;
    p.nom=nom;
    p.ville=ville;
    p.abonnements=NULL;
    p.suivi=NULL;
    if(!tab_personnes){
        tab_personnes = (Personne *) malloc(sizeof(Personne));
    }
    else{
        tab_personnes = (Personne *) realloc(tab_personnes,(indPersonnes+1) * sizeof(Personne));
    }
    if(!tab_personnes){
        perror("malloc()/realloc()");
        exit(EXIT_FAILURE);
    }
    tab_personnes[indPersonnes] = p;
    indPersonnes++;
}

Personne get_personne(int i){
    assert(indPersonnes>=i);
    return tab_personnes[i];
}

void ajoute_abo(Personne *p1, Personne *p2){
    assert(p1&&p2);
    MaillonPersonne* new_abo = (MaillonPersonne*) malloc(sizeof(MaillonPersonne));
    if(!new_abo){
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
    MaillonPersonne* abo = p1->abonnements;
    new_abo->p = p2;
    new_abo->suivant = NULL;
    if(!p1->abonnements){
        p1->abonnements = new_abo;
        return;
    }
    while(abo->suivant){
        abo = abo->suivant;
    }
    abo->suivant = new_abo;
}

void enleve_abo(Personne *p1, Personne *p2){
    assert(p1&&p2);
    MaillonPersonne* abo = p1->abonnements;
    if(!abo){
        perror("Impossible d'enlever un abonnement à cet utilisateur");
        exit(EXIT_FAILURE);
    }
    if(abo->p == p2){
        MaillonPersonne* tmp=abo;
        if(abo->suivant) p1->abonnements=p1->abonnements->suivant;
        else abo=NULL;
        free(tmp);
        return;
    }
    while(abo->suivant->p != p2){
        abo=abo->suivant;
    }
    MaillonPersonne* tmp=abo->suivant;
    abo->suivant=abo->suivant->suivant;
    free(tmp);
}

void parsePersonnesInfos(char * chaine_personne){
    char delim[] = "[,] \n\t";
    char * nom_personne = strtok(chaine_personne, delim);
    while(nom_personne){
        ajoute_personne(nom_personne, NULL);
        nom_personne=strtok(NULL, delim);
    }
}

void parseVillesInfos(char * v){
    char delim[] = "[,]) \n\t";
    char * ville = strtok(v, delim);
    while(ville){
        if(!villes){
            villes = (char **) malloc(sizeof(char *));
        }
        else{
            villes = (char **) realloc(villes,(indVilles+1) * sizeof(char *));
        }
        if(!villes){
            perror("malloc()/realloc()");
            exit(EXIT_FAILURE);
        }
        villes[indVilles] = ville;
        indVilles++;
        ville=strtok(NULL, delim);
    }
}

void parsePersonne(char * infos){
    char *saveptr1, *saveptr2;
    char *token1, *token2, *token3;

    //le nom de la personne
    token1 = strtok_r(infos, " :\n", &saveptr1);
    token2 = strtok_r(NULL, " {\n", &saveptr1);
    //les prénoms entre crochets
    token2 = strtok_r(token2, " ]\n", &saveptr1);
    token2 = token2 + 1;
    //la ville
    token3 = strtok_r(NULL, " },\n", &saveptr1);

    for(int i=0; i<indPersonnes; i++){
        if(strcmp(tab_personnes[i].nom, token1) == 0){
            //Parcourir et ajouter les abonnements pour chaque prénom entre crochets
            char *prenom = strtok_r(token2, ", \n", &saveptr2);
            while(prenom) {
                for(int j=0; j<indPersonnes; j++){
                    if(strcmp(tab_personnes[j].nom, prenom)==0){
                        ajoute_abo(&tab_personnes[i], &tab_personnes[j]);
                    }
                }
                prenom = strtok_r(NULL, ", \n", &saveptr2);
            }
            tab_personnes[i].ville=token3;
        }
    }
}

void parseVille(char * dist_v){
    char delim[] = "(,) \n\t";
    //On récupère les 2 villes et le nombre de kilometre entre les deux
    char * ville1 = strtok(dist_v, delim);
    char * kilometre = strtok(NULL, delim);
    char * ville2 = strtok(NULL, delim);
    int x=-1;
    int y=-1;

    //On crée le tableau des villes si on l'a pas encore créé
    if(!distance_villes){
        distance_villes = (int **) malloc(indVilles* sizeof(int *));
        if(!distance_villes){
            perror("malloc()\n");
            exit(EXIT_FAILURE);
        }
        for(int i=0; i<indVilles; i++){
            distance_villes[i] = (int *) calloc(indVilles, sizeof(int));
            if(!distance_villes[i]){
                perror("calloc()\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    //On met 0 aux memes villes qui se croise dans la matrice
    for(int i=0; i<indVilles; i++){
        distance_villes[i][i]=0;
    }
    //On récupère les indices du tableaux où on doit stocker la distance
    for(int i=0; i<indVilles;i++){
        if(strcmp(villes[i], ville1) == 0){
            x=i;
        }if(strcmp(villes[i], ville2) == 0){
            y=i;
        }
    }
    if(x==-1 || y==-1){
        perror("distance_ville()\n");
        exit(EXIT_FAILURE);
    }else{
        distance_villes[x][y] = atoi(kilometre);
        distance_villes[y][x] = atoi(kilometre);
    }
}

void parseFichier(FILE *f){
    static char buffer[500];
    static char p[200];
    static char v[200];
    static char liens[100][200];
    static char dist_v[100][200];

    fgets(buffer, 500, f);

    //Personnes Infos
    fgets(p, 200, f);
    parsePersonnesInfos(p);

    //Villes Infos
    fgets(v, 200, f);
    parseVillesInfos(v);

    fgets(buffer, 500, f);

    //Personne Liens
    for(int i=0;i<indPersonnes;i++){
        fgets(liens[i], 200, f);
        parsePersonne(liens[i]);
    }
    
    fgets(buffer, 500, f);
    fgets(buffer, 500, f);

    //Ville distances
    for(int i=0; i<(indVilles*(indVilles-1))/2; i++){
        fgets(dist_v[i], 200, f);
        parseVille(dist_v[i]);
    }
}

//Remplir le tableau qui permet de calculer la distance entre 2 personnes
void remplir_dist_pers(int cpt, char *dist_pers, Personne *p){
    MaillonPersonne *abo= p->abonnements;
    while(abo){
        for(int i=0; i<indPersonnes; i++){
            if(abo->p == &tab_personnes[i]){
                if(dist_pers[i] == -1) dist_pers[i] = cpt;
            }
        }
        abo=abo->suivant;
    }
}

//Calcul de la distance (longueur de la chaine) entre 2 personnes
char distance_personne(Personne *p1, Personne *p2){
    if(p1==p2) return 0;
    int ind_p2;
    char *dist_pers = (char *) malloc(indPersonnes * sizeof(char));
    if(!dist_pers){
        perror("malloc()\n");
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<indPersonnes; i++){
        dist_pers[i]=-1;
        if(&tab_personnes[i] == p1){
            dist_pers[i]=0;
        }
        if(&tab_personnes[i] == p2){
            ind_p2 = i;
        }
    }
    
    int cpt=0;
    int dedans=1;
    //Récursivité
    while(dist_pers[ind_p2] == -1 && dedans){
        dedans=0;
        for(int i=0; i<indPersonnes; i++){
            if(dist_pers[i] == cpt){
                remplir_dist_pers(cpt+1, dist_pers, &tab_personnes[i]);
                dedans=1;
            }
        }
        cpt++;
    }

    free(dist_pers);
    if(!dedans) return -1;
    return cpt;
}

//Calcul de la distance moyenne entre 2 personnes liées du graphe
float distance_moyenne(){
    char moyenne=0;
    char nb=0;
    char dist=0;
    for(int i=0; i<indPersonnes; i++){
        for(int j=0; j<indPersonnes; j++){
            printf("%d\n", i*indPersonnes+j);
            if(&tab_personnes[i] != &tab_personnes[j]){
                dist = distance_personne(&tab_personnes[i], &tab_personnes[j]);
                if(dist!=-1){
                    moyenne +=dist;
                    nb++;
                }
            }
        }
    }
    if(nb!=0) return (float) moyenne / nb;
    else{
        perror("Pas de liens");
        exit(EXIT_FAILURE);
    }
}

//Rempli les listes chainées pour les suivi_par
void remplir_suivi_par(){
    for(int i=0; i<indPersonnes; i++){
        MaillonPersonne *abo = tab_personnes[i].abonnements;
        while(abo){
            MaillonPersonne *s = (MaillonPersonne*) malloc(sizeof(MaillonPersonne));
            if(!s){
                perror("malloc()\n");
                exit(EXIT_FAILURE);
            }
            s->p=&tab_personnes[i];
            s->suivant=NULL;
            //Si elle n'avait pas encore d'abonné
            if(!abo->p->suivi){
                abo->p->suivi = s;
            }else{
                //On l'ajoute au bout de la liste
                MaillonPersonne *tmp = abo->p->suivi;
                while(tmp->suivant){
                    tmp=tmp->suivant;
                }
                tmp->suivant = s;
            }
            abo=abo->suivant;
        }
    }
}

//attribut le num_compo à tout les voisins
void attribuer_composante(int sommet, int num_compo){
    tab_composantes[sommet] = num_compo;

    MaillonPersonne *abo = tab_personnes[sommet].abonnements;
    while(abo){
        int ind;
        for(int i=0; i<indPersonnes; i++){
            if(&tab_personnes[i] == abo->p){
                ind=i;
            }
        }
        if(tab_composantes[ind] == -1){
            attribuer_composante(ind, num_compo);
        }
        abo=abo->suivant;
    }

    MaillonPersonne *suivi = tab_personnes[sommet].suivi;
    while(suivi){
        int ind;
        for(int i=0; i<indPersonnes; i++){
            if(&tab_personnes[i] == suivi->p){
                ind=i;
            }
        }
        if(tab_composantes[ind] == -1){
            attribuer_composante(ind, num_compo);
        }
        suivi=suivi->suivant;
    }
}

//calcule une composante
void calcul_une_composante_connexe(int num_compo){
    for(int i=0; i<indPersonnes; i++){
        if(tab_composantes[i] == -1){
            attribuer_composante(i, num_compo);
            return;
        }
    }
}

//calcule toutes les composantes
void calcul_les_composante_connexes(){
    tab_composantes = (int *) malloc(indPersonnes *sizeof(int));
    for(int i=0; i<indPersonnes; i++){
        tab_composantes[i]=-1;
    }

    int num_compo = 0;

    while(1){
        int attribue = 0;
        for(int i=0; i<indPersonnes; i++){
            if(tab_composantes[i] == -1){
                attribue=1;
                calcul_une_composante_connexe(num_compo);
                num_compo++;
                break;
            }
        }
        if(!attribue) break;
    }
}

void afficher_personne(Personne p){
    printf("\n[Nom : %s, Ville : %s, Abonnements : {", p.nom, p.ville);
    MaillonPersonne* abo = p.abonnements;
    if(!abo){
        printf("Aucun}]\n\n");
        return; 
    }
    while(abo->suivant){
        printf("%s,", abo->p->nom);
        abo=abo->suivant;
    }
    printf("%s}, Suivi par : {", abo->p->nom);
    MaillonPersonne* suivi = p.suivi;
    if(!suivi){
        printf("Aucun}]\n\n");
        return; 
    }
    while(suivi->suivant){
        printf("%s,", suivi->p->nom);
        suivi=suivi->suivant;
    }
    printf("%s}]\n", suivi->p->nom);
}

void afficher_tab_personne(){
    for(int i=0; i<indPersonnes; i++){
        afficher_personne(tab_personnes[i]);
        printf("\n");
    }
}

void afficher_villes(char ** v){
    if(!v) return;
    printf("Les villes : ");
    for(int i=0; i<indVilles-1; i++){
        printf("%s, ", v[i]);
    }
    printf("%s.\n\n", v[indVilles-1]);
}

void afficher_dist_villes(){
    if(!distance_villes) return;
    //Affichage sous forme d'une ligne (sans répétition)
    /*printf("Distances villes : [ ");
    for(int i=0; i<indVilles;i++){
        for(int j=i+1; j<indVilles-1;j++){
            printf("[%s-%s:%d], ", villes[i], villes[j], distance_villes[i][j]);
        }
        printf("[%s-%s:%d] ", villes[i], villes[indVilles-1], distance_villes[i][indVilles-1]);
    }
    printf(" ]\n\n");*/
    //Affichage en matrice
    printf("\nDistances villes :\n\n");
    for(int i=0; i<indVilles;i++){
        for(int j=0; j<indVilles;j++){
            printf("%4d", distance_villes[i][j]);
        }
        printf("\n");
    }
}

//affiche une composante (toutes les personnes de la composante) selon le numéro de la composante
void affiche_une_composante(int num_compo){
    printf("\nLa composante n°%d contient ces personnes :\n", num_compo);
    for(int i=0; i<indPersonnes; i++){
        if(tab_composantes[i] == num_compo){
            afficher_personne(tab_personnes[i]);
        }
    }
}

//affiche toutes les composantes sur graphe
void affiche_les_composantes(){
    int max=0;
    for(int i=0; i<indPersonnes ; i++){
        if(tab_composantes[i]>max) max=tab_composantes[i];
    }
    for(int i=0; i<=max; i++){
        affiche_une_composante(i);
    }
}

void clear_abo_suivi_personne(MaillonPersonne* l){
    if(!l){
        free(l);
        return;
    }
    while(l){
        MaillonPersonne *tmp = l;
        l = l->suivant;
        free(tmp);
    }
}

void clear_tab_personnes(Personne* tab){
    if(!tab) return;
    for(unsigned int i=0; i<indPersonnes; i++){
        clear_abo_suivi_personne(tab[i].abonnements);
        clear_abo_suivi_personne(tab[i].suivi);
    }
    free(tab);
}

void clear_dist_villes(int ** dist){
    if(!dist) return;
    for(int i=0; i<indVilles; i++){
        free(dist[i]);
    }
    free(dist);
}

int main(){

    FILE *f = fopen("reseau2.txt", "r");
    if(!f){
        perror("Ouverture fichier");
        exit(EXIT_FAILURE);
    }

    tab_personnes = NULL;
    villes=NULL;
    distance_villes=NULL;
    indVilles=0;
    indPersonnes=0;

    //Remplir les données
    parseFichier(f);
    remplir_suivi_par();
    calcul_les_composante_connexes();

    //Test des fonctions
    printf("____________________________________________________________________________________________\n");
    printf("\nDistance entre %s et %s : %d\n", tab_personnes[0].nom, tab_personnes[1].nom, distance_personne(&tab_personnes[0], &tab_personnes[1]));
    printf("Distance entre %s et %s : %d\n", tab_personnes[3].nom, tab_personnes[5].nom, distance_personne(&tab_personnes[3], &tab_personnes[5]));
    printf("Distance entre %s et %s : %d\n", tab_personnes[6].nom, tab_personnes[4].nom, distance_personne(&tab_personnes[6], &tab_personnes[4]));
    printf("Distance entre %s et %s : %d\n", tab_personnes[0].nom, tab_personnes[4].nom, distance_personne(&tab_personnes[0], &tab_personnes[4]));
    printf("Distance moyenne du graphe : %f\n", distance_moyenne());

    //Affichage
    printf("____________________________________________________________________________________________\n");
    afficher_tab_personne();
    printf("____________________________________________________________________________________________\n\n");
    afficher_villes(villes);
    printf("____________________________________________________________________________________________\n");
    afficher_dist_villes();
    printf("____________________________________________________________________________________________\n");
    affiche_les_composantes();

    //Free
    clear_tab_personnes(tab_personnes);
    clear_dist_villes(distance_villes);
    free(villes);
    free(tab_composantes);

    if(fclose(f)){
        perror("Fermeture fichier");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}