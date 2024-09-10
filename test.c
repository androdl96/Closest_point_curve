#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define MAX_LINES 10000  // Ajusta esto según la cantidad de líneas que esperas en el archivo

void find_min_max(double points[][2], int size, double *min_x, double *max_x, double *min_y, double *max_y) {
    *min_x = points[0][0];
    *max_x = points[MAX_LINES-1][0];
    *min_y = points[MAX_LINES-1][1];
    *max_y = points[0][1];

    /*for (int i = 1; i < size; i++) {
        if (points[i][0] < *min_x) *min_x = points[i][0];
        if (points[i][0] > *max_x) *max_x = points[i][0];
        if (points[i][1] < *min_y) *min_y = points[i][1];
        if (points[i][1] > *max_y) *max_y = points[i][1];
    }*/
}

void normalize(double points[][2], int size, double min_x, double max_x, double min_y, double max_y) {
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        //int thread_id = omp_get_thread_num();
        //printf("Thread ID: %d\n", thread_id);
        points[i][0] = (points[i][0] - min_x) / (max_x - min_x);
        points[i][1] = (points[i][1] - min_y) / (max_y - min_y);
    }
}

void normalize_point(double *point, double min_x, double max_x, double min_y, double max_y) {
    point[0] = (point[0] - min_x) / (max_x - min_x);
    point[1] = (point[1] - min_y) / (max_y - min_y);
}

double euclidean_distance(double point1[2], double point2[2]) {
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

int find_closest_point(double points[][2], int size, double target_point[2]) {
    int closest_index = 0;
    double min_distance = euclidean_distance(points[0], target_point);

    for (int i = 1; i < size; i++) {
        double distance = euclidean_distance(points[i], target_point);
        if (distance < min_distance) {
            min_distance = distance;
            closest_index = i;
        }
    }

    return closest_index;
}

int main() {
    clock_t start, end;
    double cpu_time_used;
    
    FILE *file = fopen("line_data.txt", "r");
    if (file == NULL) {
        perror("Debug | Error al abrir el archivo");
        return 1;
    }
    
    double line[MAX_LINES][2];
    int i = 0;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), file) != NULL && i < MAX_LINES) {
        sscanf(buffer, "%lf,%lf", &line[i][0], &line[i][1]);
        i++;
    }
    fclose(file);

    /*for (int j = 5000; j < 5010; j++) {
        printf("Point %d: x = %.10f, y = %.10f\n", j, line[j][0], line[j][1]);
    }*/

    double new_point[2] = {-0.8797029702970297, 29.819387891489004};
    
    // Normalización
    double min_x, max_x, min_y, max_y;
    find_min_max(line, MAX_LINES, &min_x, &max_x, &min_y, &max_y);
    normalize(line, MAX_LINES, min_x, max_x, min_y, max_y);
    normalize_point(new_point, min_x, max_x, min_y, max_y);

    start = clock();
    int closest_index = find_closest_point(line, MAX_LINES, new_point);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("El punto más cercano a new_point es: (%f, %f)\n", line[closest_index][0], line[closest_index][1]);
    printf("Tiempo de ejecución: %f segundos\n", cpu_time_used);
    printf("Debug | Exit\n");
    return 0;
}