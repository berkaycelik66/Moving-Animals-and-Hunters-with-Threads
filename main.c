/**
 * @file main.c 
 * @author adaskin
 * @brief this simulates the movement of a hunter and animal game in 2D site grid
 * @version 0.1
 * @date 2023-05-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

time_t start_time;
time_t end_time;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

typedef enum { BEAR, BIRD, PANDA } AnimalType;

typedef enum { ALIVE, DEAD } AnimalStatus;

typedef struct {
    int x;
    int y;
} Location;

typedef enum { FEEDING, NESTING, WINTERING } SiteType;

typedef struct {
    /** animal can be DEAD or ALIVE*/
    AnimalStatus status;
    /** animal type, bear, bird, panda*/
    AnimalType type;
    /** its location in 2D site grid*/
    Location location;
} Animal;

/*example usage*/
Animal bird, bear, panda;

/** type of Hunter*/
typedef struct {
    /** points indicate the number of animals, a hunter killed*/
    int points;
    /** its location in the site grid*/
    Location location;
} Hunter;

/** type of a site (a cell in the grid)*/
typedef struct {
    /** array of pointers to the hunters located at this site*/
    Hunter **hunters;
    /** the number of hunters at this site*/
    int nhunters;
    /** array of pointers to the animals located at this site*/
    Animal **animals;
    /** the number of animals at this site*/
    int nanimals;
    /** the type of site*/
    SiteType type;
    /** for syncronization*/
    pthread_mutex_t mutex;
} Site;

/** 2D site grid*/
typedef struct {
    /** number of rows, length at the x-coordinate*/
    int xlength;
    /** number of columns, length at the y-coordinate*/
    int ylength;
    /** the 2d site array*/
    Site **sites;
} Grid;

/* initial grid, empty*/
Grid grid = { 0, 0, NULL};

/**
 * @brief initialize grid with random site types
 * @param xlength
 * @param ylength
 * @return Grid
 */
Grid initgrid(int xlength, int ylength) {
    grid.xlength = xlength;
    grid.ylength = ylength;

    grid.sites = (Site **)malloc(sizeof(Site *) * xlength);

    for (int i = 0; i < xlength; i++) {
        grid.sites[i] = (Site *)malloc(sizeof(Site) * ylength);

        for (int j = 0; j < ylength; j++) {
            grid.sites[i][j].animals = NULL;
            grid.sites[i][j].hunters = NULL;
            grid.sites[i][j].nhunters = 0;
            grid.sites[i][j].nanimals = 0;
            double r = rand() / (double)RAND_MAX;
            SiteType st;
            if (r < 0.33)
                st = WINTERING;
            else if (r < 0.66)
                st = FEEDING;
            else
                st = NESTING;
            grid.sites[i][j].type = st;

        }
    }

    return grid;
}

/**
 * @brief
 *
 */
void deletegrid() {
    for (int i = 0; i < grid.xlength; i++) {
        free(grid.sites[i]);
    }

    free(grid.sites);

    grid.sites = NULL;
    grid.xlength = -1;
    grid.ylength = -1;
}
/**
 * @brief prints the number animals and hunters in each site
 * of a given grid
 * @param grid
 */
void printgrid() {
    for (int i = 0; i < grid.xlength; i++) {
        for (int j = 0; j < grid.ylength; j++) {

            Site *site = &grid.sites[i][j];
            int count[3] = { 0 };

            for (int a = 0; a < site->nanimals; a++) {
                Animal *animal = site->animals[a];
                count[animal->type]++;
            }

            printf("|%d-{%d, %d, %d}{%d}|", site->type, count[0], count[1],
                count[2], site->nhunters);

        }
        printf("\n");
    }
}
/**
 * @brief prints the info of a given site
 *
 */
void printsite(Site *site) {
    int count[3] = { 0 }; /* do not forget to initialize*/

    for (int a = 0; a < site->nanimals; a++) {
        Animal *animal = site->animals[a];
        count[animal->type]++;
    }

    printf("|%d-{%d,%d,%d}{%d}|", site->type, count[0], count[1], count[2],
           site->nhunters);
}

/*Animal'ın komşu lokasyona geçmesini sağlar.*/
void moveAnimal(Animal *animal){ 
    int X, Y;
    Site *site;
    X = rand() % grid.xlength;
    Y = rand() % grid.ylength;
    site = &grid.sites[X][Y];
    pthread_mutex_lock(&site->mutex);
    site->animals = (Animal **)realloc(site->animals, sizeof(Animal *) * (site->nanimals + 1));
    site->animals[site->nanimals] = animal;
    site->nanimals++;
    animal->location.x = X;
    animal->location.y = Y;
    pthread_mutex_unlock(&site->mutex);
}

/**
 * @brief  it moves a given hunter or animal
 * randomly in the grid
 * @param args is an animalarray
 * @return void*
 */
void *simulateanimal(void *args) {
    Animal *animal = (Animal*)args;
    Site *present_site;
    while ((animal->status != DEAD) || (time(NULL) < end_time)) {
        present_site = &grid.sites[animal->location.x][animal->location.y];

        if (animal->status == ALIVE) {
            if (present_site->type == NESTING) {
                pthread_t thread_new_animal;
                Animal *newAnimal = (Animal *)malloc(sizeof(Animal));
                newAnimal->status = ALIVE;
                newAnimal->type = animal->type;

                pthread_mutex_lock(&present_site->mutex);
                present_site->animals = (Animal **)realloc(present_site->animals, sizeof(Animal *) * (present_site->nanimals + 1));
                present_site->animals[present_site->nanimals] = newAnimal;
                newAnimal->location.x = animal->location.x;
                newAnimal->location.y = animal->location.y;            
                pthread_mutex_unlock(&present_site->mutex);

                moveAnimal(newAnimal);

                pthread_create(&thread_new_animal, NULL, simulateanimal, (void *)newAnimal); 
                pthread_join(thread_new_animal, NULL);
                
            } else if (present_site->type == FEEDING) {
                double r = rand() / (double)RAND_MAX;
                if (r <= 0.2) {
                    pthread_mutex_lock(&present_site->mutex);
                    present_site->nanimals--;
                    pthread_mutex_unlock(&present_site->mutex);
                    
                    moveAnimal(animal);
                }
            } else if (present_site->type == WINTERING) {
                double r = rand() / (double)RAND_MAX;
                if (r <= 0.5) {
                    pthread_mutex_lock(&present_site->mutex);
                    present_site->nanimals--;
                    pthread_mutex_unlock(&present_site->mutex);

                    moveAnimal(animal);
                } else {
                    /*Ölecek olan hayvan, önce son indexe geçecek ardından öldürülecek.
                                                            (Piazzada dediğiniz kolaylaştırma adımı)*/
                    pthread_mutex_lock(&m1);
                    int index;
                    for (index = 0; index < present_site->nanimals; index++) {
                        if (present_site->animals[index] == animal) {
                            break;
                        }
                    }
                    present_site->animals[index] = present_site->animals[present_site->nanimals - 1];
                    present_site->nanimals--;
                    pthread_mutex_unlock(&m1);
                    
                    animal->status = DEAD;
                }
            }
        }
        usleep(1000); 
    }
    pthread_exit(NULL);
    free(animal);
}

/*Hunter'ın komşu lokasyona geçmesini sağlar*/
void moveHunter(Hunter *hunter){
    int X, Y;
    Site *site;
    X = rand() % grid.xlength;
    Y = rand() % grid.ylength;
    site = &grid.sites[X][Y];

    pthread_mutex_lock(&site->mutex);
    site->hunters = (Hunter **)realloc(site->hunters, sizeof(Hunter *) * (site->nhunters + 1));
    site->hunters[site->nhunters] = hunter;
    site->nhunters++;
    hunter->location.x = X;
    hunter->location.y = Y;
    pthread_mutex_unlock(&site->mutex);
}
/**
 * @brief simulates the moving of a hunter
 *
 * @param args
 * @return void*
 */
void *simulatehunter(void *args) { 
    Hunter *hunter = (Hunter*)args;
    Site *present_site;

    while (time(NULL) < end_time) {
        present_site = &grid.sites[hunter->location.x][hunter->location.y];
        
        pthread_mutex_lock(&present_site->mutex);
        /*Hunter, Animal sayısının 0 olmadığı bir Site'da ise tüm Animalları öldür.*/
        if(present_site->nanimals != 0){
            hunter->points += present_site->nanimals;
            for (int i = 0; i < present_site->nanimals; i++) {
                Animal *animal = present_site->animals[i];
                animal->status = DEAD; 
            }
            present_site->nanimals = 0;          
        } 
        present_site->nhunters--;
        pthread_mutex_unlock(&present_site->mutex);

        moveHunter(hunter);

        usleep(1000);
    }
    pthread_exit(NULL);

}

/**
 * the main function for the simulation
 */
int main(int argc, char* argv[])
{
    srand(time(NULL));
    /*Terminalden verilecek olan input (avcı sayısı)*/
    int hunter_Input; 
    /*Kodun başangıç zamanı*/
    start_time = time(NULL); 
    /*Kodun bitiş zamanı. Kod 1 saniye sonra biteceği için +1 ekledim.*/
    end_time = start_time + 1; 
    
    if (argc < 2) {
        hunter_Input = 2;
    }
    else {
        hunter_Input = atoi(argv[1]);
    }

    initgrid(5, 5);

    pthread_t thread_animal_arr[3];
    for (int i = 0; i < 3; i++) {
        Animal* animal = (Animal *)malloc(sizeof(Animal));
        animal->status = ALIVE;
        animal->type = i;
        animal->location.x = rand() % grid.xlength;
        animal->location.y = rand() % grid.ylength;

        Site* site = &grid.sites[animal->location.x][animal->location.y];

        pthread_mutex_lock(&site->mutex);
        site->animals = (Animal **)realloc(site->animals, sizeof(Animal *) * (site->nanimals + 1));
        site->animals[site->nanimals] = animal;
        site->nanimals++;
        pthread_mutex_unlock(&site->mutex);

        pthread_create(&thread_animal_arr[i], NULL, simulateanimal, (void *)animal);
    }

    pthread_t thread_hunter_arr[hunter_Input];
    for (int i = 0; i < hunter_Input; i++) {
        Hunter* hunter = (Hunter *)malloc(sizeof(Hunter));
        hunter->points = 0;
        hunter->location.x = rand() % grid.xlength;
        hunter->location.y = rand() % grid.ylength;

        Site* site = &grid.sites[hunter->location.x][hunter->location.y];

        pthread_mutex_lock(&site->mutex);    
        site->hunters = (Hunter **)realloc(site->hunters, sizeof(Hunter *) * (site->nhunters + 1));
        site->hunters[site->nhunters] = hunter;
        site->nhunters++;
        pthread_mutex_unlock(&site->mutex);

        pthread_create(&thread_hunter_arr[i], NULL, simulatehunter, (void *)hunter);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(thread_animal_arr[i], NULL);
    }
    for (int i = 0; i < hunter_Input; i++) {
        pthread_join(thread_hunter_arr[i], NULL);
    }

    printgrid();
    deletegrid();

    return 0;
}
