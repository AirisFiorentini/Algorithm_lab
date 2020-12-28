#include <iostream>
#include <ctime>
#include <random>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <string.h>

bool print = true;

using namespace std;
//вывод графа- матрицы в консоль
void print_graph(bool** matrix, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			cout << matrix[i][j] << " ";
		cout << endl;
	}
	cout << endl;
}
//вывод графа в виде списка вершин в консоль
void print_graph(vector<vector<int>>& x, int n) {
	for (int i = 0; i < n; i++) {
		cout << i << ": ";
		for (int j = 0; j < x[i].size(); j++)
			cout << x[i][j] << " ";
		cout << endl;
	}
    cout << endl;
}
//перевод матрицы смежности в список вершин
void matrix_to_list(vector<vector<int>>& list, bool** matrix, int n) {
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (i != j && matrix[i][j])
				list[i].push_back(j);
	for (int p = 0; p < n; p++)
		delete[] matrix[p];
}

void generate_graph(bool** matrix, int n, int edges) {
    if (edges < (n - 1)) assert("Wrong number of edges");
    //Задается количество ребер, которые необходимо догенерировать
    edges -= (n - 1);
    int tmp = 0;
    //Создаются два связных подграфа, чтобы на выходе всегда была хотя бы 1 точка сочленения
    //Первый подграф: сначала свяжем 0 и 1 вершину, дальше будем связывать с псевдослучайной вершиной,
    //номер которой меньшей номера текущей
    matrix[0][1] = true;
    matrix[1][0] = true;
    int half = n / 2;
    for (int i = 2; i < half; i++) {
        tmp = rand() % i;
        matrix[i][tmp] = true;
        matrix[tmp][i] = true;
    }
    //Добавляем дополнительные ребра, чтобы не было слишком много точек сочленения
    int tmp1 = 0;
    int tmp2 = 0;
    for (int i = 0; i < edges / 2; i++) {
        tmp1 = rand() % half;
        tmp2 = rand() % half;
        if (tmp1 == tmp2) {
            i--;
            continue;
        }
        matrix[tmp1][tmp2] = true;
        matrix[tmp2][tmp1] = true;
    }
    //Второй подграф: генерация аналогична первому, но другие границы
    //Добавляем ребра в этот подграф, чтобы получить нужно кол-во
    matrix[half][half + 1] = true;
    matrix[half + 1][half] = true;
    for (int i = half + 2; i < n; i++) {
        tmp = rand() % (i - half) + (half);
        matrix[i][tmp] = true;
        matrix[tmp][i] = true;
    }
    //Добавляем дополнительные ребра, чтобы не было слишком много точек сочленения
    for (int i = 0; i <edges - (edges / 2); i++) {
        tmp1 = rand() % (half)+(half);
        tmp2 = rand() % (half)+(half);
        if (tmp1 == tmp2) {
            i--;
            continue;
        }
        matrix[tmp1][tmp2] = true;
        matrix[tmp2][tmp1] = true;
    }
    //Соединяем два подграфа
    matrix[half][half - 1] = true;
    matrix[half - 1][half] = true;
}

//Вывод в поток сильно влияет на время
void IS_CUTPOINT(int v) {
    //cout << "Cut point: " << v << endl;
}

//обход в глубину, работа с матрицей
/*
void dfs(int v, int p, int n, bool** g, int timer, bool* visited, int* tin, int* fup) {
    //print_graph(g, n);
    visited[v] = true;
	tin[v] = fup[v] = timer++;
	int children = 0;
	for (size_t i = 0; i < n; ++i) {
		int to = 0;
		if (g[v][i])
			to = i;
		else
			continue;
		if (to == p || i == v)  continue;
		if (visited[to])
			fup[v] = min(fup[v], tin[to]);
		else {
			dfs(to, p, n, g, timer, visited, tin, fup);
			fup[v] = min(fup[v], fup[to]);
			if (fup[to] >= tin[v] && p != -1)//Стандартное условие алгоритма
				IS_CUTPOINT(v);
			++children;
		}
	}
	if (p == -1 && children > 1)//Проверка при условии, что вершина - корень
		IS_CUTPOINT(v);
}*/

//обход в глубину, работа со списком вершин
void dfs_list(int v, int p, int n, vector<vector<int>>& g, int timer, bool* visited, int* tin, int* fup) {
    //print_graph(g, n);
	visited[v] = true;
	timer++;
	tin[v] = fup[v] = timer;
	int children = 0;
	for (size_t i = 0; i < g[v].size(); ++i) {
		int to = g[v][i];
		if (to == p)  continue;
		if (visited[to])
			fup[v] = min(fup[v], tin[to]);
		else {
			dfs_list(to, p, n, g, timer, visited, tin, fup);
			fup[v] = min(fup[v], fup[to]);
			if (fup[to] >= tin[v] && p != -1)//Стандартное условие алгоритма
				IS_CUTPOINT(v);
			++children;
		}
	}
	if (p == -1 && children > 1) //Проверка : вершина - корень
		IS_CUTPOINT(v);
}

void empirical_func(int start, int i_, int k_, int m) {
	int n = 2;
	int edges = 0;
    unsigned long long sum;
	// Проверка на указанных входных данных
	for (int i = i_; i < i_ + 30; i++) {
		n *= 2; //start + i * m; //Генерация кол-ва вершин
		bool* used = new bool[n]{false};
		int timer = 0; sum = 0;
		int* tin = new int[n];
		int* fup = new int[n];
		for (int k = k_; k <= k_ + 10; k++) {
			edges = rand() % (int)((n - 1) * (0.5 * n - 1)) + (n-1);//рандомная генерация ребер
			memset(used, false, n);
			timer = 0;
			bool** matrix = new bool*[n];
			for (int j = 0; j < n; j++)
				matrix[j] = new bool[n]{false};
			vector<vector<int>> list;
			list.resize(n);
			list.shrink_to_fit();
			//Генерация входных данных
            generate_graph(matrix, n, edges);
			matrix_to_list(list, matrix, n); //Перевод матрицы смежности в список вершин и удаление из памяти
			//if (n < 15) print_graph(list, n);//вывод полученного графа
			//dfs(0, -1, n, matrix, timer, used, tin, fup);

			for (int t = 0; t < 10; t++) {
				memset(used, false, n);
				timer = 0;
				size_t start = clock();
				//Вызов самого алгоритма
				dfs_list(0, -1, n, list, timer, used, tin, fup);
				size_t end = clock();
				sum += (end - start);
				print = false;
			}

			//print = true;
		}
        cout << "Number of vertex: " << n << endl; // << "  edges: " << edges << endl;
        cout << "Average time of execution is " << (sum / 10.) << " s" << endl << endl;
		delete[] used;
		delete[] tin;
		delete[] fup;
	}
}

int main() {
	cout << "Standart input: " << endl;
    empirical_func(1000, 0, 10, 1000);// запускается на изначальных входных данных
	cout << endl << endl;
	//cout << "Input doubled: " << endl;
    //empirical_func(1000, 0, 10, 200);// запускается на удвоенных данных
	system("pause");
	return 0;
}