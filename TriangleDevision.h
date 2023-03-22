#pragma once
#ifndef TRIANGLEDEVISION_H
#define TRIANGLEDEVISION_H

#include <iostream>
#include "sweepline.h"
#include <vector>
#include <stack>
#include <map>
#include <set>
using namespace std;

//anti_clockwise_order
void GetOrder(vector<vec2>& points, vector<int>& order) {
	int index_top = 0, index_button = 0;
	int length = points.size();
	for (int i = 0; i < length; i++) {
		int before = (i + length - 1) % length;
		int after = (i + 1) % length;
		if (points[i].y > points[before].y && points[i].y > points[before].y) {
			index_top = i;
		}

		if (points[i].y < points[after].y && points[i].y < points[after].y) {
			index_button = i;
		}
	}

	int l = index_top + 1, r = index_top - 1;
	order.push_back(index_top);
	while ((r + length) % length != (l + length) % length) {
		if (points[(l + length) % length].y > points[(r + length) % length].y) {
			order.push_back((l + length) % length);
			l++;
		}
		else {
			order.push_back((r + length) % length);
			r--;
		}
	}
	order.push_back(index_button);
	return;
}

bool comp(vec2 v1, vec2 v2) {
	if (v1.y != v2.y) return v1.y > v2.y;
	return v1.x < v2.x;
}

void new_getOrder(vector<vec2>& points, vector<int>& order) {
	vector<vec2> orderedpoints;
	for (int i = 0; i < order.size(); i++) {
		orderedpoints.push_back(points[order[i]]);
	}
	sort(orderedpoints.begin(), orderedpoints.end(), comp);
	order.clear();
	for (int i = 0; i < orderedpoints.size(); i++) {
		order.push_back(orderedpoints[i].index);
	}
}

bool isConvex(vec2 p, vec2 s, vec2 t) {
	return !InLeftTest(p, s, t);

}

bool inLeft(int x, int top, int button) {
	if (top < button) {
		return x > top && x < button;
	}
	else {
		return x < button || x > top;
	}
}

void SplitIntoMonotonePolygons(vector<vec2>& points, vector<vector<int>>& Monos, vector<pair<int, int>> segs) {
	int length = points.size();
	int segslength = segs.size();
	bool** record = new bool* [length];
	for (int i = 0; i < length; i++) {
		record[i] = new bool[length];
		for (int j = 0; j < length; j++) {
			record[i][j] = false;
		}
	}
	vector<int>* G = new vector<int>[length];
	for (int i = 0; i < length; i++) {
		G[i].push_back((i + 1) % length);
	}
	for (int i = 0; i < segslength; i++) {
		G[segs[i].first].push_back(segs[i].second);
	}
	int edgeNum = length + segslength;
	int index = 0;
	int next_index = -1;
	int fork = -1;
	bool selectFork = true;
	vector<int> Mono;
	int edge = 0;
	while (true) {
		if (!Mono.empty() && index == Mono[0]) {
			Monos.push_back(Mono);
			Mono.clear();
			index = fork;
			selectFork = true;
			fork = -1;
		}
		if (edge == edgeNum) break;
		Mono.push_back(index);
		int len = G[index].size();

		if (G[index].size() == 1) {
			next_index = G[index][0];
		}
		else if (Mono.size() >= 2 && points[index].y < points[Mono[Mono.size() - 2]].y) {
			float maxCos = -1.01;
			int forkNum = 0;
			for (int j = 0; j < len; j++) {
				if (!record[index][G[index][j]]) {
					forkNum++;
					if (points[G[index][j]].y < points[index].y) {
						float co = getCosine(points[G[index][j]] - points[index], vec2(1, 0));
						if (maxCos < co) {
							maxCos = co;
							next_index = G[index][j];
						}
					}
				}
			}
			if (forkNum > 1 && selectFork) {
				fork = index;
				selectFork = false;
			}
		}
		else if (Mono.size() >= 2 && points[index].y > points[Mono[Mono.size() - 2]].y) {
			float maxCos = -1.01;
			int forkNum = 0;
			for (int j = 0; j < len; j++) {
				if (!record[index][G[index][j]]) {
					forkNum++;
					if (points[G[index][j]].y > points[index].y) {
						float co = getCosine(points[G[index][j]] - points[index], vec2(-1, 0));
						if (maxCos < co) {
							maxCos = co;
							next_index = G[index][j];
						}
					}
				}
			}
			if (forkNum > 1 && selectFork) {
				fork = index;
				selectFork = false;
			}
		}
		else {
			next_index = G[index][0];
		}

		record[index][next_index] = true;
		edge++;
		index = next_index;
	}

	return;
}

void Devide(vector<vec2>& points, vector<int>& order, vector<vector<int>>& MonotonePolygones, vector<pair<int, int>>& segs) {
	for (int i = 0; i < MonotonePolygones.size(); i++) {
		stack<int> si;
		order.clear();
		order.resize(MonotonePolygones[i].size());
		copy(MonotonePolygones[i].begin(), MonotonePolygones[i].end(), order.rbegin());
		new_getOrder(points, order);

		int length = order.size();
		int top = order[0];
		int button = order[length - 1];

		if (MonotonePolygones[i].size() <= 3) continue;
		while (!si.empty()) {
			si.pop();
		}

		si.push(top);
		int prev_direction = 0;//0:left, 1:right
		for (int i = 1; i < length - 1; i++) {
			int p = order[i];
			//左半边链
			int direction = inLeft(p, top, button) ? 0 : 1;
			if (direction == prev_direction) {
				while (si.size() >= 2) {
					int s = si.top();
					si.pop();
					int t = si.top();
					bool lc = isConvex(points[p], points[s], points[t]);
					if ((direction == 0 && lc) || (direction == 1 && !lc)) {
						segs.push_back(pair<int, int>(t, p));
					}
					else {
						si.push(s);
						break;
					}
				}
				si.push(p);
			}
			//右半边链
			else {
				int stackTop = si.top();
				while (si.size() > 1) {
					int s = si.top();
					si.pop();
					segs.push_back(pair<int, int>(p, s));
				}
				si.pop();
				si.push(stackTop);
				si.push(p);
			}
			prev_direction = direction;
		}
	}
	return;
}

void RemoveReatedSegs(vector<pair<int, int>>& segs) {
	set<pair<int, int>> st;
	for (auto sg : segs) {
		int f = sg.first, g = sg.second;
		if (f > g) std::swap(f, g);
		st.insert(make_pair(f, g));
	}
	segs.clear();
	for (auto it : st) {
		segs.push_back(it);
	}
}

//只对y轴方向单调的无洞多边形有效
//画图时逆时针
void DevideIntoTriangle(vector<vec2>& points, vector<pair<int, int>>& segs) {
	int length = points.size();
	vector<int> order;
	for (int i = 0; i < length; i++) {
		order.push_back(i);
	}

	new_getOrder(points, order);

	SweepLineM(points, order, segs);
	SweepLineT(points, order, segs);

	vector<vector<int>> MonotonePolygones;
	SplitIntoMonotonePolygons(points, MonotonePolygones, segs);

	Devide(points, order, MonotonePolygones, segs);
	RemoveReatedSegs(segs);

	return;
}

#endif