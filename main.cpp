#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib> // для atoi
#include <fstream>


const int MAX = 32;		// Используется для 
						// ограничения размера массивов связей
// Абстрактный класс элемента, наследуемый Bone и Point
class Element {
	friend class Graph;
	protected:
		int id;
		std::string type;
		int weight;
		
		// linkBack и linkFront - массивы указателей на другие
		// элементы, с которым связан данный.
		// back - те, от которых идёт элемент,
		// front - те, которые берут начало в элементе.
		// Например, ребро, направленное в вершину A,
		// Попадёт в её linkBack.
		// Массивы были разделены для более удобной навигации
		// при поиске путей среди точек и ускорения работы.
		
		Element* linkBack[MAX];
		int backsize;
		Element* linkFront[MAX];
		int frontsize;
		
		void removeFront(Element* link) {
			int i;
			for (i = 0; i < frontsize; i++) {
				if (linkFront[i] == link)
					break;
			}
			for (i; i < frontsize; i++) {
				linkFront[i] = linkFront[i+1];
			}
			frontsize--;
		}
		
		void removeBack(Element* link) {
			int i;
			for (i = 0; i < backsize; i++) {
				if (linkBack[i] == link)
					break;
			}
			for (i; i < backsize; i++) {
				linkBack[i] = linkBack[i+1];
			}
			backsize--;
		}
		
	private:
		virtual void addFront(Element* link) { };
		
		virtual void addBack(Element* link) { };
		
	public:
		Element() {
			
		}
		~Element() {
			if (type == "bone") {
				linkBack[0]->removeFront(this);
				linkFront[0]->removeBack(this);
			}
		}
};

// Класс ребра
class Bone : Element {
	friend class Graph;
	private:
		
		void addFront(Element* link) {
			linkFront[0] = link;
		}
		
		void addBack(Element* link) {
			linkBack[0] = link;
		}
		
	public:
		Bone(Element* start, Element* end, int weight, int id) {
			type = "bone";
			backsize = 1;
			frontsize = 1;
			
			linkBack[0] = start;
			linkFront[0] = end;
			this->weight = weight;
			this->id = id;
		}
		
		int getWeight() {
			return weight;
		}
};

// Класс вершины
class Point : Element {
	friend class Graph;
	private:
		std::string name;
		
		void addFront(Element* link) {
			linkFront[frontsize] = link;
			frontsize++;
		}
		
		void addBack(Element* link) {
			linkBack[backsize] = link;
			backsize++;
		}
		
	public:
		Point(int id) {
			type = "point";
			this->id = id;
			backsize = 0;
			frontsize = 0;
		}
		
		Point(int id, std::string name) : Point(id) {
			this->name = name;
		}
		
		std::string getName() {
			return name;
		}
};

// Класс двумерного массива, вынесен из Graph только для перегрузки
class Grid {
	public:
		int m[MAX][MAX];
		Grid(int size) {
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					m[i][j] = 0;
		};
};

const void operator+(Grid& grid, int* point) {
	grid.m[point[0]][point[1]] = 1;
}
const void operator-(Grid& grid, int* point) {
	grid.m[point[0]][point[1]] = 0;
}
// Класс графа
class Graph {
	private:
		Bone* bones[MAX];
		int bonesAmount;
		Point* points[MAX];
		int pointsAmount;
		
	public:
		Graph() {
			bonesAmount = 0;
			pointsAmount = 0;
		}
		
		Graph(char* filename) : Graph() {
			std::fstream tgf;
			tgf.open(filename);	
			
			std::string in;
			char inC[128];
			
	 		getline(tgf,in);
	 		if (in.empty()) {
	 			std::cout << "(!) Файл не найден";
	 			tgf.close();
			} else {
			
			int size = 0;
			
	 		while (in.compare("#")) {
				strcpy(inC,in.c_str());
	 			char *token = strtok(inC," ");
				token = strtok(NULL," ");
				if (token != NULL) {
					addPoint(token);
				} else {
					addPoint("");
				}
				getline(tgf,in);
			}
			
			getline(tgf,in);
			while (!tgf.eof()) {
				strcpy(inC,in.c_str());
	 			char *token = strtok(inC," ");
	 			
	 			int id1 = std::atoi(token)-1;
	 			token = strtok(NULL," ");
	 					
	 			int id2 = std::atoi(token)-1;		 
	 			token = strtok(NULL," ");
					 	
	 			if (token != NULL) {
	 				int weight = std::atoi(token);
					addBone(id1,id2,weight);
	 			} else {
					addBone(id1,id2,1);
				}
				
				getline(tgf,in);
				
			}
	 			
			tgf.close();
		}
		
		}
		
		void addPoint(std::string name) {	
		
			points[pointsAmount] = new Point(pointsAmount,name);
			pointsAmount++;
			
		}
		
		bool delPoint(int id) {
			if (id >= pointsAmount || id < 0) 
				return false;
		
			for (int k = 0; k < bonesAmount; k++) {
				if (bones[k]->linkBack[0] == points[id]) {
					delBone(k);
					k--;
				} else if (bones[k]->linkFront[0] == points[id]) {
					delBone(k);
					k--; 
				}
						
			}
					
			delete points[id];
			pointsAmount--;
			for (int i = id; i < pointsAmount; i++) {
				points[i] = points[i+1];
				points[i]->id--;
			}
					
			return true;
			
		}
		
		bool addBone(int id1, int id2, int weight) {
			if (id1 >= pointsAmount || id2 >= pointsAmount || id1 < 0 || id2 < 0)
				return false;
			
			bones[bonesAmount] = new Bone(points[id1],points[id2],weight,bonesAmount);
			
			points[id1]->linkFront[points[id1]->frontsize] = bones[bonesAmount];
			points[id1]->frontsize++;
			
			points[id2]->linkBack[points[id2]->backsize] = bones[bonesAmount];
			points[id2]->backsize++;

			bonesAmount++;
			return true;
		}
		
		bool delBone(int id) {
			if (id >= bonesAmount || id < 0)
				return false;
				
			delete bones[id];
			bonesAmount--;
			for (int i = id; i < bonesAmount; i++) {
				bones[i] = bones[i+1];
				bones[i]->id--;
			}
			return true;
		}
		
		bool changeWeight(int id, int inweight) {
			if (id >= bonesAmount || id < 0)
				return false;
			
			bones[id]->weight = inweight;
			return true;
		}
		
		bool changeDir(int id) {
			if (id >= bonesAmount || id < 0)
				return false;
			
			Element* temp = bones[id]->linkBack[0];
			bones[id]->linkBack[0] = bones[id]->linkFront[0];
			bones[id]->linkFront[0] = temp;
			
			points[bones[id]->linkBack[0]->id]->addFront(bones[id]);
			points[bones[id]->linkBack[0]->id]->removeBack(bones[id]);
			
			points[bones[id]->linkFront[0]->id]->addBack(bones[id]);
			points[bones[id]->linkFront[0]->id]->removeFront(bones[id]);
			
			return true;
		}
		
		int first() {
			if (pointsAmount == 0) {
				return -1;
			}
			return 0;
		}
		
		int next(int id1, int id2) {
			if (id1 >= pointsAmount || id2 >= pointsAmount || id1 < 0 || id2 < 0)
				return -1;
			
			Point* source = points[id1];
			Point* target = points[id2];
			
			int tid = target->id;
			int idlist[source->frontsize];
			
			for (int i = 0; i < source->frontsize; i++) {
				idlist[i] = source->linkFront[i]->linkFront[0]->id; // айди вершины, являющейся концом дуги, начинающейся из вершины, записанной в source
			}
			
			int tmp;
    		for (int i = 0; i < source->frontsize; ++i) { 
        		int pos = i; 
        		tmp = idlist[i];
        		for (int j = i + 1; j < source->frontsize; ++j) {
            		if (idlist[j] < tmp) {
		               pos = j; 
        		       tmp = idlist[j]; 
           			}
        		}
        		idlist[pos] = idlist[i]; 
        		idlist[i] = tmp;
		    }	
		    
		    bool found = false;
		    for (int i = 0; i < source->frontsize; i++) {
		    	if (found)
		    		return idlist[i];
				
				if (idlist[i] == tid)
		    		found = true;
			}
			return idlist[0];
		}
		
		int vertex(int id1, int n) {
			if (id1 >= pointsAmount || n >= points[id1]->frontsize || id1 < 0 || n < 0)
				return -1;
			
			Point* source = points[id1];
			
			int idlist[source->frontsize];
			
			for (int i = 0; i < source->frontsize; i++) {
				idlist[i] = source->linkFront[i]->linkFront[0]->id; // айди вершины, являющейся концом дуги, начинающейся из вершины, записанной в source
			}	
			
			int tmp;
    		for (int i = 0; i < source->frontsize; ++i) { 
        		int pos = i; 
        		tmp = idlist[i];
        		for (int j = i + 1; j < source->frontsize; ++j) {
            		if (idlist[j] < tmp) {
		               pos = j; 
        		       tmp = idlist[j]; 
           			}
        		}
        		idlist[pos] = idlist[i]; 
        		idlist[i] = tmp;
		    }
		    
		    return idlist[n];
		}
		
		void print() {
			std::cout << "\nВершины: \n";
			for (int i = 0; i < pointsAmount; i++) {
				Point* p = points[i];
				std::cout 
					<< "ID " << p->id << ": \n    "
					<< "Имя - " << p->getName() << "\n"
					<< "    Входящие рёбра: \n";
					for (int i = 0; i < p->backsize; i++) {
						std::cout
							<< "    > id " 
							<< p->linkBack[i]->id
							<< " (из вершины "
							<< p->linkBack[i]->linkBack[0]->id
							<< " c весом "
							<< p->linkBack[i]->weight
							<< ");\n";
					} 
					if (p->backsize == 0) std::cout << "      отсутствуют;\n";
					std::cout << "    Исходящие рёбра: \n";
					for (int i = 0; i < p->frontsize; i++) {
						std::cout 
							<< "    > id "
							<< p->linkFront[i]->id
							<< " (в вершину "
							<< p->linkFront[i]->linkFront[0]->id
							<< " c весом "
							<< p->linkFront[i]->weight
							<< ");\n";
					} 
					if (p->frontsize == 0) std::cout << "      отсутствуют;\n";
				std::cout << "\n";
			}
			if (pointsAmount == 0)
				std::cout << "отсутствуют.\n";
			std::cout << "\nРёбра: \n";
			for (int i = 0; i < bonesAmount; i++) {
				Bone* b = bones[i];
				std::cout 
					<< b->id << ": "
					<< b->linkBack[0]->id
					<< " -> "
					<< b->linkFront[0]->id
					<< " с весом "
					<< b->getWeight()
					<< "\n";
			}
			if (bonesAmount == 0)
				std::cout << "отсутствуют.\n";
		}
		
		// TASK
		
		Grid* wayMap = new Grid(pointsAmount);
		Grid* weightTable = new Grid(pointsAmount);
		int wayId;
		int way[MAX];
		int wayLength;
		int requestedLength;
		
		void fillMatrix() {
			wayMap = new Grid(pointsAmount);
			weightTable = new Grid(pointsAmount);
			
			for (int i = 0; i < bonesAmount; i++) {
				int x = bones[i]->linkBack[0]->id;
				int y = bones[i]->linkFront[0]->id;
				weightTable->m[x][y] = bones[i]->weight;
			}
			std::cout << "\nМатрица весов дуг заполнена:\n\n";
			for (int i = 0; i < pointsAmount; i++) {
				for (int j = 0; j < pointsAmount; j++)
					std::cout << weightTable->m[i][j] << "  ";
				std::cout << "\n";
			}
		}
		
		void printWay() {
			std::cout << "Путь " << wayId++ << ":";
			for (int i = 0; i < wayLength; i++) {
				if (i > 0)
					std::cout << " -> ";
				std::cout << " id " << way[i];	
			}
			std::cout << ".\n";
		}
		
		void Checker(int myway[2], int weight) {
			if (myway[0] != -1) 
				*wayMap + myway;
			way[wayLength] = myway[1];
			wayLength++;
			
			for (int i = 0; i < pointsAmount; i++) {
				if (weightTable->m[myway[1]][i] >= 1) {
					if (wayMap->m[myway[1]][i] == 0 && (weightTable->m[myway[1]][i] + weight <= requestedLength)) { 
											// ^ для включения отрицательных чисел удалить второе условие
						if (weightTable->m[myway[1]][i] + weight == requestedLength && way[0] == i) {
							way[wayLength] = i;
							wayLength++;	
							printWay();
							wayLength--;
						} else {
							int newway[2] = {myway[1],i};
							Checker(newway,weightTable->m[myway[1]][i] + weight);
						}
					}
				}
			}
			
			wayLength--;
			if (myway[0] != -1) 
				*wayMap - myway;
		}
		
		void findCycles(int length) {
			
			requestedLength = length;
			fillMatrix();
			wayId = 1;
			wayLength = 0;
	
			for (int i = 0; i < pointsAmount; i++) {
				int myway[2] = {-1,i};
				Checker(myway,0);
			}
						
			
		}
};		
/*===========================================================*/
/*===========================================================*/
/*===========================================================*/

Graph* g = new Graph();

void readTgf() {
	std::cout << "\n\n[Чтение из TGF]\n\n";
	
	std::cout << "Введите имя файла:  ";
	
	char filename[64];
	std::cin >> filename;
	
	g = new Graph(filename);
	
	std::cout << "\n\nОперация завершена.\n\n";
}

void printGraph() {
	std::cout << "\n\n[Вывод графа]\n\n";
	g->print();
	std::cout << "\n\n";
}
/*===========================================================*/

void addPoint() {
	std::cout << "\n\n[Добавление вершины]\n\n"
		<< "Введите имя вершины:  ";
	
	std::string name;
	std::cin >> name;
	
	g->addPoint(name);
	
	std::cout << "\n\nОперация завершена.\n\n";
}

void removePoint() {
	std::cout << "\n\n[Удаление вершины]\n\n"
		<< "Введите id удаляемой вершины:  ";
		
	int id;
	std::cin >> id;
	
	if (g->delPoint(id)) {
		std::cout << "\n\nОперация завершена.\n\n";	
	} else {
		std::cout << "\n\n(!) Индексы находятся за пределами пространства индексов.\n\n";
	}	
}

void addBone() {
	std::cout << "\n\n[Добавление ребра]\n\n";
	
	std::cout << "Введите id вершины-источника:  ";
	int id1;
	std::cin >> id1;
	
	std::cout << "Введите id вершины-цели:  ";
	int id2;
	std::cin >> id2;
	
	std::cout << "Введите вес ребра:  ";
	
	int weight;
	std::cin >> weight;
	
	if (g->addBone(id1,id2,weight)) {
		std::cout << "\n\nОперация завершена.\n\n";	
	} else {
		std::cout << "\n\n(!) Индексы находятся за пределами пространства индексов.\n\n";
	}
	
}

void removeBone() {
	std::cout << "\n\n[Удаление ребра]\n\n"
		<< "Введите id удаляемой вершины:  ";
		
	int id;
	std::cin >> id;
	
	if (g->delBone(id)) {
		std::cout << "\n\nОперация завершена.\n\n";	
	} else {
		std::cout << "\n\n(!) Индексы находятся за пределами пространства индексов.\n\n";
	}
}

void changeWeight() {
	std::cout << "\n\n[Изменение веса]\n\n";
	
	std::cout << "Введите id ребра:  ";
	
	int id;
	std::cin >> id;
	
	std::cout << "Введите новый вес:  ";
	
	int weight;
	std::cin >> weight;
	
	if (g->changeWeight(id,weight)) {
		std::cout << "\n\nОперация завершена.\n\n";	
	} else {
		std::cout << "\n\n(!) Индексы находятся за пределами пространства индексов.\n\n";
	}
}

void changeDir() {
	std::cout << "\n\n[Изменение направления]\n\n";
	
	std::cout << "Введите id ребра:  ";
	
	int id;
	std::cin >> id;
	
	if (g->changeDir(id)) {
		std::cout << "\n\nОперация завершена.\n\n";	
	} else {
		std::cout << "\n\n(!) Индексы находятся за пределами пространства индексов.\n\n";
	}
}

/*===========================================================*/

void prefirst() {
	std::cout << "\n\n[first]\n\n";
	
	std::cout << "Результат: " << g->first()
		<< "\n-1 означает, что вершин нет.";
	
	std::cout << "\n\nОперация завершена.\n\n";	
}

void prenext() {
	std::cout << "\n\n[next]\n\n";
	
	std::cout << "Введите id вершины-источника:  ";
	int id1;
	std::cin >> id1;
	
	std::cout << "Введите id вершины-цели:  ";
	int id2;
	std::cin >> id2;
	
	std::cout << "Результат: " << g->next(id1,id2);
	
	std::cout << "\n\nОперация завершена.\n\n";
}

void prevertex() {
	std::cout << "\n\n[vertex]\n\n";
	
	std::cout << "Введите id вершины-источника:  ";
	int id1;
	std::cin >> id1;
	
	std::cout << "Введите id вершины-цели:  ";
	int id2;
	std::cin >> id2;
	
	std::cout << "Результат: " << g->vertex(id1,id2);
	
	std::cout << "\n\nОперация завершена.\n\n";	
}

/*===========================================================*/

void findCycles() {
	std::cout << "\n\n[Поиск циклов]\n\n";
	
	std::cout << "Введите длину цикла:  ";
	
	int length;
	std::cin >> length;
	
	g->findCycles(length);
	
	std::cout << "\n\nОперация завершена.\n\n";	
}

/*===========================================================*/

// Основной цикл меню
bool loop() {
	std::cout << "[Menu]\n"
			  << "[-----------------------------------]\n";
	
	
	std::cout << "> 0 - Прочитать TGF файл\n"
			  << "> 1 - Вывести список смежности\n"
			  << "> 2 - Добавить вершину\n"
			  << "> 3 - Удалить вершину\n"
			  << "> 4 - Добавить ребро\n"
			  << "> 5 - Удалить ребро\n"
			  << "> 6 - Изменить вес ребра\n"
			  << "> 7 - Изменить направление ребра\n"
			  << "> 8 - first\n"
			  << "> 9 - next\n"
			  << ">10 - vertex\n"
			  << ">11 - Поиск циклов\n"
			  << ">12 - Выход\n"
			  << "[-----------------------------------]\n\n";
			  
	int choice;
	std::cin >> choice;
	
	switch(choice) {
		case 0:		// Прочитать TGF
			readTgf();
			break;
		case 1:		// Вывод списка смежности
			printGraph();
			break;
		case 2:		// Добавить вершину
			addPoint();
			break;
		case 3:		// Удалить вершину
			removePoint();
			break;
		case 4:		// Добавить ребро
			addBone();
			break;
		case 5:		// Удалить ребро
			removeBone();
			break;
		case 6:		// Изменить вес ребра
			changeWeight();
			break;
		case 7:		// Изменить направление ребра
			changeDir();
			break;
		case 8:		// first
			prefirst();
			break;
		case 9:		// next
			prenext();
			break;
		case 10:	// vertex
			prevertex();
			break;
		case 11: 	// Поиск циклов
			findCycles();
			break;
		case 12: // Выход
			return false;
		default:
			std::cout << "Выберите число от 0 до 12";
	}
	
	return true;
	
}

int main() {
	
	setlocale(LC_ALL,"Russian");
	std::cout << "var 67, Alexeenko Dmitry, BSBO-01-18, 2019.\n\n";
	std::cout << "(!) Ошибка ввода не явлется ошибкой программы\n\n";
	
	while (loop()) { }
	
	return 0;
}
