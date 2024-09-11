#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define MAX_LINES 908 // Hacer esto no manual, maybe con wc
#define EARTH_RADIUS_KM 6371.0
#define PI 3.14159

double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

double haversine_distance(double point1[2], double point2[2]) {
    double lat1 = degrees_to_radians(point1[0]);
    double lon1 = degrees_to_radians(point1[1]);
    double lat2 = degrees_to_radians(point2[0]);
    double lon2 = degrees_to_radians(point2[1]);
    
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS_KM * c;
}

int find_closest_point(double points[][2], int size, double target_point[2]) {
    int closest_index = 0;
    double min_distance = haversine_distance(points[0], target_point);

    for (int i = 1; i < size; i++) {
        double distance = haversine_distance(points[i], target_point);
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

    FILE *file = fopen("Closest_point_curve/line_BEL_360.txt", "r");
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

    double new_point[2] = {51.1008044, 3.433197};

    start = clock();
    int closest_index = find_closest_point(line, MAX_LINES, new_point);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("El punto más cercano a new_point es: (%f, %f)\n", line[closest_index][0], line[closest_index][1]);
    printf("Tiempo de ejecución: %f segundos\n", cpu_time_used);

    start = clock();
    int num_points = 50;
    int* coarse_search_indexes = (int*) malloc(num_points * sizeof(int));
    //double (*coarse_search_points)[2] = malloc(num_points * sizeof(*coarse_search_points));
    double coarse_search_points[num_points][2];

    linspace(0, MAX_LINES - 1, num_points, coarse_search_indexes);

    for (int i = 0; i < num_points; i++) {
        coarse_search_points[i][0] = line[coarse_search_indexes[i]][0];
        coarse_search_points[i][1] = line[coarse_search_indexes[i]][1];
    }
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