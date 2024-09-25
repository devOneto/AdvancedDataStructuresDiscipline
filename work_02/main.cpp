#include <cmath>
#include<iostream>
#include <fstream>
#include <vector>


// Cache Independent B Tree

class Tree;
class TreeNode;

class TreeNode {
public:

	int key;
	int aux_index;
	TreeNode* left;
	TreeNode* right;
	TreeNode* parent;
	bool is_empty = false;

	TreeNode(){}
	TreeNode( int _key ) : key(_key) {}
	TreeNode( int _key, TreeNode* _left, TreeNode* _right ) : key(_key), left(_left), right(_right) {}

};

class Tree {

	int height = 0;
	int aux_size = 0;
	int veb_size = 0;
	int population = 0;
	TreeNode* root = nullptr;
	std::vector< TreeNode* > aux_vector = { nullptr };
	std::vector< TreeNode* > veb_vector = { nullptr };

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
			if( node->key < current_node->key ) {
				sucessor = current_node;
				current_node = current_node->left;
			}
			else if ( node->key > current_node->key ) {
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
			if( node->key > current_node->key) {
				predecessor = current_node;
				current_node = current_node->right;
			}
			else if( node->key < current_node->key ) {
				current_node = current_node->left;
			}
		}

		return predecessor;

	}

	TreeNode* find_successor_by_key( int key ) {

		// a partir da raiz
		TreeNode* current_node = this->root;

		// down right until get a bigger value
		while ( current_node->key < key ) {
			if( current_node->right == nullptr || current_node->right->is_empty ) return nullptr;
			current_node = current_node->right;
		}

		// down left until get a smaller value
		while (
			current_node->left != nullptr &&
			!current_node->left->is_empty &&
			current_node->left->key > key
			) {
			current_node = current_node->left;
		}

		return current_node;

	}

	TreeNode* find_predecessor_by_key( int key ) {

		// from root
		TreeNode* current_node = this->root;

		// down left until get a smaller value
		while( current_node->key > key ) {
			if ( current_node->left == nullptr || current_node->left->is_empty ) return nullptr;
			current_node = current_node->left;
		}

		while ( current_node->right != nullptr
			&& !current_node->right->is_empty
			&& current_node->right->key < key ) {
			current_node = current_node->right;
		}

		return current_node;

	}

	TreeNode* _get_bigger_tree_node(TreeNode* a, TreeNode* b) {

		if ( a->is_empty && !b->is_empty ) return b;
		if ( !a->is_empty && b->is_empty ) return a;
		if ( a->is_empty && b->is_empty ) return nullptr;

		return a->key > b->key ? a : b;

	}

	void _table_doubling() {
		// table doubling
		for ( int i = 0; i < this->aux_size; i++ ) {

			//creates the empty spaces
			TreeNode* new_empty_tree_node = new TreeNode();
			new_empty_tree_node->is_empty = true;
			// fulfill the aux and veb vector with "empty" nodes
			this->aux_vector.push_back( new_empty_tree_node );
			this->veb_vector.push_back( new_empty_tree_node );

		}
		this->aux_size *= 2;
		this->veb_size *= 2;
	}

	void _table_halving() {

		int scan_index = 0;
		std::vector< int > empty_spaces;
		TreeNode* current_node = this->aux_vector[scan_index];

		// scans the aux vector
		while ( scan_index != aux_size ) {

			// stores all empty spaces indexes on an array
			if ( current_node->is_empty )
				empty_spaces.push_back(scan_index);

			// if finds a non empty index, pushes it into the first empty space
			if ( !current_node->is_empty && !empty_spaces.empty() ) {
				current_node->aux_index = empty_spaces[0];
				aux_vector[empty_spaces[0]] = current_node;
				// gambiarra para fazer pop front...
				empty_spaces.erase(empty_spaces.begin());
			}

			scan_index++;
		}

		for ( int i = this->aux_size; i > aux_size/2; i-- ) {
			this->aux_vector.pop_back();
			this->veb_vector.pop_back();
		}

		this->aux_size /= 2;
		this->veb_size /= 2;

	}

	void _rebuild_subtree_from_leaft_until_root( TreeNode* leaf_node ) {

		TreeNode* current_node = leaf_node;

		do {
			TreeNode* bigger = _get_bigger_tree_node( current_node->parent->left, current_node->parent->right );
			current_node->parent->aux_index = bigger->aux_index;
			current_node->parent->is_empty = bigger->is_empty;
			current_node->parent->key = bigger->key;
			current_node = current_node->parent;
		} while ( current_node != this->root );

	}

	void _build_static_tree() {

		// rebuild tree
		this->height = 1;
		int scan_index = 0;

		// create nodes with relashionships on three until the root
		scan_index = 1;
		std::vector < TreeNode* > new_row = {};
		std::vector < TreeNode* > current_row = this->aux_vector;

		// scans the tree_nodes array to create relashionship
		while ( scan_index != current_row.size() ) {

			TreeNode* new_left = current_row[ scan_index - 1 ];
			TreeNode* new_right = current_row[ scan_index ];
			TreeNode* bigger = this->_get_bigger_tree_node(new_left, new_right);
			TreeNode* new_tree_node = new TreeNode( bigger->key, new_left, new_right );
			new_tree_node->aux_index = bigger->aux_index;
			new_left->parent = new_tree_node;
			new_right->parent = new_tree_node;
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

		// this->_build_veb_vector(){}

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

	TreeNode* find_leaf_by_value_vector_search( int value ) {

		int left = 0;
		int right = aux_vector.size();

		while ( left <= right ) {

			//find the mid
			int mid = (left+right)/2;

			//if mid element is null, searches two sides to find a non null
			int mid_left = mid;
			int mid_right = mid;

			while ( mid_left >= left && aux_vector[mid_left]->is_empty ) {
				mid_left--;
			}
			while ( mid_right <= right && aux_vector[mid_right]->is_empty ) {
				mid_right++;
			}

			if ( mid_left >= left && !aux_vector[mid_left]->is_empty ) {
				mid = mid_left;
			}
			else if ( mid_right <= right && !aux_vector[mid_right]->is_empty ) {
				mid = mid_right;
			} else
				return nullptr;

			if ( aux_vector[mid]->key == value ) return aux_vector[mid];
			else if ( aux_vector[mid]->key < value ) left = mid + 1;
			else right = mid - 1;

		}

		return nullptr;

	}

	TreeNode* find_leaf_by_value_tree_search( int value ) {

		// TreeNode* current_node = this->root;
		//
		// while ( current_node->aux_vector_node->key != value ) {
		//
		// 	if( current_node->aux_vector_node->key < value ) {
		// 		while ( current_node->right->aux_vector_node == current_node->aux_vector_node ) {
		// 			TreeNode* min_from_right = get_min_from(current_node->right);
		// 			if( min_from_right->aux_vector_node->key == value ) return min_from_right;
		// 		}
		// 	}
		// 	if ( current_node->aux_vector_node->key > value ) current_node = current_node->left;
		//
		// }

	}

	TreeNode* find_leaf_by_value_linear( int value ) {

		TreeNode* current_node = this->aux_vector[0];

		if ( current_node == nullptr ) return nullptr;

		int scan_index = 0;

		while ( current_node->key != value && scan_index < this->aux_size ) {
			scan_index++;
			current_node = this->aux_vector[scan_index];
		}

		return current_node->key == value ? current_node : nullptr;

	}

	void insert( int value ) {

		if ( this->root == nullptr ) {
			this->aux_vector = { new TreeNode(value) };
			this->veb_vector = { new TreeNode(value) };
			this->root = aux_vector[0];
			this->root->aux_index = 0;
			this->root->parent = nullptr;
			this->aux_size = 1;
			this->veb_size = 1;
			this->population++;
			return;
		}

		TreeNode* successor_tree_node = this->find_successor_by_key(value);
		TreeNode* predecessor_tree_node = this->find_predecessor_by_key(value);

		int scan_index = ( predecessor_tree_node != nullptr && !predecessor_tree_node->is_empty) ? predecessor_tree_node->aux_index : 0;
		int end_scan_index = ( successor_tree_node != nullptr && !successor_tree_node->is_empty ) ? successor_tree_node->aux_index : this->aux_size;
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
			aux_vector[empty_space_index] = new TreeNode(value);
			aux_vector[empty_space_index]->aux_index = empty_space_index;
			population++;
			// this->_rebuild_subtree_from_leaft_until_root( aux_vector[empty_space_index] );
			// TODO: rebuild only necessary sub trees
			return;
		}

		// if not, finds empty space at left of predecessor
		if (  predecessor_tree_node != nullptr && !predecessor_tree_node->is_empty ) {
			int before_predecessor_empty_index = -1;
			scan_index = 0;
			while ( scan_index != predecessor_tree_node->aux_index ) {
				if( !aux_vector[scan_index]->is_empty ) {
					scan_index++;
				} else {
					before_predecessor_empty_index = scan_index;
					scan_index = predecessor_tree_node->aux_index;
				}

			}

			// if there is space moves all the nodes to left
			// puts the new node on the place of the predecessor
			// and ends
			if ( before_predecessor_empty_index != -1 ) {
				int old_predecessor_index = predecessor_tree_node->aux_index;
				for ( int i = before_predecessor_empty_index; i<predecessor_tree_node->aux_index; i++ ) {
					this->aux_vector[i]->aux_index--;
					this->aux_vector[i-1] = this->aux_vector[i];
				}
				this->aux_vector[old_predecessor_index] = new TreeNode(value);
				this->aux_vector[old_predecessor_index]->aux_index = old_predecessor_index;
				population++;
				// TODO: rebuild only necessary tree branches
				// this->_rebuild_subtree_from_leaft_until_root( aux_vector[empty_space_index] );
				this->_build_static_tree();
				return;
			}
		}

		// if not, finds empty space at right of successor
		if ( successor_tree_node != nullptr && !successor_tree_node->is_empty ) {
			int after_successor_empty_index = -1;
			scan_index = successor_tree_node->aux_index;
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
				int old_successor_index = successor_tree_node->aux_index;
				for ( int i = after_successor_empty_index; i > old_successor_index; i-- ) {
					this->aux_vector[i-1]->aux_index++;
					this->aux_vector[ this->aux_vector[i-1]->aux_index ] = this->aux_vector[i-1];
				}
				this->aux_vector[old_successor_index] = new TreeNode(value);
				this->aux_vector[old_successor_index]->aux_index = old_successor_index;
				population++;
				// TODO: rebuild only necessary tree branches
				// this->_rebuild_subtree_from_leaft_until_root( aux_vector[empty_space_index] );
				this->_build_static_tree();
				return;
			}
		}

		// there is no space, so doubles the array size
		this->_table_doubling();

		// after creating enoght space, inserts the new node
		this->insert(value);

		// build the entire tree after doubling
		this->_build_static_tree();

		return;
	}

	void remove( int value ) {

		TreeNode* current_node = this->find_leaf_by_value_vector_search(value);

		aux_vector[current_node->aux_index] = new TreeNode();
		aux_vector[current_node->aux_index]->is_empty = true;
		aux_vector[current_node->is_empty]->aux_index = current_node->aux_index;

		this->population--;

		if ( this->population <= aux_size/4 ) this->_table_halving();

		this->_build_static_tree();

	}

	std::string get_strict_sucessor( int value ) {

		TreeNode* current_node = this->root;

		while( current_node->key < value ) {
			if ( current_node->right == nullptr ) return "NULL";
			current_node = current_node->right;
		}

		while (
			!current_node->left->is_empty &&
			current_node->left->key > value
			) {
			current_node = current_node->left;
		}

		return std::to_string( current_node->key );
	}

	std::string print() {

		std::string result;


		result = "Aux Vector: ";
		for( int i = 0; i < this->aux_vector.size(); i++ ) {
			std::string value = !this->aux_vector[i]->is_empty ? std::to_string(this->aux_vector[i]->key) : "NULL";
			result +=  value + " ";
		}

		// result += "\nVeb Vector: ";
		// for( int i = 0; i < this->veb_vector.size(); i++ ) {
		// 	std::string value = ! this->veb_vector[i]->is_empty ?  std::to_string(this->veb_vector[i]->key) : "NULL";
		// 	result += value + " ";
		// }

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

// TODO: reconstruir somente os ramos da arvore que foram alterados
// TODO: gerar ordem de Van Emdeboas