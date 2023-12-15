#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct position
{
	unsigned int x, y;
} Position;

union caseContent {
	int numberLastStep;
	char content;
};

typedef struct Case {
	union caseContent content;
	bool isCaseMarked;
} Case;
bool is_case_empty(Case);

Case **labyrinthe=NULL;
unsigned int sizeX, sizeY;
unsigned int numberCaseVisited=0;

void allocate_labyrinthe_memory();
void initialize_labyrinthe_value();
void free_labyrinthe();
void print_labyrinthe(Position posCharacter);
bool is_empty_case(Position);
void set_case_visited(Position);
bool is_on_win_case(Position p) {
	Case c = labyrinthe[p.x][p.y];
	return (!c.isCaseMarked) && (c.content.content == 'G');
}


typedef struct treeElement {
	Position value;
	struct treeElement* child[4];
	struct treeElement* parent;
	int numberChild;
} TreeElement;
TreeElement* create_tree_element(TreeElement* parent, Position value);
void free_tree_element_and_child(TreeElement*);
TreeElement* add_child(TreeElement* parent, Position);
void free_labyrinthe_way(TreeElement*);


typedef struct listElement {
	Position value;
	struct listElement* nextElement;
	struct listElement* lastElement;
	TreeElement* aliasInTree;
} ListElement;
ListElement* create_list_element(ListElement* lastElement, TreeElement* equivalentTreeData, Position value);
void free_list_element_and_child(ListElement*);

typedef struct tree {
	TreeElement* head;
} Tree;
Tree create_new_tree();
void set_tree_head(Tree*, Position);
void free_tree(Tree);
void display_solved_labyrinthe(Tree);


typedef struct list {
	ListElement* firstElement;
	ListElement* lastElement;
} List;
List create_new_list();
void free_list(List);
void add_element_in_list(List*, TreeElement* treeParent, Position);
bool is_list_not_empty(List);
ListElement* pop_first_element(List*);
void free_all_sub_way(List*);

Position get_start_position();

void sleep_millisecond(unsigned long long);

void initialize_position_next_to(Position p, Position next[4]){
	next[0].x = p.x+1;
	next[1].x = p.x-1;
	next[2].x = p.x;
	next[3].x = p.x;
	
	next[0].y = p.y;
	next[1].y = p.y;
	next[2].y = p.y-1;
	next[3].y = p.y+1;
} 
void find_way_BFS(){
	Position startingPosition = get_start_position();
	
	Tree possibleWays = create_new_tree();
	List elementsToCheck = create_new_list();

	set_tree_head(&possibleWays, startingPosition);
	add_element_in_list(&elementsToCheck, possibleWays.head, startingPosition); //!\ possibleWays.head to avoid using getter, can become buggy
	bool did_find_goal = false;
	
	// printf("Le point de départ se trouve sur la position (%d, %d).\n", startingPosition.x, startingPosition.y);
	while(is_list_not_empty(elementsToCheck)){
		ListElement* element_worked = pop_first_element(&elementsToCheck);
		Position actualPosition = element_worked->value;
		TreeElement* actualNode = element_worked->aliasInTree;

		if(is_on_win_case(actualPosition)) {
			did_find_goal = true;
			free_list_element_and_child(element_worked);
			break;
		}

		Position positionNextToActual[4]; initialize_position_next_to(actualPosition, positionNextToActual);

		bool isADeadPath = true;
		for(int i=0; i<4; i++) {
			if(is_empty_case(positionNextToActual[i])) {
				TreeElement* nexEmptyCase = add_child(actualNode, positionNextToActual[i]);
				add_element_in_list(&elementsToCheck, nexEmptyCase, positionNextToActual[i]);
				set_case_visited(positionNextToActual[i]);
				isADeadPath = false;
			}
		}

		if(isADeadPath) {
			free_labyrinthe_way(actualNode);
		}
		free_list_element_and_child(element_worked);
		printf("\n");
		print_labyrinthe(actualPosition);

	}

	free_all_sub_way(&elementsToCheck);
	if(did_find_goal) {
		printf("\n");
		printf("Vous avez trouver la solution en passant sur %d cases différentes\n", numberCaseVisited);
		display_solved_labyrinthe(possibleWays);
	} else {
		fprintf(stderr, "Aucune solution trouvé\n");
	}
	free_tree(possibleWays);
	// free_list(elementsToCheck);
}

int main() {
	allocate_labyrinthe_memory();
	initialize_labyrinthe_value();
	find_way_BFS();
	free_labyrinthe();
}

Position get_start_position() {
	for(int x=0; x<sizeX; x++) {
		for(int y=0; y<sizeY; y++) {
			if (labyrinthe[x][y].isCaseMarked == false && labyrinthe[x][y].content.content == 'S') {
				Position p;
				p.x = x;
				p.y = y;
				return p;
			}
		}
	}
	fprintf(stderr, "Pas de position de départ trouvé!\n");
	exit(1);
}
// Case function
/*
union caseContent {
	int numberLastStep;
	char content;
};

typedef struct Case {
	union caseContent content;
	bool isCaseMarked;
} Case;
*/
bool is_case_empty(Case c){
	return !c.isCaseMarked && (c.content.content=='.' || c.content.content == 'G');
}

// Labyrinthe functions
void allocate_labyrinthe_memory() {
	scanf("%d %d", &sizeX, &sizeY);

	labyrinthe = (Case**) malloc(sizeX*sizeof(Case*));
	for (int x = 0; x<sizeX; x++) { 
		labyrinthe[x] = (Case *) malloc(sizeY*sizeof(Case));
	}
}
void initialize_labyrinthe_value() {
	for(int y=0; y<sizeY; y++) {
		fgetc(stdin); // character '\n' of previous line
		for(int x=0; x<sizeX; x++) {
			char c = fgetc(stdin);
			labyrinthe[x][y].isCaseMarked = false;
			labyrinthe[x][y].content.content = c;
		}
	}
}
void free_labyrinthe() {
	for(int x=0; x<sizeX; x++) {
		free(labyrinthe[x]);
	}
	free(labyrinthe);
}
void print_labyrinthe(Position posActu) {
	system("clear");
	for (int y=0; y<sizeY; y++) {
		for(int x=0; x<sizeX;x++) {
			Case elementToPrint = labyrinthe[x][y];
			if(x==posActu.x && y==posActu.y ){
				printf("  m ");
			}
			else if (elementToPrint.isCaseMarked) {
				int numberOfCaseVisited = elementToPrint.content.numberLastStep;
				if(numberOfCaseVisited < 10) {
					printf("  %d ", numberOfCaseVisited);
				} else if (numberOfCaseVisited < 100) {
					printf(" %d ", numberOfCaseVisited);
				}
				else {
					printf("%d ", numberOfCaseVisited);
				}
			} else {
				char character_to_print = elementToPrint.content.content;
				printf("  %c ", character_to_print);
			}
		}
		printf("\n");
	}
	sleep_millisecond(500);
}
bool is_empty_case(Position p) {
	if(p.x>=sizeX || p.y>=sizeY) return false;

	return is_case_empty(labyrinthe[p.x][p.y]);
}
void set_case_visited(Position p) {
	++numberCaseVisited;
	if(labyrinthe[p.x][p.y].content.content == 'G') {
		return ;
	}
	labyrinthe[p.x][p.y].isCaseMarked = true;
	labyrinthe[p.x][p.y].content.numberLastStep = numberCaseVisited;
}

// TreeElement
/*
typedef struct treeElement {
	Position value;
	struct treeElement* child[4];
	struct treeElement* parent;
	int numberChild;
} TreeElement;
*/
TreeElement* create_tree_element(TreeElement* parent, Position value){
	TreeElement* newElement = (TreeElement*)malloc(sizeof(TreeElement));
	newElement->parent = parent;
	newElement->value = value;
	newElement->numberChild = 0;
	return newElement;
}
void free_tree_element_and_child(TreeElement* t) {
	if (t==NULL) return;

	for(int i=0; i<t->numberChild; i++) {
		free_tree_element_and_child(t->child[i]);
	}

	free(t);
}
TreeElement* add_child(TreeElement* parent, Position p) {
	parent->numberChild++;
	parent->child[parent->numberChild-1] = create_tree_element(parent, p);
	return parent->child[parent->numberChild-1];
}
void free_labyrinthe_way(TreeElement* child) {
	TreeElement* previousChild = child->parent;
	while(previousChild != NULL && previousChild->numberChild <=1 ) {
		child = previousChild;
		previousChild = previousChild->parent;
	}

	if(previousChild != NULL) {
		int i=0;
		while(previousChild->child[i] != child) {
			i++;
		}
		previousChild->numberChild--;
		for( ; i<previousChild->numberChild;i++) {
			previousChild->child[i] = previousChild->child[i+1];
		}

		previousChild->child[i] = NULL;
	}
	free_tree_element_and_child(child);
}

// ListElement
/*
typedef struct listElement {
	Position value;
	struct listElement* nextElement;
	struct listElement* lastElement;
	TreeElement* parent;
} ListElement;
*/
ListElement* create_list_element(ListElement* lastElement, TreeElement* parent, Position value) {
	ListElement* newElement = (ListElement*)malloc(sizeof(ListElement));
	newElement->value = value;
	newElement->nextElement = NULL;
	newElement->lastElement = lastElement;
	newElement->aliasInTree = parent;

	return newElement;
}
void free_list_element_and_child(ListElement* e){
	if(e==NULL) return;

	free_list_element_and_child(e->nextElement);
	free(e);
}

// data holder
/*
typedef struct tree {
	TreeElement* head;
} Tree;
*/

Tree create_new_tree() {
	Tree t;
	t.head = NULL;
	return t;
}
void free_tree(Tree t){
	free_tree_element_and_child(t.head);
}
void set_tree_head(Tree* t, Position p) {
	free_tree(*t);
	TreeElement* newHead = create_tree_element(NULL, p);
	t->head = newHead;
}
void display_solved_labyrinthe(Tree t) {
	TreeElement* element =t.head;
	printf("Le chemin trouvé:");
	int cpt=0;
	// affichage du chemin trouvé:
	while(true) {
		cpt++;
		printf("(%d, %d)", element->value.x, element->value.y);
		if( element->numberChild != 1){
			printf("\n");
			break;
		}
		else {
			printf("->");
			element = element->child[0];
		}
	}
	printf("\nCe qui correspond à un chemin de %d cases\n", cpt);

	// changing marked cases as free cases
	for(int y=0; y<sizeY; y++) {
		for(int x=0; x<sizeX; x++) {
			if( labyrinthe[x][y].isCaseMarked ) {
				labyrinthe[x][y].isCaseMarked = false;
				labyrinthe[x][y].content.content = '.';
			}
		}
	}

	//registering path
	Position p1, p2, p3;
	element = t.head; 
	p2 = element->value;
	element = element->child[0]; //contain at least Start point and goal
	p3 = element->value;

	labyrinthe[p2.x][p2.y].content.content = 'S';
	while(element->numberChild == 1) {
		element = element->child[0];
		p1 = p2;
		p2 = p3;
		p3 = element->value;
		labyrinthe[p2.x][p2.y].isCaseMarked = true;

		if( p1.x== p3.x ) {
			labyrinthe[p2.x][p2.y].content.numberLastStep = 1;
		}
		else if (p1.y==p3.y) {
			labyrinthe[p2.x][p2.y].content.numberLastStep = 2;
		}
		else if (p1.y < p2.y || p3.y < p2.y) {
			if( p1.x > p2.x || p3.x > p2.x ){
				labyrinthe[p2.x][p2.y].content.numberLastStep = 3;
			}
			else {
				labyrinthe[p2.x][p2.y].content.numberLastStep = 4;
			}
		}
		else if (p1.x > p2.x || p3.x > p2.x ) {
			labyrinthe[p2.x][p2.y].content.numberLastStep = 5;
		}
		else {
			labyrinthe[p2.x][p2.y].content.numberLastStep = 6;
		}
	}

	for(int y=0; y<sizeY; y++) {
		for(int x=0; x<sizeX; x++) {
			if(labyrinthe[x][y].isCaseMarked){
				switch(labyrinthe[x][y].content.numberLastStep) {
				case 1:
					printf("│");
					break;
				case 2:
					printf("─");
					break;
				case 3:
					printf("└");
					break;
				case 4:
					printf("┘");
					break;
				case 5:
					printf("┌");
					break;
				default:
					printf("┐");

				}
			}
			else {
				printf("%c", labyrinthe[x][y].content.content);
			}
		}
		printf("\n");
	}
}

/*
typedef struct list {
	ListElement* firstElement;
	ListElement* lastElement;
} List;
*/
List create_new_list() {
	List l;
	l.firstElement = NULL;
	l.lastElement = NULL;
	return l;
}
void free_list(List l) {
	free_list_element_and_child(l.firstElement);
}
void add_element_in_list(List* l, TreeElement* treeParent, Position p) {
	if(l->firstElement == NULL) {
		l->firstElement = create_list_element(NULL, treeParent, p);
		l->lastElement = l->firstElement;
	} else {
		ListElement* newElement = create_list_element(l->lastElement, treeParent, p);
		l->lastElement->nextElement = newElement;
		l->lastElement = newElement;
	}
}
bool is_list_not_empty(List l){
	return l.firstElement!=NULL;
}
ListElement* pop_first_element(List* l) {
	ListElement * firstElement = l->firstElement;
	if( l->firstElement == l->lastElement) {
		l->firstElement = NULL;
		l->lastElement = NULL;
	} else {
		l->firstElement = firstElement->nextElement;
		firstElement->nextElement = NULL;
		firstElement->lastElement = NULL;
		l->firstElement->lastElement = NULL;
	}
	return firstElement;
}
void free_all_sub_way(List* l){
	while(is_list_not_empty(*l)) {
		numberCaseVisited--;
		ListElement* element = pop_first_element(l);
		free_labyrinthe_way(element->aliasInTree);
		free_list_element_and_child(element);
	}
}


void sleep_millisecond(unsigned long long timeInMilliseconds) {
	struct timespec timeLeft;
	timeLeft.tv_sec = timeInMilliseconds / 1000;
	timeLeft.tv_nsec = (timeInMilliseconds % 1000) * 1000000; // converting 1 nanosecond in 1 millisecond

	nanosleep(&timeLeft, &timeLeft);
}