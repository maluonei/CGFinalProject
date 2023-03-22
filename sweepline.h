#pragma once
#ifndef SWEEPLINE_H
#define SWEEPLINE_H

#include <vector>
#include "vec2.h"
#include <algorithm>

using namespace std;

struct trapezoid {
	int l;
	int r;
	int helper;
	trapezoid() {}
	trapezoid(int _l, int _r, int _helper) :l(_l), r(_r), helper(_helper) {}
};

void InsertIntoTrapesM(vector<trapezoid>& tp, int x, int length) {
	int before = (x + length - 1) % length;
	int after = (x + 1) % length;
	tp.push_back(trapezoid(after, before, x));
}

void MergeTrapseM(vector<trapezoid>& tp, int x) {
	int left, right;
	int tplength = tp.size();
	for (int i = 0; i < tplength; i++) {
		//cout << tp[i].l << "  " << tp[i].r << "  " << tp[i].helper << endl;
		if (tp[i].l == x) right = i;
		if (tp[i].r == x) left = i;
	}
	trapezoid newt(tp[left].l, tp[right].r, x);
	if (left > right) std::swap(left, right);
	tp.erase(tp.begin() + left);
	tp.erase(tp.begin() + right - 1);
	tp.push_back(newt);
}

void DealLeftVertexM(vector<trapezoid>& tp, int x, int length) {
	int tplength = tp.size();
	int left = -1;
	int after = (x + 1) % length;
	for (int i = 0; i < tplength; i++) {
		if (tp[i].l == x) {
			left = i;
			break;
		}
	}
	trapezoid newt(after, tp[left].r, x);
	tp.erase(tp.begin() + left);
	tp.push_back(newt);
}

void DealRightVertexM(vector<trapezoid>& tp, int x, int length) {
	int tplength = tp.size();
	int right = -1;
	int before = (x + length - 1) % length;
	for (int i = 0; i < tplength; i++) {
		if (tp[i].r == x) {
			right = i;
			break;
		}
	}
	trapezoid newt(tp[right].l, before, x);
	tp.erase(tp.begin() + right);
	tp.push_back(newt);
}

void SplitTrapesM(vector<trapezoid>& tp, int x, vector<vec2>& points, vector<pair<int, int>>& segs) {
	int tplength = tp.size();
	int length = points.size();
	int left = 0, right = 0, index = 0;
	for (int i = 0; i < tplength; i++) {
		if (points[tp[i].l].x < points[x].x && points[tp[i].r].x > points[x].x) {
			left = tp[i].l;
			right = tp[i].r;
			segs.push_back(make_pair(tp[i].helper, x));
			segs.push_back(make_pair(x, tp[i].helper));
			index = i;
			break;
		}
	}
	trapezoid lt(left, (x + length - 1) % length, x);
	trapezoid rt((x + 1) % length, right, x);
	tp.erase(tp.begin() + index);
	tp.push_back(lt);
	tp.push_back(rt);
}

bool comp3(int x, int y) {
	return x > y;
}

void DeleteFromTrapesM(vector<trapezoid>& tp, int x) {
	vector<int> delets;
	int tplength = tp.size();
	for (int i = 0; i < tplength; i++) {
		if (tp[i].l == x) delets.push_back(i);
	}
	sort(delets.begin(), delets.end(), comp3);
	for (int i = 0; i < delets.size(); i++) {
		tp.erase(tp.begin() + delets[i]);
	}
}

int GetVertexType(vector<vec2>& points, int x) {
	int length = points.size();
	int before = (x + length - 1) % length;
	int after = (x + 1) % length;
	if (points[x].y > points[before].y && points[x].y > points[after].y && points[x].x < points[before].x && points[x].x > points[after].x) {
		return 0;
	}
	//button
	else if (points[x].y < points[before].y && points[x].y < points[after].y && points[x].x > points[before].x && points[x].x < points[after].x) {
		return 1;
	}
	//left
	else if (points[x].y < points[before].y && points[x].y > points[after].y) {
		return 2;
	}
	//right
	else if (points[x].y > points[before].y && points[x].y < points[after].y) {
		return 3;
	}
	//M
	else if (points[x].y > points[after].y && points[x].y > points[before].y
		&& points[x].x > points[before].x && points[x].x < points[after].x) {
		return 4;
	}
	//T
	if (points[x].y < points[after].y && points[x].y < points[before].y
		&& points[x].x < points[before].x && points[x].x > points[after].x) {
		return 5;
	}
}

void SweepLineM(vector<vec2>& points, vector<int>& order, vector<pair<int, int>>& segs) {
	vector<trapezoid> tps;
	int length = order.size();
	for (int i = 0; i < length; i++) {
		switch (GetVertexType(points, order[i])) {
		case 0:
			InsertIntoTrapesM(tps, order[i], length);
			break;
		case 1:
			DeleteFromTrapesM(tps, order[i]);
			break;
		case 2:
			DealLeftVertexM(tps, order[i], length);
			break;
		case 3:
			DealRightVertexM(tps, order[i], length);
			break;
		case 4:
			SplitTrapesM(tps, order[i], points, segs);
			break;
		case 5:
			MergeTrapseM(tps, order[i]);
			break;
		}
	}
}


void InsertIntoTrapesT(vector<trapezoid>& tp, int x, int length) {
	int before = (x + length - 1) % length;
	int after = (x + 1) % length;
	tp.push_back(trapezoid(before, after, x));
}

void MergeTrapseT(vector<trapezoid>& tp, int x) {
	int left, right;
	int tplength = tp.size();
	for (int i = 0; i < tplength; i++) {
		if (tp[i].l == x) right = i;
		if (tp[i].r == x) left = i;
	}
	trapezoid newt(tp[left].l, tp[right].r, x);
	if (left > right) std::swap(left, right);
	tp.erase(tp.begin() + left);
	tp.erase(tp.begin() + right - 1);
	tp.push_back(newt);
}

void DealLeftVertexT(vector<trapezoid>& tp, int x, int length) {
	int tplength = tp.size();
	int left = -1;
	int before = (x + length - 1) % length;
	for (int i = 0; i < tplength; i++) {
		if (tp[i].l == x) {
			left = i;
			break;
		}
	}
	trapezoid newt(before, tp[left].r, x);
	tp.erase(tp.begin() + left);
	tp.push_back(newt);
}

void DealRightVertexT(vector<trapezoid>& tp, int x, int length) {
	int tplength = tp.size();
	int right = -1;
	int after = (x + 1) % length;
	for (int i = 0; i < tplength; i++) {
		if (tp[i].r == x) {
			right = i;
			break;
		}
	}
	trapezoid newt(tp[right].l, after, x);
	tp.erase(tp.begin() + right);
	tp.push_back(newt);
}

void SplitTrapesT(vector<trapezoid>& tp, int x, vector<vec2>& points, vector<pair<int, int>>& segs) {
	int tplength = tp.size();
	int length = points.size();
	int left, right, index;
	for (int i = 0; i < tplength; i++) {
		if (points[tp[i].l].x < points[x].x && points[tp[i].r].x > points[x].x) {
			left = tp[i].l;
			right = tp[i].r;
			bool flag = false;
			for (auto sg : segs) {
				if (sg.first == tp[i].helper && sg.second == x) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				segs.push_back(make_pair(tp[i].helper, x));
				segs.push_back(make_pair(x, tp[i].helper));
			}
			index = i;
			break;
		}
	}
	trapezoid lt(left, (x + 1) % length, x);
	trapezoid rt((x + length - 1) % length, right, x);
	tp.erase(tp.begin() + index);
	tp.push_back(lt);
	tp.push_back(rt);
}


void DeleteFromTrapesT(vector<trapezoid>& tp, int x) {
	vector<int> delets;
	int tplength = tp.size();
	for (int i = 0; i < tplength; i++) {
		if (tp[i].l == x) delets.push_back(i);
	}
	sort(delets.begin(), delets.end(), comp3);
	for (int i = 0; i < delets.size(); i++) {
		tp.erase(tp.begin() + delets[i]);
	}
}

void SweepLineT(vector<vec2>& points, vector<int>& order, vector<pair<int, int>>& segs) {
	vector<trapezoid> tps;
	int length = order.size();
	for (int i = length - 1; i >= 0; i--) {
		switch (GetVertexType(points, order[i])) {
		case 1:
			InsertIntoTrapesT(tps, order[i], length);
			break;
		case 0:
			DeleteFromTrapesT(tps, order[i]);
			break;
		case 2:
			DealLeftVertexT(tps, order[i], length);
			break;
		case 3:
			DealRightVertexT(tps, order[i], length);
			break;
		case 5:
			SplitTrapesT(tps, order[i], points, segs);
			break;
		case 4:
			MergeTrapseT(tps, order[i]);
			break;
		}
	}
}


#endif


