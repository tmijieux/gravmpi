#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *const argv[])
{
    int size, rank;
    int input_stars_count;
    int remote_stars_count;
    int local_stars_count;
    struct star *input_stars;
    struct star *remote_stars;
    struct star *local_stars;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_WORLD_COMM, &size);
    MPI_Comm_rank(MPI_WORLD_COMM, &rank);

    data_struct();
    int star_buf_size;
    lire_fichier_entree("input.data", rank, &local_stars, &star_buf_size);
    remote_stars = copier_les_stars(local_stars, &star_buf_size);
    Initialisation_des_forces();

    Mettre_en_place_les_4_canaux_de_communications();
    // 2 (proc precedant et proc suivant) pour input_stars
    // et 2 (proc precedant et proc suivant) pour remote_stars

    while (t < tmax) {
        int n = size;
        while (n--) {
            Initialiser_reception_input_stars(); // MPI_Start
            Initialiser_envoi_remote_stars();    // MPI_Start
            Calcul_des_forces_appliques_sur_les_local_stars_par_les_remote_stars();

            Finaliser_reception_input_stars(); // MPI_Wait
            Finaliser_envoi_remote_stars();    // MPI_Wait

            SWAP_POINTEUR(local_stars, remote_stars);
        }

        // a partir d'ici la valeur de la force est "complète"
        Calculer_les_vitesses_et_positions();

        // MPI reduce sur les distances minimales

        Remettre_les_forces_a_zero();
        double pas = Se_mettre_daccord_sur_le_pas_de_temps();
        t = t + pas;
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
