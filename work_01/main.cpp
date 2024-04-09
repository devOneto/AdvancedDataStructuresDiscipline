#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

class RedBlackTree;
class Node;
class Mod;
class LeftChildMod;
class RightChildMod;
class Line;

class Point {

    public:
        float x, y;
        Line* owner_line;

        Point (float x, float y){
            this->x = x;
            this->y = y;
        }

};

class Line {

    private:

    public:

        int id;
        Point* PointA;
        Point* PointB;

        float linear_coeficient = 0.0f;
        float angular_coeficient = 0.0f;

        Line( int id, Point* pointA, Point* pointB ) {
            this->id = id;
            this->PointA = pointA;
            this->PointB = pointB;
            this->angular_coeficient = float( pointB->y - pointA->y ) / float( pointB->x - pointA->x );
            this->linear_coeficient = pointA->y - float(this->angular_coeficient * pointA->x);
        }

        Line( int x1, int y1, int x2, int y2 ) {
            PointA = new Point(x1, y1);
            PointB = new Point(x2, y2);
            this->angular_coeficient = float( y2 - y1 ) / float( x2 - x1 );
            this->linear_coeficient = y1 - float(this->angular_coeficient * x1);
        }
};

class Mod { 

};

class LeftChildMod : public Mod {
    Node* node;
};

class RightChildMod : public Mod {
    Node* node;
};

class ColorMod: public Mod {
    char color;
};

class Node {

        public:

            int id;
            Line *line;
            char color = 'R';
            Node* left;
            Node* right;
            Mod* modifications[2];

            Node(){}
            Node( Line* line ): line( line ){}

};

class RedBlackTree {

    public:

        Node *root;
        std::vector< Node* > root_history;

        RedBlackTree() {}

        Node *find_node (Node* node, Node* root);
        Node *remove( Line *line, Node *root );
        Node *find_strict_successor();

        Node* push( Line *line, Node *root ) { 

            if( !this->root ){
                Node* new_node = new Node(line);
                this->root->color = 'B';
                this->root = new_node;
                this->root_history.push_back(new_node);
                return this->root;
            }

            if ( !root ) return new Node(line);

        }
            
};

bool compare_points_x (Point* pointA, Point* pointB ) {
    return (pointB->x > pointA->x);
}

int main() {

    int file_line_count = 0;
    int number_of_retas = 0;
    
    std::vector< Line* > retas;
    std::vector< Point* > pontos;

    std::vector< std::string > input_lines;
    std::ifstream input_file ("./input.txt");

    for( std::string line; getline( input_file, line ); )
    {
        input_lines.push_back(line);
    }

    number_of_retas = std::stoi(input_lines[0]);

    int current_line_number = 1;
    while ( current_line_number != number_of_retas + 1 ) {
        
        std::string current_line = input_lines[current_line_number];

        float x1 = float(current_line[0] - 48);
        float y1 = float(current_line[2] - 48);
        Point *pointA = new Point(x1,y1);

        float x2 = float(current_line[4] - 48);
        float y2 = float(current_line[6] - 48);
        Point *pointB = new Point(x2,y2);

        Line* newLine = new Line( current_line_number, pointA, pointB);

        pointA->owner_line = newLine;
        pointB->owner_line = newLine;

        retas.push_back( newLine );
        pontos.push_back(pointA);
        pontos.push_back(pointB);

        current_line_number++;
    }

    std::sort( pontos.begin(), pontos.end(), compare_points_x );

    std::vector<Line*> inputed_lines;
    RedBlackTree *auxiliaryRedBlackTree = new RedBlackTree();

    for ( int i = 0; i < pontos.size(); i ++) {
        Line *current_line = pontos[i]->owner_line;
        if( !(std::find(inputed_lines.begin(), inputed_lines.end(), current_line ) != inputed_lines.end()) ){
            auxiliaryRedBlackTree->push( current_line, auxiliaryRedBlackTree->root );
            inputed_lines.push_back(current_line);
        } else {
            auxiliaryRedBlackTree->remove(current_line, auxiliaryRedBlackTree->root );
        }
    }

    return 0;
}