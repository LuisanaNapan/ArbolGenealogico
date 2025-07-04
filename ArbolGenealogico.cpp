#include <iostream>
#include <string>
#include <queue> //se usa para implementar el algoritmo de B�squeda en Anchura (BFS) 
using namespace std;

struct Nodo {
    int id;
    string nombre;
    int nacimiento;
    string estado;
    Nodo* izq; //hijo menor o hermano menor
    Nodo* der; //hijo mayor o hermano mayor
    Nodo* padre;
};

Nodo* raiz = NULL;
int generaciones=0;

// Crear nuevo nodo
Nodo* crearNodo(int id, string nombre, int nacimiento, string estado) {
    Nodo* nuevo = new Nodo;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->nacimiento = nacimiento;
    nuevo->estado = estado;
    nuevo->izq = nuevo->der = nuevo->padre = NULL;
    return nuevo;
}

// Buscar miembro por ID
Nodo* buscar(Nodo* actual, int id) {
    if (actual == NULL || actual->id == id) return actual;
    return (id < actual->id) ? buscar(actual->izq, id) : buscar(actual->der, id);
}

//si el ID existe, primero buscamos el id
bool idExiste(int id) {
    return buscar(raiz, id) != NULL;
}

//validamos el ID
void pedirID(int& id) {
    do {
        cout << "\n\tID (DNI de 8 d�gitos)\t: ";
        cin >> id;
        if (id < 10000000 || id > 99999999)
            cout << "\tERROR: El DNI debe tener 8 d�gitos.\n";
        else if (idExiste(id))
            cout << "\tERROR: Ya existe un miembro con ese ID.\n";
        else
            break;
    } while (true);
}

//Solicita y valida los datos de un nuevo miembro
Nodo* solicitarDatosMiembro() {
    int id, nacimiento;
    string nombre, estado;
    bool estadoValido=false; //Bandera para validar el estado de vida

    pedirID(id); //llamamos a nuestra funcion de pedirID llamando tambien idExiste

    cout << "\tNombre\t\t\t: ";
    cin >> nombre;

    do {
        cout << "\tA�o de nacimiento\t: ";
        cin >> nacimiento;
        if (nacimiento <= 0 || nacimiento >= 2026)
            cout << "\tERROR: A�o inv�lido. Debe ser mayor a 0 y menor a 2026.\n\n";
    } while (nacimiento <= 0 || nacimiento >= 2026);

    while (!estadoValido) {
        cout << "\t�Est� vivo o ya falleci�? (vivo/fallecido): ";
        cin >> estado;

        if (estado != "vivo" && estado != "fallecido") {
            cout << "\tERROR: Debe ingresar 'vivo' o 'fallecido'.\n";
        } else if (nacimiento < 1925 && estado == "vivo") {
            cout << "\tERROR: Incoherente. Personas nacidas antes de 1925 deben haber fallecido.\n\n";
        } else {
            estadoValido = true; //Si pasa ambas condiciones, se acepta
        }
    }
    //Se retorna el nodo creado con los datos v�lidos
    return crearNodo(id, nombre, nacimiento, estado);
}

//Calcula el nivel de un nodo desde la ra�z (ra�z=nivel 1)
int calcularNivel(Nodo* nodo) {
    int nivel=1;
    while (nodo->padre != NULL) {
        nivel++;
        nodo = nodo->padre;
    }
    return nivel;
}

int calcularProfundidad(Nodo* nodo, int nivel) {
    if (nodo == NULL) return nivel - 1;
    int izq = calcularProfundidad(nodo->izq, nivel + 1);
    int der = calcularProfundidad(nodo->der, nivel + 1);
    return (izq > der) ? izq : der;
}

// Buscar Miembro por nombre
Nodo* buscarPorNombre(Nodo* nodo, string nombre) {
    if (!nodo) return NULL;// Caso base: si el nodo actual es NULL, el nombre no est� aqu�
    if (nodo->nombre == nombre) return nodo;// Si el nombre del nodo actual coincide con el buscado, lo devolvemos

    Nodo* izq = buscarPorNombre(nodo->izq, nombre);// Buscamos recursivamente en el sub�rbol izquierdo
    if (izq) return izq;// Si lo encontramos en el lado izquierdo, lo devolvemos

    return buscarPorNombre(nodo->der, nombre);// Si no lo encontramos en el izquierdo, buscamos en el derecho
}

bool validaranio(Nodo* nuevo, Nodo* relativo, int relacion){
	int diferencia;

    switch (relacion) {
        case 1: { // Padre / madre
            diferencia = relativo->nacimiento - nuevo->nacimiento;
            if (diferencia < 12) { //si la diferencia de la edad del hijo(relativo) - la edad de la raiz(nuevo) es menor a 12...
                cout << "\tError: Es muy joven para ser padre/madre de " << relativo->nombre << ".\n";
                return false;
            }
            return true; //continua y valida la edad en caso de que el padre si sea mayor o igual a 12 a�os
        }
		//si la edad del nuevo ingresante - el del relativo es menor a 12 tambi�n decimos que al diferencia de edad no es valida
        case 2: // Hijo izquierdo
        case 3: { // Hijo derecho
            diferencia = nuevo->nacimiento - relativo->nacimiento;
            if (diferencia < 12) {
                cout << "\n\tError: muy poca diferencia de edad para ser hijo/a de " << relativo->nombre << ".\n";
                return false;
            }
            return true; //si la edad es valida continuamos
        }
		//la diferencia de edad entre el nuevo miembro y la Raiz es mayor a 40
        case 4: // Hermano izquierdo
        case 5: { // Hermano derecho
            diferencia = nuevo->nacimiento - relativo->nacimiento;
            if (diferencia > 40) { //decimos que hay un error ya que no siempre la edad es tan diferente
                cout << "\n\tError: demasiada diferencia de edad para ser hermanos.\n";
                return false;
            }
            return true; //si est� dentro del rango etonces continuamos
        }

        default: //si no cumple ninguno de esos casos decimos que no es v�lida la relaci�n
            cout << "\tRelaci�n no v�lida.\n";
            return false;
    }
}

//Inserta un nuevo nodo al �rbol geneal�gico seg�n la relaci�n especificada
void insertarFlexible(Nodo*& raiz, Nodo* nuevo, int generacionesMax, int idRelacionado, int relacion) {
    //Se busca al familiar con el que se establecer� la relaci�n
    Nodo* relativo = buscar(raiz, idRelacionado);
    if (!relativo) {
        cout << "Miembro no encontrado.\n";
        return;
    }
    
    //Se eval�a el tipo de relaci�n indicada (1 a 5)
    switch (relacion) {
        case 1: //Padre/Madre
            if (relativo == raiz) {
                //Si el relativo es la ra�z, el nuevo se convierte en la nueva ra�z
                nuevo->izq = raiz;       //El anterior ra�z pasa a ser hijo menor del nuevo padre
                raiz->padre = nuevo;     //Se actualiza el puntero padre de la antigua ra�z
                raiz = nuevo;            //Se actualiza la ra�z del �rbol
            } else {
                //Si el relativo no es la ra�z
                Nodo* padreAnt = relativo->padre; //Se guarda su padre anterior
                nuevo->padre = padreAnt;          //El nuevo miembro apunta como padre al padre anterior
                relativo->padre = nuevo;          //El relativo ahora es hijo del nuevo
                if (padreAnt) {
                    //Se actualizan los punteros del padre anterior para enlazarlo con el nuevo
                    if (padreAnt->izq == relativo) padreAnt->izq = nuevo;
                    else padreAnt->der = nuevo;
                }
                nuevo->izq = relativo;  //El nuevo miembro adopta al relativo como hijo menor
            }
            cout << "\tAsignado como padre/madre.\n";
            break;

        case 2: //Hijo menor
            if (!relativo->izq) {
                //Si el relativo no tiene hijo menor
                relativo->izq = nuevo;      //Se asigna el nuevo como hijo menor
                nuevo->padre = relativo;    //Se enlaza al padre
                cout << "\tAsignado como hijo menor.\n";
            } else {
                cout << "\tYa tiene hijo menor.\n"; //Ya existe un hijo menor
            }
            break;

        case 3: //Hijo mayor
            if (!relativo->der) {
                //Si el relativo no tiene hijo mayor
                relativo->der = nuevo;      //Se asigna como hijo mayor
                nuevo->padre = relativo;    //Se enlaza al padre
                cout << "\tAsignado como hijo mayor.\n";
            } else {
                cout << "\tYa tiene hijo mayor.\n"; //Ya existe un hijo mayor
            }
            break;

        case 4: //Hermano menor
            if (!relativo->padre) {
                //Si el relativo no tiene padre registrado, no se puede agregar hermano
                cout << "\tNo tiene padre registrado, no se puede agregar hermano.\n";
                break;
            }
            if (relativo->padre->izq == NULL) {
                //Si el padre no tiene hijo menor a�n
                relativo->padre->izq = nuevo;        //Se coloca al nuevo como hijo menor
                nuevo->padre = relativo->padre;      //Se enlaza al mismo padre
                cout << "\tAsignado como hermano menor.\n";
            } else {
                //Ya existe un hijo menor, por lo tanto ya hay hermano en ese lugar
                cout << "\tYa existe un hijo menor, no se puede agregar como hermano.\n";
            }
            break;

        case 5: //Hermano mayor
            if (!relativo->padre) {
                //Si el relativo no tiene padre registrado, no se puede agregar hermano
                cout << "\tNo tiene padre registrado, no se puede agregar hermano.\n";
                break;
            }
            if (relativo->padre->der == NULL) {
                //Si el padre no tiene hijo mayor a�n
                relativo->padre->der = nuevo;        //Se coloca al nuevo como hijo mayor
                nuevo->padre = relativo->padre;      //Se enlaza al mismo padre
                cout << "\tAsignado como hermano mayor.\n";
            } else {
                //Ya existe un hijo mayor, por lo tanto ya hay hermano en ese lugar
                cout << "\tYa existe un hijo mayor, no se puede agregar como hermano.\n";
            }
            break;

        default: //En caso de que la opci�n de relaci�n no sea v�lida
            cout << "\tRelaci�n no v�lida.\n";
    }
}

void ingresarMiembro() {
	//Si a�n no se ha definido el n�mero de generaciones, se solicita al usuario
    if (generaciones == 0) {
        do {
            cout << "\n\tIngrese cu�ntas generaciones tendr� el �rbol geneal�gico: ";
            cin >> generaciones;
            if (generaciones <= 0) {
                cout << "\tDebe ser mayor a 0.\n";
            }
        } while (generaciones <= 0);
    }
	//Si la ra�z no existe, se inserta el primer miembro como ra�z
    if (!raiz) {
        raiz = solicitarDatosMiembro();
        cout << "\tPrimer miembro insertado como ra�z.\n";
	}
	else{ //Si ya hay miembros, se solicita ID del familiar relacionado
	    int idRelacionado, relacion;
	    cout << "\n\tIngrese el ID del familiar relacionado: ";
	    cin >> idRelacionado;
	
	    // Se busca el nodo relacionado en el �rbol
	    Nodo* relativo = buscar(raiz, idRelacionado);
	    if (!relativo) { //Si no se encuentra, se cancela
	        cout << "\tMiembro no encontrado.\n";
	        return;
	    }
	
	    cout << "\n\t�Qu� relaci�n tiene con " << relativo->nombre << "?\n";
	    cout << "\t1-> Padre/Madre\n";
	    cout << "\t2-> Hijo/a menor/a\n";
	    cout << "\t3-> Hijo/a mayor/a\n";
	    cout << "\t4-> Hermano/a menor/a\n";
	    cout << "\t5-> Hermano/a mayor/a\n";
	    cout << "\n\tSeleccione una opci�n: ";
	    cin >> relacion;
	    
	    //Validaci�n previa: no tiene padre, pero quiere insertar hermano
	    if ((relacion==4||relacion==5) && relativo->padre == NULL) {
	        cout << "\tError: no se puede insertar hermano/a, el miembro relacionado no tiene padre registrado.\n";
	        return;
		}
	
	    //Se calcula el nivel actual del nodo relacionado
	    int nivelActual = calcularNivel(relativo);
	
	    //Validaciones para no exceder el l�mite de generaciones seg�n el tipo de relaci�n
	    if ((relacion == 1 && nivelActual >= generaciones) ||
	        ((relacion == 2 || relacion == 3) && nivelActual + 1 > generaciones) ||
	        ((relacion == 4 || relacion == 5) && nivelActual > generaciones)) {
	        cout << "\tNo se puede insertar: excede las generaciones permitidas.\n";
	        return;
	    }
	    
	    //Se solicita los datos del nuevo miembro
	    Nodo* nuevo = solicitarDatosMiembro();
	    
	    //Se valida si los a�os de nacimiento son coherentes con la relaci�n elegida
	    if (!validaranio(nuevo, relativo, relacion)) {
	        delete nuevo; // Se libera memoria
	        return;
	    }
	    //Si todo es v�lido, se inserta el nodo en el �rbol con la relaci�n adecuada
	    insertarFlexible(raiz, nuevo, generaciones, idRelacionado, relacion);
    }
}

// M�nimo del sub�rbol
Nodo* minimo(Nodo* actual) {
    while (actual && actual->izq != NULL)
        actual = actual->izq;
    return actual;
}
//ELIMINAR 
void eliminarMiembro(Nodo*& nodo, int id) {
    if (nodo == NULL) return;  // Si el nodo es nulo, no hay nada que eliminar

    // Recorrer primero el sub�rbol izquierdo y derecho (postorden)
    eliminarMiembro(nodo->izq, id);  // Eliminar en sub�rbol izquierdo
    eliminarMiembro(nodo->der, id);  // Eliminar en sub�rbol derecho

    // Procesar el nodo actual si su id coincide con el id a eliminar
    if (nodo->id == id) {

        // Caso 1: Nodo hoja (sin hijos)
        if (nodo->izq == NULL && nodo->der == NULL) {
            cout << "Se elimin� un nodo hoja (sin hijos): " << nodo->nombre << endl;  // Mensaje informativo
            delete nodo;   // Liberar memoria del nodo
            nodo = NULL;   // Desconectar el puntero para evitar acceso a memoria liberada
        }

        // Caso 2: Nodo con solo hijo izquierdo
        else if (nodo->izq != NULL && nodo->der == NULL) {
            cout << "Se elimin� un nodo con solo hijo izquierdo: " << nodo->nombre << endl;  // Mensaje
            Nodo* temp = nodo;      // Guardar puntero temporal al nodo a eliminar
            nodo = nodo->izq;       // Reemplazar el nodo actual por su hijo izquierdo
            if (nodo != NULL) nodo->padre = temp->padre;  // Actualizar puntero padre del nuevo nodo
            delete temp;            // Eliminar el nodo original
        }

        // Caso 3: Nodo con solo hijo derecho
        else if (nodo->izq == NULL && nodo->der != NULL) {
            cout << "Se elimin� un nodo con solo hijo derecho: " << nodo->nombre << endl;  // Mensaje
            Nodo* temp = nodo;      // Guardar puntero temporal
            nodo = nodo->der;       // Reemplazar el nodo actual por su hijo derecho
            if (nodo != NULL) nodo->padre = temp->padre;  // Actualizar padre
            delete temp;            // Liberar memoria del nodo original
        }

        // Caso 4: Nodo con dos hijos
        else {
            cout << "Se elimin� un nodo con dos hijos (reemplazo por sub�rbol izquierdo): " << nodo->nombre << endl;  // Mensaje
            Nodo* reemplazo = nodo->izq;  // Buscar nodo m�ximo en sub�rbol izquierdo (predecesor)
            Nodo* padreReemplazo = nodo;  // Puntero para seguir al padre del reemplazo
            while (reemplazo->der != NULL) {  // Mientras haya hijo derecho
                padreReemplazo = reemplazo;    // Avanzar padre
                reemplazo = reemplazo->der;    // Avanzar al hijo derecho
            }
            // Copiar datos del nodo reemplazo al nodo actual
            nodo->id = reemplazo->id;
            nodo->nombre = reemplazo->nombre;
            // Eliminar recursivamente el nodo duplicado en sub�rbol izquierdo
            eliminarMiembro(nodo->izq, reemplazo->id);
        }
    }
}


//ELIMINAR POR NIVELES
void eliminarPorNivel(Nodo*& raiz, int nivelObjetivo) {
    if (raiz == NULL) return;  // Si el �rbol est� vac�o, no hay nada que eliminar
    
    // Cola para recorrer el �rbol en anchura (BFS), guarda pares <nodo, nivel>
    queue<pair<Nodo*, int> > cola;  // Nota el espacio entre > >
    cola.push(make_pair(raiz, 0)); // Insertar la ra�z con nivel 0

    while (!cola.empty()) {
        Nodo* actual = cola.front().first;  // Nodo actual a procesar
        int nivel = cola.front().second;    // Nivel del nodo actual
        cola.pop();                         // Sacar el nodo de la cola

        if (actual == NULL) continue;       // Si el nodo es nulo, saltar

        // Encolar hijos con nivel incrementado
        if (actual->izq != NULL) cola.push(make_pair(actual->izq, nivel + 1));
        if (actual->der != NULL) cola.push(make_pair(actual->der, nivel + 1));

        // Si el nivel actual es el nivel objetivo, eliminar el nodo
        if (nivel == nivelObjetivo) {
            cout << "Eliminando miembro en nivel " << nivel << ": " << actual->nombre << endl;
            eliminarMiembro(actual, actual->id);  // Llamar a la funci�n para eliminar el nodo espec�fico
        }
    }
}

//imprimir recorrido con datos completos
void imprimirFicha(Nodo* nodo, const string& tipo) {
    cout << "\t----------------------------------\n";
    cout << "\tTipo      : " << tipo << "\n";
    cout << "\tNombre    : " << nodo->nombre << "\n";
    cout << "\tID (DNI)  : " << nodo->id << "\n";
    cout << "\tNacimiento: " << nodo->nacimiento << "\n";
    cout << "\tFallecido : " << nodo->estado << "\n";
}

// Recorridos
void inorden(Nodo* nodo, string tipo = "RAIZ") {
    if (!nodo) return;
    inorden(nodo->izq, "Hijo Izquierdo");
    imprimirFicha(nodo, tipo);
    inorden(nodo->der, "Hijo Derecho");
}

void preorden(Nodo* nodo, string tipo = "RA�Z") {
    if (!nodo) return;
    imprimirFicha(nodo, tipo);
    preorden(nodo->izq, "Hijo Izquierdo");
    preorden(nodo->der, "Hijo Derecho");
}

void postorden(Nodo* nodo, string tipo = "RA�Z") {
    if (!nodo) return;
    postorden(nodo->izq, "Hijo Izquierdo");
    postorden(nodo->der, "Hijo Derecho");
    imprimirFicha(nodo, tipo);
}

void porNiveles(Nodo* raiz) {
    if (raiz == NULL) return;

    const int MAX = 100;
    Nodo* cola[MAX];
    int inicio = 0, fin = 0;

    // Encolar ra�z
    cola[fin++] = raiz;

    while (inicio < fin) {
        Nodo* actual = cola[inicio++]; // Desencolar

        imprimirFicha(actual, "Pariente");

        // Encolar hijos
        if (actual->izq != NULL) {
            cola[fin++] = actual->izq;
        }
        if (actual->der != NULL) {
            cola[fin++] = actual->der;
        }
    }
}

//Ancestros
void mostrarAncestros(Nodo* ancestro, Nodo* hijoOriginal) {
    if (ancestro == NULL) {// Si no hay ancestro
        cout << "\n\tEste nodo no tiene ancestros registrados.\n";
        return;
    }

    int nivel = 1; // Contador de generaciones ascendentes

    // Recorremos hacia arriba por el �rbol mientras existan padres
    while (ancestro) {
        // Mostramos el nivel generacional y los datos del ancestro actual
        cout << "\n\tNivel " << nivel << " - Ancestro:\n";
        imprimirFicha(ancestro, "Ancestro");

        // Calculamos la diferencia de nacimiento entre el ancestro y el miembro consultado
        int diferencia = (hijoOriginal->nacimiento > ancestro->nacimiento) 
                         ? (hijoOriginal->nacimiento - ancestro->nacimiento) 
                         : (ancestro->nacimiento - hijoOriginal->nacimiento);

        // Mostramos esa diferencia de a�os
        cout << "\tDiferencia de nacimiento con respecto a " << hijoOriginal->nombre
             << ": " << diferencia << " a�os\n";

        // Subimos un nivel m�s arriba en la l�nea de ancestros
        ancestro = ancestro->padre;
        nivel++; // Aumentamos el nivel generacional
    }
}

//Descendientes (en preorden)
void mostrarDescendientes(Nodo* nodo) {
    
    if (!nodo) return;// Caso base: si el nodo es nulo, salimos de la funci�n

    // Si el nodo es hoja, mostramos esto
    if (nodo->izq == NULL && nodo->der == NULL) {
        cout << "\n\tEste nodo no tiene hijos (hojas).\n";
        return;
    }

    // Si tiene hijo izquierdo, mostramos su informaci�n
    // y llamamos recursivamente para buscar m�s descendientes desde all�
    if (nodo->izq) {
        imprimirFicha(nodo->izq, "Descendiente (izquierdo)");
        mostrarDescendientes(nodo->izq);
    }

    // Si tiene hijo derecho, lo mostramos e iniciamos recorrido desde ese hijo
    if (nodo->der) {
        imprimirFicha(nodo->der, "Descendiente (derecho)");
        mostrarDescendientes(nodo->der);
    }
}

// Men� principal
int main() {
    setlocale(LC_CTYPE, "Spanish");

    int opcion;
    do {
	cout << "\n\t----------------------------------\n";
        cout << "\t|---- MENU �RBOL GENEAL�GICO ----|\n";
	cout << "\t----------------------------------\n";
        cout << "\n\t[1]. Insertar miembro\n";
        cout << "\t[2]. Buscar miembro\n";
        cout << "\t[3]. Eliminar miembro\n";
        cout << "\t[4]. Consultar ancestros\n";
        cout << "\t[5]. Consultar descendientes\n";
        cout << "\t[6]. Recorridos\n";
        cout << "\t[7]. Salir\n";
        cout << "\n\tSeleccione una opci�n: ";
        cin >> opcion;

        switch (opcion) {
			case 1: 
				ingresarMiembro();
			break;
            case 2: {
                int id;
                cout << "\n\tIngrese ID a buscar: ";
                cin >> id;
                Nodo* resultado = buscar(raiz, id);
                if (resultado)
                    cout << "\tEncontrado: " << resultado->nombre << " (" << resultado->id << ")\n";
                else
                    cout << "\tNo encontrado.\n";
                break;
            }

	    case 3: {
                int tipoEliminacion;
                cout << "\n\tTipo de eliminaci�n:\n";
                cout << "\t[1] Eliminar por ID\n";
                cout << "\t[2] Eliminar por nivel\n";
                cout << "\tSeleccione: ";
                cin >> tipoEliminacion;

                if (tipoEliminacion == 1) {
                    int id;
                    cout << "\tID a eliminar: ";
                    cin >> id;
                    eliminarMiembro(raiz, id);
                } 
                else if (tipoEliminacion == 2) {
                    int nivel;
                    cout << "\tNivel a eliminar: ";
                    cin >> nivel;
                    eliminarPorNivel(raiz, nivel);
                } 
                else {
                    cout << "\tOpci�n inv�lida\n";
                }
                break;
            }

            case 4: {
                int tipoBusqueda;
                cout << "\n\t�C�mo desea buscar al miembro?\n";
                cout << "\t[1] Por ID\n";
                cout << "\t[2] Por nombre\n";
                cout << "\tSeleccione una opci�n: ";
                cin >> tipoBusqueda;

                Nodo* n = NULL;
                if (tipoBusqueda == 1) {
                    int id;
                    cout << "\tIngrese ID para ver ancestros: ";
                    cin >> id;
                    n = buscar(raiz, id);
                } else if (tipoBusqueda == 2) {
                    string nombre;
                    cout << "\tIngrese nombre exacto para ver ancestros: ";
                    cin >> nombre;
                    n = buscarPorNombre(raiz, nombre);
                } else {
                    cout << "\tOpci�n inv�lida.\n";
                    break;
                }

                if (n)
                     mostrarAncestros(n->padre, n);
                else
                    cout << "\tMiembro no encontrado.\n";

                break;
            }

            case 5: {
                int tipoBusqueda;
                cout << "\n\t�C�mo desea buscar al miembro?\n";
                cout << "\t[1] Por ID\n";
                cout << "\t[2] Por nombre\n";
                cout << "\tSeleccione una opci�n: ";
                cin >> tipoBusqueda;

                Nodo* n = NULL;
                if (tipoBusqueda == 1) {
                    int id;
                    cout << "\tIngrese ID para ver descendientes: ";
                    cin >> id;
                    n = buscar(raiz, id);
                } else if (tipoBusqueda == 2) {
                    string nombre;
                    cout << "\tIngrese nombre exacto para ver descendientes: ";
                    cin >> nombre;
                    n = buscarPorNombre(raiz, nombre);
                } else {
                    cout << "\tOpci�n inv�lida.\n";
                    break;
                }

                if (n)
                    mostrarDescendientes(n);
                else
                    cout << "\tMiembro no encontrado.\n";

                break;
            }

            case 6: {
                int tipo;
                cout << "\n\t1. Inorden\n\t2. Preorden\n\t3. Postorden\n\t4. Por niveles\n\n\tOpci�n: ";
                cin >> tipo;
                switch (tipo) {
                    case 1:
			cout << "\n\t========= RECORRIDO EN INORDEN =========\n";
			inorden(raiz); break;
                    case 2:
                    	cout << "\n\t========= RECORRIDO EN PREORDEN =========\n";
			preorden(raiz); break;
                    case 3:
                    	cout << "\n\t========= RECORRIDO EN POSTORDEN =========\n";
			postorden(raiz); break;
                    case 4:
                    	cout << "\n\t========= RECORRIDO POR NIVELES =========\n";
			porNiveles(raiz); break;
                    default: cout << "\n\tOpci�n no v�lida.\n"; break;
                }
                break;
            }

            case 7:
                cout << "\tPrograma finalizado.\n";
                break;

            default:
                cout << "\tOpci�n inv�lida. Intente de nuevo.\n";
                break;
        }

    } while (opcion != 7);

    return 0;
}
