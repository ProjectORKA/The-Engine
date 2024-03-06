#pragma once#include "Basics.hpp"//#include <vector>//#include <numeric>//#include <algorithm>//#include <exception>//#include <functional>template <class PointT> class KDTree23D{public:	KDTree2() : root_(nullptr) {};	KDTree2(const Vector<PointT>& points) : root_(nullptr)	{		build(points);	}	~KDTree2()	{		clear();	}	void build(const Vector<PointT>& points)	{		clear();		points_ = points;		Vector<Int> indices(points.size());		std::iota(std::begin(indices), std::end(indices), 0);		root_ = buildRecursive(indices.data(), (Int)points.size(), 0);	}	void clear()	{		clearRecursive(root_);		root_ = nullptr;		points_.clear();	}	Bool validate() const	{		try		{			validateRecursive(root_, 0);		}		catch(const Exception&)		{			return false;		}		return true;	}	Int nnSearch(const PointT& query, Double* minDist = nullptr) const	{		Int    guess;		Double _minDist = std::numeric_limits<Double>::max();		nnSearchRecursive(query, root_, &guess, &_minDist);		if(minDist) *minDist = _minDist;		return guess;	}	Vector<Int> knnSearch(const PointT& query, Int k) const	{		KnnQueue queue(k);		knnSearchRecursive(query, root_, queue, k);		Vector<Int> indices(queue.size());		for(SizeT i = 0; i < queue.size(); i++) indices[i] = queue[i].second;		return indices;	}	Vector<Int> radiusSearch(const PointT& query, Double radius) const	{		Vector<Int> indices;		radiusSearchRecursive(query, root_, indices, radius);		return indices;	}private:	struct Node	{		Int   idx; //!< index to the original point		Node* next[2]; //!< pointers to the child nodes		Int   axis; //!< dimension's axis		Node() : idx(-1), axis(-1)		{			next[0] = next[1] = nullptr;		}	};	class Exception : public std::exception	{		using std::exception::exception;	};	template <class T, class Compare = std::less<T>> class BoundedPriorityQueue	{	public:		BoundedPriorityQueue() = delete;		BoundedPriorityQueue(SizeT bound) : bound_(bound)		{			elements_.reserve(bound + 1);		};		void push(const T& val)		{			auto it = std::find_if(std::begin(elements_), std::end(elements_), [&](const T& element)			{				return Compare()(val, element);			});			elements_.insert(it, val);			if(elements_.size() > bound_) elements_.resize(bound_);		}		const T& back() const		{			return elements_.back();		};		const T& operator[](SizeT index) const		{			return elements_[index];		}		SizeT size() const		{			return elements_.size();		}	private:		SizeT     bound_;		Vector<T> elements_;	};	using KnnQueue = BoundedPriorityQueue<std::pair<Double, Int>>;	Node* buildRecursive(Int* indices, const Int npoints, Int depth)	{		if(npoints <= 0) return nullptr;		const Int axis = depth % PointT::DIM;		const Int mid  = (npoints - 1) / 2;		std::nth_element(indices, indices + mid, indices + npoints, [&](Int lhs, Int rhs)		{			return points_[lhs][axis] < points_[rhs][axis];		});		Node* node = new Node();		node->idx  = indices[mid];		node->axis = axis;		node->next[0] = buildRecursive(indices, mid, depth + 1);		node->next[1] = buildRecursive(indices + mid + 1, npoints - mid - 1, depth + 1);		return node;	}	void clearRecursive(Node* node)	{		if(node == nullptr) return;		if(node->next[0]) clearRecursive(node->next[0]);		if(node->next[1]) clearRecursive(node->next[1]);		delete node;	}	void validateRecursive(const Node* node, Int depth) const	{		if(node == nullptr) return;		const Int   axis  = node->axis;		const Node* node0 = node->next[0];		const Node* node1 = node->next[1];		if(node0 && node1)		{			if(points_[node->idx][axis] < points_[node0->idx][axis]) throw Exception();			if(points_[node->idx][axis] > points_[node1->idx][axis]) throw Exception();		}		if(node0) validateRecursive(node0, depth + 1);		if(node1) validateRecursive(node1, depth + 1);	}	static Double distance(const PointT& p, const PointT& q)	{		Double dist = 0;		for(SizeT i = 0; i < PointT::DIM; i++) dist += (p[i] - q[i]) * (p[i] - q[i]);		return sqrt(dist);	}	void nnSearchRecursive(const PointT& query, const Node* node, Int* guess, Double* minDist) const	{		if(node == nullptr) return;		const PointT& train = points_[node->idx];		const Double dist = distance(query, train);		if(dist < *minDist)		{			*minDist = dist;			*guess   = node->idx;		}		const Int axis = node->axis;		const Int dir  = query[axis] < train[axis] ? 0 : 1;		nnSearchRecursive(query, node->next[dir], guess, minDist);		const Double diff = fabs(query[axis] - train[axis]);		if(diff < *minDist) nnSearchRecursive(query, node->next[!dir], guess, minDist);	}	void knnSearchRecursive(const PointT& query, const Node* node, KnnQueue& queue, Int k) const	{		if(node == nullptr) return;		const PointT& train = points_[node->idx];		const Double dist = distance(query, train);		queue.push(std::make_pair(dist, node->idx));		const Int axis = node->axis;		const Int dir  = query[axis] < train[axis] ? 0 : 1;		knnSearchRecursive(query, node->next[dir], queue, k);		const Double diff = fabs(query[axis] - train[axis]);		if((Int)queue.size() < k || diff < queue.back().first) knnSearchRecursive(query, node->next[!dir], queue, k);	}	void radiusSearchRecursive(const PointT& query, const Node* node, Vector<Int>& indices, Double radius) const	{		if(node == nullptr) return;		const PointT& train = points_[node->idx];		const Double dist = distance(query, train);		if(dist < radius) indices.push_back(node->idx);		const Int axis = node->axis;		const Int dir  = query[axis] < train[axis] ? 0 : 1;		radiusSearchRecursive(query, node->next[dir], indices, radius);		const Double diff = fabs(query[axis] - train[axis]);		if(diff < radius) radiusSearchRecursive(query, node->next[!dir], indices, radius);	}	Node*          root_;	Vector<PointT> points_;};