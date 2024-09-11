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

void linspace(int start, int end, int num, int* result) {
    double step = (double)(end - start) / (num - 1);
    for (int i = 0; i < num; i++) {
        result[i] = start + (int)(i * step);
    }
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
    
    start = clock();
    int num_points = 20;
    int* coarse_search_indexes = (int*) malloc(num_points * sizeof(int));
    //double (*coarse_search_points)[2] = malloc(num_points * sizeof(*coarse_search_points));
    double coarse_search_points[num_points][2];

    linspace(0, MAX_LINES - 1, num_points, coarse_search_indexes);

    /*for (int i = 0; i < num_points; i++) {
        printf("%d ", coarse_search_indexes[i]);
    }
    printf("\n");*/

    for (int i = 0; i < num_points; i++) {
        coarse_search_points[i][0] = line[coarse_search_indexes[i]][0];
        coarse_search_points[i][1] = line[coarse_search_indexes[i]][1];
    }

    /*for (int i = 0; i < num_points; i++) {
            printf("%f, %f\n", coarse_search_points[i][0],coarse_search_points[i][1]);
    }*/
    int closest_index_coarse = find_closest_point(coarse_search_points, num_points, new_point);

    int start_index, end_index;
    if (closest_index_coarse > 0) start_index = coarse_search_indexes[closest_index_coarse - 1]; else start_index = coarse_search_indexes[0];
    if (closest_index_coarse < num_points - 1) end_index = coarse_search_indexes[closest_index_coarse + 1]; else end_index = coarse_search_indexes[num_points - 1];

    double line_segment[end_index-start_index][2];
    for (int i = 0; i <= end_index-start_index; i++) {
        line_segment[i][0]=line[start_index+i][0];
        line_segment[i][1]=line[start_index+i][1];
    }
    int closest_index2= find_closest_point(line_segment, end_index-start_index, new_point);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución (II): %f segundos\n", cpu_time_used);

    printf("El punto más cercano a new_point es (II): (%f, %f)\n", line_segment[closest_index2][0], line_segment[closest_index2][1]);

    printf("Debug | Exit\n");
    return 0;
}