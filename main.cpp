#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib> // ��� atoi
#include <fstream>


const int MAX = 32;		// ������������ ��� 
						// ����������� ������� �������� ������
// ����������� ����� ��������, ����������� Bone � Point
class Element {
	friend class Graph;
	protected:
		int id;
		std::string type;
		int weight;
		
		// linkBack � linkFront - ������� ���������� �� ������
		// ��������, � ������� ������ ������.
		// back - ��, �� ������� ��� �������,
		// front - ��, ������� ����� ������ � ��������.
		// ��������, �����, ������������ � ������� A,
		// ������ � � linkBack.
		// ������� ���� ��������� ��� ����� ������� ���������
		// ��� ������ ����� ����� ����� � ��������� ������.
		
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

// ����� �����
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

// ����� �������
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

// ����� ���������� �������, ������� �� Graph ������ ��� ����������
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
// ����� �����
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
	 			std::cout << "(!) ���� �� ������";
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
				idlist[i] = source->linkFront[i]->linkFront[0]->id; // ���� �������, ���������� ������ ����, ������������ �� �������, ���������� � source
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
				idlist[i] = source->linkFront[i]->linkFront[0]->id; // ���� �������, ���������� ������ ����, ������������ �� �������, ���������� � source
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
			std::cout << "\n�������: \n";
			for (int i = 0; i < pointsAmount; i++) {
				Point* p = points[i];
				std::cout 
					<< "ID " << p->id << ": \n    "
					<< "��� - " << p->getName() << "\n"
					<< "    �������� ����: \n";
					for (int i = 0; i < p->backsize; i++) {
						std::cout
							<< "    > id " 
							<< p->linkBack[i]->id
							<< " (�� ������� "
							<< p->linkBack[i]->linkBack[0]->id
							<< " c ����� "
							<< p->linkBack[i]->weight
							<< ");\n";
					} 
					if (p->backsize == 0) std::cout << "      �����������;\n";
					std::cout << "    ��������� ����: \n";
					for (int i = 0; i < p->frontsize; i++) {
						std::cout 
							<< "    > id "
							<< p->linkFront[i]->id
							<< " (� ������� "
							<< p->linkFront[i]->linkFront[0]->id
							<< " c ����� "
							<< p->linkFront[i]->weight
							<< ");\n";
					} 
					if (p->frontsize == 0) std::cout << "      �����������;\n";
				std::cout << "\n";
			}
			if (pointsAmount == 0)
				std::cout << "�����������.\n";
			std::cout << "\nи���: \n";
			for (int i = 0; i < bonesAmount; i++) {
				Bone* b = bones[i];
				std::cout 
					<< b->id << ": "
					<< b->linkBack[0]->id
					<< " -> "
					<< b->linkFront[0]->id
					<< " � ����� "
					<< b->getWeight()
					<< "\n";
			}
			if (bonesAmount == 0)
				std::cout << "�����������.\n";
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
			std::cout << "\n������� ����� ��� ���������:\n\n";
			for (int i = 0; i < pointsAmount; i++) {
				for (int j = 0; j < pointsAmount; j++)
					std::cout << weightTable->m[i][j] << "  ";
				std::cout << "\n";
			}
		}
		
		void printWay() {
			std::cout << "���� " << wayId++ << ":";
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
											// ^ ��� ��������� ������������� ����� ������� ������ �������
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
	std::cout << "\n\n[������ �� TGF]\n\n";
	
	std::cout << "������� ��� �����:  ";
	
	char filename[64];
	std::cin >> filename;
	
	g = new Graph(filename);
	
	std::cout << "\n\n�������� ���������.\n\n";
}

void printGraph() {
	std::cout << "\n\n[����� �����]\n\n";
	g->print();
	std::cout << "\n\n";
}
/*===========================================================*/

void addPoint() {
	std::cout << "\n\n[���������� �������]\n\n"
		<< "������� ��� �������:  ";
	
	std::string name;
	std::cin >> name;
	
	g->addPoint(name);
	
	std::cout << "\n\n�������� ���������.\n\n";
}

void removePoint() {
	std::cout << "\n\n[�������� �������]\n\n"
		<< "������� id ��������� �������:  ";
		
	int id;
	std::cin >> id;
	
	if (g->delPoint(id)) {
		std::cout << "\n\n�������� ���������.\n\n";	
	} else {
		std::cout << "\n\n(!) ������� ��������� �� ��������� ������������ ��������.\n\n";
	}	
}

void addBone() {
	std::cout << "\n\n[���������� �����]\n\n";
	
	std::cout << "������� id �������-���������:  ";
	int id1;
	std::cin >> id1;
	
	std::cout << "������� id �������-����:  ";
	int id2;
	std::cin >> id2;
	
	std::cout << "������� ��� �����:  ";
	
	int weight;
	std::cin >> weight;
	
	if (g->addBone(id1,id2,weight)) {
		std::cout << "\n\n�������� ���������.\n\n";	
	} else {
		std::cout << "\n\n(!) ������� ��������� �� ��������� ������������ ��������.\n\n";
	}
	
}

void removeBone() {
	std::cout << "\n\n[�������� �����]\n\n"
		<< "������� id ��������� �������:  ";
		
	int id;
	std::cin >> id;
	
	if (g->delBone(id)) {
		std::cout << "\n\n�������� ���������.\n\n";	
	} else {
		std::cout << "\n\n(!) ������� ��������� �� ��������� ������������ ��������.\n\n";
	}
}

void changeWeight() {
	std::cout << "\n\n[��������� ����]\n\n";
	
	std::cout << "������� id �����:  ";
	
	int id;
	std::cin >> id;
	
	std::cout << "������� ����� ���:  ";
	
	int weight;
	std::cin >> weight;
	
	if (g->changeWeight(id,weight)) {
		std::cout << "\n\n�������� ���������.\n\n";	
	} else {
		std::cout << "\n\n(!) ������� ��������� �� ��������� ������������ ��������.\n\n";
	}
}

void changeDir() {
	std::cout << "\n\n[��������� �����������]\n\n";
	
	std::cout << "������� id �����:  ";
	
	int id;
	std::cin >> id;
	
	if (g->changeDir(id)) {
		std::cout << "\n\n�������� ���������.\n\n";	
	} else {
		std::cout << "\n\n(!) ������� ��������� �� ��������� ������������ ��������.\n\n";
	}
}

/*===========================================================*/

void prefirst() {
	std::cout << "\n\n[first]\n\n";
	
	std::cout << "���������: " << g->first()
		<< "\n-1 ��������, ��� ������ ���.";
	
	std::cout << "\n\n�������� ���������.\n\n";	
}

void prenext() {
	std::cout << "\n\n[next]\n\n";
	
	std::cout << "������� id �������-���������:  ";
	int id1;
	std::cin >> id1;
	
	std::cout << "������� id �������-����:  ";
	int id2;
	std::cin >> id2;
	
	std::cout << "���������: " << g->next(id1,id2);
	
	std::cout << "\n\n�������� ���������.\n\n";
}

void prevertex() {
	std::cout << "\n\n[vertex]\n\n";
	
	std::cout << "������� id �������-���������:  ";
	int id1;
	std::cin >> id1;
	
	std::cout << "������� id �������-����:  ";
	int id2;
	std::cin >> id2;
	
	std::cout << "���������: " << g->vertex(id1,id2);
	
	std::cout << "\n\n�������� ���������.\n\n";	
}

/*===========================================================*/

void findCycles() {
	std::cout << "\n\n[����� ������]\n\n";
	
	std::cout << "������� ����� �����:  ";
	
	int length;
	std::cin >> length;
	
	g->findCycles(length);
	
	std::cout << "\n\n�������� ���������.\n\n";	
}

/*===========================================================*/

// �������� ���� ����
bool loop() {
	std::cout << "[Menu]\n"
			  << "[-----------------------------------]\n";
	
	
	std::cout << "> 0 - ��������� TGF ����\n"
			  << "> 1 - ������� ������ ���������\n"
			  << "> 2 - �������� �������\n"
			  << "> 3 - ������� �������\n"
			  << "> 4 - �������� �����\n"
			  << "> 5 - ������� �����\n"
			  << "> 6 - �������� ��� �����\n"
			  << "> 7 - �������� ����������� �����\n"
			  << "> 8 - first\n"
			  << "> 9 - next\n"
			  << ">10 - vertex\n"
			  << ">11 - ����� ������\n"
			  << ">12 - �����\n"
			  << "[-----------------------------------]\n\n";
			  
	int choice;
	std::cin >> choice;
	
	switch(choice) {
		case 0:		// ��������� TGF
			readTgf();
			break;
		case 1:		// ����� ������ ���������
			printGraph();
			break;
		case 2:		// �������� �������
			addPoint();
			break;
		case 3:		// ������� �������
			removePoint();
			break;
		case 4:		// �������� �����
			addBone();
			break;
		case 5:		// ������� �����
			removeBone();
			break;
		case 6:		// �������� ��� �����
			changeWeight();
			break;
		case 7:		// �������� ����������� �����
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
		case 11: 	// ����� ������
			findCycles();
			break;
		case 12: // �����
			return false;
		default:
			std::cout << "�������� ����� �� 0 �� 12";
	}
	
	return true;
	
}

int main() {
	
	setlocale(LC_ALL,"Russian");
	std::cout << "var 67, Alexeenko Dmitry, BSBO-01-18, 2019.\n\n";
	std::cout << "(!) ������ ����� �� ������� ������� ���������\n\n";
	
	while (loop()) { }
	
	return 0;
}
