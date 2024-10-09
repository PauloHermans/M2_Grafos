///Alunos: Paulo Martino Hermans e Pedro José Garcia
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <unordered_map>
#include <set>
#include <unordered_set>

using namespace std;

void limpaConsole() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

struct Grafo {
    int nVertices;
    bool ehDirigido;
    unordered_map<int, list<int>> listaAdjacencia;
    int nArestas = 0;
    unordered_map<int, int> graus;

    Grafo(int v, bool dirigido);
    void imprimirCaminho(vector<int> caminho);
    void adicionarVertice(int v);
    void removerVertice(int v);
    void adicionarAresta(int u, int v);
    void removerAresta(int u, int v);
    vector<int> obterListaVertices();
    unordered_map<int, unordered_map<int, int>> obterMatrizAdjacencia();
    void mostrarMatrizAdjacencia();
    vector<int> buscaEmLargura(int verticeInicial);
    vector<int> buscaEmProfundidade(int verticeInicial);
    unordered_map<int, int> fechoTransitivoDireto(int verticeInicial);
    unordered_map<int, int> fechoTransitivoInverso(int verticeInicial);
    vector<int> obterSubgrafoFortementeConexo(int vertice);
    void verificarConectividade();
    void verificarPlanaridade();
    void desenhar(unordered_map<int, int> listaCores);
    void colorir();

private:
    void buscaEmProfundidadeRecursivo(int vertice, unordered_map<int, bool>& visitado, vector<int>& caminho);
    string corParaString(int cor);
};

Grafo::Grafo(int v, bool dirigido) : nVertices(v), ehDirigido(dirigido) {
}

void Grafo::imprimirCaminho(vector<int> caminho) {
    for (int i : caminho) {
        cout << i << " ";
    }
    cout << "\n";
}

void Grafo::adicionarVertice(int v) {
    if (listaAdjacencia.find(v) == listaAdjacencia.end()) {
        listaAdjacencia[v] = list<int>();

        nVertices++;
        graus[v] = 0;

        cout << "Vértice " << v << " adicionado." << endl;
    } else {
        cout << "Vértice " << v << " já existe." << endl;
    }
}

void Grafo::removerVertice(int v) {
    if (listaAdjacencia.find(v) != listaAdjacencia.end()) {
        listaAdjacencia.erase(v);
        for (auto& par : listaAdjacencia) {
            par.second.remove(v);
        }

        nVertices--;
        graus.erase(v);

        cout << "Vértice " << v << " removido." << endl;
    } else {
        cout << "Vértice " << v << " não existe." << endl;
    }
}

void Grafo::adicionarAresta(int u, int v) {
    if (listaAdjacencia.find(u) == listaAdjacencia.end()) {
        cout << "Vértice " << u << " não existe." << endl;
        return;
    }
    if (listaAdjacencia.find(v) == listaAdjacencia.end()) {
        cout << "Vértice " << v << " não existe." << endl;
        return;
    }

    listaAdjacencia[u].push_back(v);
    if (!ehDirigido) {
        listaAdjacencia[v].push_back(u);
    }

    nArestas++;
    graus[u]++;
    graus[v]++;

    cout << "Aresta " << u << " -> " << v << " adicionada." << endl;
}

void Grafo::removerAresta(int u, int v) {
    if (listaAdjacencia.find(u) == listaAdjacencia.end()) {
        cout << "Vértice " << u << " não existe." << endl;
        return;
    }
    if (listaAdjacencia.find(v) == listaAdjacencia.end()) {
        cout << "Vértice " << v << " não existe." << endl;
        return;
    }

    listaAdjacencia[u].remove(v);
    if (!ehDirigido) {
        listaAdjacencia[v].remove(u);
    }

    nArestas--;
    graus[u]--;
    graus[v]--;

    cout << "Aresta " << u << " -> " << v << " removida." << endl;
}

vector<int> Grafo::obterListaVertices() {
    vector<int> verticesOrdenados;
    for (const auto& par : listaAdjacencia) {
        verticesOrdenados.push_back(par.first);
    }
    sort(verticesOrdenados.begin(), verticesOrdenados.end());

    return verticesOrdenados;
}

unordered_map<int, unordered_map<int, int>> Grafo::obterMatrizAdjacencia() {
    unordered_map<int, unordered_map<int, int>> matAdj;
    vector<int> listaVertices = obterListaVertices();

    for (int i : listaVertices) {
        for (int j : listaVertices) {
            if (find(listaAdjacencia[i].begin(), listaAdjacencia[i].end(), j) != listaAdjacencia[i].end()) {
                matAdj[i][j] = 1;
            } else {
                matAdj[i][j] = 0;
            }
        }
    }

    return matAdj;
}

void Grafo::mostrarMatrizAdjacencia() {
    auto matAdj = obterMatrizAdjacencia();
    vector<int> listaVertices = obterListaVertices();

    // Imprime o cabeçalho da matriz
    cout << "   ";
    for (int i = 1; i <= nVertices; i++) {
        cout << i << " ";
    }
    cout << endl;

    // Imprime a matriz
    for (auto i : listaVertices) {
        cout << i << "| ";
        for (int j : listaVertices) {
            if (matAdj[i][j]) {
                cout << 1 << " ";
            } else {
                cout << ". ";
            }
        }
        cout << endl;
    }
}

vector<int> Grafo::buscaEmLargura(int verticeInicial) {
    vector<int> caminho;
    unordered_map<int, bool> visitados;
    queue<int> fila;

    caminho.push_back(verticeInicial);
    visitados[verticeInicial] = true;
    fila.push(verticeInicial);

    while (!fila.empty()) {
        int vertice = fila.front();
        fila.pop();

        for (const int& verticeAdj : listaAdjacencia[vertice]) {
            if (!visitados[verticeAdj]) {
                caminho.push_back(verticeAdj);
                visitados[verticeAdj] = true;
                fila.push(verticeAdj);
            }
        }
    }

    return caminho;
}

void Grafo::buscaEmProfundidadeRecursivo(int vertice, unordered_map<int, bool>& visitados, vector<int>& caminho) {
    caminho.push_back(vertice);
    visitados[vertice] = true;

    for (const int& verticeAdj : listaAdjacencia[vertice]) {
        if (!visitados[verticeAdj]) {
            buscaEmProfundidadeRecursivo(verticeAdj, visitados, caminho);
        }
    }
}

vector<int> Grafo::buscaEmProfundidade(int verticeInicial) {
    vector<int> caminho;
    unordered_map<int, bool> visitados;
    buscaEmProfundidadeRecursivo(verticeInicial, visitados, caminho);

    return caminho;
}

unordered_map<int, int> Grafo::fechoTransitivoDireto(int verticeInicial) {
    unordered_map<int, int> ftd;
    unordered_map<int, bool> estudados;
    queue<pair<int, int>> fila;

    int nivel = 0;

    ftd[verticeInicial] = nivel;
    estudados[verticeInicial] = true;
    fila.push(pair(verticeInicial, nivel));

    while (!fila.empty()) {
        auto vertice = fila.front();
        fila.pop();

        nivel = vertice.second + 1;

        for (const int& verticeAdj : listaAdjacencia[vertice.first]) {
            if (ftd.find(verticeAdj) == ftd.end()) {
                ftd[verticeAdj] = nivel;
                fila.push(pair(verticeAdj, nivel));
            }
        }
        
        estudados[vertice.first] = true;
    }

    return ftd;
}

unordered_map<int, int> Grafo::fechoTransitivoInverso(int verticeInicial) {
    // Inverte o grafo, transpondo a lista de adjacência
    unordered_map<int, list<int>> listaAdjacenciaInv;

    for (const auto& par : listaAdjacencia) {
        int u = par.first;
        for (const int& v : par.second) {
            listaAdjacenciaInv[v].push_back(u);
        }
    }

    unordered_map<int, int> fti;
    unordered_map<int, bool> estudados;
    queue<pair<int, int>> fila;

    int nivel = 0;

    fti[verticeInicial] = nivel;
    estudados[verticeInicial] = true;
    fila.push(pair(verticeInicial, nivel));

    while (!fila.empty()) {
        auto vertice = fila.front();
        fila.pop();

        nivel = vertice.second + 1;

        for (const int& verticeAdj : listaAdjacenciaInv[vertice.first]) {
            if (fti.find(verticeAdj) == fti.end()) {
                fti[verticeAdj] = nivel;
                fila.push(pair(verticeAdj, nivel));
            }
        }
        
        estudados[vertice.first] = true;
    }

    return fti;
}

vector<int> Grafo::obterSubgrafoFortementeConexo(int vertice) {
    vector<int> ftd;
    vector<int> fti;

    for (const auto& par : fechoTransitivoDireto(vertice)) {
        ftd.push_back(par.first);
    }
    for (const auto& par : fechoTransitivoInverso(vertice)) {
        fti.push_back(par.first);
    }

    sort(ftd.begin(), ftd.end());
    sort(fti.begin(), fti.end());

    // Obter a intersecção dos dois fechos transitivos
    vector<int> interseccao;
    set_intersection(ftd.begin(), ftd.end(), fti.begin(), fti.end(),
                     back_inserter(interseccao));

    return interseccao;
}

void Grafo::verificarConectividade() {
    list<vector<int>> subgrafos;

    set<int> verticesExplorados;

    vector<int> listaVertices = obterListaVertices();
    int primeiroVertice = listaVertices.front();

    vector<int> interseccao = obterSubgrafoFortementeConexo(primeiroVertice);
    subgrafos.push_back(interseccao);

    for (int vertice : interseccao) {
        verticesExplorados.insert(vertice);
    }

    // Caso a intersecção não contenha um ou mais vértices, o grafo não é conexo
    if (interseccao.size() < nVertices) {
        cout << "O grafo não é conexo.\n";

        for (int i : listaVertices) {
            if (verticesExplorados.find(i) == verticesExplorados.end()) {
                interseccao = obterSubgrafoFortementeConexo(i);
                subgrafos.push_back(interseccao);

                for (int vertice : interseccao) {
                    verticesExplorados.insert(vertice);
                }
            }
        }

        cout << "Subgrafos fortemente conexos:\n";
        for (vector<int> subgrafo : subgrafos) {
            cout << "{";
            for (int i : subgrafo) {
                cout << i;
                if (i != subgrafo.back()) {
                    cout << ", ";
                }
            }
            cout << "}\n";
        }
    } else {
        cout << "O grafo é conexo.\n";
    }
}

void Grafo::verificarPlanaridade() {
    auto matAdj = obterMatrizAdjacencia();
    vector<int> listaVertices = obterListaVertices();

    // Multiplicação da matriz de adjacência até a terceira ordem
    unordered_map<int, unordered_map<int, int>> m2;
    for (int i : listaVertices) {
        for (int j : listaVertices) {
            m2[i][j] = 0;
            for (int k : listaVertices) {
                // Álgebra booleana: a função max equivale à soma, enquanto min equivale à multiplicação
                m2[i][j] = max(m2[i][j], min(matAdj[i][k], matAdj[k][j]));
            }
        }
    }
    unordered_map<int, unordered_map<int, int>> m3;
    for (int i : listaVertices) {
        for (int j : listaVertices) {
            m3[i][j] = 0;
            for (int k : listaVertices) {
                m3[i][j] = max(m3[i][j], min(m2[i][k], matAdj[k][j]));
            }
        }
    }

    bool ehPlanar;

    // Grafos de um ou dois vértices são sempre planares
    if (nVertices < 3) {
        ehPlanar = true;
    }
    else {
        // Verifica-se as duas condições assumindo, a princípio, que não há ciclos de comprimento 3
        ehPlanar = (nArestas <= 2*nVertices - 6) && (nArestas <= 2*nVertices - 4);

        // Se houver ciclos de comprimento 3, verificar apenas uma das condições
        for (int i : listaVertices) {
            if (m3[i][i]) {
                ehPlanar = (nArestas <= 3*nVertices - 6);
                break;
            }
        }
    }

    if (ehPlanar) {
        cout << "O grafo é planar." << endl;
        int nRegioes = 2 - nVertices + nArestas;  // Fórmula de Euler
        cout << "Número de regiões: " << nRegioes << endl;
    }
    else {
        cout << "O grafo não é planar." << endl;
    }
}

string Grafo::corParaString(int cor) {
    switch (cor) {
        case 1: return "red";
        case 2: return "blue";
        case 3: return "green";
        case 4: return "yellow";
        case 5: return "purple";
        case 6: return "orange";
        default: return "white";
    }
}

void Grafo::desenhar(unordered_map<int, int> listaCores = {}) {
    // Abrindo arquivo para escrever o grafo no formato DOT
    ofstream arquivo;
    arquivo.open("grafo.dot", ofstream::out | ofstream::trunc);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo!" << endl;
        return;
    }

    // Escreve o cabeçalho do arquivo DOT
    arquivo << (ehDirigido ? "digraph" : "graph") << " G {\n";

    // Adiciona os vértices com suas cores
    for (int i : obterListaVertices()) {
        string cor;
        if (listaCores.find(i) != listaCores.end()) {
            cor = corParaString(listaCores[i]);
        }
        else {
            cor = "white";
        }

        arquivo << "    " << i << " [style=filled, fillcolor=" << cor << "];\n";
    }

    // Itera sobre os vértices e arestas do grafo
    for (const auto& adjacentes : listaAdjacencia) {
        int u = adjacentes.first;
        for (int v : adjacentes.second) {
            // Se o grafo for dirigido, use " -> ", senão use " -- " para arestas não dirigidas
            if (ehDirigido) {
                arquivo << "    " << u << " -> " << v << ";\n";
            } else {
                // Para grafos não dirigidos, desenha apenas uma vez a linha entre u e v
                if (u < v) {  // Evita duplicar a aresta em grafos não dirigidos
                    arquivo << "    " << u << " -- " << v << ";\n";
                }
            }
        }
    }

    // Fechando o arquivo DOT
    arquivo << "}\n";
    arquivo.close();

    // Chama o Graphviz para renderizar o arquivo DOT
    system("dot -Tpng grafo.dot -o grafo.png");  // Gera um arquivo PNG
    cout << "Grafo desenhado e salvo em 'grafo.png'." << endl;
}

void Grafo::colorir() {
    vector<int> listaVertices = obterListaVertices();
    unordered_map<int, int> listaCores;
    int crom = 1;  // Número cromático

    // Inicia-se pelo vértice de maior grau
    int verticeMaxGrau = listaVertices.front();
    for (int i : listaVertices) {
        if (listaAdjacencia[i].size() > listaAdjacencia[verticeMaxGrau].size()) {
            verticeMaxGrau = i;
        }
    }

    listaCores[verticeMaxGrau] = crom;

    // Realiza-se o processo até que todos os vértices estejam coloridos
    while (listaCores.size() < listaVertices.size()) {
        // Lista dos vértices não coloridos e cores diferentes em seus adjacentes
        unordered_map<int, unordered_set<int>> listaGrausSat;

        for (int i : listaVertices) {
            // Considerar apenas vértices ainda não coloridos
            if (listaCores.find(i) == listaCores.end()) {
                for (const int& verticeAdj : listaAdjacencia[i]) {
                    if (listaCores.find(verticeAdj) != listaCores.end()) {
                        listaGrausSat[i].insert(listaCores[verticeAdj]);
                    }
                }
            }
        }

        // Procura o vértice de maior grau de saturação
        int verticeMaxGrauSat = listaGrausSat.begin()->first;
        for (const auto& par : listaGrausSat) {
            if (par.second.size() > listaGrausSat[verticeMaxGrauSat].size()) {
                verticeMaxGrauSat = par.first;
            }
        }

        // Encontra a menor cor disponível
        unordered_set<int> coresIndisponiveis = listaGrausSat[verticeMaxGrauSat];
        int corEscolhida = 1;
        while (coresIndisponiveis.find(corEscolhida) != coresIndisponiveis.end()) {
            corEscolhida++;
        }

        listaCores[verticeMaxGrauSat] = corEscolhida;

        // Atualiza o número cromático se necessário
        if (corEscolhida > crom) {
            crom = corEscolhida;
        }
    }

    // Exibe as cores dos vértices
    cout << "Coloração dos vértices:" << endl;
    for (const auto& par : listaCores) {
        cout << "Vértice: " << par.first << " -> Cor: " << par.second << endl;
    }

    cout << "Número cromático: " << crom << endl;

    desenhar(listaCores);
}

int main() {
    // https://github.com/alf-p-steinbach/C---how-to---make-non-English-text-work-in-Windows/blob/main/how-to-use-utf8-in-windows.md
    #ifdef _WIN32
    system("chcp 65001 >nul");
    #endif

    int nVertices;
    cout << "Digite o número de vértices: ";
    cin >> nVertices;

    bool ehDirigido;
    cout << "O grafo é dirigido (1 para sim, 0 para não)? ";
    cin >> ehDirigido;

    Grafo g(0, ehDirigido);

    // Inicializa os vértices no unordered_map
    for (int i = 1; i <= nVertices; ++i) {
        g.adicionarVertice(i);
    }

    limpaConsole();

    while (true) {
        //Criação do menu
        cout << "Menu:\n";
        cout << "1. Adicionar Vértice\n";
        cout << "2. Remover Vértice\n";
        cout << "3. Adicionar Aresta\n";
        cout << "4. Remover Aresta\n";
        cout << "5. Mostrar Matriz de Adjacência\n";
        cout << "6. Busca em Largura\n";
        cout << "7. Busca em Profundidade\n";
        cout << "8. Verificar Conectividade / Mostrar Subgrafos Fortemente Conexos\n";
        if (!ehDirigido) {
            cout << "9. Verificar Planaridade / Número de Regiões\n";
            cout << "10. Colorir\n";
        }
        cout << "0. Sair\n";
        cout << "Digite sua escolha: ";
        int escolha;
        cin >> escolha;

        limpaConsole();

        if (escolha == 0) {
            break;
        }

        int u, v, verticeInicial;
        switch (escolha) {
        case 1:
            cout << "Digite o vértice a ser adicionado: ";
            cin >> u;
            g.adicionarVertice(u);
            break;
        case 2:
            cout << "Digite o vértice a ser removido: ";
            cin >> u;
            g.removerVertice(u);
            break;
        case 3:
            cout << "Digite a aresta (u v): ";
            cin >> u >> v;
            g.adicionarAresta(u, v);
            break;
        case 4:
            cout << "Digite a aresta a ser removida (u v): ";
            cin >> u >> v;
            g.removerAresta(u, v);
            break;
        case 5:
            g.mostrarMatrizAdjacencia();
            break;
        case 6:
            cout << "Digite o vértice inicial para a Busca em Largura: ";
            cin >> verticeInicial;
            cout << "Caminho: ";
            g.imprimirCaminho(g.buscaEmLargura(verticeInicial));
            break;
        case 7:
            cout << "Digite o vértice inicial para a Busca em Profundidade: ";
            cin >> verticeInicial;
            cout << "Caminho: ";
            g.imprimirCaminho(g.buscaEmProfundidade(verticeInicial));
            break;
        case 8:
            g.verificarConectividade();
            break;
        case 9:
            if (!ehDirigido) {
                g.verificarPlanaridade();
                break;
            }
        case 10:
            if (!ehDirigido) {
                g.colorir();
                break;
            }
        default:
            cout << "Escolha inválida!" << endl;
        }

        cout << endl;
    }

    return 0;
}
