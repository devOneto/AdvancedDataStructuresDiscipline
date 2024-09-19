#include<iostream>
#include <fstream>
#include <vector>


// Cache Independent B Tree

class Tree;
class TreeNode;
class VectorNode;

class VectorNode {
public:

	int key;
	int index;
	bool is_empty = false;
	TreeNode* tree_node;

	VectorNode(){}
	VectorNode( int _key ): key(_key) {}
	VectorNode( int _key, int _index ): key(_key), index(_index) {}

};

class TreeNode {
public:

	TreeNode* left;
	TreeNode* right;
	VectorNode* aux_vector_node;

	TreeNode(){}
	TreeNode( VectorNode* _aux_vector_node ) : aux_vector_node(_aux_vector_node) {}
	TreeNode( VectorNode* _aux_vector_node, TreeNode* _left, TreeNode* _right ) : aux_vector_node(_aux_vector_node), left(_left), right(_right) {}

};

class Tree {

	int height = 0;
	int aux_size = 0;
	int veb_size = 0;
	int population = 0;
	TreeNode* root = nullptr;
	std::vector< VectorNode* > aux_vector = { nullptr };
	std::vector< VectorNode* > veb_vector = { nullptr };

	TreeNode* get_min_from( TreeNode* node ) {
		while ( node->left != nullptr ) return get_min_from(node->left);
		return node;
	}

	TreeNode* get_max_from( TreeNode* node ) {
		while ( node->right != nullptr ) return get_max_from(node->right);
		return node;
	}

	TreeNode* find_successor_of( TreeNode* node ) {

		if ( node == nullptr ) return nullptr;

		if ( node->right != nullptr ) return get_min_from(node->right);

		TreeNode* current_node = this->root;
		TreeNode* sucessor  = nullptr;

		while ( current_node != nullptr ) {
			if( node->aux_vector_node->key < current_node->aux_vector_node->key ) {
				sucessor = current_node;
				current_node = current_node->left;
			}
			else if ( node->aux_vector_node->key > current_node->aux_vector_node->key ) {
				current_node = current_node->right;
			}
		}

		return sucessor;

	}

	TreeNode* find_predecessor_of( TreeNode* node ) {

		if ( node == nullptr ) return nullptr;

		if ( node->left != nullptr ) return get_max_from(node->left);

		TreeNode* current_node = this->root;
		TreeNode* predecessor = nullptr;

		while ( current_node != nullptr ) {
			if( node->aux_vector_node->key > current_node->aux_vector_node->key) {
				predecessor = current_node;
				current_node = current_node->right;
			}
			else if( node->aux_vector_node->key < current_node->aux_vector_node->key ) {
				current_node = current_node->left;
			}
		}

		return predecessor;

	}

	TreeNode* find_successor_by_key( int key ) {

		TreeNode* current_node = this->root;

		while ( current_node->aux_vector_node->key < key ) {
			if( current_node->right == nullptr ) return nullptr;
			current_node = current_node->right;
		}

		return get_min_from(current_node);

	}

	TreeNode* find_predecessor_by_key( int key ) {

		TreeNode* current_node = this->root;

		while( current_node->aux_vector_node->key > key ) {
			if ( current_node->left == nullptr ) return nullptr;
			current_node = current_node->left;
		}

		return get_max_from(current_node);

	}

	TreeNode* _get_bigger_tree_node(TreeNode* a, TreeNode* b) {

		if ( a->aux_vector_node->is_empty && !b->aux_vector_node->is_empty ) return b;
		if ( !a->aux_vector_node->is_empty && b->aux_vector_node->is_empty ) return a;
		if ( a->aux_vector_node->is_empty && b->aux_vector_node->is_empty ) return nullptr;

		return a->aux_vector_node->key > b->aux_vector_node->key ? a : b;

	}

	std::vector< TreeNode* > get_array_from_subtree ( TreeNode* node, int height ) {
		return {};
	}

	std::vector< TreeNode* > get_veb_array_from_btree( TreeNode* tree, int height ) {

		// if ( height > 2 ) get_veb_array_from_btree(  )
		return {};
	}

public:

	Tree(){}

	VectorNode* find(){
		return nullptr;
	}

	void insert( int value ) {

		VectorNode* new_node = new VectorNode(value);

		if ( this->root == nullptr ) {
			new_node->index = 0;
			this->root = new TreeNode(new_node);
			this->aux_vector = { root->aux_vector_node };
			this->veb_vector = { root->aux_vector_node };
			this->aux_size = 1;
			this->veb_size = 1;
			return;
		}

		TreeNode* successor_tree_node = this->find_successor_by_key(new_node->key);
		TreeNode* predecessor_tree_node = this->find_predecessor_by_key(new_node->key);

		VectorNode* successor = successor_tree_node != nullptr ? successor_tree_node->aux_vector_node : nullptr;
		VectorNode* predecessor = predecessor_tree_node != nullptr ? predecessor_tree_node->aux_vector_node : nullptr;

		int scan_index = predecessor != nullptr ? predecessor->index : 0;
		int end_scan_index = successor != nullptr ? successor->index : this->aux_size;
		int empty_space_index = -1;

		// search for empty space between predecessor and sucessor
		while ( scan_index != end_scan_index && scan_index <= this->aux_size ) {
			if( !aux_vector[scan_index]->is_empty ) {
				scan_index++;
			}else {
				empty_space_index = scan_index;
				scan_index = end_scan_index;
			}
		}

		// if finds, put the new node there and end
		if ( empty_space_index != -1 ) {
			new_node->index = empty_space_index;
			aux_vector[empty_space_index] = new_node;
			population++;
			return;
		}

		if ( predecessor != nullptr ) {
			// if not, finds empty space at left of predecessor
			int before_predecessor_empty_index = -1;
			scan_index = 0;
			while ( scan_index != predecessor->index ) {
				if( !aux_vector[scan_index]->is_empty ) {
					scan_index++;
				} else {
					before_predecessor_empty_index = scan_index;
					scan_index = predecessor->index;
				}

			}

			// if there is space moves all the nodes to left
			// puts the new node on the place of the predecessor
			// and ends
			if ( before_predecessor_empty_index != -1 ) {
				int old_predecessor_index = predecessor->index;
				for ( int i = before_predecessor_empty_index; i<predecessor->index; i++ ) {
					this->aux_vector[i]->index--;
				}
				new_node->index = old_predecessor_index;
				this->aux_vector[old_predecessor_index] = new_node;
				population++;
				return;
			}
		}

		if ( successor != nullptr ) {
			// if not, finds empty space at right of successor
			int after_successor_empty_index = -1;
			scan_index = successor->index;
			while ( scan_index != this->aux_size ) {
				if( !aux_vector[scan_index]->is_empty ) {
					scan_index++;
				} else {
					after_successor_empty_index = scan_index;
					scan_index = this->aux_size;
				}
			}

			// if there is space moves all the nodes to right
			// puts the new node on the place of the sucessor
			// and ends
			if( after_successor_empty_index != -1 ) {
				int old_successor_index = successor->index;
				for ( int i = successor->index; i < after_successor_empty_index; i++ ) {
					this->aux_vector[i]->index++;
					this->aux_vector[ this->aux_vector[i]->index ] = this->aux_vector[i];
				}
				new_node->index = old_successor_index;
				this->aux_vector[old_successor_index] = new_node;
				population++;
				return;
			}
		}

		// table doubling
		for ( int i = 0; i < this->aux_size; i++ ) {

			//creates the empty spaces
			VectorNode* new_empty_vector_node = new VectorNode();
			new_empty_vector_node->is_empty = true;
			TreeNode* new_empty_tree_node = new TreeNode(new_empty_vector_node);
			new_empty_vector_node->tree_node = new_empty_tree_node;
			// fulfill the aux and veb vector with "empty" nodes
			this->aux_vector.push_back( new_empty_vector_node );
			this->veb_vector.push_back( new_empty_vector_node );

		}
		this->aux_size *= 2;
		this->veb_size *= 2;

		// after creating enoght space, inserts the new node
		this->insert(new_node->key);

		// rebuild tree

		// init leaves without relashionship
		scan_index = 0;
		std::vector < TreeNode* > leaves_row = {};

		while ( scan_index != aux_size ) {
			TreeNode* new_tree_node = new TreeNode( aux_vector[scan_index] );
			aux_vector[scan_index]->tree_node = new_tree_node;
			leaves_row.push_back( aux_vector[scan_index]->tree_node );
			scan_index ++;
		}
		this->height = 1;

		// create nodes with relashionships on three until the root
		scan_index = 1;
		std::vector < TreeNode* > new_row = {};
		std::vector < TreeNode* > current_row = leaves_row;

		// scans the tree_nodes array to create relashionship
		while ( scan_index != current_row.size() ) {

			TreeNode* new_left = current_row[ scan_index - 1 ];
			TreeNode* new_right = current_row[ scan_index ];
			TreeNode* new_tree_node = new TreeNode( this->_get_bigger_tree_node(new_left, new_right)->aux_vector_node, new_left, new_right );
			this->height++;

			//if only two elements left, the father is root
			if ( current_row.size() == 2 ) {
				this->root = new_tree_node;
				return;
			}
			//if on the even position, have created father
			if ( scan_index % 2 != 0 ) {
				new_row.push_back( new_tree_node );
			}
			//if on last element reset to scan above formed line
			if ( scan_index + 1 == current_row.size() ) {
				current_row = new_row;
				new_row = {};
				scan_index = 1;
			}

			//if on last element reset, else goes to next even
			scan_index = scan_index + 1 == current_row.size() ? 1 : scan_index+=2;

		}

		return;
	}

	void remove( int value ) {

	}

	std::string get_strict_sucessor( int value ) {

		TreeNode* current_node = this->root;

		while( current_node->aux_vector_node->key < value ) {
			current_node = current_node->right;
		}

		while ( !current_node->left->aux_vector_node->is_empty && current_node->left->aux_vector_node->key > value  ) {
			current_node = current_node->left;
		}

		return std::to_string( current_node->aux_vector_node->key );
	}

	std::string print() {

		std::string result;


		result = "Aux Vector: ";
		for( int i = 0; i < this->aux_vector.size(); i++ ) {
			std::string value = !this->aux_vector[i]->is_empty ? std::to_string(this->aux_vector[i]->key) : "NULL";
			result +=  value + " ";
		}

		result += "\nVeb Vector: ";
		for( int i = 0; i < this->veb_vector.size(); i++ ) {
			std::string value = ! this->veb_vector[i]->is_empty ?  std::to_string(this->veb_vector[i]->key) : "NULL";
			result += value + " ";
		}

		return result;

	}

};

int main(){

	Tree* tree = new Tree();

	std::string line;
	std::string file_id = "A";
	std::ifstream input_file("./input_A.txt");
	std::ofstream output_file ("./output_A.txt");

	if (!input_file.is_open()) {
		std::cerr << "Error opening the file!" << std::endl;
		return 1;
	}

	while ( getline(input_file,line) )
	{
		std::string opperation_code = line.substr(0,3);
		int value = line.length() > 4 ? std::stoi( line.substr(3, line.length() )) : 0;

		if( opperation_code == "INC" ) tree->insert( value );
		if( opperation_code == "REM" ) tree->remove( value );
		if( opperation_code == "SUC" ) output_file << tree->get_strict_sucessor( value ) << "\n";
		if( opperation_code == "IMP" ) output_file << tree->print() << "\n";

	}

	return 0;

}