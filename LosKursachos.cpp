#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
int main(void);
double f1(double);
double f2(double);
double f3(double);
double integrator(double, double, double, unsigned, double (*)(double));
double Rimm_sum(double, unsigned, double, double (*)(double));
void max_n_min(double*, double*, double (*)(double), double, double);
void draw(double, double, double);
void graphic(HPEN, HDC, double, double, double, int, int, double(*)(double));
int main(void)
{
    FILE* in = fopen("input.txt", "r");
    char buf[257], * check;
    bool flag2 = 0, flag3 = 0;
    unsigned i, space_place;
    int flag1, j, dot1 = -1, dot2 = -1, min1 = 1, min2 = 1, tmp, dot1_ind, dot2_ind, min1_ind, min2_ind;
    double x_min = 0, x_max = 0;
    double int_sin, int_cos, int_other;
    printf("press 0 if you want to enter data yourself or 1 if you want to read it from a file: ");
    scanf("%d", &flag1);
    char is_endl[1] = { '\0' };
    size_t err = fscanf(in, "%lf%lf[\n]", &x_min, &x_max, is_endl);
    switch (flag1) {
    case 0:
        printf("enter x_min:");
        scanf("%lf", &x_min);
        printf("enter x_max:");
        scanf("%lf", &x_max);
        break;
    case 1:
        scanf("reading file...\n");
        if (in == NULL) {
            printf("could not open file\n");
            return -1;
        }
        if (err != 2 || is_endl[0] != '\0') {
            printf("Вы ввели не два числа!\n");
            return 0;
        }
        break;
    default:
        printf("wrong input .!. !");
        return -1;
    }
    fclose(in);
    if (x_min > x_max) {
        printf("bad user!\n*software cannot kill you, but it sure wants to*\n");
        return -1;
    }
    int_sin = integrator(x_min, x_max, 0.0001, 10, f1);
    int_cos = integrator(x_min, x_max, 0.0001, 10, f2);
    int_other = integrator(x_min, x_max, 0.0001, 10, f3);
    printf("integral of sin(x) in [a,b] = %f\nintegral of cos(x) in [a,b] = %f\nintegral of 1/x^2 in [a,b] = %f\n", int_sin, int_cos, int_other);
    printf("green one is sin(x), red one is cos(x), blue one - (|x|)^(1/2)");
    Sleep(1000);
    //чистим чистим чистим
    printf("\033[2J");
    // \033[2J = "ESC\" String Terminator https://en.wikipedia.org/wiki/ANSI_escape_code
    double diap_x = (x_max - x_min) / 78;
    draw(x_min, x_max, diap_x);
}

double f1(double x) {
    return sin(x);
}
double f2(double x) {
    return cos(x);
}
double f3(double x) {
    return pow(fabs(x),1.0/2);
}

void draw(double x_min, double x_max, double diap_x) {
    double y_max, y_min, i, tmp, y, x, diap_y;
    y_min = f1(x_min);
    y_max = y_min;
    max_n_min(&y_min, &y_max, f1, x_min, x_max);
    max_n_min(&y_min, &y_max, f2, x_min, x_max);
    max_n_min(&y_min, &y_max, f3, x_min, x_max);
    diap_y = (y_max - y_min) / 45;//диапазон делений для оY
    HDC hDC = GetDC(GetConsoleWindow());//настройка консоли для рисования
    HPEN Pen = CreatePen(PS_SOLID, 0.5, RGB(255, 255, 255));//ручка для разметки
    HPEN Pen1 = CreatePen(PS_SOLID, 2, RGB(0, 200, 0));//ручка для графика sin(x)
    HPEN Pen2 = CreatePen(PS_SOLID, 2, RGB(200, 0, 0));//ручка для графика cos(x)
    HPEN Pen3 = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));//ручка для графика 1/x^2
    while (true) {//график постоянно обновляется для соответствия сжатию окна
        HWND hwnd = GetConsoleWindow();
        RECT rect;
        //через прямоугольник rect описывается консолька
        GetWindowRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        SelectObject(hDC, Pen);
        //оси координат
        MoveToEx(hDC, 0, height / 2, NULL);
        LineTo(hDC, width, height / 2);
        MoveToEx(hDC, width / 2, height, NULL);
        LineTo(hDC, width / 2, 0);
        //стрелка oY
        MoveToEx(hDC, width / 2, 0, NULL);
        LineTo(hDC, width / 2 + 10, 10);
        MoveToEx(hDC, width / 2 - 10, 10, NULL);
        LineTo(hDC, width / 2, 0);
        //стрелка оХ
        MoveToEx(hDC, width - 45, height / 2 - 10, NULL);
        LineTo(hDC, width - 35, height / 2);
        MoveToEx(hDC, width - 35, height / 2, NULL);
        LineTo(hDC, width - 45, height / 2 + 10);
        //деления oX
        for (x = x_min; x <= x_max; x += diap_x) {
            MoveToEx(hDC, width / 11 * x +width / 2, height / 2 - height / 250 - 1, NULL);
            LineTo(hDC, width / 11 * x +width / 2, height / 2 + height / 250 + 1);
        }
        //деления oY
        for (y = y_min; y <= y_max; y += diap_y){
            MoveToEx(hDC, width / 2 - width / 250 - 1, -height /11 * y + height / 2, NULL);
            LineTo(hDC, width / 2 + width / 250 + 1, -height /11 * y + height / 2);
        }
        //вывод графика 
        graphic(Pen1, hDC, x_min, x_max, diap_x, width, height, f1);
        graphic(Pen2, hDC, x_min, x_max, diap_x, width, height, f2);
        graphic(Pen3, hDC, x_min, x_max, diap_x, width, height, f3);
    }
    _getch();//функция помогает графику нормально отобразиться
}

void graphic(HPEN Pen, HDC hDC, double x_min, double x_max, double diap_x, int width, int height, double (*func)(double)) {
    SelectObject(hDC, Pen);
    float x;
    for (x = x_min; x <= x_max; x += diap_x)
    {
        //11 - для крупности графика, чем меньше число, тем крупнее
        MoveToEx(hDC, width / 11 * x + width / 2, -height / 11 * func(x) + height / 2, NULL);
        LineTo(hDC, width / 11 * (x+diap_x) + width / 2, -height / 11 * func(x+diap_x) + height / 2);
    }
}
double integrator(double c, double d, double Q, unsigned n, double (func)(double)) {
    float S_0, S_1;
    S_0 = Rimm_sum(d, n, c, func);
    S_1 = Rimm_sum(d, n + 16, c, func);
    if (fabs(S_0 - S_1) <= Q) {
        return S_1;
    }
    else {
        return integrator(c, d, Q, n + 16, func);
    }
}

 double Rimm_sum(double d, unsigned n, double c, double (func)(double)) {
        long double sum = 0, h = (d - c) / n;
        unsigned i;
        for (i = 0; i < n; ++i) {
            sum += func(c + h * i + h / 2);
        }
        sum *= h;
        return sum;
}
 void max_n_min(double* y_min, double* y_max, double (*func)(double), double x_min, double x_max) {
     double tmp, x, diap_x = (x_max - x_min) / 78;
     for (x = x_min; x < x_max + diap_x; x += diap_x) {
         if ((tmp=func(x)) < *y_min) {
             *y_min = tmp;
         }
         else if (tmp > * y_max) {
             *y_max = tmp;
         }
     }
 }