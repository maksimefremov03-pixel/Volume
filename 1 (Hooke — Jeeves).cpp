#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>
#include <stdio.h>

using namespace std;

double pi = 2 * acos(0.0);

void input_data(int& length, double ***resn, double ****restriangle)// ввод координат точек
{
    ifstream File("../shar005.stl");
    char buf[50];
    double **temp_n, ***temp_triangle, **n, ***triangle;
    int i, j;

    n = new double* [0];
    triangle = new double** [0];

    while (File >> buf)
    {
        if (strcmp(buf, "normal") == 0)
        {
            temp_n = new double* [length + 1];
            for (i = 0; i < length; i++) temp_n[i] = n[i];
            temp_n[length] = new double[3];
            for (j = 0; j < 3; j++)
            {
                File >> buf;
                temp_n[length][j] = atof(buf);
            }
            delete[] n;
            n = temp_n;
        }
        if (strcmp(buf, "vertex") == 0)
        {
            temp_triangle = new double** [length + 1];
            for (i = 0; i < length; i++) temp_triangle[i] = triangle[i];
            temp_triangle[length] = new double*[3];
            for (j = 0; j < 3; j++)
            {
                temp_triangle[length][j] = new double[3];
                for (int k = 0; k < 3; k++)
                {
                    File >> buf;
                    temp_triangle[length][j][k] = atof(buf);
                }
                File >> buf;
            }
            delete[] triangle;
            triangle = temp_triangle;
            length++;
        }
    }
    File.close();

    *resn = n;
    *restriangle = triangle;
}

double find_res(double* n_0, double* point_0, double** triangle, int i) // проекция ребра тетраэдра на нормаль плоскости
{
    return n_0[0] * (triangle[i][0] - point_0[0]) + n_0[1] * (triangle[i][1] - point_0[1]) + n_0[2] * (triangle[i][2] - point_0[2]);;
}

int locate_triangle(double* point_0, double* n_0, double** triangle, double* res, short int* mark)// расположение треугольника относительно плоскости
{
    double min, max;
    res[0] = find_res(n_0, point_0, triangle, 0);
    res[1] = find_res(n_0, point_0, triangle, 1);
    res[2] = find_res(n_0, point_0, triangle, 2);
    if (res[0] > res[1])
    {
        min = res[1];
        max = res[0];
        mark[0] = 0; mark[1] = 1;
    }
    else
    {
        min = res[0];
        max = res[1];
        mark[0] = 1; mark[1] = 0;
    }

    if (res[2] < min)
    {
        min = res[2];
        mark[1] = 2;
    }
    else
        if (res[2] > max)
        {
            max = res[2];
            mark[0] = 2;
        }

    if (min >= 0) return 0;
    else 
        if (max <= 0) return 2;
        else return 1;
}

short int signum(double* point_0, double* n, double** triangle)// определение знака
{
    double mult = n[0] * (triangle[0][0] - point_0[0]) + n[1] * (triangle[0][1] - point_0[1]) + n[2] * (triangle[0][2] - point_0[2]);//одна из точек треугольника
    if (mult > 0) return 1;
    else
        if (mult < 0) return -1;
        else return 0;
}

double find_abc(double** triangle, int i, int j)// нахождение длины стороны треугольника
{
    return sqrt((triangle[j][0] - triangle[i][0]) * (triangle[j][0] - triangle[i][0]) + (triangle[j][1] - triangle[i][1]) * (triangle[j][1] - triangle[i][1]) + (triangle[j][2] - triangle[i][2]) * (triangle[j][2] - triangle[i][2]));
}

double this_volume(double* point_0, double* n, double** triangle)// нахождение объёма тетраэдра
{
    double p, s, v, a, b, c, h_volume;
    //   x  y  z
    //1) 00 01 02
    //2) 10 11 12
    //3) 20 21 22
    a = find_abc(triangle, 1, 0);
    b = find_abc(triangle, 2, 0);
    c = find_abc(triangle, 2, 1);

    p = (a + b + c) / 2.0;

    s = sqrt(fabs(p * (p - a) * (p - b) * (p - c)));

    h_volume = fabs(n[0] * (triangle[0][0] - point_0[0]) + n[1] * (triangle[0][1] - point_0[1]) + n[2] * (triangle[0][2] - point_0[2])) / sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);

    v = s * h_volume / 3;

    return v;
}

double* intersection(double* n_0, double* point_0, double** triangle, int i, int j)// нахождение точки пересечения
{
    double t, result[3];
    t = find_res(n_0, point_0, triangle, i) / find_res(n_0, triangle[j], triangle, i);
    for (int k = 0; k < 3; k++) result[k] = (triangle[j][k] - triangle[i][k]) * t + triangle[i][k];
    return result;
}

double cutted_volume(double* point_0, double* n, double* n_0, double** triangle, double* res, short int* mark)// нахождение объёма тетраэдра усечённого тетраэдра
{
    int i, j, k = -1, ii = 0;
    double v, temp_p1[3];
    i = mark[0];
    j = mark[1];
    while (k == -1)//находим коэффициент для третьей точки
    {
        if (ii != i && ii != j) k = ii;
        ii++;
    }
    if (res[k] > 0)
    {
        double* temp_p2 = new double[3];
        temp_p2 = intersection(n_0, point_0, triangle, i, j);

        delete[] temp_p2;
    }
    return 0;
}

double volume(double* point_0, double* n_0, double length, double** n, double*** triangle)// нахождение отсечённого объёма
{
    double result = 0, res[3];
    short int sign, mark[2];
    for (int i = 0; i < length; i++)
    {
        switch (locate_triangle(point_0, n_0, triangle[i], res, mark))
        {
        case 0:
            sign = signum(point_0, n[i], triangle[i]);
            result += sign * this_volume(point_0, n[i], triangle[i]);
            break;
        case 1:
            sign = signum(point_0, n[i], triangle[i]);
            result += sign * this_volume(point_0, n[i], triangle[i])/2;/*cutted_volume(point_0, n[i], n_0, triangle[i], res, mark);*/
            break;
        default:
            break;
        }

    }
    return result;
}

void n_create(double* n, double* x)// нахождение компонент вектора нормали по двум углам
{
    n[0] = cos(x[0]) * cos(x[1]);
    n[1] = sin(x[0]) * cos(x[1]);
    n[2] = sin(x[1]);
}

void func(double &f, double s, double* x, double* d, double* point_0, double* n_0, double** n, double length, double*** triangle)// функция, возвращающая очередной отсечённый объём
{
    double* temp_x = new double[2];
    for (int i = 0; i < 2; i++) temp_x[i] = x[i] + s * d[i];
    n_create(n_0, temp_x);
    f = volume(point_0, n_0, length, n, triangle);
    delete[] temp_x;
}

double GoldSech(double left, double rigth, double eps, double* x, double* d, double* point_0, double* n_0, double** n, double length, double*** triangle)// метод золотого сечения
{
    double delta, f1, f2;
    const double fi = (1 + sqrt(5)) / 2;
    while (rigth - left > eps * 0.1)
    {
        delta = (rigth - left) / fi;
        func(f1, rigth - delta, x, d, point_0, n_0, n, length, triangle);
        func(f2, left + delta, x, d, point_0, n_0, n, length, triangle);
        if (f1 > f2)//> - find min,   < - find max
            left = rigth - delta;
        else
            rigth = left + delta;
    }
    return (left + rigth) / 2;
}

void next_point(double* x, double eps, double* d, int& k, double* point_0, double* n_0, double** n, double length, double*** triangle)// функция, возвращающая минимальный отсечённый объём в данном направлении
{
    double s, left = -pi / 4, right = pi / 4;
    s = GoldSech(left, right, eps, x, d, point_0, n_0, n, length, triangle);
    for (int i = 0; i < 2; i++) x[i] = x[i] + s * d[i];
    n_create(n_0, x);
    printf("%2d   %10.7lf    %11.8lf  %11.8lf   %10.7lf      %10.7lf     %10.7lf\nn(%10.7lf      %10.7lf      %10.7lf)\n", k, s, x[0], x[1], volume(point_0, n_0, length, n, triangle), d[0], d[1], n_0[0], n_0[1], n_0[2]);
    k++;
}

double norma(double* x, double* y)// определение длины вектора
{
    double norm = 0;
    double* n1 = new double[3];
    double* n2 = new double[3];
    n_create(n1, x);
    n_create(n2, y);

    for (int i = 0; i < 2; i++) norm += (n1[i] - n2[i]) * (n1[i] - n2[i]);
    delete[] n1;
    delete[] n2;
    return sqrt(norm);
}

int main()
{
    int length = 0;
    double **n, ***triangle;

    input_data(length, &n, &triangle);

    double* n_min = new double[3];
    double* n_0 = new double[3];
    double fi, tetta, d_fi, d_tetta, result, temp_result=0, result_min;
    int numder_of_d_fi = 3, numder_of_d_tetta = 2, min_flag = 0;

    d_fi = 2 * pi / numder_of_d_fi;
    d_tetta = pi / (numder_of_d_tetta + 1);

    double* point_0 = new double[3];
    double eps,/*0.01*/ * x, * y, * d;
    int flag, k, npoint;

    x = new double[2];
    y = new double[2];
    d = new double[2];

    do {
        cout << "Input epsilon" << endl;
        cin >> eps;
        fi = 0;
        tetta = -pi / 2 + d_tetta;
        cout << "Input point" << endl;
        cin >> point_0[0] >> point_0[1] >> point_0[2];
        flag = 0;
        for (int i = 0; i < numder_of_d_fi; i++)
        {
            for (int j = 0; j < numder_of_d_tetta; j++)
            {
                k = 1;
                x[0] = fi;
                x[1] = tetta;
                cout << "fi=" << fi << ", tetta=" << tetta << endl;
                for (int i = 0; i < 2; i++) y[i] = x[i];
                cout << "===RESULT===" << endl;
                cout << " k    s             fi          tetta          f(x)            d[0]           d[1]" << endl;

                while (flag == 0)
                {
                    for (int i = 0; i < 2; i++) d[i] = 0;
                    cout << "Step" << endl;
                    int j = 0;
                    while (j < 2)
                    {
                        d[j] = 1;
                        next_point(y, eps, d, k, point_0, n_0, n, length, triangle);
                        d[j] = 0;
                        j++;
                    }
                    result = volume(point_0, n_0, length, n, triangle);
                    if (norma(x, y) < eps || fabs(temp_result - result) < eps)
                    {
                        flag = 1;
                        n_create(n_0, y);
                        cout << "THIS RESULT" << endl << "n(" << n_0[0] << ", " << n_0[1] << ", " << n_0[2] << ")" << endl << "v=" << result << endl << endl;
                        if (min_flag == 0 || result < result_min)
                        {
                            result_min = result;
                            for (int i = 0; i < 3; i++) n_min[i] = n_0[i];
                            min_flag = 1;
                        }
                    }
                    else 
                    {
                        temp_result = result;
                        for (int i = 0; i < 2; i++) d[i] = y[i] - x[i];
                        for (int i = 0; i < 2; i++) x[i] = y[i];
                        cout << "Jump" << endl;
                        next_point(x, eps, d, k, point_0, n_0, n, length, triangle);
                        for (int i = 0; i < 2; i++) y[i] = x[i];
                    }
                }
                tetta += d_tetta;
                flag = 0;
            }
            tetta = -pi / 2 + d_tetta;
            fi += d_fi;
        }
        cout << "===GLOBAL RESULT===" << endl;
        cout << "n_min(" << n_min[0] << ", " << n_min[1] << ", " << n_min[2] << ")" << endl << "Min volume = " << result_min << endl;
        cout << "Input 1 for new point" << endl;
        cin >> npoint;
    } while (npoint == 1);

    delete[] x;
    delete[] y;
    delete[] d;

    delete[] point_0;

    for (int i = 0; i < length; i++)
    {
        delete[] n[i];
        for (int j = 0; j < 3; j++)
        delete[] triangle[i][j];
    }
    delete[] n;
    delete[] n_0;
    delete[] triangle;

    return 0;
}