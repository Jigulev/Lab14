// Lab14.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
using namespace std;

// УПР.1-3: ПРЕДСТАВЛЕНИЕ ГРАФОВ 
void task1_3() {
    // Проверка существования файла
    ifstream fin("graph_in.txt");
    if (!fin) {
        cout << "ОШИБКА: Файл graph_in.txt не найден!\n";
        return;
    }

    int n, m;
    fin >> n >> m;

    if (n <= 0 || m < 0) {
        cout << "ОШИБКА: Неверные значения n=" << n << " или m=" << m << "\n";
        return;
    }

    // Упр.1: матрица смежности
    vector<vector<int>> M(n, vector<int>(n, 0));
    for (int i = 0; i < m; i++) {
        int u, v;
        fin >> u >> v;

        // Проверка корректности вершин
        if (u < 0 || u >= n || v < 0 || v >= n) {
            cout << "ОШИБКА: Вершина " << u << " или " << v << " вне диапазона 0.." << n - 1 << "\n";
            return;
        }

        M[u][v] = M[v][u] = 1;
    }
    fin.close();

    // Сохранение матрицы
    ofstream fout("matrix.txt");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fout << M[i][j] << " ";
        }
        fout << "\n";
    }
    fout.close();
    cout << "Матрица смежности сохранена в matrix.txt\n";

    // Упр.2: массивы L, S, D
    vector<int> L(n, 0); //степень вершины i (кол-во соседей)
    vector<pair<int, int>> edges;

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (M[i][j]) { // если есть ребро
                edges.push_back({ i, j }); //он добавляется в список
                L[i]++; //увеличение степени обоих вершин
                L[j]++;
            }

    int m2 = edges.size(); //реальное кол-во рёбер
    vector<int> S(n, 0); //сумма степеней предыдущих вершин
    for (int i = 1; i < n; i++)
        S[i] = S[i - 1] + L[i - 1];

    vector<int> U = S; //копия S
    vector<int> D(2 * m2); //массив для соседей

    for (int i = 0; i < m2; i++) {
        int u = edges[i].first;
        int v = edges[i].second;
        D[U[u]++] = v; //запись v как соседа u и увеличение указателя
        D[U[v]++] = u; //запись u как соседа v
    }

    fout.open("adjacency_array.txt");
    fout << "L: "; for (int x : L) fout << x << " ";
    fout << "\nS: "; for (int x : S) fout << x << " ";
    fout << "\nD: "; for (int x : D) fout << x << " ";
    fout.close();
    cout << "Массивы L, S, D сохранены в adjacency_array.txt\n";

    // Упр.3: восстановление рёбер
    fout.open("edges_restored.txt");
    for (int i = 0; i < n; i++) {
        int end = S[i] + L[i]; //конец блока соседей вершины i
        if (end > (int)D.size()) end = D.size();
        for (int j = S[i]; j < end; j++) {
            if (D[j] > i) {
                fout << i << " " << D[j] << "\n";
            }
        }
    }
    fout.close();
    cout << "Рёбра восстановлены в edges_restored.txt\n";
}

// ГЕНЕАЛОГИЯ 
void taskGenealogy() {
    ifstream fin("genealogy.txt");
    if (!fin) {
        cout << "ОШИБКА: Файл genealogy.txt не найден!\n";
        cout << "Создайте файл с парами (последняя пара -1 -1)\n";
        return;
    }

    vector<pair<int, int>> edges;
    int a, b, maxV = 0;
    while (fin >> a >> b && a != -1) {
        edges.push_back({ a, b });
        maxV = max(maxV, max(a, b)); //максимальная вершина
    }
    fin.close();

    if (maxV < 0) {
        cout << "Файл пуст\n";
        return;
    }

    vector<vector<int>> g(maxV + 1); //список смежности для каждой вершины
    for (auto& e : edges) {
        if (e.first <= maxV && e.second <= maxV) {
            g[e.first].push_back(e.second);
            g[e.second].push_back(e.first);
        }
    }

    vector<int> comp(maxV + 1, 0); //номер компоненты (0 - не посещена)
    vector<vector<int>> trees; //каждая компонента - отдельный массив вершин
    int treeCount = 0;

    for (int i = 0; i <= maxV; i++) {
        if (comp[i] == 0 && !g[i].empty()) { //новый корень компоненты
            treeCount++;
            vector<int> stack = { i };
            comp[i] = treeCount;
            vector<int> tree;

            while (!stack.empty()) {
                int v = stack.back();
                stack.pop_back();
                tree.push_back(v);
                for (int to : g[v]) //проходим по соседям
                    if (comp[to] == 0) { //если не посещали
                        comp[to] = treeCount;
                        stack.push_back(to);
                    }
            }
            trees.push_back(tree);
        }
    }

    ofstream fout("genealogy_result.txt");
    fout << "Количество деревьев: " << treeCount << "\n";
    for (size_t i = 0; i < trees.size(); i++) {
        fout << "Дерево " << i + 1 << ": ";
        for (int v : trees[i]) fout << v << " ";
        fout << "\n";
    }
    fout.close();
    cout << "Результат сохранён в genealogy_result.txt\n";
    cout << "Количество деревьев: " << treeCount << "\n";
}

// ПУТЕШЕСТВИЯ 
void taskTravel() {
    ifstream fin("travel.txt");
    if (!fin) {
        cout << "ОШИБКА: Файл travel.txt не найден!\n";
        return;
    }

    int n, m;
    fin >> n >> m;

    if (n <= 0 || m < 0) {
        cout << "ОШИБКА: Неверные данные\n";
        return;
    }

    vector<int> L(n, 0); //массив для степеней
    vector<pair<int, int>> edges(m);

    for (int i = 0; i < m; i++) {
        fin >> edges[i].first >> edges[i].second;
        if (edges[i].first >= 0 && edges[i].first < n)
            L[edges[i].first]++;
    }
    fin.close();

    vector<int> S(n, 0), D(m); //массивы для сумм степеней и соседей
    for (int i = 1; i < n; i++)
        S[i] = S[i - 1] + L[i - 1];

    vector<int> U = S;
    for (int i = 0; i < m; i++) {
        int u = edges[i].first;
        if (U[u] < m) {
            D[U[u]++] = edges[i].second;
        }
    }

    vector<int> ways(n, 0);
    ways[0] = 1; //в начальную вершину

    for (int v = 0; v < n; v++) { //для каждой вершины
        for (int j = S[v]; j < S[v] + L[v] && j < m; j++) { //для каждого соседа
            int to = D[j];
            if (to >= 0 && to < n) {
                ways[to] += ways[v]; //добавляем пути через текущую вершину
            }
        }
    }

    cout << "Количество маршрутов из А (вершина 0) в Л (вершина " << n - 1 << "): " << ways[n - 1] << "\n";
}

// ЛАБИРИНТ 
void taskMaze() {
    ifstream fin("maze.txt");
    if (!fin) {
        cout << "ОШИБКА: Файл maze.txt не найден!\n";
        return;
    }

    int n;
    fin >> n;

    if (n <= 0) {
        cout << "ОШИБКА: Неверный размер лабиринта\n";
        return;
    }

    vector<string> lab(n);
    int si = -1, sj = -1, fi = -1, fj = -1;

    for (int i = 0; i < n; i++) {
        fin >> lab[i];
        for (int j = 0; j < n; j++) {
            if (lab[i][j] == 's') { si = i; sj = j; }
            if (lab[i][j] == 'f') { fi = i; fj = j; }
        }
    }
    fin.close();

    if (si == -1 || fj == -1) {
        cout << "ОШИБКА: Не найдены s (начало) или f (конец)\n";
        return;
    }

    vector<vector<int>> dist(n, vector<int>(n, -1));
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(n, { -1,-1 }));
    queue<pair<int, int>> q;

    int dx[] = { -1,-1,-1,0,0,1,1,1 }; //по x
    int dy[] = { -1,0,1,-1,1,-1,0,1 }; //по y

    dist[si][sj] = 0; //расстояние до старта
    q.push({ si, sj }); //добавление старта в очередь

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop(); //извлечение элемента из очереди
        if (x == fi && y == fj) break; //достигли конца

        for (int d = 0; d < 8; d++) {
            int nx = x + dx[d], ny = y + dy[d]; //соседняя клетка
            if (nx >= 0 && nx < n && ny >= 0 && ny < n && dist[nx][ny] == -1) {
                if (lab[nx][ny] != '#') { //если не стена
                    dist[nx][ny] = dist[x][y] + 1; //увеличение пути
                    parent[nx][ny] = { x, y }; //запоминание пути
                    q.push({ nx, ny });
                }
            }
        }
    }

    if (dist[fi][fj] == -1) {
        cout << "Пути не существует\n";
        return;
    }

    //восстановление пути обратным ходом
    vector<string> result = lab;
    int x = fi, y = fj;
    while (!(x == si && y == sj)) {
        if (result[x][y] != 's' && result[x][y] != 'f')
            result[x][y] = '*';
        auto [px, py] = parent[x][y];
        x = px; y = py;
    }

    cout << "\nЛабиринт с кратчайшим путём (*):\n";
    for (auto& row : result) cout << row << "\n";
    cout << "Длина пути: " << dist[fi][fj] << "\n";
}

// ==================== ЭЙЛЕРОВ ПУТЬ ====================
void taskEuler() {
    ifstream fin("euler.txt");
    if (!fin) {
        cout << "ОШИБКА: Файл euler.txt не найден!\n";
        return;
    }

    int n, m;
    fin >> n >> m;

    if (n <= 0 || m < 0) {
        cout << "ОШИБКА: Неверные данные\n";
        return;
    }

    vector<int> deg(n, 0);


    for (int i = 0; i < m; i++) {
        int u, v;
        fin >> u >> v;
        if (u >= 0 && u < n && v >= 0 && v < n) {
            deg[u]++;
            deg[v]++;
        }
    }
    fin.close();

    int odd = 0;
    for (int d : deg) if (d % 2) odd++; //счёт вершин с нечётное степенью

    ofstream fout("euler_result.txt");
    if (odd == 0) {
        fout << "Можно нарисовать, начиная и заканчивая в одной точке (Эйлеров цикл)\n";
        cout << "Результат: Эйлеров цикл\n";
    }
    else if (odd == 2) {
        fout << "Можно нарисовать, начиная и заканчивая в разных точках (Эйлеров путь)\n";
        cout << "Результат: Эйлеров путь\n";
    }
    else {
        fout << "Нельзя нарисовать, не отрывая карандаша\n";
        cout << "Результат: Нельзя\n";
    }
    fout.close();
    cout << "Результат сохранён в euler_result.txt\n";
}

// ==================== ГЛАВНОЕ МЕНЮ ====================
int main() {
    setlocale(LC_ALL, "Rus");
    int choice;
    do {
        cout << "1. Упр.1-3 (представление графов)\n";
        cout << "2. Генеалогия (компоненты связности)\n";
        cout << "3. Путешествия (кол-во маршрутов)\n";
        cout << "4. Лабиринт (поиск пути)\n";
        cout << "5. Детство и графы (Эйлер)\n";
        cout << "0. Выход\n";
        cout << "Выбор: ";
        cin >> choice;

        switch (choice) {
        case 1: task1_3(); break;
        case 2: taskGenealogy(); break;
        case 3: taskTravel(); break;
        case 4: taskMaze(); break;
        case 5: taskEuler(); break;
        case 0: break;
        default: cout << "Неверное значение\n";
        }
    } while (choice != 0);

    return 0;
}