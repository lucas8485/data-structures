/*
	avl.h��AVL��ƽ������������C++ʵ�֡�
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
* ����C++14�����ϵĻ����б��뱾Դ�롣
* Revision 3 By Lucas.
* �޸��˶Կ���������begin()����ʱ�����ڴ��쳣���ʵ�bug��
* �޸��˱Ƚ�����ʵ���߼������ٲ����ɾ���ıȽϴ�����
* Revision 2 By Lucas.
* �޸���remove(const T&)�ӿڵ��ش����
* �ýӿ�ԭ����ʵ�ַ�ʽ�ѱ�֤������
* Revision 1 By Lucas & __depot__.
* �����˵�����֧�֡�
* ������Ϊtypename avl<T, C>::iterator,
* ��������std::iterator<std::bidirectional_iterator_tag, const T>��
* Programmed By Lucas.
* �ڶ���������Ļ����Ͻ����޸�ʹ�����AVL��ƽ�����������Ķ��塣
*/

#pragma once

#include <utility>
#include <cstddef>
#include <exception>
#include <iterator>

// ���������͵�ǰ��������
template <typename T, typename C>
class _avl_iterator;

/*
 * class template avl��AVLƽ������������
 * ģ�����˵����
 * T���������ͣ�Ҫ��˲�����֧��operator<��Ĭ�������
 * ���Զ���Ƚ�����ͬʱ����п���������
 * C���Ƚ������ͣ�Ĭ��Ϊstd::less<T>��
 * �ñȽ���ʹ��T��operator<��ɱȽϡ�
 */
template <typename T, typename C = std::less<T>>
class avl final {
	// ��Ե��������͵���Ԫ������
	friend class _avl_iterator<T, C>;
	// ˽��ʵ�ֲ��֡�
private:
	/*
	 * �������ݽṹ��_node*(typename avl<T, C>::_node*)��
	 * ʹ�����������ʾ����ʾ�Ķ������е�ÿ���ڵ㡣
	 * ��Ա˵����
	 * value������Ϊconst T���洢�ڵ�ֵ��
	 * leftChild��rightChild������Ϊ_node*���ֱ�洢����������Ϣ��
	 * parent������Ϊ_node*���洢���ڵ���Ϣ��
	 * factor������Ϊstd::ptrdiff_t���洢�ڵ��ƽ�����ӡ�
	 */
	struct _node {
		const T value;
		_node* leftChild = nullptr;
		_node* rightChild = nullptr;
		_node* parent = nullptr;
		std::ptrdiff_t factor = 0;
		// ������������һ��const T&��Ϊ�������ò��������ڳ�ʼ��value�ֶΡ�
		_node(const T& _val) : value(_val) {}
		~_node() = default;
	};
	// ˽���ֶΣ�_root������Ϊ_node*���洢AVL���ĸ��ڵ㡣
	_node* _root;
	// ˽���ֶΣ�_size������Ϊstd::size_t���洢AVL���Ĵ�С��
	std::size_t _size;
	// ˽���ֶΣ�_comparator������ΪC���洢�Ƚ�����һ��ʵ����
	C _comparator;

	/*
	 * ����������_compare��
	 * �ж�����Ԫ�ص���Դ�С����֤�Ƚϲ������������Ρ�
	 * Ҫ��Ƚ�����������ʵ�֡�
	 * ������
	 * _lhs�����Ƚϵ�Ԫ�ء�
	 * _rhs�����Ƚϵ�Ԫ�ء�
	 * ����ֵ��int��
	 * ������ֵΪ-1����_lhs��С�ڡ�_rhs��
	 * ������ֵΪ0����_lhs�����ڡ�_rhs��
	 * ������ֵΪ1����_lhs�����ڡ�_rhs��
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
	 *	����������avl(_node*, std::size_t, C)��
	 *	˽�й��������ṩ����Ա����������������ʹ�á�
	 *	���ڶ�AVL���ĸ��������������á�
	 */
	avl(_node* _root_node, std::size_t _sz, C _comp) : _root(_root_node), _size(_sz),

		_comparator(_comp) {}

	// ���¸��������������䶨�����֮������ע�͡�
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
	 *	����������_is_left_child��
	 *	�����жϼҳ��ͺ��ӵĹ�ϵ��
	 *	������
	 *	_parent������Ϊ_node*�����жϵĸ��ڵ㡣
	 *	_child������Ϊ_node*�����жϵ��ӽڵ㡣
	 *	����ֵ��bool��ָ���ӽڵ��Ǹ��ڵ��������������������
	 *	������ֵΪtrue�����ӽڵ��Ǹ��ڵ����������
	 *	������ֵΪfalse�����ӽڵ��Ǹ��ڵ����������
	 *	�������ĵ��õ㱣֤�����ڵ����������ϵ��
	 *	�������ڵ㲻����������ϵ�������ݽṹ�Ѿ�������
	 *	��˵���std::terminate()��������
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
	// ��Ӧ�����ͱ���������
	// ��Ϊ_avl_iterator<T, C>�̳���std::iterator
	// <std::bidirectional_iterator_tag, const T>��
	// �����������Ϊconst_iteratorʹ�á�
	using size_type = std::size_t;
	using iterator = _avl_iterator<T, C>;
	using const_iterator = _avl_iterator<T, C>;
	using reverse_iterator = std::reverse_iterator<_avl_iterator<T, C>>;
	using const_reverse_iterator = std::reverse_iterator<_avl_iterator<T, C>>;

	/*
	 *	�����ӿڣ�����/Ĭ�Ϲ�������
	 *	���ܣ���ѡ�ģ�������������
	 *	�������빹���������������Ĭ�Ϲ���һ����������
	 *	�ù�����ί��˽�й�������ɹ�����
	 */
	avl(C comp = C()) : avl(nullptr, 0, comp) {}

	/*
	 *	�����ӿڣ�������������
	 *	�������������ø�������_make_copy(_node*&)������ݵĸ��ƣ�
	 *	ͬʱ���Ʊ����������_size��_comparator�ֶΡ�
	 */
	avl(const avl& src) {
		src._make_copy(_root);
		_size = src._size;
		_comparator = src._comparator;
	}

	/*
	 *	�����ӿڣ�������ֵ�������
	 *	���ȵ��ù����ӿ�clear()����ڲ����ݣ�
	 *	Ȼ���ٴӱ����������и������ݣ�
	 *	�߼��뿽�����������ơ�
	 */
	avl& operator=(const avl& src) {
		clear();
		src._make_copy(_root);
		_size = src._size;
		_comparator = src._comparator;
		return *this;
	}

	/*
	 *	�����ӿڣ��ƶ����캯����
	 *	�ƶ����캯������ӱ��ƶ���������ȡ���ݣ�
	 *	�������ƶ��������ڿ������İ�ȫ״̬�С�
	 */
	avl(avl&& src) noexcept : avl(src._root, src._size, src._comparator) {
		src._root = nullptr;
		src._size = 0;
	}

	/*
	 *	�����ӿڣ��ƶ���ֵ�������
	 *	���߼����ƶ����캯�����ƣ�
	 *	����Ҫ���������������ݡ�
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
	 *	�����ӿڣ�����������
	 *	�����ڶ�������ʱ�ͷ���������ݣ�
	 *	�ò���ͨ�����ù����ӿ�clear()��ɡ�
	 */
	~avl() {
		clear();
	}

	void put(const T&);
	bool find(const T&) const;
	bool remove(const T&);
	void clear();

	/*
	 *	�����ӿڣ�empty()��
	 *	�������ޡ�
	 *	����ֵ��bool��ָʾAVL���Ƿ�Ϊ�ա�
	 */
	bool empty() const {
		// �����ڵ�Ϊnullptr����AVL��Ϊ�ա�
		return !static_cast<bool>(_root);
	}

	/*
	 *	�����ӿڣ�size()��
	 *	�������ޡ�
	 *	����ֵ��typename avl<T, C>::size_type(std::size_t)��
	 *	ָʾAVL���Ĵ�С��
	 */
	size_type size() const {
		// ��Ϊ�ڽ��нڵ����ɾʱ_size�ֶξ���õ����£�
		// ���Դ˽ӿڼ򵥵ط���_size�ֶΡ�
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
 *	class template _avl_iterator��AVL����������ĵ�������
 *	ģ�������class template avlһ�����˴�����׸����
 *	��̳���std::iterator<std::bidirectional_iterator_tag, const T>,
 *	�˱�ǩ������˫����ʵ��������ԡ�
 */
template <typename T, typename C = std::less<T>>
class _avl_iterator : public std::iterator<std::bidirectional_iterator_tag, const T> {
	// ��ͬ����AVL��������ԪȨ�ޡ�
	friend class avl<T, C>;

private:

	// ˽���ֶΣ�_container������Ϊconst avl<T, C>*����ʾ������������������
	const avl<T, C>* _container;

	// ˽���ֶΣ�_value������Ϊconst typename avl<T, C>::_node*����ʾ��������ǰ��ָ��Ľڵ㡣
	const typename avl<T, C>::_node* _value;

	/*
	 *	˽�й�����������ָ��������ָ���ָ��ڵ��ָ�롣
	 */
	_avl_iterator(const avl<T, C>* _cont, const typename avl<T, C>::_node* _node) : _container

	(_cont), _value(_node) {}

public:

	/*
	 *	�����ӿڣ�Ĭ�Ϲ�������
	 *	���ɴ˹���������ĵ�������Ч�ڡ�β�����������
	 */
	_avl_iterator() : _avl_iterator(nullptr, nullptr) {}

	/*
	 *	�����ӿڣ��������캯����������ֵ�����������������
	 *	�������ṩ��Bitwise Copy Semantics�������㱾������Ҫ��
	 *	�������ƶ�������
	 */
	_avl_iterator(const _avl_iterator&) = default;
	_avl_iterator& operator=(const _avl_iterator&) = default;
	~_avl_iterator() = default;

	/*
	 *	�����ӿڣ������������
	 *	����ֵ��const T&���ӵ�������ָλ�������Ԫ�ء�
	 *	����Ч��������������ᵼ�²���ȷ��Ϊ��
	 */
	const T& operator*() const {
		return _value->value;
	}

	/*
	 *	�����ӿڣ���ͷ�������
	 *	����ֵ�����ص�������ָ��λ��Ԫ�صĵ�ַ��
	 *	���غ��ָ�뽫�ᱻ��������һ��ʩ�Ӽ�ͷ�����������
	 *	ʹ����Ϊ�������ڽ�ָ�롣
	 */
	const T* operator->() const {
		return &(_value->value);
	}
	_avl_iterator& operator++();
	_avl_iterator operator++(int);
	_avl_iterator& operator--();
	_avl_iterator operator--(int);

	/*
	 *	�����ӿڣ�������������
	 *	������
	 *	_lhs�����Ƚϵĵ�������
	 *	_rhs�����Ƚϵĵ�������
	 *	����ֵ��bool��ָʾ�����������Ƿ���ȡ�
	 */
	friend bool operator==(const _avl_iterator& _lhs, const _avl_iterator& _rhs) {
		// �����ڲ�ͬ�����Ľڵ�һ������ͬ��
		// �������Ƕ���β���������
		// ��β�������������ͬ�ģ�
		// ������ѭ������ֹ�������㷨���������䡣
		return _lhs._value == _rhs._value;
	}
};

/*
 *	�����ӿڣ�_avl_iterator<T, C>::operator++()/ǰ�õ����������
 *	�ýӿھ����˵������ڶ�����������ƶ���
 *	operator++��������������ķ�ʽ��
 *	���������ƶ�����ǰ�ڵ�ĺ�̡�
 *	�����߱��뱣֤��������Ч�����򽫳��ֲ���ȷ��Ϊ��
 */
template <typename T, typename C>
_avl_iterator<T, C>& _avl_iterator<T, C>::operator++() {
	// ���1����ǰ�ڵ���Ҷ�ڵ㣬����Ҫ���ϱ�����
	if (!_value->leftChild && !_value->rightChild) {
		// ��¼�丸�ڵ㡣
		const typename avl<T, C>::_node* _parent = _value->parent;
		/*
		 *	����Ϊ���ڵ���������������ĺ���Ǹ��ڵ㡣
		 *	����Ϊ���ڵ���������������丸�ڵ����ϻ��ݣ�
		 *	ֱ���˽ڵ����丸�ڵ����ڵ㣬
		 *	��˽ڵ������ĺ�̡�
		 *	���ڶ�����ĩβ��Ԫ�أ�����ָ�����ĵ�������ʹ�����������Ϊβ���������
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
		// ���2����ǰ�ڵ㲻��Ҷ�ڵ㣬����Ҫ���±�����
		// ��¼����������
		const typename avl<T, C>::_node* _p = _value->rightChild;
		/*
		 *	������������Ϊ�գ���ǰ�ڵ�������Ϊ�������������Ľڵ㣬
		 *	��Ҫ���ϻ��ݡ�
		 *	�������������ǿգ��������������������������������
		 *	ֱ����������Ϊ�գ����ݶ���������Ķ��壬�˽ڵ��ǵ�ǰ�ڵ�ĺ�̡�
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
 *	�����ӿڣ�_avl_iterator<T, C>::operator++(int)/���õ����������
 *	�Ե�ǰ���������е����������޸�֮ǰ�ĵ�������
 */
template <typename T, typename C>
_avl_iterator<T, C> _avl_iterator<T, C>::operator++(int) {
	_avl_iterator<T, C> _prev = *this;
	++* this;
	return _prev;
}

/*
 *	�����ӿڣ�_avl_iterator<T, C>::operator--()/ǰ�õݼ��������
 *	��ǰ�õ����������Ϊ������������𽫵������ƶ�������ǰ����
 *	��������Ϊβ��������Ұ�����һ����Ч��������
 *	�ò����Ὣ�������ƶ���AVL�������һ���ڵ㡣
 *	��������Ϊβ���������δ����һ����Ч��������
 *	�ò�������������ȷ��Ϊ��
 *	��������Ϊ�׵��������ò����Ὣ��������Ϊβ���������
 */
template <typename T, typename C>
_avl_iterator<T, C>& _avl_iterator<T, C>::operator--() {

	// ��valueΪ�գ����Զ�ȡ�����������ĸ��ڵ㡣
	if (!_value) {

		// Note������������δ����һ����Ч�����������������������ȷ��Ϊ��
		auto _p = _container->_root;

		// ������Ϊ�գ���ִ���κβ�����
		if (!_p)
			return *this;

		// ��������Ϊ�գ��򽫵������ƶ����������������һ���ڵ㡣
		else
			while (_p->rightChild)
				_p = _p->rightChild;
		_value = _p;
		return *this;
	}

	// ���²���Ϊoperator++()�еľ��������
	// �����operator++()�е�Դ���ע���Ķ���
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
 *	�����ӿڣ�_avl_iterator<T, C>::operator--(int)/���õݼ��������
 *	�Ե�ǰ������ִ�еݼ������������޸�֮ǰ�ĵ�������
 */
template <typename T, typename C>
_avl_iterator<T, C> _avl_iterator<T, C>::operator--(int) {
	_avl_iterator<T, C> _prev = *this;
	--* this;
	return _prev;
}

/*
 *	�����ӿڣ��������������
 *	������
 * 	_lhs�����Ƚϵĵ�������
 *	_rhs�����Ƚϵĵ�������
 *	����ֵ��bool��ָʾ�����������Ƿ񲻵ȡ�
 */
template <typename T, typename C>
bool operator!=(const _avl_iterator<T, C>& _lhs, const _avl_iterator<T, C>& _rhs) {

	//�ò����򵥵���operator==�������������ࡣ
	return !(_lhs == _rhs);
}

/*
 *	����������_swap_node��
 *	���ڽ��������ڵ���AVL���е�λ�á�
 *	������
 *	_n���������Ľڵ㡣
 *	_prev���������Ľڵ㡣��������������ȷʵûʲô���壬�����Ҳ������:-)��
 */
template <typename T, typename C>
void avl<T, C>::_swap_node(typename avl<T, C>::_node* _n, typename avl<T, C>::_node* _prev) {

	// lambda���ʽ���ȳ�������������д�ĺ���������
	auto _swap = [](typename avl<T, C>::_node*& _lhs, typename avl<T, C>::_node*& _rhs) {
		auto _temp = _lhs;
		_lhs = _rhs;
		_rhs = _temp;
	};

	/*
	 *	1.�������ǵĸ��ס�
	 *	ͬʱ���ø��׵��ӽڵ�ָ�롣
	 *	�Ը��ڵ�������⴦��ͬʱҪ����this->_rootָ���ֵ��
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

	// 2.�������Ǻ����ǵĶ��ӵ����ӡ�
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

	// 3.�������ǵ�ƽ�����ӡ�
	std::ptrdiff_t _n_factor = _n->factor;
	_n->factor = _prev->factor;
	_prev->factor = _n_factor;
}

/*
 *	����������_insert_node��
 *	��������ָ���ڵ�Ϊ���ڵ�������в���һ��Ԫ�ء�
 *	������
 *  _n��ָ���Ľڵ㡣
 *  _val��������AVL����ֵ��
 */
template <typename T, typename C>
void avl<T, C>::_insert_node(typename avl<T, C>::_node* _n, const T& _val) {

	// ���AVL��Ϊ�գ���ôֱ���ڸ��ڵ��Ϲ��졣
	if (!_root) {
		_root = new typename avl<T, C>::_node(_val);
		_size++;
		return;
	}

	// �����ǰ�ڵ�ֵ��������ֵ��ȣ���ôֱ�ӷ��ء�
	auto _result = _compare(_val, _n->value);
	if (!_result)
		return;

	// ����������µ����̡�
	else {

		// ���������ֵ��С�ڡ���ǰ�ڵ�ֵ��������뵱ǰ�ڵ����������
		if (_result == -1) {

			// ���_n����������Ϊ��...
			if (_n->leftChild) {

				// ��¼������ԭ����ƽ�����ӡ�
				auto _prev_factor = _n->leftChild->factor;

				// �ݹ�ؽ�_val������������
				_insert_node(_n->leftChild, _val);

				// ��¼ԭ������������Ȼ���ټ�����������������
				// �Ƿ����AVL���Ķ��塣��������AVL���Ķ����������Ӧ�ĵ�����
				auto _prev_left = _n->leftChild;

				// ��������AVL���Ķ��壬��δ���е����������漰��ƽ�����ӵı仯��
				// ���������ļ�顣
				if (_prev_left == _check_tree(_prev_left)) {

					/*
					 *	�������ƽ�����ӵ���0���򾭹�������������ĸ߶�δ�����仯��
					 *	������_n��ƽ�����ӡ�
					 *	�������ƽ�������ڲ������ǰ��δ�����仯�������ĸ߶�Ҳδ�����仯��
					 *	���ı�_n��ƽ�����ӡ�
					 *	��������£�_n���������߶�����1����ƽ�����Ӽ�1��
					 */
					if (_prev_left->factor != 0 && _prev_left->factor !=

						_prev_factor)
						_n->factor++;
				}
				else

					/*	������_check_node��ת���ƽ�����ӵĵ�����
					 *	_check_nodeĬ����ת�Ժ�Ը��ڵ��ƽ�����ӽ��е�����
					 *	����������У���Ϊ_prev_left��_n����������
					 *	�ڵ�����ʱ��_n��ƽ�����ӻᱻ��1��
					 *	����ʵ_prev_left�ĸ߶Ȳ�δ�����仯������ڵ�ʹ�߶ȼ�1��
					 *	��ת����ʹ�߶ȼ�1����
					 *	�����ڴ˴����貹����
					 */
					_n->factor++;
			}
			else {

				/*
				 *	���������Ϊ�գ�ֱ�Ӷ����Ӿͺ��ˡ�
				 *	��AVL���Ķ����֪����֮��һ������AVL���Ķ��壬
				 *	���Բ��ü�顣
				 */
				_n->leftChild = new typename avl<T, C>::_node(_val);
				_n->leftChild->parent = _n;
				_size++;
				_n->factor++;
			}
		}
		else {

			/*
			 *	�ݹ�������������������������Ϊ���������
			 *	������Ϸ���Դ���ע���Ķ���
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
 *	����������_find_node��
 *	������
 *	_n��������Ϊconst typename avl<T, C>::_node*�������￪ʼ���ҡ�
 *	_val�������ҵ�Ԫ�ء�
 *	����ֵ��typename avl<T, C>::_node*����ֵ�����������
 *	nullptr��δ�ҵ����ֵ��
 *	�ǿգ�����ֵ��ʾ�洢���ֵ�Ľڵ㡣
 */
template <typename T, typename C>
const typename avl<T, C>::_node* avl<T, C>::_find_node(const typename avl<T, C>::_node* _n, const T& _val) const {

	// ��_nΪ�գ�ֱ�ӷ��ؿ�ָ�뼴�ɡ�
	// ��_n��_val����ȡ�������_n��ʾ�ҵ���
	
	if (!_n)
		return nullptr;
	int _result = _compare(_val, _n->value);
	if (!_result)
		return _n;
	else {

		// ��_val��С�ڡ�_n��ʾ�Ľڵ��ֵ������_n�������������ҡ�
		if (_result == -1)
			return _find_node(_n->leftChild, _val);

		// ������_n�������������ҡ�
		else
			return _find_node(_n->rightChild, _val);
	}
}

/*
 *	����������_remove_node��
 *	����һ��ָ���Ľڵ㣬������ɾ����
 *	������
 *	_n����ɾ���Ľڵ㡣
 */
template <typename T, typename C>
void avl<T, C>::_remove_node(typename avl<T, C>::_node* _n) {

	// ��_n��Ҷ�ڵ�...
	if (!_n->leftChild && !_n->rightChild) {

		// ���������丸�ڵ�������������������������ĸ��ڵ��ƽ�����ӡ�
		auto _prev_parent = _n->parent;
		if (_is_left_child(_prev_parent, _n)) {
			_prev_parent->leftChild = nullptr;
			_prev_parent->factor--;
		}
		else {
			_prev_parent->rightChild = nullptr;
			_prev_parent->factor++;
		}

		// ����ռ�õ��ڴ�黹��ϵͳ��
		delete _n;

		// ��Ϊ�Ƴ���һ���ڵ㣬����AVL���Ĵ�С��1��
		_size--;

		// �ӱ�ɾ���ڵ�ĸ��ڵ������ڵ��飬���ҽ��е�����
		for (auto _p = _prev_parent; _p; _p = _p->parent) {
			_p = _check_tree(_p);
		}
	}
	else {
		// _n����Ҷ�ڵ㡣
		// ���_n��ǰ����
		auto _prev = const_cast<typename avl<T, C>::_node*>((--(_avl_iterator<T, C>(this, _n)))._value);

		// ��_n������ǰ��������
		_swap_node(_n, _prev);

		// �ٳ���ɾ��_n�����ջ�ת��Ϊɾ��Ҷ�ڵ�������
		_remove_node(_n);
	}
}

/*
 *	����������_clear_tree��
 *	ɾ����_currentΪ���ڵ��������
 *	������
 *	_current������Ϊtypename avl<T, C>::_node*����ɾ����������
 */
template <typename T, typename C>
void avl<T, C>::_clear_tree(typename avl<T, C>::_node* _current) {

	// ��_currentΪ�գ�����ɾ����
	if (!_current)
		return;

	// ��������������ķ�ʽɾ����������
	_clear_tree(_current->leftChild);
	_clear_tree(_current->rightChild);
	delete _current;
}

/*
 *	����������_make_copy��
 *	���������������ݸ�����_dest�С�
 *	������
 *	_dest������Ϊtypename avl<T, C>::_node*&�����Ƶ�Ŀ�ĵء�
 */
template <typename T, typename C>
void avl<T, C>::_make_copy(typename avl<T, C>::_node*& _dest) const {

	// �Ƚ����ڵ�����ݸ�����_dest�С�
	_dest = new typename avl<T, C>::_node(_root->value);
	_dest->factor = _root->factor;

	// �ٵ��ø�������_copy_node����������������������
	_copy_node(_dest, _root);
}

/*
 *	����������_copy_node��
 *	���ƶ�����������������
 *	������
 *	_dest������Ŀ�ĵء�
 *	_src������Դ��
 */
template <typename T, typename C>
void avl<T, C>::_copy_node(typename avl<T, C>::_node* _dest, const typename avl<T, C>::_node* _src) const {

	// ����ָ����Ч�ԡ����ǳ���Ҫ����
	if (!_src || !_dest)
		return;

	// ��src��������������֮��
	if (_src->leftChild) {
		_dest->leftChild = new typename avl<T, C>::_node(_src->leftChild->value);
		_dest->leftChild->parent = _dest;
		_dest->leftChild->factor = _src->leftChild->factor;
		_copy_node(_dest->leftChild, _src->leftChild);
	}

	// ��src��������������֮��
	if (_src->rightChild) {
		_dest->rightChild = new typename avl<T, C>::_node(_src->rightChild->value);
		_dest->rightChild->parent = _dest;
		_dest->rightChild->factor = _src->rightChild->factor;
		_copy_node(_dest->rightChild, _src->rightChild);
	}
}

/*
 *	����������_check_tree��
 *	������_nΪ���ڵ�����Ƿ����AVL���Ķ��塣
 *	����������ִ����Ӧ������
 *	������
 *	_n������Ϊtypename avl<T, C>::_node*���������Ľڵ㡣
 *	����ֵ��typename avl<T, C>::_node*��������Ľڵ㡣
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_check_tree(typename avl<T, C>::_node* _n) {

	// ����_n��ƽ�����ӽ��е�����
	// �������е�std::terminate()��֧��ʾ����AVL���Ķ���������ܵ���ķ�֧��
	switch (_n->factor) {
	case -2: {

		// ��_n��ƽ������Ϊ-2����Ҫ����R-��ת�Ա���AVL�������ʡ�
		auto _prev_r = _n->rightChild;
		auto _prev_rl = _prev_r->leftChild;
		auto _prev_par = _n->parent;

		// ��ת�����������ĸ߶ȼ�ȥ1����˵������ڵ��ƽ�����ӡ�
		// ��_insert_node�����еĵ���Ҳ���ڴ˴��ҵ��𰸡�
		if (_prev_par) {
			if (_prev_par->leftChild == _n)
				_prev_par->factor--;
			else
				_prev_par->factor++;
		}

		// �ٸ���������������ƽ�����ӽ��е�����
		// �����е�ƽ�����ӵ�����Ϊ����ֽ����֤�õ��Ľ����
		// ����������ʿ����Լ�������֤��
		switch (_prev_r->factor) {

			// ������ƽ������Ϊ-1�������R-R��ת��
		case -1:
			_n->factor = 0;
			_prev_r->factor = 0;
			return _rr_rotate(_n);

			// ������ƽ������Ϊ0��������R-L��ת��R-R��ת�������������
			// ��R-R��ת��������С����˴˴�ѡ��R-R��ת��
		case 0:
			_n->factor = -1;
			_prev_r->factor = 1;
			return _rr_rotate(_n);

			// ��������ƽ������Ϊ1�������R-L��ת��
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

		   // ��ƽ�����ӵľ���ֵС�ڵ���1�����õ���������_n���ɡ�
	case -1:
	case 0:
	case 1:
		return _n;

		// ��ƽ�����ӵ���2������Ҫ����L-��ת��
		// �˲�����R-��ת��Ϊ���������������Ϸ�case -2������Ķ���
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
 *	����������_ll_rotate��
 *	��ָ���Ľڵ�ִ��L-L��ת��
 *	������
 *	_n���������Ľڵ㡣
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_ll_rotate(typename avl<T, C>::_node* _n) {

	// �Ķ������Ĵ���ʱ������������ʺ�ֽ���߶��߶�������
	// ��¼_n�ĸ��ڵ����������
	auto _prev_parent = _n->parent;
	auto _prev_left = _n->leftChild;

	// ����_prev_left�ĸ��ڵ�Ϊ_prev_parent��
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

	// ����_n��������Ϊ_prev_left����������
	_n->leftChild = _prev_left->rightChild;
	if (_prev_left->rightChild)
		_prev_left->rightChild->parent = _n;

	// ����_prev_left��������Ϊ_n��
	_n->parent = _prev_left;
	_prev_left->rightChild = _n;

	// ����_prev_left��Ϊ������ɵĽڵ㡣
	return _prev_left;
}

/*
 *	����������_rr_rotate��
 *	��ָ���Ľڵ�ʵ��R-R��ת��
 *	������
 *	_n���������Ľڵ㡣
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_rr_rotate(typename avl<T, C>::_node* _n) {

	// R-R��ת��L-L��ת��Ϊ�����������ο�L-L��תԴ���Ķ���
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
 *	����������_lr_rotate��
 *	��ָ���Ľڵ�ִ��L-R��ת��
 *	������
 *	_n���������Ľڵ㡣
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_lr_rotate(typename avl<T, C>::_node* _n) {

	// ��_n��������ִ��R-R��ת��
	_rr_rotate(_n->leftChild);

	// �ٶ�_nִ��L-L��ת��
	return _ll_rotate(_n);
}


/*
 *	����������_rl_rotate��
 *	��ָ���Ľڵ�ִ��R-L��ת��
 *	������
 *	_n���������Ľڵ㡣
 */
template <typename T, typename C>
typename avl<T, C>::_node* avl<T, C>::_rl_rotate(typename avl<T, C>::_node* _n) {

	// ��_n��������ִ��L-L��ת��
	_ll_rotate(_n->rightChild);

	// �ٶ�_nִ��R-R��ת��
	return _rr_rotate(_n);
}

/*
 *	�����ӿڣ�put��
 *	��һ��ֵ����AVL����
 *	������
 *	_value��������Ϊconst T&���������ֵ��
 */
template <typename T, typename C>
void avl<T, C>::put(const T& _value) {

	// ��_root��ʼ���롣
	_insert_node(_root, _value);

	// _insert_node��δ��_root���к��飬���Ժ����ڴ˴����С�
	_root = _check_tree(_root);
}

/*
 *	�����ӿ�:find��
 *	����һ��ֵ�Ƿ�λ��AVL���С�
 *	������
 *	_value��������Ϊconst T&�������ҵ�ֵ��
 *	����ֵ��bool����ʾ�Ƿ��ҵ���
 */
template <typename T, typename C>
bool avl<T, C>::find(const T& _value) const {

	// ��ʵ�ʹ�������_find_node����_root��ʼ���ҡ�
	// �����ҵ��Ľڵ�Ϊ��ָ�룬��û���ҵ���
	// �����ҵ��Ľڵ�ǿգ����ҵ���
	return static_cast<bool>(_find_node(_root, _value));
}

/*
 *	�����ӿڣ�remove��
 *	ɾ��һ��ֵ��������ֵλ��AVL���С�����
 *	������
 *	_value��������Ϊconst T&����ɾ����ֵ��
 *	����ֵ��
 *	bool��ָʾɾ�������Ƿ�ɹ�ִ�С�
 */
template <typename T, typename C>
bool avl<T, C>::remove(const T& _value) {

	// �Ȳ��ҽڵ��Ƿ������AVL���С�
	auto _loc_node = const_cast<typename avl<T, C>::_node*>(_find_node(_root, _value));

	// ���_loc_node�ǿգ���˽ڵ������AVL���С�
	if (_loc_node) {

		// ����_remove_nodeɾ���ڵ㣬������true��ʾ�ɹ�ɾ����
		_remove_node(_loc_node);
		return true;
	}
	else {

		// δ�ҵ��ڵ㣬����false��ʾɾ��ʧ�ܡ�
		return false;
	}
}

/*
 *	�����ӿڣ�clear()��
 *	���ã����AVL���洢�����нڵ㲢�ͷ��ڴ档
 */
template <typename T, typename C>
void avl<T, C>::clear() {

	// ����_clear_treeɾ����������
	_clear_tree(_root);

	// �ø��ڵ�Ϊnullptr��_sizeΪ0��
	// ��ʱ��AVL���в������κ����ݡ�
	_root = nullptr;
	_size = 0;
}

/*
 *	�����ӿ��壺beginϵ�С�endϵ�С�
 *	����AVL�����׵�������β���������
 */
template <typename T, typename C>
typename avl<T, C>::iterator avl<T, C>::begin() {
	auto _p = _root;

	// �ظ��ڵ����·�����������ֱ����������������
	// ��ʱ�Ľڵ����AVL�����׽ڵ㡣
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
 *	�����ӿ��壺rbegin��rendϵ�С�
 *	���ش�AVL���ķ����������
 *	����ɲμ�reverse_iterator�����˵����
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
