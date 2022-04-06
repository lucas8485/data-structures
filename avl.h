/*
	avl.h：AVL自平衡二叉查找树的C++实现。
	Copyright 2022 Lucas & yydk77.cn

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
* 请在C++14及以上的环境中编译本源码。
* Revision 3 By Lucas.
* 修复了对空容器调用begin()函数时导致内存异常访问的bug。
* 修改了比较器的实现逻辑，减少插入和删除的比较次数。
* Revision 2 By Lucas.
* 修复了remove(const T&)接口的重大错误。
* 该接口原来的实现方式已被证明错误。
* Revision 1 By Lucas & __depot__.
* 增添了迭代器支持。
* 迭代器为typename avl<T, C>::iterator,
* 其隶属于std::iterator<std::bidirectional_iterator_tag, const T>。
* Programmed By Lucas.
* 在二叉查找树的基础上进行修改使其符合AVL自平衡二叉查找树的定义。
*/

#pragma once

#include <utility>
#include <cstddef>
#include <exception>
#include <iterator>

// 迭代器类型的前向声明。
template <typename T, typename C>
class _avl_iterator;

/*
 * class template avl：AVL平衡二叉查找树。
 * 模板参数说明：
 * T：数据类型，要求此参数需支持operator<（默认情况）
 * 或自定义比较器，同时需具有拷贝操作。
 * C：比较器类型，默认为std::less<T>，
 * 该比较器使用T的operator<完成比较。
 */
template <typename T, typename C = std::less<T>>
class avl final {
	// 针对迭代器类型的友元声明。
	friend class _avl_iterator<T, C>;
	// 私有实现部分。
private:
	/*
	 * 内置数据结构：_node*(typename avl<T, C>::_node*)。
	 * 使用三向链表表示法表示的二叉树中的每个节点。
	 * 成员说明：
	 * value：类型为const T，存储节点值。
	 * leftChild和rightChild：类型为_node*，分别存储左右子树信息。
	 * parent：类型为_node*，存储父节点信息。
	 * factor：类型为std::ptrdiff_t，存储节点的平衡因子。
	 */
	struct _node {
		const T value;
		_node* leftChild = nullptr;
		_node* rightChild = nullptr;
		_node* parent = nullptr;
		std::ptrdiff_t factor = 0;
		// 构造器：接受一个const T&作为参数。该参数将用于初始化value字段。
		_node(const T& _val) : value(_val) {}
		~_node() = default;
	};
	// 私有字段：_root。类型为_node*，存储AVL树的根节点。
	_node* _root;
	// 私有字段：_size。类型为std::size_t，存储AVL树的大小。
	std::size_t _size;
	// 私有字段：_comparator。类型为C，存储比较器的一个实例。
	C _comparator;

	/*
	 * 辅助函数：_compare。
	 * 判断两个元素的相对大小。保证比较操作不超过两次。
	 * 要求比较器具有良好实现。
	 * 参数：
	 * _lhs：待比较的元素。
	 * _rhs：待比较的元素。
	 * 返回值：int。
	 * 若返回值为-1，则_lhs“小于”_rhs。
	 * 若返回值为0，则_lhs“等于”_rhs。
	 * 若返回值为1，则_lhs“大于”_rhs。
	 */
	int _compare(const T& _lhs, const T& _rhs) const {
		if (_comparator(_lhs, _rhs))
			return -1;
		else {
			if (_comparator(_rhs, _lhs))
				return 1;
			else
				return 0;
		}
	}

	/*
	 *	辅助函数：avl(_node*, std::size_t, C)。
	 *	私有构造器，提供给成员函数及其它构造器使用。
	 *	用于对AVL树的各个参数进行设置。
	 */
	avl(_node* _root_node, std::size_t _sz, C _comp) : _root(_root_node), _size(_sz),

		_comparator(_comp) {}

	// 以下辅助函数将会在其定义出现之处予以注释。
	void _insert_node(_node*, const T&);
	const _node* _find_node(const _node*, const T&) const;
	void _remove_node(_node*);
	void _clear_tree(_node*);
	void _make_copy(_node*&) const;
	void _copy_node(_node*, const _node*) const;
	void _swap_node(_node*, _node*);
	_node* _check_tree(_node*);
	_node* _ll_rotate(_node*);
	_node* _rr_rotate(_node*);
	_node* _lr_rotate(_node*);
	_node* _rl_rotate(_node*);

	/*
	 *	辅助函数：_is_left_child。
	 *	用于判断家长和孩子的关系。
	 *	参数：
	 *	_parent：类型为_node*，待判断的父节点。
	 *	_child：类型为_node*，待判断的子节点。
	 *	返回值：bool，指明子节点是父节点的左子树还是右子树。
	 *	若返回值为true，则子节点是父节点的左子树。
	 *	若返回值为false，则子节点是父节点的右子树。
	 *	本函数的调用点保证两个节点存在亲属关系，
	 *	若两个节点不存在亲属关系，则数据结构已经崩坏，
	 *	因此调用std::terminate()结束程序。
	 */
	bool _is_left_child(_node* _parent, _node* _child) {
		if (_parent->leftChild == _child)
			return true;
		else if (_parent->rightChild == _child)
			return false;
		else
			std::terminate();
		return false;
	}

public:
	// 相应的类型别名声明。
	// 因为_avl_iterator<T, C>继承自std::iterator
	// <std::bidirectional_iterator_tag, const T>，
	// 所以其可以作为const_iterator使用。
	using size_type = std::size_t;
	using iterator = _avl_iterator<T, C>;
	using const_iterator = _avl_iterator<T, C>;
	using reverse_iterator = std::reverse_iterator<_avl_iterator<T, C>>;
	using const_reverse_iterator = std::reverse_iterator<_avl_iterator<T, C>>;

	/*
	 *	公共接口：带参/默认构造器。
	 *	接受（可选的）构造器参数。
	 *	若不传入构造器参数，则其会默认构造一个构造器。
	 *	该构造器委托私有构造器完成工作。
	 */
	avl(C comp = C()) : avl(nullptr, 0, comp) {}

	/*
	 *	公共接口：拷贝构造器。
	 *	拷贝构造器调用辅助函数_make_copy(_node*&)完成数据的复制，
	 *	同时复制被拷贝对象的_size和_comparator字段。
	 */
	avl(const avl& src) {
		src._make_copy(_root);
		_size = src._size;
		_comparator = src._comparator;
	}

	/*
	 *	公共接口：拷贝赋值运算符。
	 *	首先调用公共接口clear()清空内部数据，
	 *	然后再从被拷贝对象中复制数据，
	 *	逻辑与拷贝构造器类似。
	 */
	avl& operator=(const avl& src) {
		clear();
		src._make_copy(_root);
		_size = src._size;
		_comparator = src._comparator;
		return *this;
	}

	/*
	 *	公共接口：移动构造函数。
	 *	移动构造函数负责从被移动对象中窃取数据，
	 *	并将被移动对象置于可析构的安全状态中。
	 */
	avl(avl&& src) noexcept : avl(src._root, src._size, src._comparator) {
		src._root = nullptr;
		src._size = 0;
	}

	/*
	 *	公共接口：移动赋值运算符。
	 *	其逻辑与移动构造函数类似，
	 *	但需要负责清空自身的数据。
	 */
	avl& operator=(avl&& src) noexcept {
		clear();
		_root = src._root;
		_size = src._size;
		_comparator = src._comparator;
		src._root = nullptr;
		src._size = 0;
		return *this;
	}

	/*
	 *	公共接口：析构函数。
	 *	负责在对象析构时释放自身的数据，
	 *	该操作通过调用公共接口clear()完成。
	 */
	~avl() {
		clear();
	}

	void put(const T&);
	bool find(const T&) const;
	bool remove(const T&);
	void clear();

	/*
	 *	公共接口：empty()。
	 *	参数：无。
	 *	返回值：bool，指示AVL树是否为空。
	 */
	bool empty() const {
		// 若根节点为nullptr，则AVL树为空。
		return !static_cast<bool>(_root);
	}

	/*
	 *	公共接口：size()。
	 *	参数：无。
	 *	返回值：typename avl<T, C>::size_type(std::size_t)，
	 *	指示AVL树的大小。
	 */
	size_type size() const {
		// 因为在进行节点的增删时_size字段均会得到更新，
		// 所以此接口简单地返回_size字段。
		return _size;
	}

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	reverse_iterator rbegin();
	reverse_iterator rend();
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;
	const_reverse_iterator crbegin() const;
	const_reverse_iterator crend() const;
};

/*
 *	class template _avl_iterator：AVL二叉查找树的迭代器。
 *	模板参数与class template avl一样，此处不再赘述。
 *	其继承自std::iterator<std::bidirectional_iterator_tag, const T>,
 *	此标签授予其双向访问迭代器特性。
 */
template <typename T, typename C = std::less<T>>
class _avl_iterator : public std::iterator<std::bidirectional_iterator_tag, const T> {
	// 对同类型AVL树授予友元权限。
	friend class avl<T, C>;

private:

	// 私有字段：_container。类型为const avl<T, C>*，表示迭代器所属的容器。
	const avl<T, C>* _container;

	// 私有字段：_value。类型为const typename avl<T, C>::_node*，表示迭代器当前所指向的节点。
	const typename avl<T, C>::_node* _value;

	/*
	 *	私有构造器：接受指向容器的指针和指向节点的指针。
	 */
	_avl_iterator(const avl<T, C>* _cont, const typename avl<T, C>::_node* _node) : _container

	(_cont), _value(_node) {}

public:

	/*
	 *	公共接口：默认构造器。
	 *	经由此构造器构造的迭代器等效于“尾后迭代器”。
	 */
	_avl_iterator() : _avl_iterator(nullptr, nullptr) {}

	/*
	 *	公共接口：拷贝构造函数、拷贝赋值运算符、析构函数。
	 *	编译器提供的Bitwise Copy Semantics已能满足本类型需要，
	 *	且无需移动操作。
	 */
	_avl_iterator(const _avl_iterator&) = default;
	_avl_iterator& operator=(const _avl_iterator&) = default;
	~_avl_iterator() = default;

	/*
	 *	公共接口：提领运算符。
	 *	返回值：const T&，从迭代器所指位置提领出元素。
	 *	对无效迭代器进行提领会导致不明确行为。
	 */
	const T& operator*() const {
		return _value->value;
	}

	/*
	 *	公共接口：箭头运算符。
	 *	返回值：返回迭代器所指向位置元素的地址。
	 *	返回后的指针将会被编译器再一次施加箭头运算符操作，
	 *	使其行为类似于内建指针。
	 */
	const T* operator->() const {
		return &(_value->value);
	}
	_avl_iterator& operator++();
	_avl_iterator operator++(int);
	_avl_iterator& operator--();
	_avl_iterator operator--(int);

	/*
	 *	公共接口：相等性运算符。
	 *	参数：
	 *	_lhs：待比较的迭代器。
	 *	_rhs：待比较的迭代器。
	 *	返回值：bool，指示两个迭代器是否相等。
	 */
	friend bool operator==(const _avl_iterator& _lhs, const _avl_iterator& _rhs) {
		// 分属于不同容器的节点一定不相同，
		// 除非它们都是尾后迭代器，
		// 而尾后迭代器都是相同的，
		// 仅用作循环的终止条件或算法的输入区间。
		return _lhs._value == _rhs._value;
	}
};

/*
 *	公共接口：_avl_iterator<T, C>::operator++()/前置递增运算符。
 *	该接口决定了迭代器在二叉树中如何移动。
 *	operator++负责以中序遍历的方式，
 *	将迭代器移动至当前节点的后继。
 *	调用者必须保证迭代器有效，否则将出现不明确行为。
 */
template <typename T, typename C>
_avl_iterator<T, C>& _avl_iterator<T, C>::operator++() {
	// 情况1：当前节点是叶节点，则需要向上遍历。
	if (!_value->leftChild && !_value->rightChild) {
		// 记录其父节点。
		const typename avl<T, C>::_node* _parent = _value->parent;
		/*
		 *	若其为父节点的左子树，则它的后继是父节点。
		 *	若其为父节点的右子树，则沿其父节点向上回溯，
		 *	直到此节点是其父节点的左节点，
		 *	则此节点是它的后继。
		 *	对于二叉树末尾的元素，递增指向它的迭代器会使这个迭代器变为尾后迭代器。
		 */
		if (_parent->leftChild == _value) {
			_value = _parent;
		}
		else {
			const typename avl<T, C>::_node* _p = _value;
			while ((_p->parent) && (_p->parent->rightChild == _p))
				_p = _p->parent;
			_value = _p->parent;
		}
	}
	else {
		// 情况2：当前节点不是叶节点，则需要向下遍历。
		// 记录其右子树。
		const typename avl<T, C>::_node* _p = _value->rightChild;
		/*
		 *	若它的右子树为空，则当前节点是以它为根的子树里最大的节点，
		 *	需要向上回溯。
		 *	若它的右子树非空，则从它的右子树出发，访问其左子树
		 *	直到其左子树为空，根据二叉查找树的定义，此节点是当前节点的后继。
		 */
		if (_p) {
			while (_p->leftChild)
				_p = _p->leftChild;
		}
		else {
			_p = _value->parent;
		}
		_value = _p;
	}
	return *this;
}

/*
 *	公共接口：_avl_iterator<T, C>::operator++(int)/后置递增运算符。
 *	对当前迭代器进行递增，返回修改之前的迭代器。
 */
template <typename T, typename C>
_avl_iterator<T, C> _avl_iterator<T, C>::operator++(int) {
	_avl_iterator<T, C> _prev = *this;
	++* this;
	return _prev;
}

/*
 *	公共接口：_avl_iterator<T, C>::operator--()/前置递减运算符。
 *	与前置递增运算符互为镜像操作，负责将迭代器移动到它的前驱。
 *	若迭代器为尾后迭代器且绑定至了一个有效的容器，
 *	该操作会将迭代器移动至AVL树的最后一个节点。
 *	若迭代器为尾后迭代器且未绑定至一个有效的容器，
 *	该操作会引发不明确行为。
 *	若迭代器为首迭代器，该操作会将迭代器变为尾后迭代器。
 */
template <typename T, typename C>
_avl_iterator<T, C>& _avl_iterator<T, C>::operator--() {

	// 若value为空，则尝试读取其所属容器的根节点。
	if (!_value) {

		// Note：若本迭代器未绑定至一个有效的容器，本操作会带来不明确行为。
		auto _p = _container->_root;

		// 若容器为空，则不执行任何操作。
		if (!_p)
			return *this;

		// 若容器不为空，则将迭代器移动至所属容器的最后一个节点。
		else
			while (_p->rightChild)
				_p = _p->rightChild;
		_value = _p;
		return *this;
	}

	// 以下操作为operator++()中的镜像操作，
	// 请对照operator++()中的源码和注释阅读。
	if ((!_value->leftChild) && (!_value->rightChild)) {
		const typename avl<T, C>::_node* _parent = _value->parent;
		if (_parent->rightChild == _value) {
			_value = _parent;
		}
		else {
			const typename avl<T, C>::_node* _p = _value;
			while ((_p->parent) && (_p->parent->leftChild == _p))
				_p = _p->parent;
			_value = _p->parent;
		}
	}
	else {
		const typename avl<T, C>::_node* _p = _value->leftChild;
		if (_p) {
			while (_p->rightChild)
				_p = _p->rightChild;
		}
		else {
			_p = _value->parent;
		}
		_value = _p;
	}
	return *this;
}

/*
 *	公共接口：_avl_iterator<T, C>::operator--(int)/后置递减运算符。
 *	对当前迭代器执行递减操作，返回修改之前的迭代器。
 */
template <typename T, typename C>
_avl_iterator<T, C> _avl_iterator<T, C>::operator--(int) {
	_avl_iterator<T, C> _prev = *this;
	--* this;
	return _prev;
}

/*
 *	公共接口：不等性运算符。
 *	参数：
 * 	_lhs：待比较的迭代器。
 *	_rhs：待比较的迭代器。
 *	返回值：bool，指示两个迭代器是否不等。
 */
template <typename T, typename C>
bool operator!=(const _avl_iterator<T, C>& _lhs, const _avl_iterator<T, C>& _rhs) {

	//该操作简单调用operator==，并将其结果反相。
	return !(_lhs == _rhs);
}

/*
 *	辅助函数：_swap_node。
 *	用于交换两个节点在AVL树中的位置。
 *	参数：
 *	_n：待交换的节点。
 *	_prev：待交换的节点。（这两个参数名确实没什么意义，可是我不想改了:-)）
 */
template <typename T, typename C>
void avl<T, C>::_swap_node(typename avl<T, C>::_node* _n, typename avl<T, C>::_node* _prev) {

	// lambda表达式：热炒热卖，现用现写的函数来咯！
	auto _swap = [](typename avl<T, C>::_node*& _lhs, typename avl<T, C>::_node*& _rhs) {
		auto _temp = _lhs;
		_lhs = _rhs;
		_rhs = _temp;
	};

	/*
	 *	1.交换它们的父亲。
	 *	同时设置父亲的子节点指针。
	 *	对根节点进行特殊处理：同时要交换this->_root指向的值。
	 */
	if (_n->parent) {
		if (_is_left_child(_n->parent, _n))
			_n->parent->leftChild = _prev;
		else
			_n->parent->rightChild = _prev;
	}
	else
		_root = _prev;
	if (_prev->parent) {
		if (_is_left_child(_prev->parent, _prev))
			_prev->parent->leftChild = _n;
		else
			_prev->parent->rightChild = _n;
	}
	else
		_root = _n;
	_swap(_n->parent, _prev->parent);

	// 2.交换它们和它们的儿子的连接。
	if (_n->leftChild)
		_n->leftChild->parent = _prev;
	if (_n->rightChild)
		_n->rightChild->parent = _prev;
	if (_prev->leftChild)
		_prev->leftChild->parent = _n;
	if (_prev->rightChild)
		_prev->rightChild->parent = _n;
	_swap(_n->rightChild, _prev->rightChild);
	_swap(_n->leftChild, _prev->leftChild);

	// 3.交换它们的平衡因子。
	std::ptrdiff_t _n_factor = _n->factor;
	_n->factor = _prev->factor;
	_prev->factor = _n_factor;
}

/*
 *	辅助函数：_insert_node。
 *	用于往以指定节点为根节点的子树中插入一个元素。
 *	参数：
 *  _n：指定的节点。
 *  _val：待插入AVL树的值。
 */
template <typename T, typename C>
void avl<T, C>::_insert_node(typename avl<T, C>::_node* _n, const T& _val) {

	// 如果AVL树为空，那么直接在根节点上构造。
	if (!_root) {
		_root = new typename avl<T, C>::_node(_val);
		_size++;
		return;
	}

	// 如果当前节点值与待插入的值相等，那么直接返回。
	auto _result = _compare(_val, _n->value);
	if (!_result)
		return;

	// 否则进入如下的流程。
	else {

		// 如果待插入值“小于”当前节点值，则将其插入当前节点的左子树。
		if (_result == -1) {

			// 如果_n的左子树不为空...
			if (_n->leftChild) {

				// 记录左子树原来的平衡因子。
				auto _prev_factor = _n->leftChild->factor;

				// 递归地将_val插入左子树。
				_insert_node(_n->leftChild, _val);

				// 记录原来的左子树，然后再检查它经过插入操作后
				// 是否符合AVL树的定义。若不符合AVL树的定义则进行相应的调整。
				auto _prev_left = _n->leftChild;

				// 如果其符合AVL树的定义，则未进行调整，可能涉及到平衡因子的变化。
				// 则进入后续的检查。
				if (_prev_left == _check_tree(_prev_left)) {

					/*
					 *	如果它的平衡因子等于0，则经过插入操作后它的高度未发生变化，
					 *	不更改_n的平衡因子。
					 *	如果它的平衡因子在插入操作前后未发生变化，则它的高度也未发生变化，
					 *	不改变_n的平衡因子。
					 *	其他情况下，_n的左子树高度增加1，则平衡因子加1。
					 */
					if (_prev_left->factor != 0 && _prev_left->factor !=

						_prev_factor)
						_n->factor++;
				}
				else

					/*	补偿在_check_node旋转后对平衡因子的调整。
					 *	_check_node默认旋转以后对父节点的平衡因子进行调整。
					 *	在这种情况中，因为_prev_left是_n的左子树，
					 *	在调整的时候_n的平衡因子会被减1，
					 *	而其实_prev_left的高度并未发生变化（插入节点使高度加1，
					 *	旋转调整使高度减1），
					 *	所以在此处给予补偿。
					 */
					_n->factor++;
			}
			else {

				/*
				 *	如果左子树为空，直接动刀子就好了。
				 *	由AVL树的定义可知插入之后一定符合AVL树的定义，
				 *	所以不用检查。
				 */
				_n->leftChild = new typename avl<T, C>::_node(_val);
				_n->leftChild->parent = _n;
				_size++;
				_n->factor++;
			}
		}
		else {

			/*
			 *	递归插入右子树，与插入左子树互为镜像操作。
			 *	请参照上方的源码和注释阅读。
			 */
			if (_n->rightChild) {
				auto _prev_factor = _n->rightChild->factor;
				_insert_node(_n->rightChild, _val);
				auto _prev_right = _n->rightChild;
				if (_prev_right == _check_tree(_prev_right)) {
					if (_prev_right->factor != 0 && _prev_right->factor !=

						_prev_factor)
						_n->factor--;
				}
				else
					_n->factor--;
			}
			else {
				_n->rightChild = new typename avl<T, C>::_node(_val);
				_n->rightChild->parent = _n;
				_size++;
				_n->factor--;
			}
		}
	}
}

/*
 *	辅助函数：_find_node。
 *	参数：
 *	_n：其类型为const typename avl<T, C>::_node*，从哪里开始查找。
 *	_val：待查找的元素。
 *	返回值：typename avl<T, C>::_node*，其值有两种情况：
 *	nullptr，未找到这个值；
 *	非空，返回值表示存储这个值的节点。
 */
template <typename T, typename C>
const typename avl<T, C>::_node* avl<T, C>::_find_node(const typename avl<T, C>::_node* _n, const T& _val) const {

	// 若_n为空，直接返回空指针即可。
	// 若_n和_val“相等”，返回_n表示找到。
	
	if (!_n)
		return nullptr;
	int _result = _compare(_val, _n->value);
	if (!_result)
		return _n;
	else {

		// 若_val“小于”_n表示的节点的值，则在_n的左子树里面找。
		if (_result == -1)
			return _find_node(_n->leftChild, _val);

		// 否则，在_n的右子树里面找。
		else
			return _find_node(_n->rightChild, _val);
	}
}

/*
 *	辅助函数：_remove_node。
 *	接受一个指定的节点，并将其删除。
 *	参数：
 *	_n：待删除的节点。
 */
template <typename T, typename C>
void avl<T, C>::_remove_node(typename avl<T, C>::_node* _n) {

	// 若_n是叶节点...
	if (!_n->leftChild && !_n->rightChild) {

		// 根据它是其父节点的左子树或右子树，调整它的父节点的平衡因子。
		auto _prev_parent = _n->parent;
		if (_is_left_child(_prev_parent, _n)) {
			_prev_parent->leftChild = nullptr;
			_prev_parent->factor--;
		}
		else {
			_prev_parent->rightChild = nullptr;
			_prev_parent->factor++;
		}

		// 将其占用的内存归还给系统。
		delete _n;

		// 因为移除了一个节点，所以AVL树的大小减1。
		_size--;

		// 从被删除节点的父节点往根节点检查，并且进行调整。
		for (auto _p = _prev_parent; _p; _p = _p->parent) {
			_p = _check_tree(_p);
		}
	}
	else {
		// _n不是叶节点。
		// 获得_n的前驱。
		auto _prev = const_cast<typename avl<T, C>::_node*>((--(_avl_iterator<T, C>(this, _n)))._value);

		// 将_n和它的前驱交换。
		_swap_node(_n, _prev);

		// 再尝试删除_n。最终会转化为删除叶节点的情况。
		_remove_node(_n);
	}
}

/*
 *	辅助函数：_clear_tree。
 *	删除以_current为根节点的子树。
 *	参数：
 *	_current：类型为typename avl<T, C>::_node*，待删除的子树。
 */
template <typename T, typename C>
void avl<T, C>::_clear_tree(typename avl<T, C>::_node* _current) {

	// 若_current为空，则不用删除。
	if (!_current)
		return;

	// 否则以先序遍历的方式删除整棵树。
	_clear_tree(_current->leftChild);
	_clear_tree(_current->rightChild);
	delete _current;
}

/*
 *	辅助函数：_make_copy。
 *	将本二叉树的内容复制入_dest中。
 *	参数：
 *	_dest：类型为typename avl<T, C>::_node*&，复制的目的地。
 */
template <typename T, typename C>
void avl<T, C>::_make_copy(typename avl<T, C>::_node*& _dest) const {

	// 先将根节点的内容复制入_dest中。
	_dest = new typename avl<T, C>::_node(_root->value);
	_dest->factor = _root->factor;

	// 再调用辅助函数_copy_node复制其左子树和右子树。
	_copy_node(_dest, _root);
}

/*
 *	辅助函数：_copy_node。
 *	复制二叉树的左右子树。
 *	参数：
 *	_dest：复制目的地。
 *	_src：复制源。
 */
template <typename T, typename C>
void avl<T, C>::_copy_node(typename avl<T, C>::_node* _dest, const typename avl<T, C>::_node* _src) const {

	// 检验指针有效性。（非常重要！）
	if (!_src || !_dest)
		return;

	// 若src有左子树，则复制之。
	if (_src->leftChild) {
		_dest->leftChild = new typename avl<T, C>::_node(_src->leftChild->value);
		_dest->leftChild->parent = _dest;
		_dest->leftChild->factor = _src->leftChild->factor;
		_copy_node(_dest->leftChild, _src->leftChild);
	}

	// 若src有右子树，复制之。
	if (_src->rightChild) {
		_dest->rightChild = new typename avl<T, C>::_node(_src->rightChild->value);
		_dest->rightChild->parent = _dest;
		_dest->rightChild->factor = _src->rightChild->factor;
		_copy_node(_dest->rightChild, _src->rightChild);
	}
}

/*
 *	辅助函数：_check_tree。
 *	检验以_n为根节点的树是否符合AVL树的定义。
 *	若不符合则执行相应调整。
 *	参数：
 *	_n：类型为typename avl<T, C>::_node*，待调整的节点。
 *	返回值：typename avl<T, C>::_node*，调整完的节点。
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_check_tree(typename avl<T, C>::_node* _n) {

	// 根据_n的平衡因子进行调整。
	// 本函数中的std::terminate()分支表示根据AVL树的定义绝不可能到达的分支。
	switch (_n->factor) {
	case -2: {

		// 若_n的平衡因子为-2则需要进行R-旋转以保持AVL树的性质。
		auto _prev_r = _n->rightChild;
		auto _prev_rl = _prev_r->leftChild;
		auto _prev_par = _n->parent;

		// 旋转过后整棵树的高度减去1，因此调整父节点的平衡因子。
		// 在_insert_node函数中的调整也可在此处找到答案。
		if (_prev_par) {
			if (_prev_par->leftChild == _n)
				_prev_par->factor--;
			else
				_prev_par->factor++;
		}

		// 再根据它的右子树的平衡因子进行调整。
		// 代码中的平衡因子调整均为我在纸上推证得到的结果，
		// 您如果有疑问可以自己动手推证。
		switch (_prev_r->factor) {

			// 右子树平衡因子为-1，则进行R-R旋转。
		case -1:
			_n->factor = 0;
			_prev_r->factor = 0;
			return _rr_rotate(_n);

			// 右子树平衡因子为0，理论上R-L旋转和R-R旋转均可以完成任务。
			// 但R-R旋转计算量更小，因此此处选择R-R旋转。
		case 0:
			_n->factor = -1;
			_prev_r->factor = 1;
			return _rr_rotate(_n);

			// 右子树的平衡因子为1，则进行R-L旋转。
		case 1:
			switch (_prev_rl->factor) {
			case -1:
				_n->factor = 1;
				_prev_r->factor = _prev_rl->factor = 0;
				break;
			case 0:
				_prev_r->factor = _prev_rl->factor = _n->factor = 0;
				break;
			case 1:
				_prev_r->factor = _n->factor = 0;
				_prev_rl->factor = 1;
				break;
			default:
				std::terminate();
			}
			return _rl_rotate(_n);
		default:
			std::terminate();
		}
	}

		   // 若平衡因子的绝对值小于等于1，则不用调整，返回_n即可。
	case -1:
	case 0:
	case 1:
		return _n;

		// 若平衡因子等于2，则需要进行L-旋转。
		// 此操作与R-旋转互为镜像操作，请参照上方case -2的情况阅读。
	case 2: {
		auto _prev_l = _n->leftChild;
		auto _prev_lr = _prev_l->rightChild;
		auto _prev_par = _n->parent;
		if (_prev_par) {
			if (_prev_par->leftChild == _n)
				_prev_par->factor--;
			else
				_prev_par->factor++;
		}
		switch (_prev_l->factor) {
		case 1:
			_prev_l->factor = _n->factor = 0;
			return _ll_rotate(_n);
		case 0:
			_prev_l->factor = -1;
			_n->factor = 1;
			return _ll_rotate(_n);
		case -1:
			switch (_prev_lr->factor) {
			case -1:
				_prev_l->factor = 1;
				_prev_lr->factor = _n->factor = 0;
				break;
			case 0:
				_prev_l->factor = _prev_lr->factor = _n->factor = 0;
				break;
			case 1:
				_prev_l->factor = _prev_lr->factor = 0;
				_n->factor = -1;
				break;
			default:
				std::terminate();
			}
			return _lr_rotate(_n);
		default:
			break;
		}
	}
	default:
		break;
	}
	std::terminate();
	return nullptr;
}

/*
 *	辅助函数：_ll_rotate。
 *	对指定的节点执行L-L旋转。
 *	参数：
 *	_n：待调整的节点。
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_ll_rotate(typename avl<T, C>::_node* _n) {

	// 阅读本处的代码时，建议您拿起笔和纸，边读边动手推理。
	// 记录_n的父节点和左子树。
	auto _prev_parent = _n->parent;
	auto _prev_left = _n->leftChild;

	// 设置_prev_left的父节点为_prev_parent。
	_prev_left->parent = _prev_parent;
	if (_prev_parent) {
		if (_prev_parent->rightChild == _n) {
			_prev_parent->rightChild = _prev_left;
		}
		else {
			_prev_parent->leftChild = _prev_left;
		}
	}
	else
		_root = _prev_left;

	// 设置_n的左子树为_prev_left的右子树。
	_n->leftChild = _prev_left->rightChild;
	if (_prev_left->rightChild)
		_prev_left->rightChild->parent = _n;

	// 设置_prev_left的右子树为_n。
	_n->parent = _prev_left;
	_prev_left->rightChild = _n;

	// 返回_prev_left作为调整完成的节点。
	return _prev_left;
}

/*
 *	辅助函数：_rr_rotate。
 *	对指定的节点实行R-R旋转。
 *	参数：
 *	_n：待调整的节点。
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_rr_rotate(typename avl<T, C>::_node* _n) {

	// R-R旋转与L-L旋转互为镜像操作，请参考L-L旋转源码阅读。
	auto _prev_parent = _n->parent;
	auto _prev_right = _n->rightChild;
	_prev_right->parent = _prev_parent;
	if (_prev_parent) {
		if (_prev_parent->rightChild == _n) {
			_prev_parent->rightChild = _prev_right;
		}
		else {
			_prev_parent->leftChild = _prev_right;
		}
	}
	else
		_root = _prev_right;
	_n->rightChild = _prev_right->leftChild;
	if (_prev_right->leftChild)
		_prev_right->leftChild->parent = _n;
	_n->parent = _prev_right;
	_prev_right->leftChild = _n;
	return _prev_right;
}

/*
 *	辅助函数：_lr_rotate。
 *	对指定的节点执行L-R旋转。
 *	参数：
 *	_n：待调整的节点。
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_lr_rotate(typename avl<T, C>::_node* _n) {

	// 对_n的左子树执行R-R旋转。
	_rr_rotate(_n->leftChild);

	// 再对_n执行L-L旋转。
	return _ll_rotate(_n);
}


/*
 *	辅助函数：_rl_rotate。
 *	对指定的节点执行R-L旋转。
 *	参数：
 *	_n：待调整的节点。
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_rl_rotate(typename avl<T, C>::_node* _n) {

	// 对_n的右子树执行L-L旋转。
	_ll_rotate(_n->rightChild);

	// 再对_n执行R-R旋转。
	return _rr_rotate(_n);
}

/*
 *	公共接口：put。
 *	将一个值放入AVL树。
 *	参数：
 *	_value：其类型为const T&，待插入的值。
 */
template <typename T, typename C>
void avl<T, C>::put(const T& _value) {

	// 从_root开始插入。
	_insert_node(_root, _value);

	// _insert_node中未对_root进行核验，所以核验在此处进行。
	_root = _check_tree(_root);
}

/*
 *	公共接口:find。
 *	查找一个值是否位于AVL树中。
 *	参数：
 *	_value：其类型为const T&，待查找的值。
 *	返回值：bool，表示是否找到。
 */
template <typename T, typename C>
bool avl<T, C>::find(const T& _value) const {

	// 将实际工作交给_find_node，从_root开始查找。
	// 若查找到的节点为空指针，则没有找到。
	// 若查找到的节点非空，则找到。
	return static_cast<bool>(_find_node(_root, _value));
}

/*
 *	公共接口：remove。
 *	删除一个值（如果这个值位于AVL树中。）。
 *	参数：
 *	_value：其类型为const T&，待删除的值。
 *	返回值：
 *	bool，指示删除操作是否成功执行。
 */
template <typename T, typename C>
bool avl<T, C>::remove(const T& _value) {

	// 先查找节点是否存在于AVL树中。
	auto _loc_node = const_cast<typename avl<T, C>::_node*>(_find_node(_root, _value));

	// 如果_loc_node非空，则此节点存在于AVL树中。
	if (_loc_node) {

		// 交由_remove_node删除节点，并返回true表示成功删除。
		_remove_node(_loc_node);
		return true;
	}
	else {

		// 未找到节点，返回false表示删除失败。
		return false;
	}
}

/*
 *	公共接口：clear()。
 *	作用：清空AVL树存储的所有节点并释放内存。
 */
template <typename T, typename C>
void avl<T, C>::clear() {

	// 调用_clear_tree删除整棵树。
	_clear_tree(_root);

	// 置根节点为nullptr，_size为0。
	// 此时本AVL树中不含有任何数据。
	_root = nullptr;
	_size = 0;
}

/*
 *	公共接口族：begin系列、end系列。
 *	返回AVL树的首迭代器和尾后迭代器。
 */
template <typename T, typename C>
typename avl<T, C>::iterator avl<T, C>::begin() {
	auto _p = _root;

	// 沿根节点往下访问左子树，直到不再有左子树。
	// 此时的节点便是AVL树的首节点。
	if (_p)
		while (_p->leftChild)
			_p = _p->leftChild;
	return _avl_iterator<T, C>(this, _p);
}

template <typename T, typename C>
typename avl<T, C>::iterator avl<T, C>::end() {
	return _avl_iterator<T, C>(this, nullptr);
}

template <typename T, typename C>
typename avl<T, C>::const_iterator avl<T, C>::begin() const {
	auto _p = _root;
	if (_p)
		while (_p->leftChild)
			_p = _p->leftChild;
	return _avl_iterator<T, C>(this, _p);
}

template <typename T, typename C>
typename avl<T, C>::const_iterator avl<T, C>::end() const {
	return _avl_iterator<T, C>(this, nullptr);
}

template <typename T, typename C>
typename avl<T, C>::const_iterator avl<T, C>::cbegin() const {
	auto _p = _root;
	if (_p)
		while (_p->leftChild)
			_p = _p->leftChild;
	return _avl_iterator<T, C>(this, _p);
}

template <typename T, typename C>
typename avl<T, C>::iterator avl<T, C>::cend() const {
	return _avl_iterator<T, C>(this, nullptr);
}

/*
 *	公共接口族：rbegin和rend系列。
 *	返回此AVL树的反向迭代器。
 *	具体可参见reverse_iterator的相关说明。
 */
template <typename T, typename C>
typename avl<T, C>::reverse_iterator
avl<T, C>::rbegin() {
	return std::make_reverse_iterator(end());
}

template <typename T, typename C>
typename avl<T, C>::reverse_iterator
avl<T, C>::rend() {
	return std::make_reverse_iterator(begin());
}

template <typename T, typename C>
typename avl<T, C>::const_reverse_iterator
avl<T, C>::rbegin() const {
	return std::make_reverse_iterator(cend());
}

template <typename T, typename C>
typename avl<T, C>::const_reverse_iterator
avl<T, C>::rend() const {
	return std::make_reverse_iterator(cbegin());
}

template <typename T, typename C>
typename avl<T, C>::const_reverse_iterator
avl<T, C>::crbegin() const {
	return std::make_reverse_iterator(cend());
}

template <typename T, typename C>
typename avl<T, C>::const_reverse_iterator
avl<T, C>::crend() const {
	return std::make_reverse_iterator(cbegin());
}
