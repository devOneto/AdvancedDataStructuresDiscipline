#include<iostream>
#include <vector>

// Cache Independent B Tree

class Tree;
class Node;

class Node {
public:
	Node* left;
	Node* right;
	int index;
	int key;

	Node(){}
	Node( int _key ): key(_key) {}
	Node( int _key, int _index ): key(_key), index(_index) {}
};

class Tree {


	int height = 0;
	int aux_size = 0;
	int veb_size = 0;
	Node* root = nullptr;
	std::vector< Node* > aux_vector = { root };
	std::vector< Node* > veb_vector = { root };

private:

	Node* get_min_from( Node* node ) {
		while ( node->left != nullptr ) return get_min_from(node->left);
		return node;
	}

	Node* get_max_from( Node* node ) {
		while ( node->right != nullptr ) return get_max_from(node->right);
		return node;
	}

	Node* find_successor_of( Node* node ) {

		if ( node == nullptr ) return nullptr;

		if ( node->right != nullptr ) return get_min_from(node->right);

		Node* current_node = this->root;
		Node* sucessor  = nullptr;

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

	Node* find_predecessor_of( Node* node ) {

		if ( node == nullptr ) return nullptr;

		if ( node->left != nullptr ) return get_max_from(node->left);

		Node* current_node = this->root;
		Node* predecessor = nullptr;

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

public:

	Node* find(){
		return nullptr;
	}

	void insert( int value ) {

		Node* new_node = new Node(value);
		new_node->left = nullptr;
		new_node->right = nullptr;

		if ( this->root == nullptr ) {
			new_node->index = 0;
			this->root = new_node;
			aux_vector = { root };
			veb_vector = { root };
			this->aux_size = 1;
			this->veb_size = 1;
			return;
		}

		Node* successor = this->find_successor_of(new_node);
		Node* predecessor = this->find_predecessor_of(new_node);

		int scan_index = predecessor != nullptr ? predecessor->index : 0;
		int end_scan_index = successor != nullptr ? successor->index : this->aux_size;
		int empty_space_index = -1;

		// search for empty space between predecessor and sucessor
		while ( scan_index != end_scan_index && scan_index <= this->aux_size ) {
			if( aux_vector[scan_index] != nullptr ) {
				scan_index++;
			}else {
				empty_space_index = scan_index;
				scan_index = end_scan_index;
			}
		}

		// if finds, put the new node there and end
		if ( empty_space_index != -1 ) {
			aux_vector[empty_space_index] = new_node;
			return;
		}

		if ( predecessor != nullptr ) {
			// if not, finds empty space at left of predecessor
			int before_predecessor_empty_index = -1;
			scan_index = 0;
			while ( scan_index != predecessor->index ) {
				if( aux_vector[scan_index] != nullptr ) {
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
				this->aux_vector[old_predecessor_index] = new_node;
				return;
			}
		}

		if ( successor != nullptr ) {
			// if not, finds empty space at right of successor
			int after_successor_empty_index = -1;
			scan_index = successor->index;
			while ( scan_index != this->aux_size ) {
				if( aux_vector[scan_index] != nullptr ) {
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
				this->aux_vector[old_successor_index] = new_node;
				return;
			}
		}

		// table doubling
		for ( int i = 0; i < this->aux_size; i++ ) {
			this->aux_vector.push_back(nullptr);
		}
		this->aux_size *= 2;

		for ( int i = 0; i < this->veb_size; i++ ) {
			this->veb_vector.push_back(nullptr);
		}
		this->veb_size *= 2;

		this->insert(new_node->key);
		return;

	}

	void remove(){}

};

int main(){

	Tree* tree = new Tree();

	tree->insert(20);
	tree->insert(12);
	tree->insert(28);

	std::cout << "Hello World!";
	return 0;
}