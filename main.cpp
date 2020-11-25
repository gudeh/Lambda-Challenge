#include <cstdlib>
#include <set>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

//opção de compilação para testes
#define test 0

using namespace std;
struct member{
    string variable;
    set<string> free_variables;
    };
    
    //função usada para printar os elements da pilha principal
void    printMembers(vector<member> input){
        cout<<"stack size:"<<input.size()<<endl;
        for(int i=0;i<input.size();i++)
        {
            cout<<input[i].variable<<":[";
            for(set<string>::iterator it=input[i].free_variables.begin();it!=input[i].free_variables.end();it++)
                cout<<*it<<",";
            cout<<"]"<<endl;
        }
        cout<<endl;
    }

int main() {
    vector<member> my_stack;
    string previous,str;
    string line;
    member aux;
    int input_size=0,n;
    
    //exemplos criados e utilizados para testes
    //este vector só é utilizado quando test==1
    vector<string> examples={
        "(lambda (y) (y))",
        "(a A b B C c d D e E)",
        "(ab Ab (Ze ab) AAA aa AA Aa aA A)",
        "lambdax",
        "((lambdx a) (lambda (h) ((lambda (y) y) ( (lambda (y) ((a x)y))))))",
        "(lambda (lambdaa) (lambdab lambdac))",
        "(lambda (lambda a) (lambda b lambda c))",
        "(a ((A Z)z))",
        "(A ((a Z)z))",
        " (Ab ((ab Ze)Zx))",
        " (ab (Ab Ze))",
        "(y x)",
        "(y x)",
        "(lambda ( x ) (x y))",
        "((lambda ( gfdgfdg-hh-hh-- ) (gfdgfdg-hh-hh-- y)) (lambda (xx) ((z(y xx))z) ))",
        "(lambda (   y  ) (   y    abc-x ))",
        "x",
        "y",
        "(lambda (x) (x y))",
        "(lambda (y) (x y))",
        "( (lambda (x) x) (x y) )",
        "( lambda (y) ( lambda (z) (x (y z)) ) )",
        "( (lambda (y) (x z) )( lambda (z) (x (y z)) ) )",
        "((x y) (lambda (x) x))",
        "(((abc)((lambda (x) x) (x y)) )((lambda (x) x) z))",
        "(((lambda (x) x) (x y)) ((lambda (x) x) (z abc)))",
        "(((lambda (x) x) (x y)) (lambda (abc) abc))",
        "((((abc ifg)-kfg)hd-c)hdc)",
    };
#if test == 0
    string in;
    in.clear();
    getline(cin,in);
    input_size=atoi(in.c_str());
#else
        input_size=examples.size();
#endif

    //após receber a primeira entrada, por cin, com o número de equações.
    //realiza um laço principal responsável por processar uma equação por vez.
    for(n=0;n<input_size;n++)
    {
        my_stack.clear(); line.clear();
        
#if test == 0
        getline(cin,line);
#else
        line=examples[n];
#endif
        //lê a equação letra por letra.
        for(int i=0;i<line.size();i++)
        {
            //empilha quando a letra for um '('
            if(line.at(i)=='(')
            {
                aux.variable=line.at(i);
                aux.free_variables.clear();
                my_stack.push_back(aux);
            }
            //empilha o numero 0 em char como um coringa no lugar da palavra chave lambda.
            //(poderia ser qualquer string fora do conjunto das variáveis)
            else if (line.substr(i,6)=="lambda" && ((line.at(i+6)==' ') || (line.at(i+6)==')')))
            {
                    aux.variable='0';
                    aux.free_variables.clear();
                    my_stack.push_back(aux);
                    i+=6;
                    //logo em seguida já empilha também a variável de saída da expressão lambda.
                    //por exemplo, (lambda (y) (x y)), estariamos empilhando 0 e y apenas, as outras expressões ainda não foram lidas.
                    aux.variable=line.substr(line.find_first_of("(",i)+1,line.find_first_of(")",i)-i-2);
                    aux.variable.erase(remove_if(aux.variable.begin(), aux.variable.end(), ::isspace), aux.variable.end());
                    my_stack.push_back(aux);
            }
            //quando um fecha parentesis é encontrado, desempilha até encontrar um abre parenteses dentro da pilha.
            //um conjunto de variaveis livres é gerado a partir das variáveis internas aos parenteses.
            //cada variavel na pilha também carrega um conjunto de variaveis livres.
            else if (line.at(i)==')')
            {
                aux.variable="";
                aux.free_variables.clear();
                if(my_stack.size()>0)
                {            
                    while(my_stack.back().variable!="(")
                    {             
                        //caso um lambda é encontrado, lê a variável auxiliar "previous" que contem a string da 
                        //variavel a ser removida do conjunto de variaveis livres. Ou seja, a variável armazenada logo
                        //a frente do coringa 0.
                        if(my_stack.back().variable=="0")
                        {
                            if(aux.free_variables.find(previous)!=aux.free_variables.end())
                                aux.free_variables.erase(aux.free_variables.find(previous));
                            my_stack.pop_back();
                        }
                        else
                        {
                            //cada variavel terá um conjunto de variaveis livres já descobertas.
                            //aqui é feito a união destes conjuntos.
                            for(set<string>::iterator it=my_stack.back().free_variables.begin();it!=my_stack.back().free_variables.end();it++)
                                aux.free_variables.insert(*it);
                            //armazena qual é a variável atualmente lida caso um lambda seja encontrado no desempilhamento.
                            previous=my_stack.back().variable;
                            my_stack.pop_back();
                        }
                    }
                }
                if(my_stack.size()>0)
                    my_stack.pop_back();
                my_stack.push_back(aux);
            }
            //nada é feito para espaços em branco.
            else if 
                (line.at(i)==' ');
            //qualquer outra coisa é uma variável (presumindo que tenhamos uma entrada de formato valido).
            else
            {
                str.clear();
                //descobrindo a string inteira da variável. A pilha é lida letra por letra, 
                //mas quando uma variável é encontrada, unimos as letras para formar uma string.
                str=line.substr(i,min(line.find_first_of(' ',i),min(line.find_first_of('(',i),line.find_first_of(')',i)))-i);
                //o cursor de leitura da pilha também é atualizado
                i+=str.size()-1;
                aux.variable=str;
                //essa variavel carregara um novo conjunto de variáveis livres, 
                //onde ela é o unico elemnto deste conjunto.
                aux.free_variables.clear();
                aux.free_variables.insert(aux.variable);
                my_stack.push_back(aux); 
            }  
        }
        vector<string> lexogr;
        lexogr.clear();
        //printando a saída em ordem lexográfica.
        cout<<"Case #"<<n+1<<":";
        if(my_stack.back().free_variables.size()>0)
        {
            for(set<string>::iterator it=my_stack.back().free_variables.begin();it!=my_stack.back().free_variables.end();it++)
                    lexogr.push_back(*it);

            sort(begin(lexogr),end(lexogr),[](const string& str1, const string& str2){
                return lexicographical_compare(
                    begin(str1), end(str1),
                    begin(str2), end(str2),
                    [](const char& char1, const char& char2) {
                        return tolower(char1) < tolower(char2);});});
                        
            for(vector<string>::iterator it2=lexogr.begin();it2!=lexogr.end();it2++)
                    cout<<" "<<*it2; 
        }
        else
            cout<<" ";
        cout<<endl;
    } 
    return 0;
}

