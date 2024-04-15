#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <array>

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
    
    public:
        char type; // 'l' for left, 'r' for right and 'c' for color
        char new_color;
        Node* new_node;
        int version;
        
        Mod(char type, char new_color, int version ){
            this->type = type;
            this->version = version;
            this->new_color = new_color;
        }
        
        Mod(char type, Node* new_node, int version ){
            this->type = type;
            this->version = version;
            this->new_node = new_node;
        }

};

class Node {

        private:
        
        int get_number_of_mods() {
            int count = 0;
            if ( modifications[0] != nullptr ) count++;
            if ( modifications[1] != nullptr ) count++;
            return count;
        }

        Node* duplicate_node_with_mods(){
            Node* resultNode = new Node();
            resultNode->line = this->line;
            for (size_t i = 0; i < get_number_of_mods(); i++)
            {
                switch (this->modifications[i]->type)
                {
                case 'l': resultNode->left = modifications[i]->new_node; break;
                case 'r': resultNode->right = modifications[i]->new_node; break;
                case 'c': resultNode->color = modifications[i]->new_color; break;
                }
            }
            return resultNode;
        }

        public:

            Line *line;
            char color = 'R';
            Node* left = nullptr;
            Node* right = nullptr;
            Mod* modifications[2] = {nullptr, nullptr};

            Node(){}
            Node( Line* line ): line( line ){}

            Node* add_mod(Mod* new_mod) {
                if ( this->get_number_of_mods() < 2) {
                    if( modifications[0] == nullptr )
                    modifications[0] = new_mod;
                    else if (modifications[1] == nullptr )
                    modifications[1] = new_mod;
                    return this;
                } else {
                    Node* resultNode = duplicate_node_with_mods();
                    resultNode->modifications[0] = new_mod;
                    return resultNode;
                }
            }

            Node* get_left_child_by_version(int version){
                Node* left_child_node = nullptr;
                if(this->left != nullptr){
                    left_child_node = this->left;
                }
                if(this->modifications[0] && this->modifications[0]->type == 'l' && this->modifications[0]->version <= version ) {
                    left_child_node = this->modifications[0]->new_node;
                }
                if(this->modifications[1] && this->modifications[1]->type == 'l' && this->modifications[1]->version <= version ) {
                    left_child_node = this->modifications[1]->new_node;
                }
                return left_child_node;
            }


            Node* set_left_child_node_by_version(int version, Node *node){
                return this->add_mod( new Mod('l', node, version));
            }

            Node* set_right_child_node_by_version(int version, Node *node){
                return this->add_mod( new Mod('r', node, version));
            }

            Node* set_new_left_child_node_by_version( int version, Line *line ){
                return this->add_mod(new Mod( 'l' , new Node(line), version ) );
            }

            Node* set_new_right_child_node_by_version( int version, Line* line ){
                return this->add_mod(new Mod( 'r' , new Node(line), version ) );
            }

            Node* get_right_child_by_version( int version ){
                Node* right_child_node = nullptr;
                if(this->right != nullptr){
                    right_child_node = this->right;
                }
                if(  this->modifications[0] && this->modifications[0]->type == 'r' && this->modifications[0]->version <= version ) {
                    right_child_node = this->modifications[0]->new_node;
                }
                if(  this->modifications[1] && this->modifications[1]->type == 'r' && this->modifications[1]->version <= version ) {
                    right_child_node = this->modifications[1]->new_node;
                }
                return right_child_node;
            }

            char get_color_by_version( int version ) {
                char result = this->color;
                if ( this->modifications[0] && this->modifications[0]->type == 'c' && this->modifications[0]->version <= version ) 
                    result = this->modifications[0]->new_color;
                if ( this->modifications[1] && this->modifications[1]->type == 'c' && this->modifications[1]->version <= version )
                    result = this->modifications[1]->new_color;
                return result;
            }

            Node* set_color_by_version( char new_color, int version ) {
                return this->add_mod(new Mod('c', new_color, version ));
            }

};

class RedBlackTree {

    private:

        int get_current_version_number() { return this->root_history.size(); }

        Node* get_node_by_line_on_current_version(Line *line) {
            
            Node* current_node = this->root_history[ this->root_history.size() -1 ];

            while ( current_node->line != line ) {
                if ( current_node->line->PointA->y > line->PointA->y )
                    current_node = current_node->get_left_child_by_version(this->get_current_version_number());
                if ( current_node->line->PointA->y < line->PointA->y  )
                    current_node = current_node->get_right_child_by_version(this->get_current_version_number());
            }

            return current_node;

        }

        Node* rotate_left(Node* root, int version) {

            //       x                               y
            //      / \       Left Rotation         / \ 
            //    T1   y      - - - - - - - >      x  T3
            //        / \                         / \ 
            //      T2   T3                     T1   T2

            Node* x = root;
            Node* y = x->get_right_child_by_version(version);
            Node* T2 = y->get_left_child_by_version(version);

            x = x->set_right_child_node_by_version(version, T2);
            y = y->set_left_child_node_by_version(version, x);
            
            y = y->set_color_by_version(x->get_color_by_version(version), version);
            x = x->set_color_by_version('R', version);

            if ( y->get_left_child_by_version(version) != x ) {
                y = y->set_left_child_node_by_version(version, x);
            }

            if ( root == this->root ) {
                this->root = y;
            }

            return y;

        }

        Node* rotate_right(Node* root, int version) {

            //             y                                 x     
            //            / \       Right Rotation          / \   
            //           x   T3     - - - - - - - >        T1  y  
            //          / \                                   / \   
            //        T1   T2                               T2   T3

            Node* y = root;
            Node* x = y->get_left_child_by_version(version);
            Node* T2 = x->get_right_child_by_version(version);

            y = y->set_left_child_node_by_version(version,T2);
            x = x->set_right_child_node_by_version(version, y);

            x = x->set_color_by_version( y->get_color_by_version(version), version);
            y = y->set_color_by_version('R', version);

            if ( x->get_right_child_by_version(version) != y ) {
                x = x->set_right_child_node_by_version(version, y);
            }

            if ( root != this->root ){
                this->root = x;
            }

            return x;

        }

        void invert_node_color( Node* node ) {
            int version = this->get_current_version_number();
            if( !node  ) return;
            if(node->color == 'B') 
                node->set_color_by_version( 'R', version);
            else
                node->set_color_by_version( 'R', version );
        }

        char get_node_color_on_current_version(Node* node, int version) {
            
            if( node == nullptr) return 'B';
        
            return node->get_color_by_version(version);
        }

    public:

        Node *root = nullptr;
        std::vector< Node* > root_history = {nullptr};

        RedBlackTree() {}

        Node* get_node_left_child_on_last_version(Node* node){
            return node->get_left_child_by_version( this->get_current_version_number() );
        }

        Node* get_node_right_child_on_last_version(Node* node){
            return node->get_right_child_by_version( this->get_current_version_number() );
        }

        Node *find_node (Node* node, Node* root);

        Node *find_strict_successor(Node* node, int version);

        Node *get_successor(Node* node, int version){
            
            Node* current_node = node;
            Node* left_child_node = node->get_left_child_by_version(this->get_current_version_number());

            while ( left_child_node != nullptr )
            {
                current_node = left_child_node;
                left_child_node = current_node->get_left_child_by_version(this->get_current_version_number());
            }
            
            return current_node;

        }

        Node* push( Line *line, Node *root ) { 

            int version = this->get_current_version_number();

            if( this->root == nullptr ){
                Node* new_node = new Node(line);
                this->root = new_node;
                this->root_history.push_back(new_node);
                this->root->color = 'B';
                return this->root;
            }

            if ( line->PointA->y < root->line->PointA->y ) {
                if ( this->get_node_left_child_on_last_version(root) == nullptr ) {
                    root = root->set_new_left_child_node_by_version( version, line );
                } else {
                    Node* push_result = this->push( line, root->get_left_child_by_version( version ) );
                    if( push_result != root->get_left_child_by_version( version ) ){
                        root = root->set_left_child_node_by_version(version, push_result);
                    }
                }
            }

            if ( line->PointA->y > root->line->PointA->y ) {
                if ( this->get_node_right_child_on_last_version(root) == nullptr ){
                    root = root->set_new_right_child_node_by_version( version, line );
                } else {
                    Node* push_result = this->push( line, root->get_right_child_by_version( version ) );
                    if ( push_result != root->get_right_child_by_version(version) ){
                        root = root->set_right_child_node_by_version(version, push_result);
                    }
                }
            }

            if ( line->PointA->y == root->line->PointA->y ) {
                if( line->angular_coeficient < root->line->angular_coeficient ){
                    if ( this->get_node_left_child_on_last_version(root) == nullptr ) {
                        root = root->set_new_left_child_node_by_version( version, line );
                    } else {
                        Node* push_result = this->push( line, root->get_left_child_by_version( version ) );
                        if( push_result != root->get_left_child_by_version( version ) ){
                            root = root->set_left_child_node_by_version(version, push_result);                    
                        }
                    }
                }
                if (line->angular_coeficient > root->line->angular_coeficient){
                    if ( this->get_node_right_child_on_last_version(root) == nullptr ){
                        root = root->set_new_right_child_node_by_version( version, line );
                    } else {
                        Node* push_result = this->push( line, root->get_right_child_by_version( version ) );
                        if ( push_result != root->get_right_child_by_version(version) ){
                            root = root->set_right_child_node_by_version(version, push_result);
                        }
                    }
                }
            }

            // Fix Violations
            char left_color = this->get_node_color_on_current_version(root->get_left_child_by_version(version), version);
            char right_color = this->get_node_color_on_current_version(root->get_right_child_by_version(version), version);
            char left_left_color = this->get_node_left_child_on_last_version(root) != nullptr ? this->get_node_color_on_current_version( get_node_left_child_on_last_version(root->get_left_child_by_version(version)) ,version) : ' ';

            if ( right_color == 'R' && left_color == 'B' ) {
                root = rotate_left(root, version);
            }
            if ( left_color == 'R' && left_left_color == 'R' ) {
                root = rotate_right(root, version);
            }
            if ( left_color == 'R' && right_color == 'R' ) {
                invert_node_color(root);
                invert_node_color(root->left);
                invert_node_color(root->right);
            }

            // Handle Root
            if( this->root == root ) {
                this->root_history.push_back(this->root);
            }
            
            if(this->root->get_color_by_version(version) == 'R') this->root->set_color_by_version('B',version);

            return root;

        }

        Node *remove( Line *line, Node *root ) {
            
            int version = this->get_current_version_number();

            if ( root == nullptr ) return nullptr;
            
            if ( root->line->PointA->y < line->PointA->y ) {
                Node* result = this->remove( line, root->get_left_child_by_version(version) );
                result;
            }
            if ( root->line->PointA->y > line->PointA->y ) {
                Node* result = this->remove( line, root->get_right_child_by_version(version) );
                result;
            }

            if ( root->line->PointA->y == line->PointA->y && line != root->line ) {
               if( root->line->angular_coeficient < line->angular_coeficient ) {
                    Node* result = this->remove( line, root->get_left_child_by_version(version) );
                    result;
               }
               if( root->line->angular_coeficient > line->angular_coeficient ) {
                    Node* result = this->remove( line, root->get_left_child_by_version(version) );
                    result;
               } 
               if ( root->line->angular_coeficient == line->angular_coeficient ) {
                // TODO: two different lines, overlaped, can have the same y and angular_coeficient

               }
            }

            // Here, reached the desired line
            if ( root->line == line ) {

                // No children
                if ( root->get_left_child_by_version(version) == nullptr && root->get_right_child_by_version(version) == nullptr ) {
                    return nullptr;
                }
                // One child
                if ( root->get_left_child_by_version(version) != nullptr || root->get_right_child_by_version(version) == nullptr ) {
                    Node* child = (root->get_left_child_by_version(version) != nullptr) ? root->get_left_child_by_version(version) : root->get_right_child_by_version(version);                    
                    
                    if ( root == this->root ) {
                        this->root = child;
                        if(this->root->get_color_by_version(version) == 'R') this->root->set_color_by_version('B',version);
                        this->root_history.push_back(this->root);
                    }
                    
                    return child;
                }
                // Two Children
                if ( root->get_left_child_by_version(version) != nullptr && root->get_right_child_by_version(version) != nullptr ) {
                    return this->get_successor(root, version);
                }

            }

            if( this->root == root ) {
                this->root_history.push_back(this->root);
            }
            
            if(this->root->get_color_by_version(version) == 'R') this->root->set_color_by_version('B',version);

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

    // auxiliaryRedBlackTree->remove(current_line, auxiliaryRedBlackTree->root );

    return 0;
}