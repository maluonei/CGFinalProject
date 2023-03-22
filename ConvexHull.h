#pragma once
#ifndef CONVEXHULL_H
#define CONVEXHULL_H
//#include <iostream>
#include "vec2.h"
#include <vector>
#include <stack>
//#include "list.h"
#include <algorithm>
#include <time.h>
#include <random>
using namespace std;

vector<point2> getConvexHull_n4(vector<point2>& points) { //n_4时间复杂度算法，对每个点判断，若其不在任一个三角形内部，则为凸包上一个点
	vector<point2> convexHull;
	int length = points.size();

	if (points.size() <= 3) {
		copy(points.begin(), points.end(), convexHull.rbegin());
		return convexHull;
	}

	for (int i = 0; i < length; i++) {
		//cout << "calculating points " << i << "\r";
		bool flag = false;
		for (int j = 0; j < length; j++) {
			if (i == j) continue;
			for (int k = 0; k < length; k++) {
				if (k == j || k == i) continue;;
				for (int l = 0; l < length; l++) {
					if (l == i || l == j || l == k) continue;
					if (InTriangleTest(points[j], points[k], points[l], points[i])) {
						flag = true;
						goto End;
					}
				}
			}
		}
	End:
		if (!flag) convexHull.push_back(points[i]);
	}
	return convexHull;
}

vector<point2> getConvexHull_n3(vector<point2>& points) { //n_3复杂度算法，对每条边，判断其余点是否全在其的一边，若是，则该边的两个点在凸包上
	vector<point2> convexHull;
	int length = points.size();

	if (points.size() <= 3) {
		copy(points.begin(), points.end(), convexHull.rbegin());
		return convexHull;
	}

	for (int i = 0; i < length; i++) {
		//cout << "calculating points " << i << "\r";
		for (int j = 0; j < length; j++) {
			if (i == j) continue;
			bool flag = false;
			for (int k = 0; k < length; k++) {
				if (k == i || k == j) continue;
				if (!InLeftTest(points[i], points[j], points[k])) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				convexHull.push_back(points[i]);
				break;
			}
		}
	}
	return convexHull;
}
/*
vector<point2> getConvexHull_n2_1(vector<point2>& points) { //n_2算法1， 每次选择一个点，判断其是否应该加入当前凸包
	myList::list<point2> convexHullList;
	vector<point2> convexHull;
	int length = points.size();

	if (points.size() <= 3) {
		copy(points.begin(), points.end(), convexHull.rbegin());
		return convexHull;
	}
	sort(points.begin(), points.end());

	if (!InLeftTest(points[0], points[1], points[2])) {
		std::swap(points[1], points[2]);
	}

	for (int i = 0; i < 3; i++) {
		convexHullList.push_back(new myList::Node<point2>(points[i]));
	}

	for (int i = 3; i < length; i++) {
		//cout << "i:" << i << "\r";
		myList::Node<point2>* s = nullptr, * t = nullptr;
		myList::Node<point2>* current = convexHullList.first;
		do
		{
			current = current->next;
			bool b1 = InLeftTest(points[i], current->element, current->previous->element);
			bool b2 = InLeftTest(points[i], current->element, current->next->element);
			if (b1 && b2) s = current;
			if (!b1 && !b2) t = current;
		} while (current != convexHullList.last);

		if (s == nullptr && t == nullptr) continue;
		myList::Node<point2>* node = new myList::Node<point2>(points[i]);

		if (i > 35730) {
			cout << endl;
			current = convexHullList.first;
			do
			{
				current = current->next;
				cout << current->element << endl;
			} while (current != convexHullList.last);
			cout << "node:" << node->element << endl;
			cout << "t:" << t->element << endl;
			cout << "s:" << s->element << endl;
		}

		convexHullList.insert(t, s, node);
		convexHullList.setLast();
	}

	myList::Node<point2>* current = convexHullList.first;
	do
	{
		current = current->next;
		convexHull.push_back(current->element);
	} while (current != convexHullList.last);
	//cout << "last->next" << convexHullList.last->next->element << endl;
	return convexHull;
}
*/
int LowestthenLeftmost(vector<point2>& points) {
	int target = 0;
	int length = points.size();
	for (int i = 1; i < length; i++) {
		if (points[i].y < points[target].y ||
			(points[i].y == points[target].y && points[i].x < points[target].x)) {
			target = i;
		}
	}
	return target;
}

//Verified
vector<point2> getConvexHull_n2_2(vector<point2>& points) {
	vector<point2> convexHull;
	int length = points.size();

	if (length <= 3) {
		for (int i = 0; i < length; i++) {
			convexHull.push_back(points[i]);
		}
		return convexHull;
	}

	bool* inConvexHull = new bool[length];
	fill(inConvexHull, inConvexHull + length, false);

	int ltl = LowestthenLeftmost(points);
	int k = ltl;

	do {
		inConvexHull[k] = true;
		int s = -1;
		for (int i = 0; i < length; i++) {
			if (i != k && i != s &&
				(s == -1 || !InLeftTest(points[k], points[s], points[i])))

				s = i;
		}
		k = s;
	} while (ltl != k);
	for (int i = 0; i < length; i++) {
		if (inConvexHull[i]) {
			convexHull.push_back(points[i]);
		}
	}
	return convexHull;
}

void printstack(stack<point2> s) {
	while (!s.empty()) {
		cout << (s.top()) << endl;
		s.pop();
	}
}

vec2 vltl;
bool comp2(vec2 v1, vec2 v2) {
	return InLeftTest_noEqual(vltl, v1, v2);
}

vector<point2> getConvexHull_nlgn(vector<point2> points) {
	vector<point2> convexHull;
	int length = points.size();

	if (points.size() <= 3) {
		copy(points.begin(), points.end(), convexHull.rbegin());
		return convexHull;
	}

	int ltl = LowestthenLeftmost(points);
	//cout << "ltl:" << points[ltl] << endl;
	//point2 pLTL = points[ltl];
	//vector<vec2> convexHullVec;
	//for (int i = 0; i < length; i++) {
	//	if (i != ltl) {
	//		point2 ptemp = points[i];
	//		ptemp.setAngle(pLTL, point2(1, 0));
	//		convexHullVec.push_back(ptemp);
	//	}
	//}
	vltl = points[ltl];
	std::swap(points[0], points[ltl]);
	//clock_t start, ends;
	//start = clock();
	sort(points.begin() + 1, points.end(), comp2);
	//for (int i = 0; i < points.size(); i++) {
	//	cout << points[i] << endl;
	//}
	//cout << points[0] << endl;
	//ends = clock();
	//cout << ends - start << "ms\n";

	stack<vec2> S, T;
	S.push(points[0]);
	S.push(points[1]);

	length = points.size();
	for (int i = length - 1; i > 1; i--) {
		//cout << convexHullVec[i].angle << endl;
		T.push(points[i]);
	}

	while (!T.empty()) {
		//cout << S.size() << ";" << endl;
		//cout << endl;
		//printstack(S);

		point2 s1 = S.top();
		S.pop();
		point2 s0 = S.top();
		point2 t0 = T.top();
		//cout << "s1:" << s1 << endl;
		//cout << "s0:" << s0 << endl;
		//cout << "t0:" << t0 << endl;

		if (!InLeftTest_noEqual(s1, s0, t0)) {
			S.push(s1);
			S.push(t0);
			T.pop();
		}
	}

	while (!S.empty()) {
		point2 s = S.top();
		S.pop();
		convexHull.push_back(point2(s.x, s.y));
	}
	return convexHull;
}

void Conquer(vector<point2>& leftConvex, vector<point2>& rightConvex, vector<point2>& finalConvex) {
	int leftConvexRightest = 0;
	int rightConvexLeftest = 0;
	int leftSize = leftConvex.size();
	int rightSize = rightConvex.size();
	while (true) {
		int before = (rightConvexLeftest - 1 + rightSize) % rightSize;
		int after = (rightConvexLeftest + 1) % rightSize;
		if (rightConvex[rightConvexLeftest].x <= rightConvex[before].x && rightConvex[rightConvexLeftest].x <= rightConvex[after].x) {
			break;
		}
		rightConvexLeftest++;
	}

	int leftUp = leftConvexRightest, leftDown = leftConvexRightest, rightUp = rightConvexLeftest, rightDown = rightConvexLeftest;
	//zigzagUp
	while (true) {
		bool leftFlag = false, rightFlag = false;
		int rightNext = (rightUp - 1 + rightSize) % rightSize;
		int leftNext = (leftUp + 1) % leftSize;
		while (true) {
			rightNext = (rightUp - 1 + rightSize) % rightSize;
			if (leftConvex[leftUp].x == rightConvex[rightUp].x && rightConvex[rightUp].x == rightConvex[rightNext].x) {
				if (rightConvex[rightNext].y < rightConvex[rightUp].y) break;
			}
			if (InLeftTest(leftConvex[leftUp], rightConvex[rightUp], rightConvex[rightNext])) {
				rightFlag = true;
				rightUp = rightNext;
			}
			else {
				break;
			}
		}
		while (true) {
			leftNext = (leftUp + 1) % leftSize;
			if (leftConvex[leftUp].x == rightConvex[rightUp].x && leftConvex[leftUp].x == leftConvex[leftNext].x) {
				if (leftConvex[leftNext].y < leftConvex[leftDown].y) break;
			}
			if (!InLeftTest_noEqual(rightConvex[rightUp], leftConvex[leftUp], leftConvex[leftNext])) {
				leftFlag = true;
				leftUp = leftNext;
			}
			else {
				break;
			}
		}
		if (!leftFlag && !rightFlag) break;
	}

	//zigzagDown
	while (true) {
		bool leftFlag = false, rightFlag = false;
		int rightNext = (rightDown + 1) % rightSize;
		int leftNext = (leftDown - 1 + leftSize) % leftSize;
		while (true) {
			rightNext = (rightDown + 1) % rightSize;
			if (leftConvex[leftDown].x == rightConvex[rightDown].x && rightConvex[rightDown].x == rightConvex[rightNext].x) {
				if (rightConvex[rightNext].y > rightConvex[rightDown].y) break;
			}
			if (!InLeftTest_noEqual(leftConvex[leftDown], rightConvex[rightDown], rightConvex[rightNext])) {
				rightFlag = true;
				rightDown = rightNext;
			}
			else {
				break;
			}
		}
		while (true) {
			leftNext = (leftDown - 1 + leftSize) % leftSize;
			if (leftConvex[leftDown].x == rightConvex[rightDown].x && leftConvex[leftDown].x == leftConvex[leftNext].x) {
				if (leftConvex[leftNext].y > leftConvex[leftDown].y) break;
			}
			if (InLeftTest(rightConvex[rightDown], leftConvex[leftDown], leftConvex[leftNext])) {
				leftFlag = true;
				leftDown = leftNext;
			}
			else {
				break;
			}
		}
		if (!leftFlag && !rightFlag) break;
	}

	if (leftDown == 0) leftDown = leftSize;
	if (rightDown == 0) rightDown = rightSize;
	//construct finalConvex
	for (int i = 0; i <= rightUp; i++) {
		finalConvex.push_back(rightConvex[i]);
	}
	for (int i = leftUp; i <= leftDown; i++) {
		finalConvex.push_back(leftConvex[i % leftSize]);
	}
	for (int i = rightDown; i < rightSize; i++) {
		finalConvex.push_back(rightConvex[i]);
	}
}

bool linear = false;
void DevideAndConquer(vector<point2>& points, vector<point2>& convexHull, int start, int end) {
	int mid = (start + end) / 2;
	int pointNum = end - start + 1;

	if (points[start].x == 8078 || points[end].x == 8078) {
		int i = 0;
	}

	if (pointNum == 1) {
		convexHull.push_back(points[end]);
	}
	else if (pointNum == 2) {
		convexHull.push_back(points[end]);
		convexHull.push_back(points[start]);
	}
	else if (pointNum == 3) {
		convexHull.push_back(points[end]);
		if (InLeftTest(points[end], points[start], points[mid])) {
			convexHull.push_back(points[start]);
			convexHull.push_back(points[mid]);
		}
		else {
			convexHull.push_back(points[mid]);
			convexHull.push_back(points[start]);
		}
	}
	else if (pointNum > 3) {
		if (points[start].x == points[end].x) {
			if (linear) {
				for (int j = end; j >= start; j--) {
					convexHull.push_back(points[j]);
				}
			}
			else {
				convexHull.push_back(points[end]);
				convexHull.push_back(points[start]);
			}
			return;
		}

		vector<point2> leftConvex, rightConvex;

		if (points[start].x == points[mid].x && mid - start >= 2) {
			while (points[start].x == points[mid].x) {
				mid++;
			}
			if (mid + 1 == end) {
				if (linear) {
					for (int i = mid - 1; i >= start; i--) {
						leftConvex.push_back(points[i]);
					}
				}
				else {  //start, mid-1, mid, end -->  start---mid-1的x坐标相同，且右边只剩mid，end
					leftConvex.push_back(points[mid - 1]);
					leftConvex.push_back(points[start]);
				}
				rightConvex.push_back(points[end]);
				rightConvex.push_back(points[mid]);
				Conquer(leftConvex, rightConvex, convexHull);
				return;
			}
			if (mid == end) {
				if (linear) {
					for (int j = end; j >= start; j--) {
						convexHull.push_back(points[j]);
					}
				}
				else {
					convexHull.push_back(points[end]);
					convexHull.push_back(points[end - 1]);
					convexHull.push_back(points[start]);
				}
				return;
			}

			if (linear) {
				for (int j = mid; j >= start; j--) {
					leftConvex.push_back(points[j]);
				}
			}
			else {
				leftConvex.push_back(points[mid]);
				leftConvex.push_back(points[mid - 1]);
				leftConvex.push_back(points[start]);
			}
			DevideAndConquer(points, rightConvex, mid + 1, end);
		}
		else if (points[mid + 1].x == points[end].x && end - mid >= 3) {
			while (points[mid + 1].x == points[end].x) {
				mid--;
			}
			if (mid == start) {
				leftConvex.push_back(points[mid]);
				leftConvex.push_back(points[start]);
				rightConvex.push_back(points[end]);
				if (linear) {
					for (int i = mid + 1; i < end; i++) {
						rightConvex.push_back(points[i]);
					}
				}
				else { //start, mid, mid+1, end ，mid+1......end的x坐标相同，且左边只剩start，mid
					rightConvex.push_back(points[mid + 1]);
				}
				Conquer(leftConvex, rightConvex, convexHull);
				return;
			}
			if (mid + 1 == start) {
				//convexHull.push_back(points[start + 1]);
				//convexHull.push_back(points[end]);
				//convexHull.push_back(points[start]);
				//convexHull.push_back(points[start + 1]);
				if (linear) {
					convexHull.push_back(points[end]);
					for (int j = start; j < end; j++) {
						convexHull.push_back(points[j]);
					}
				}
				else {
					convexHull.push_back(points[end]);
					convexHull.push_back(points[start]);
					convexHull.push_back(points[start + 1]);
				}

				return;
			}

			if (linear) {
				rightConvex.push_back(points[end]);
				for (int j = mid + 1; j < end; j++) {
					rightConvex.push_back(points[j]);
				}
			}
			else {
				rightConvex.push_back(points[end]);
				rightConvex.push_back(points[mid + 1]);
				rightConvex.push_back(points[mid + 2]);
			}

			DevideAndConquer(points, leftConvex, start, mid);
		}
		else {
			DevideAndConquer(points, leftConvex, start, mid);
			DevideAndConquer(points, rightConvex, mid + 1, end);
		}
		Conquer(leftConvex, rightConvex, convexHull);
	}
}

//Verified
vector<point2> zigzag(vector<point2>& points) {
	vector<point2> convexHull;
	int start = 0;
	int end = points.size() - 1;
	sort(points.begin(), points.end(), ComparatorXmin);
	DevideAndConquer(points, convexHull, start, end);
	return convexHull;
}

#endif
