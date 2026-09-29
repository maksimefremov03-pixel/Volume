#include <iostream>
#include <stdio.h>
#include <cmath>
using namespace std;

double pi = 2 * acos(0.0);

void function(double fi, double tetta, double* point)
{
    // if(tetta<=-0.45 || tetta>=0.45)
    // {
    //     point[0]=cos(fi)*cos(tetta);
    //     point[1]=sin(fi)*cos(tetta);
    //     point[2]=sin(tetta);
    // }
    // else
    // {
    //     point[0]=2*cos(fi)*cos(tetta*atan(2)/0.45);
    //     point[1]=2*sin(fi)*cos(tetta*atan(2)/0.45);
    //     point[2]=sin(tetta*atan(2)/0.45)/2;
    // }

    // point[0]=sin(3*fi)*cos(fi)*cos(tetta);
    // point[1]=fi*sin(fi)*cos(tetta);
    // point[2]=sin(tetta);

    // point[0]=(-sin(fi) + 2*cos(fi))*cos(tetta);
    // point[1]=(3*cos(fi) + sin(fi))*cos(tetta);//эллипс
    // point[2]=sin(tetta);

    point[0] = cos(fi) * cos(tetta);
    point[1] = sin(fi) * cos(tetta);//сфера
    point[2] = sin(tetta);
}

void net(double fi_max, double fi_min, double tetta_max, double tetta_min, int numder_of_fi, int numder_of_tetta, double*** node, double* down_tetta, double* up_tetta)//сетка для всех углов
{
    int i = 0, j = 0;
    double fi, tetta, step_fi, step_tetta;

    step_fi = (fi_max - fi_min) / numder_of_fi;
    step_tetta = (tetta_max - tetta_min) / (numder_of_tetta + 1);

    fi = fi_min;
    tetta = tetta_min + step_tetta;

    function(fi, tetta_min, down_tetta);
    function(fi, tetta_max, up_tetta);

    for (int i = 0; i < numder_of_fi; i++)
    {
        for (int j = 0; j < numder_of_tetta; j++)
        {
            function(fi, tetta, node[i][j]);
            tetta += step_tetta;
        }
        tetta = tetta_min + step_tetta;
        fi += step_fi;
    }
}

void this_net(int numder_of_fi, int numder_of_tetta, double*** node, double*** thisnode, double* point_0, double* n, double* down_tetta, double* up_tetta, double* this_down_tetta, double* this_up_tetta)//крутим сетку
{
    int i = 0, j = 0;
    double alfa_xy, alfa_yz=0, x, y;

    double* temp_n = new double[3];

    if (n[0] == 0 && n[1] == 0) alfa_xy = 0;
    else
        if (n[0] >= 0) alfa_xy = acos(n[1] / sqrt(n[0] * n[0] + n[1] * n[1]));
        else alfa_xy = 2 * pi - acos(n[1] / sqrt(n[0] * n[0] + n[1] * n[1]));

    x = n[0] * cos(alfa_xy) - n[1] * sin(alfa_xy);
    y = n[0] * sin(alfa_xy) + n[1] * cos(alfa_xy);

    temp_n[1] = y * cos(alfa_yz) - n[2] * sin(alfa_yz);
    temp_n[2] = y * sin(alfa_yz) + n[2] * cos(alfa_yz);

    if (temp_n[1] >= 0) alfa_yz = acos(temp_n[2] / sqrt(temp_n[1] * temp_n[1] + temp_n[2] * temp_n[2]));
    else alfa_yz = 2 * pi - acos(temp_n[2] / sqrt(temp_n[1] * temp_n[1] + temp_n[2] * temp_n[2]));
    delete temp_n;

    for (int i = 0; i < 3; i++)
    {
        this_down_tetta[i] = down_tetta[i] - point_0[i];
        this_up_tetta[i] = up_tetta[i] - point_0[i];
    }

    x = this_down_tetta[0] * cos(alfa_xy) - this_down_tetta[1] * sin(alfa_xy);
    y = this_down_tetta[0] * sin(alfa_xy) + this_down_tetta[1] * cos(alfa_xy);

    this_down_tetta[1] = y * cos(alfa_yz) - this_down_tetta[2] * sin(alfa_yz);
    this_down_tetta[2] = y * sin(alfa_yz) + this_down_tetta[2] * cos(alfa_yz);

    this_down_tetta[0] = x;

    x = this_up_tetta[0] * cos(alfa_xy) - this_up_tetta[1] * sin(alfa_xy);
    y = this_up_tetta[0] * sin(alfa_xy) + this_up_tetta[1] * cos(alfa_xy);

    this_up_tetta[1] = y * cos(alfa_yz) - this_up_tetta[2] * sin(alfa_yz);
    this_up_tetta[2] = y * sin(alfa_yz) + this_up_tetta[2] * cos(alfa_yz);

    this_up_tetta[0] = x;

    for (int i = 0; i < numder_of_fi; i++)
    {
        for (int j = 0; j < numder_of_tetta; j++)
        {
            thisnode[i][j][0] = node[i][j][0] - point_0[0]; //x
            thisnode[i][j][1] = node[i][j][1] - point_0[1]; //y
            thisnode[i][j][2] = node[i][j][2] - point_0[2]; //z

            x = thisnode[i][j][0] * cos(alfa_xy) - thisnode[i][j][1] * sin(alfa_xy);
            y = thisnode[i][j][0] * sin(alfa_xy) + thisnode[i][j][1] * cos(alfa_xy);

            thisnode[i][j][1] = y * cos(alfa_yz) - thisnode[i][j][2] * sin(alfa_yz);
            thisnode[i][j][2] = y * sin(alfa_yz) + thisnode[i][j][2] * cos(alfa_yz);

            thisnode[i][j][0] = x;
        }
    }
}

double sсal_mult_n(double* point1, double* point2, double* point3)
{
    return (point2[0] - point1[0]) * (point3[1] - point1[1]) - (point3[0] - point1[0]) * (point2[1] - point1[1]);
}

double this_volume(double* point1, double* point2, double* point3)
{
    double p, s, v, a, b, c, min_z, num_min;
    if (point1[2] > point2[2])
    {
        min_z = point2[2];
        num_min = 2;
    }
    else
    {
        min_z = point1[2];
        num_min = 1;
    }
    if (min_z > point3[2])
    {
        min_z = point3[2];
        num_min = 3;
    }

    a = sqrt((point2[0] - point1[0]) * (point2[0] - point1[0]) + (point2[1] - point1[1]) * (point2[1] - point1[1]));
    b = sqrt((point3[0] - point1[0]) * (point3[0] - point1[0]) + (point3[1] - point1[1]) * (point3[1] - point1[1]));
    c = sqrt((point3[0] - point2[0]) * (point3[0] - point2[0]) + (point3[1] - point2[1]) * (point3[1] - point2[1]));

    p = (a + b + c) / 2.0;

    s = sqrt(fabs(p * (p - a) * (p - b) * (p - c)));//погрешность - одна из сторон м б больше полупериметра -> корень из отрицательного числа

    v = s * min_z;

    double h, a_trap, b_trap, h_trap, s_trap;

    if (num_min == 1) { a_trap = point2[2] - min_z; b_trap = point3[2] - min_z; h_trap = c; }
    else
        if (num_min == 2) { a_trap = point1[2] - min_z; b_trap = point3[2] - min_z; h_trap = b; }
        else
            if (num_min == 3) { a_trap = point1[2] - min_z; b_trap = point2[2] - min_z; h_trap = a; }

    s_trap = h_trap * (a_trap + b_trap) / 2;

    h = 2 * s / h_trap;

    v += s_trap * h / 3;

    return v;
}

double volume(int numder_of_fi, int numder_of_tetta, double*** thisnode, double* this_down_tetta, double* this_up_tetta)
{
    double result = 0, z_eps = 0.00001, res_sсal_mult_n;
    short int sign;
    if (this_down_tetta[2] >= 0 - z_eps)//объём для треугольников вокруг нижней точки
    {
        for (int i = 0; i < numder_of_fi - 1; i++)
            if (thisnode[i][0][2] >= 0 - z_eps && thisnode[i + 1][0][2] >= 0 - z_eps)
            {
                res_sсal_mult_n = sсal_mult_n(this_down_tetta, thisnode[i + 1][0], thisnode[i][0]);
                if (res_sсal_mult_n != 0)
                {
                    if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                    result += sign * this_volume(this_down_tetta, thisnode[i + 1][0], thisnode[i][0]);
                }
            }
        if (thisnode[numder_of_fi - 1][0][2] >= 0 - z_eps && thisnode[0][0][2] >= 0 - z_eps)//сшивание по фи
        {
            res_sсal_mult_n = sсal_mult_n(this_down_tetta, thisnode[0][0], thisnode[numder_of_fi - 1][0]);
            if (res_sсal_mult_n != 0)
            {
                if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                result += sign * this_volume(this_down_tetta, thisnode[0][0], thisnode[numder_of_fi - 1][0]);
            }
        }
    }
    if (this_up_tetta[2] >= 0 - z_eps)//объём для треугольников вокруг верхней точки
    {
        for (int i = 0; i < numder_of_fi - 1; i++)
            if (thisnode[i][numder_of_tetta - 1][2] >= 0 - z_eps && thisnode[i + 1][numder_of_tetta - 1][2] >= 0 - z_eps)
            {
                res_sсal_mult_n = sсal_mult_n(this_up_tetta, thisnode[i][numder_of_tetta - 1], thisnode[i + 1][numder_of_tetta - 1]);
                if (res_sсal_mult_n != 0)
                {
                    if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                    result += sign * this_volume(this_up_tetta, thisnode[i][numder_of_tetta - 1], thisnode[i + 1][numder_of_tetta - 1]);
                }
            }
        if (thisnode[numder_of_fi - 1][numder_of_tetta - 1][2] >= 0 - z_eps && thisnode[0][numder_of_tetta - 1][2] >= 0 - z_eps)//сшивание по фи
        {
            res_sсal_mult_n = sсal_mult_n(this_up_tetta, thisnode[numder_of_fi - 1][numder_of_tetta - 1], thisnode[0][numder_of_tetta - 1]);
            if (res_sсal_mult_n != 0)
            {
                if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                result += sign * this_volume(this_up_tetta, thisnode[numder_of_fi - 1][numder_of_tetta - 1], thisnode[0][numder_of_tetta - 1]);
            }
        }
    }
    for (int i = 0; i < numder_of_fi - 1; i++)//объём для четырёхугольников
    {
        for (int j = 0; j < numder_of_tetta - 1; j++)
        {
            if (thisnode[i][j][2] >= 0 - z_eps && thisnode[i + 1][j][2] >= 0 - z_eps && thisnode[i][j + 1][2] >= 0 - z_eps && thisnode[i + 1][j + 1][2] >= 0 - z_eps)//4 точки
            {
                res_sсal_mult_n = sсal_mult_n(thisnode[i][j], thisnode[i + 1][j], thisnode[i][j + 1]);
                if (res_sсal_mult_n != 0)
                {
                    if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                    result += sign * this_volume(thisnode[i][j], thisnode[i + 1][j], thisnode[i][j + 1]);
                }
                res_sсal_mult_n = sсal_mult_n(thisnode[i][j + 1], thisnode[i + 1][j], thisnode[i + 1][j + 1]);
                if (res_sсal_mult_n != 0)
                {
                    if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                    result += sign * this_volume(thisnode[i][j + 1], thisnode[i + 1][j], thisnode[i + 1][j + 1]);
                }
            }
            else
            {
                if (thisnode[i][j][2] >= 0 - z_eps && thisnode[i + 1][j][2] >= 0 - z_eps)//12
                {
                    if (thisnode[i][j + 1][2] >= 0 - z_eps)//123
                    {
                        res_sсal_mult_n = sсal_mult_n(thisnode[i][j], thisnode[i + 1][j], thisnode[i][j + 1]);
                        if (res_sсal_mult_n != 0)
                        {
                            if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                            result += sign * this_volume(thisnode[i][j], thisnode[i + 1][j], thisnode[i][j + 1]);
                        }
                    }
                    if (thisnode[i + 1][j + 1][2] >= 0 - z_eps)//124
                    {
                        res_sсal_mult_n = sсal_mult_n(thisnode[i][j], thisnode[i + 1][j], thisnode[i + 1][j + 1]);
                        if (res_sсal_mult_n != 0)
                        {
                            if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                            result += sign * this_volume(thisnode[i][j], thisnode[i + 1][j], thisnode[i + 1][j + 1]);
                        }
                    }
                }
                if (thisnode[i][j + 1][2] >= 0 - z_eps && thisnode[i + 1][j + 1][2] >= 0 - z_eps)//34
                {
                    if (thisnode[i][j][2] >= 0 - z_eps)//134
                    {
                        res_sсal_mult_n = sсal_mult_n(thisnode[i][j], thisnode[i + 1][j + 1], thisnode[i][j + 1]);
                        if (res_sсal_mult_n != 0)
                        {
                            if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                            result += sign * this_volume(thisnode[i][j], thisnode[i + 1][j + 1], thisnode[i][j + 1]);
                        }
                    }
                    if (thisnode[i + 1][j][2] >= 0 - z_eps)//234
                    {
                        res_sсal_mult_n = sсal_mult_n(thisnode[i][j + 1], thisnode[i + 1][j], thisnode[i + 1][j + 1]);
                        if (res_sсal_mult_n != 0)
                        {
                            if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                            result += sign * this_volume(thisnode[i][j + 1], thisnode[i + 1][j], thisnode[i + 1][j + 1]);
                        }
                    }
                }
            }
            //cout<<j<<" "<<result<<endl;
        }
    }

    for (int j = 0; j < numder_of_tetta - 1; j++)//сшивание первых и последних точек по фи
    {
        if (thisnode[numder_of_fi - 1][j][2] >= 0 - z_eps && thisnode[0][j][2] >= 0 - z_eps && thisnode[numder_of_fi - 1][j + 1][2] >= 0 - z_eps && thisnode[0][j + 1][2] >= 0 - z_eps)
        {
            res_sсal_mult_n = sсal_mult_n(thisnode[numder_of_fi - 1][j], thisnode[0][j], thisnode[numder_of_fi - 1][j + 1]);
            if (res_sсal_mult_n != 0)
            {
                if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                result += sign * this_volume(thisnode[numder_of_fi - 1][j], thisnode[0][j], thisnode[numder_of_fi - 1][j + 1]);
            }
            res_sсal_mult_n = sсal_mult_n(thisnode[0][j], thisnode[0][j + 1], thisnode[numder_of_fi - 1][j + 1]);
            if (res_sсal_mult_n != 0)
            {
                if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                result += sign * this_volume(thisnode[0][j], thisnode[0][j + 1], thisnode[numder_of_fi - 1][j + 1]);
            }
        }
        else
        {
            if (thisnode[numder_of_fi - 1][j][2] >= 0 - z_eps && thisnode[0][j][2] >= 0 - z_eps)
            {
                if (thisnode[numder_of_fi - 1][j + 1][2] >= 0 - z_eps)
                {
                    res_sсal_mult_n = sсal_mult_n(thisnode[numder_of_fi - 1][j], thisnode[0][j], thisnode[numder_of_fi - 1][j + 1]);
                    if (res_sсal_mult_n != 0)
                    {
                        if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                        result += sign * this_volume(thisnode[numder_of_fi - 1][j], thisnode[0][j], thisnode[numder_of_fi - 1][j + 1]);
                    }
                }
                if (thisnode[0][j + 1][2] >= 0 - z_eps)
                {
                    res_sсal_mult_n = sсal_mult_n(thisnode[numder_of_fi - 1][j], thisnode[0][j], thisnode[0][j + 1]);
                    if (res_sсal_mult_n != 0)
                    {
                        if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                        result += sign * this_volume(thisnode[numder_of_fi - 1][j], thisnode[0][j], thisnode[0][j + 1]);
                    }
                }
            }
            if (thisnode[numder_of_fi - 1][j + 1][2] >= 0 - z_eps && thisnode[0][j + 1][2] >= 0 - z_eps)
            {
                if (thisnode[numder_of_fi - 1][j][2] >= 0 - z_eps)
                {
                    res_sсal_mult_n = sсal_mult_n(thisnode[numder_of_fi - 1][j], thisnode[0][j + 1], thisnode[numder_of_fi - 1][j + 1]);
                    if (res_sсal_mult_n != 0)
                    {
                        if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                        result += sign * this_volume(thisnode[numder_of_fi - 1][j], thisnode[0][j + 1], thisnode[numder_of_fi - 1][j + 1]);
                    }
                }
                if (thisnode[0][j][2] >= 0 - z_eps)
                {
                    res_sсal_mult_n = sсal_mult_n(thisnode[0][j], thisnode[0][j + 1], thisnode[numder_of_fi - 1][j + 1]);
                    if (res_sсal_mult_n != 0)
                    {
                        if (res_sсal_mult_n > 0) sign = 1; else sign = -1;
                        result += sign * this_volume(thisnode[0][j], thisnode[0][j + 1], thisnode[numder_of_fi - 1][j + 1]);
                    }
                }
            }
        }
    }

    return result;
}

void n_create(double* n, double* x)
{
    n[0] = cos(x[0]) * cos(x[1]);
    n[1] = sin(x[0]) * cos(x[1]);
    n[2] = sin(x[1]);
}

double func(double s, double* x, int numvar, double* d, int numder_of_fi, int numder_of_tetta, double*** node, double*** thisnode, double* down_tetta, double* up_tetta, double* this_down_tetta, double* this_up_tetta, double* point_0, double* n)
{
    double* temp_x = new double[numvar];
    for (int i = 0; i < numvar; i++) temp_x[i] = x[i] + s * d[i];
    n_create(n, temp_x);
    this_net(numder_of_fi, numder_of_tetta, node, thisnode, point_0, n, down_tetta, up_tetta, this_down_tetta, this_up_tetta);
    double f = volume(numder_of_fi, numder_of_tetta, thisnode, this_down_tetta, this_up_tetta);
    delete[] temp_x;
    return f;
}

double GoldSech(double left, double rigth, double eps, double* x, int numvar, double* d, int numder_of_fi, int numder_of_tetta, double*** node, double*** thisnode, double* down_tetta, double* up_tetta, double* this_down_tetta, double* this_up_tetta, double* point_0, double* n)
{
    double delta;
    const double fi = (1 + sqrt(5)) / 2;
    while (rigth - left > eps * 0.1)
    {
        delta = (rigth - left) / fi;
        if (func(rigth - delta, x, numvar, d, numder_of_fi, numder_of_tetta, node, thisnode, down_tetta, up_tetta, this_down_tetta, this_up_tetta, point_0, n) > func(left + delta, x, numvar, d, numder_of_fi, numder_of_tetta, node, thisnode, down_tetta, up_tetta, this_down_tetta, this_up_tetta, point_0, n))//> - find min,   < - find max
            left = rigth - delta;
        else
            rigth = left + delta;
    }
    return (left + rigth) / 2;
}

void next_point(double* x, int numvar, double fi_max, double fi_min, double tetta_max, double tetta_min, double eps, double* d, int& k, int numder_of_fi, int numder_of_tetta, double*** node, double*** thisnode, double* down_tetta, double* up_tetta, double* this_down_tetta, double* this_up_tetta, double* point_0, double* n)
{
    double s, left = -pi / 4, right = pi / 4;
    s = GoldSech(left, right, eps, x, numvar, d, numder_of_fi, numder_of_tetta, node, thisnode, down_tetta, up_tetta, this_down_tetta, this_up_tetta, point_0, n);
    for (int i = 0; i < numvar; i++) x[i] = x[i] + s * d[i];
    printf("%2d   %10.7lf    %11.8lf  %11.8lf   %10.7lf      %10.7lf     %10.7lf\nn(%10.7lf      %10.7lf      %10.7lf)\n", k, s, x[0], x[1], volume(numder_of_fi, numder_of_tetta, thisnode, this_down_tetta, this_up_tetta), d[0], d[1], n[0], n[1], n[2]);
    k++;
}

double norma(double* x, double* y, int numvar)
{
    double norm = 0;
    double* n1 = new double[3];
    double* n2 = new double[3];
    n_create(n1, x);
    n_create(n2, y);

    for (int i = 0; i < numvar; i++) norm += (n1[i] - n2[i]) * (n1[i] - n2[i]);
    delete[] n1;
    delete[] n2;
    return sqrt(norm);
}

int main()
{
    double fi_max, fi_min, tetta_max, tetta_min;
    int numder_of_fi, numder_of_tetta; //число разбиений

    fi_max = 2 * pi;
    fi_min = 0;
    tetta_max = pi / 2;
    tetta_min = -pi / 2;
    numder_of_fi = 360;//360
    numder_of_tetta = 178;//178

    //cout<<"Enter point"<<endl;
    double* point_0 = new double[3];
    //for(int i=0; i<3; i++) cin>>point_0[i];
    point_0[0] = 2.0;
    point_0[1] = 2.0;
    point_0[2] = 2.0;

    double*** node = new double** [numder_of_fi];
    for (int i = 0; i < numder_of_fi; i++)
    {
        node[i] = new double* [numder_of_tetta];
        for (int j = 0; j < numder_of_tetta; j++)
            node[i][j] = new double[3];
    }
    double* down_tetta = new double[3];
    double* up_tetta = new double[3];

    net(fi_max, fi_min, tetta_max, tetta_min, numder_of_fi, numder_of_tetta, node, down_tetta, up_tetta);       //формируем сетку

    // for(int i=0; i < numder_of_fi; i++)
    // {
    //     for(int j=0; j < numder_of_tetta; j++)
    //         cout<<"("<<node[i][j][0]<<" "<<node[i][j][1]<<" "<<node[i][j][2]<<") ";
    //     cout<<endl;
    // }

    double*** thisnode = new double** [numder_of_fi];
    for (int i = 0; i < numder_of_fi; i++)
    {
        thisnode[i] = new double* [numder_of_tetta];
        for (int j = 0; j < numder_of_tetta; j++)
            thisnode[i][j] = new double[3];
    }
    double* this_down_tetta = new double[3];
    double* this_up_tetta = new double[3];

    double* n = new double[3];
    double* n_min = new double[3];
    double fi, tetta, d_fi, d_tetta, result, temp_result=0, result_min;
    int numder_of_d_fi = 3, numder_of_d_tetta = 2, min_flag = 0;

    d_fi = 2 * pi / numder_of_d_fi;
    d_tetta = pi / (numder_of_d_tetta + 1);
    fi = fi_min;
    tetta = tetta_min + d_tetta;
    // for(int i=0; i < 16; i++)
    // {
    //     for(int j=0; j < 5; j++)
    //     {
    //         n[0]=cos(fi)*cos(tetta);
    //         n[1]=sin(fi)*cos(tetta);
    //         n[2]=sin(tetta);
    //         this_net(numder_of_fi, numder_of_tetta, node, thisnode, point_0, n, down_tetta, up_tetta, this_down_tetta, this_up_tetta);
    //         result = volume(numder_of_fi, numder_of_tetta, thisnode, this_down_tetta, this_up_tetta);
    //         cout<<result<<" ";
    //         tetta+=pi/4;
    //     }
    //     tetta=tetta_min;
    //     fi+=pi/8;
    //     cout<<endl;
    // }

    // n[0]=cos(5*pi/4);
    // n[1]=sin(5*pi/4);
    // n[2]=0;
    // this_net(numder_of_fi, numder_of_tetta, node, thisnode, point_0, n, down_tetta, up_tetta, this_down_tetta, this_up_tetta);
    // result = volume(numder_of_fi, numder_of_tetta, thisnode, this_down_tetta, this_up_tetta);
    // cout<<result<<" ";
    // <<"n("<<n[0]<<","<<n[1]<<","<<n[2]<<") "

    double eps = 0.01, s, * x, * y, * d;
    int flag = 0, numvar = 2, k;

    x = new double[numvar];
    y = new double[numvar];
    d = new double[numvar];
    for (int i = 0; i < numder_of_d_fi; i++)
    {
        for (int j = 0; j < numder_of_d_tetta; j++)
        {
            k = 1;
            //for(int i=0; i<numvar; i++) x[i]=0.0;
            x[0] = fi;
            x[1] = tetta;
            cout << "fi=" << fi << ", tetta=" << tetta << endl;
            for (int i = 0; i < numvar; i++) y[i] = x[i];
            cout << "===RESULT===" << endl;
            cout << " k    s             fi          tetta          f(x)            d[0]           d[1]" << endl;

            while (flag == 0)
            {
                for (int i = 0; i < numvar; i++) d[i] = 0;
                cout << "Step" << endl;
                int j = 0;
                while (j < numvar)
                {
                    d[j] = 1;
                    next_point(y, numvar, fi_max, fi_min, tetta_max, tetta_min, eps, d, k, numder_of_fi, numder_of_tetta, node, thisnode, down_tetta, up_tetta, this_down_tetta, this_up_tetta, point_0, n);
                    d[j] = 0;
                    j++;
                }
                result = volume(numder_of_fi, numder_of_tetta, thisnode, this_down_tetta, this_up_tetta);
                if (norma(x, y, numvar) < eps || fabs(temp_result - result) < eps)
                {
                    flag = 1;
                    n_create(n, y);
                    //result=volume(numder_of_fi, numder_of_tetta, thisnode, this_down_tetta, this_up_tetta);
                    cout << "THIS RESULT" << endl << "n(" << n[0] << ", " << n[1] << ", " << n[2] << ")" << endl << "v=" << result << endl << endl;
                    if (min_flag == 0 || result < result_min)
                    {
                        result_min = result;
                        for (int i = 0; i < 3; i++) n_min[i] = n[i];
                        min_flag = 1;
                    }
                }
                else temp_result = volume(numder_of_fi, numder_of_tetta, thisnode, this_down_tetta, this_up_tetta);
                if (flag == 0)
                {
                    for (int i = 0; i < numvar; i++) d[i] = y[i] - x[i];
                    for (int i = 0; i < numvar; i++) x[i] = y[i];
                    cout << "Jump" << endl;
                    next_point(x, numvar, fi_max, fi_min, tetta_max, tetta_min, eps, d, k, numder_of_fi, numder_of_tetta, node, thisnode, down_tetta, up_tetta, this_down_tetta, this_up_tetta, point_0, n);
                    for (int i = 0; i < numvar; i++) y[i] = x[i];
                }
            }
            tetta += d_tetta;
            flag = 0;
        }
        tetta = tetta_min + d_tetta;
        fi += d_fi;
    }
    cout << "===GLOBAL RESULT===" << endl;
    cout << "n_min(" << n[0] << ", " << n[1] << ", " << n[2] << ")" << endl << "Min volume = " << result_min << endl;

    delete[] x;
    delete[] y;
    delete[] d;

    delete[] point_0;
    for (int i = 0; i < numder_of_fi; i++)
    {
        for (int j = 0; j < numder_of_tetta; j++) delete[] node[i][j];
        delete[] node[i];
    }
    delete[] node;

    delete[] down_tetta;
    delete[] up_tetta;

    for (int i = 0; i < numder_of_fi; i++)
    {
        for (int j = 0; j < numder_of_tetta; j++) delete[] thisnode[i][j];
        delete[] thisnode[i];
    }
    delete[] thisnode;

    delete[] this_down_tetta;
    delete[] this_up_tetta;
    delete[] n;

    return 0;
}